
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using Quantum;
using UnityEditor;
using Sirenix.OdinInspector;
using Quantum.Core;
using Photon.Deterministic;
using UniRx;
using RxMsg.ReviewMsg;

public partial class MultiplayRunner
{
    private QuantumRunner _runner = null;
    private ReviewFileManager.ReviewFileInfo reviewFileInfo = null;
    private IDeterministicReplayProvider _replayInputProvider = null;
    private IResourceManager _resourceManager;

    private const int snapshotSize = 25;
    private const int effectCreateTimeInTimeOffset = 4; /// TimeOffset중 끝나기전 n초부터 이펙트 생성을 한다.

    private int curFrameNum; /// 현재 진행 프레임
    private int targetFrameNum; /// 유저 입력에 따른 이동할 프레임
    private bool isPlay = true;
    private float simulationSpeed = 1f;

    /// 프레임 진행 설정값
    private int reviewFps;
    private int startFrameNum;
    private int endFrameNum;
    private int totalFrameSize;         /// 다시보기 전체 프레임 크기('startFrameNum' ~ 'endFrameNum')
    private float totalFrameTime;        /// 총 재생시간

    private int playFrameSize;          /// 다시보기 실행 프레임 크기(totalFrameSize에서 골리프레이 구간은 제외되었다.)
    private float playFrameTime;

    private List<int> saveFrameNumList; /// 저장할 프레임넘버 리스트

    private List<ReviewFileManager.GoalReplaySection> grSectionList; ///골리플레이 플레이 구간 리스트

    
    /// 진행 시간값 (골리플레이 구간은 제외된 값)
    private float curTimeSec;           /// 현재 재생중인 시간
    
    private float reviewUINotiTime = 0.5f; /// 현재 재생시간 ui업데이트 주기

    DeterministicFrameRingBuffer _rewindSnapshots = null;
    Native.Allocator _resourceAllocator;

    /// applyTimeOffset 실행 관련 정보(이동할 대상프레임과 가장 가까운 프레임으로 싱크를 한뒤 대상프레임에 맞게 시간을 진행시킨다.)
    private class ApplyTimeInfo
    {
        public bool isOnGoing = false;  /// applyTime 진행 여부
        public double startSimulationTime; /// applyTime 실행했을때의 SimulationTimeElasped 값
        public double applyTimeValue;
        public float effectTime;
        public bool isCurPlay = false; ///applyTime가 실행되어졌을떄 play or pause 상태 여부 저장
        public float speed = 1f; ///applyTime가 실행되어졌을때 'simulationSpeed' 상태 저장

        public bool isEffectTimeGoing = false;/// applyTime이후 이펙트 진행 시간중인지 여부

        public void Start(double _startTime, double _addValue, bool _isPlay, float _effectTime, float _speed)
        {
            isOnGoing = true;

            startSimulationTime = _startTime;
            applyTimeValue = _addValue;
            this.isCurPlay = _isPlay;
            effectTime = _effectTime;
            speed = _speed;

            isEffectTimeGoing = false;
        }
    }
    private ApplyTimeInfo applyTimeInfo;
    private float effectProgressTime = 0.5f; /// 시간을 진행시킬때 이펙트 진행을 위한 시간

    private void UpdateReview()
    {
        if (_runner == null || _runner.Session == null) return;
        if (gameMode != GameMode.Review) return;

        if (isPlay && curFrameNum < endFrameNum)
        {
            _runner.Session.Update(Time.deltaTime * simulationSpeed);
        }
        else
        {
            /// 모든 재생이 끝나면 퍼즈 상태로 설정
            if(isPlay)
            {
                isPlay = false;
                Time.timeScale = 0;
                G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_TimeInfo()
                {
                    isPause = !isPlay,
                    curTime = curTimeSec,
                });
            }
            
        }
        
        /// 진행 시간ui 정보 갱신
        reviewUINotiTime -= Time.deltaTime;
        if (reviewUINotiTime <= 0)
        {
            G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_curTime(){ curTime = curTimeSec });
            reviewUINotiTime = 0.5f;
        }

    }
    private void Destroy_Review(QuantumGame game)
    {
        _rewindSnapshots = null;

        G.MsgBroker.Publish(new RxMsg.MouseLock.ReplayModFinish());

        _resourceManager?.Dispose();
        _resourceManager = null;

        streamContainer?.Dispose();

        if(saveFrameNumList != null)
        {
            saveFrameNumList.Clear();
            saveFrameNumList = null;
        }
        if(grSectionList != null)
        {
            grSectionList.Clear();
            grSectionList = null;
        }

        _resourceAllocator?.Dispose();
        _resourceAllocator = null;
    }

    public void OnGameStart_Review(QuantumGame game)
    {
        if (gameMode != GameMode.Review) return;

        G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_TimeInfo()
        {
            isPause = !isPlay,
            curTime = curTimeSec,
        });

        UpdateScoreInfo(game.Frames.Verified);
        G.MsgBroker.Publish(new ReviewStart());
        G.MsgBroker.Publish(new RxMsg.MouseLock.ReplayModStart());
        G.MsgBroker.Publish(new RxMsg.LoadingFinish());
    }
    public void OnSimulateFinished_Review(QuantumGame game, Frame f)
    {
        if (gameMode != GameMode.Review) return;

        /// 스냅샷 저장 프레임여부 확인및 저장
        if (f.IsVerified && saveFrameNumList.Contains(f.Number))
        {
            saveFrameNumList.Remove(f.Number);
            //game.SaveReviewSnapShot(f);
            _rewindSnapshots.PushBack(f, game.CreateFrame);
        }
        
        /// 현재 프레임이 골리플레이 구간여부 확인
        CheckCurFrame(f);

        /// checksum error찾기 위한 테스트 소스
        //if (f.Number % 1000 == 0)
        //{
        //    G.MsgBroker.Publish(new RxMsg.QuantumMsg.CreateDumpFrameFile() { frame = f });
        //}
    }
    public void OnGameResync_Review(QuantumGame game)
    {
        curFrameNum = game.Frames.Verified.Number;

        /// 스냅샷 데이터로 동기화된 프레임에서 타겟 프레임으로 이동
        if (curFrameNum < targetFrameNum)
        {
            double addTime = (double)(targetFrameNum - curFrameNum) / (double)reviewFps;
            addTime += addTime >= effectProgressTime ? -effectProgressTime : 0;

            if (applyTimeGoingCo != null)
            {
                applyTimeInfo.isOnGoing = false;
                StopCoroutine(applyTimeGoingCo);
                applyTimeGoingCo = null;
            }

            applyTimeGoingCo = StartCoroutine(Co_ApplyTimeGoing(_runner.Session.SimulationTimeElasped, addTime, isPlay));
            //_runner.Session.ApplyTimeOffset(addTime);
        }
        else
        {
            curTimeSec = UpdateCurTimeSec(curFrameNum);
            G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_curTime() { curTime = curTimeSec });
            G.MsgBroker.Publish(new RxMsg.ReviewMsg.TimeShiftCompletion());
            UpdateScoreInfo(_runner.Game.Frames.Verified);
        }
    }

    public void Init_Review(ReviewFileManager.ReviewFileInfo fileInfo)
    {
        gameMode = GameMode.Review;

        reviewFileInfo = fileInfo;
        
        InitFrameSetValue();

        isPlay = true;
        simulationSpeed = 1f;
        curTimeSec = 0f;
        applyTimeInfo = new ApplyTimeInfo();

        InitListenRxMsg_Review();

        _rewindSnapshots = new DeterministicFrameRingBuffer(snapshotSize);
    }

    StreamContainer streamContainer = new StreamContainer();
    public void InitListenRxMsg_Review()
    {
        streamContainer?.Dispose();
        streamContainer = new StreamContainer();

        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.Play>()
            .Subscribe(msg =>
            {
                ReviewPlay();
                msg.isPause = !isPlay;
            }).AddTo(this);
        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.Pause>()
            .Subscribe(msg =>
            {
                ReviewPause();
                msg.isPause = !isPlay;
            }).AddTo(this);
        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.SpeedChange>()
            .Subscribe(msg =>
            {
                /// applyTime진행중에는 입력 불가
                if (applyTimeInfo.isOnGoing) return;

                ReviewSpeedChange(msg.speed);
            }).AddTo(this);
        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.SetTimeShift>()
            .Subscribe(msg =>
            {
                ReviewTimeShift(msg);
            }).AddTo(this);

        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.IsEffectCreateInReview>()
            .Subscribe(msg =>
            {
                if (applyTimeInfo.isOnGoing)
                {
                    msg.isCreate = curFrameNum >= (targetFrameNum - (effectCreateTimeInTimeOffset * reviewFps));
                }
                else
                    msg.isCreate = true;
            }).AddTo(this);

        streamContainer += G.MsgBroker.Receive<RxMsg.ReviewMsg.SetReviewEventFilter>()
            .Subscribe(msg =>
            {
                curEventInfo = msg.eventInfo;
            }).AddTo(this);
    }
    public void StartReviewQuantumRunner()
    {
        var serializer = new QuantumUnityJsonSerializer();
        var quantumReplayFile = G.ReviewFileManager.RePlayInfo.replayInfo;

        _replayInputProvider = quantumReplayFile.CreateInputProvider();
        if (null == _replayInputProvider)
        {
            LOG.E($"StartReviewQuantumRunner error, _replayInputProvider is null");
            return;
        }

        var _arguments = new SessionRunner.Arguments
        {
            RunnerFactory = QuantumRunnerUnityFactory.DefaultFactory,
            RuntimeConfig = serializer.ConfigFromByteArray<RuntimeConfig>(quantumReplayFile.RuntimeConfigData.Decode(), compressed: false),
            SessionConfig = quantumReplayFile.DeterministicConfig,
            ReplayProvider = _replayInputProvider,
            GameMode = DeterministicGameMode.Replay,
            RunnerId = "LOCAL_REVIEW_GAME",
            PlayerCount = quantumReplayFile.DeterministicConfig.PlayerCount,
            InstantReplaySettings = InstantReplaySettings.Default,
            InitialTick = quantumReplayFile.InitialTick,
            FrameData = quantumReplayFile.InitialFrameData,
            DeltaTimeType = SimulationUpdateTime.Default,
        };

        var assets = quantumReplayFile.AssetDatabaseData?.Decode();

        if (assets?.Length > 0)
        {
            _resourceAllocator = new QuantumUnityNativeAllocator();
            _resourceManager = new ResourceManagerStatic(serializer.AssetsFromByteArray(assets), new QuantumUnityNativeAllocator());
            _arguments.ResourceManager = _resourceManager;
        }

        _runner = QuantumRunner.StartGame(_arguments);
        _runner.IsSessionUpdateDisabled = true;
    }

    #region[프레임 진행 설정]
    private void InitFrameSetValue()
    {
        applyTimeGoingCo = null;

        reviewFps = G.ReviewFileManager.RePlayInfo.fps;
        startFrameNum = G.ReviewFileManager.RePlayInfo.startFrameNum;
        endFrameNum = G.ReviewFileManager.RePlayInfo.endFrameNum;
        totalFrameSize = G.ReviewFileManager.RePlayInfo.totalFrameSize;
        totalFrameTime = G.ReviewFileManager.RePlayInfo.totalFrameTime;
        playFrameSize = G.ReviewFileManager.RePlayInfo.playFrameSize;
        playFrameTime = G.ReviewFileManager.RePlayInfo.playFrameTime;

        ///골 리프레이 구간 리스트 생성
        grSectionList = G.ReviewFileManager.RePlayInfo.goalSectionList;

        /// 시간이동을 위한 스냅샷 저장 프레임 리스트 지정
        saveFrameNumList = new List<int>();
        saveFrameNumList.Add(startFrameNum);
        for (int i = 1; i < snapshotSize; i++)
        {
            int frameNum = startFrameNum + (totalFrameSize / (snapshotSize - 1)) * i;
            if(frameNum < endFrameNum)
                saveFrameNumList.Add(frameNum);
        }
    }
    /// <summary>
    /// 시뮬레이션이 끝날때마다 현재 프레임 체크 및 저장
    /// </summary>
    private void CheckCurFrame(Frame f)
    {
        curFrameNum = f.Number;

        for (int i = 0; i < grSectionList.Count; i++)
        {
            /// 현재 시뮬레이션 골리플레이 구간이면 구간점프 실행(이전에 했던 ApplyTimeOffset작업이 완료되면 실행할수 있다.)
            if (!applyTimeInfo.isOnGoing && curFrameNum >= grSectionList[i].start && curFrameNum <= grSectionList[i].end)
            {
                double addSec = (grSectionList[i].end - curFrameNum) / reviewFps;
                addSec += addSec >= effectProgressTime ? -effectProgressTime : 0;
                
                if (applyTimeGoingCo != null)
                {
                    applyTimeInfo.isOnGoing = false;
                    StopCoroutine(applyTimeGoingCo);
                    applyTimeGoingCo = null;
                }
                applyTimeGoingCo = StartCoroutine(Co_ApplyTimeGoing(_runner.Session.SimulationTimeElasped, addSec, isPlay));
                //_runner.Session.ApplyTimeOffset(addSec);
                break;
            }
        }

        curTimeSec = UpdateCurTimeSec(curFrameNum);
    }
    /// <summary>
    /// 'frameNum'프레임이 골리플레이 구간인지 체크하고, 맞다면 구간다음 프레임값을 리턴
    /// </summary>
    private int GetPossibleShiftFrameNum(int targetFrame, float shiftValue)
    {
        for (int i = 0; i < grSectionList.Count; i++)
        {
            /// 현재 시뮬레이션 골리플레이 구간이면 구간점프 실행
            if (targetFrame >= grSectionList[i].start && targetFrame <= grSectionList[i].end)
            {
                if(shiftValue >= 0)
                    return grSectionList[i].end + (int)(shiftValue * reviewFps);
                else
                    return
                        grSectionList[i].start + (int)(shiftValue * reviewFps);
            }
        }

        return targetFrame;
    }

    #endregion

    #region[유저 컨트롤 기능]
    private void ReviewPlay()
    {
        /// 다시보기 재생 끝나 있거나, applyTime진행중에는 입력 불가
        if (curFrameNum >= endFrameNum || applyTimeInfo.isOnGoing) return;

        if (isPlay) return;

        Time.timeScale = simulationSpeed;
        isPlay = true;
    }
    private void ReviewPause()
    {
        /// 다시보기 재생 끝나 있거나, applyTime진행중에는 입력 불가
        if (curFrameNum >= endFrameNum || applyTimeInfo.isOnGoing) return;

        if (!isPlay) return;

        Time.timeScale = 0;
        isPlay = false;
    }
    private void ReviewSpeedChange(double speed)
    {
        simulationSpeed = (float)speed;
        Time.timeScale = simulationSpeed;
    }
    /// <summary>
    /// shiftValue 만큼의 시간만큼 프레임을 이동한다.
    /// -30, 30 값인경우는 가장 가까운 이벤트의 프레임으로 이동한다.
    /// </summary>
    private void ReviewTimeShift(SetTimeShift timeShift)
    {
        /// applyTime진행중에는 입력 불가
        if (applyTimeInfo.isOnGoing) return;

        float shiftValue = timeShift.shiftValue;
        /// 가장 가까운 이벤트 프레임으로 이동
        if (timeShift.type == SetTimeShift.TimeShiftType.EventLeftMove || timeShift.type == SetTimeShift.TimeShiftType.EventRightMove)
        {
            shiftValue = GetReviewEventShiftValue(timeShift);
            if (shiftValue == 0) return;
        }

        /// 처음시작이면 시간- 불가
        if (shiftValue < 0 && curFrameNum <= startFrameNum) return;
        /// 재생완료되면 시간+ 불가
        else if (shiftValue > 0 && curFrameNum >= endFrameNum) return;

        /// 이동할 프레임 설정
        int targetFrame = curFrameNum + (int)(shiftValue * reviewFps);
        if (targetFrame < startFrameNum)
            targetFrame = startFrameNum;
        else if (targetFrame > endFrameNum)
            targetFrame = endFrameNum;

        /// 골리플레이 구간에 의한 추가 이동시간 체크
        targetFrame = GetPossibleShiftFrameNum(targetFrame, shiftValue);

        TargetFrameShift(targetFrame, shiftValue);
    }
    /// <summary> 입력 방향에 따라 현재 프레임에서 가장 가까운 이벤트 프레임으로 이동할 시간값을 계산한다. </summary>
    List<ReviewFileManager.ReviewEventInfo> curEventInfo;
    private float GetReviewEventShiftValue(SetTimeShift timeShift)
    {
        int targetFrameNum = -1;
        for(int i = 0; i < curEventInfo.Count; i++)
        {
            /// 이벤트 발생위치에서 표시 위치로 계산
            int frameNum = curEventInfo[i].frameNum - (timeShift.eventFrontTime * reviewFps);
            if (frameNum < startFrameNum) frameNum = startFrameNum;

            /// 타임라인 정방향으로 현재프레임에서 가장가까운 이벤트 프레임을 검색
            if (timeShift.type == SetTimeShift.TimeShiftType.EventRightMove && curFrameNum < frameNum)
            {
                if (targetFrameNum == -1 || targetFrameNum > frameNum)
                    targetFrameNum = frameNum;
            }
            /// 타임라인 역방향으로 현재프레임에서 가장가까운 이벤트 프레임을 검색
            else if (timeShift.type == SetTimeShift.TimeShiftType.EventLeftMove && curFrameNum > frameNum)
            {
                /// 역방향일 경우는 eventCheckTime 보다 가까운 이벤트는 생략한다.
                if ((curFrameNum - frameNum) < (timeShift.eventCheckTime * reviewFps))
                    continue;

                if (targetFrameNum == -1 || targetFrameNum < frameNum)
                    targetFrameNum = frameNum;
            }
        }

        /// 대상 이벤트를 찾지 못했을 경우, 정방향은 입력 무시한다.
        if (targetFrameNum == -1 && timeShift.type == SetTimeShift.TimeShiftType.EventRightMove)
            return 0;
        /// 대상 이벤트를 찾지 못했을 경우, 역방향은 가장 처음 프레임으로 이동시킨다.
        else if (targetFrameNum == -1 && timeShift.type == SetTimeShift.TimeShiftType.EventLeftMove)
            targetFrameNum = startFrameNum;

        /// 이동할 시간값을 계산한다.
        return (float)((targetFrameNum - curFrameNum) / reviewFps);
    }
    /// <summary> 대상 프레임으로  이동</summary>
    private void TargetFrameShift(int targetFrame, float shiftValue)
    {
        targetFrameNum = targetFrame;

        /// 타겟 프레임과 가장 가까운 스냅샷정보를 가져와서 Resync 실행
        if (gameSystem == null) return;

        var game = gameSystem.GetCurQuantumGame(QuantumGameType.Live);

        var nearestTarget = _rewindSnapshots.Find(targetFrameNum, DeterministicFrameSnapshotBufferFindMode.ClosestLessThanOrEqual);
        if (nearestTarget == null)
        {
            LOG.E($"Unable to find a frame with number less or equal to {targetFrameNum}.");
            return;
        }

        //RingBuffer<Frame> temp = game.GetReviewSnapshot();
        //Frame nearestTarget = null;
        //if (temp != null)
        //{
        //    var list = temp.ToArray;
        //    for (int i = 0; i < list.Length; i++)
        //    {
        //        if (list[i] == null) continue;
        //        if (list[i].Number > targetFrameNum) continue;

        //        if (nearestTarget == null)
        //            nearestTarget = list[i];
        //        else if (targetFrameNum - nearestTarget.Number > targetFrameNum - list[i].Number)
        //            nearestTarget = list[i];
        //    }
        //}

        if (nearestTarget != null)
        {
            /// 시간+ 작업일때, nearestTarget보다 현재 프레임이 타겟프레임에 더 가까우면 리싱크를 하지않고 ApplyTimeOffset으로 처리
            if (shiftValue > 0 && targetFrameNum - nearestTarget.Number > targetFrameNum - curFrameNum)
            {
                double addTime = shiftValue >= effectProgressTime ? shiftValue - effectProgressTime : 0;
                if (applyTimeGoingCo != null)
                {
                    applyTimeInfo.isOnGoing = false;
                    StopCoroutine(applyTimeGoingCo);
                    applyTimeGoingCo = null;
                }
                applyTimeGoingCo = StartCoroutine(Co_ApplyTimeGoing(_runner.Session.SimulationTimeElasped, addTime, isPlay));

                //_runner.Session.ApplyTimeOffset(addTime);
            }
            else
            {
                _runner.Session.ResetReplay(nearestTarget);

                var tempGame = _runner.Session.Game as QuantumGame;
                //System.IO.File.WriteAllText($"LocalSaveData/CheckSumError/nearestTarget_frame({nearestTarget.Number}).txt", nearestTarget.DumpFrame());
                //System.IO.File.WriteAllText($"LocalSaveData/CheckSumError/ResetReplay_frame({tempGame.Frames.Verified.Number}).txt", tempGame.Frames.Verified.DumpFrame());
            }
        }
        else
        {
#if LOG_ENABLED
            LOG.E($"스냅샷 정보를 찾을수 없음1 ({startFrameNum}) ({targetFrameNum}) ({curFrameNum})");
#endif
        }
    }

    public int CurrentFrame => _runner.Game.Frames.Verified.Number;
    private void FastForward(int frameNumber)
    {
        if (frameNumber < CurrentFrame)
        {
            LOG.E($"Can't seek backwards to {frameNumber} from {CurrentFrame}", nameof(frameNumber));
        }
        else if (frameNumber == CurrentFrame)
        {
            return;
        }

        const int MaxAttempts = 3;
        for (int attemptsLeft = MaxAttempts; attemptsLeft > 0; --attemptsLeft)
        {

            int beforeUpdate = CurrentFrame;

            double deltaTime = GetDeltaTime(frameNumber - beforeUpdate, _runner.Session.SessionConfig.UpdateFPS);
            _runner.Session.Update(deltaTime);

            int afterUpdate = CurrentFrame;

            if (afterUpdate >= frameNumber)
            {
                if (afterUpdate > frameNumber)
                {
                    Debug.LogWarning($"Seeked after the target frame {frameNumber} (from {beforeUpdate}), got to {afterUpdate}.");
                }
                return;
            }
            else
            {
                Debug.LogWarning($"Failed to seek to frame {frameNumber} (from {beforeUpdate}), got to {afterUpdate}. {attemptsLeft} attempts left.");
            }
        }

        LOG.E($"Unable to seek to frame {frameNumber}, ended up on {CurrentFrame}");
    }
    double GetDeltaTime(int frames, int simulationRate)
    {
        // need repeated sum here, since internally Quantum performs repeated substraction
        double delta = 1.0 / simulationRate;
        double result = 0;
        for (int i = 0; i < frames; ++i)
        {
            result += delta;
        }
        return result;
    }
    #endregion

    Coroutine applyTimeGoingCo;
    IEnumerator Co_ApplyTimeGoing(double _startTime, double _addValue, bool _isPlay)
    {
        G.MsgBroker.Publish(new RxMsg.ReviewMsg.TimeShiftStart());
        
        /// 퍼즈 상태라도 applyTime이 끝날때까지 play 상태로 바꾼다.
        applyTimeInfo.Start(_startTime, _addValue, _isPlay, effectProgressTime, simulationSpeed);
        isPlay = true;

        Time.timeScale = 1;

        G.MsgBroker.Publish(new RxMsg.QuantumMsg.SetActiveEntityView() { isActive =  false});
        do
        {
            
            /// Co_ApplyTimeGoing 진행중 모든 재생이 완료되었을 경우
            if (Time.timeScale == 0)
            {
                applyTimeInfo.isOnGoing = false;
                G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_curTime() { curTime = curTimeSec });
                G.MsgBroker.Publish(new RxMsg.ReviewMsg.TimeShiftCompletion());
                UpdateScoreInfo(_runner.Game.Frames.Verified);
            }

            double curSimulationTime = _runner.Session.SimulationTimeElasped;
            double targetShiftTime = applyTimeInfo.startSimulationTime + applyTimeInfo.applyTimeValue;
            //Log.Info($"Co_ApplyTimeGoing startTime({_startTime}) addTime({_addValue}) curSimulationTime({curSimulationTime})");
            /// 시간 이동중에 'ApplyTimeOffset'을 순차적으로 실행시킨다. 
            if (!applyTimeInfo.isEffectTimeGoing && curSimulationTime < targetShiftTime)
            {
                if (targetShiftTime - curSimulationTime >= 1)
                    _runner.Session.ApplyTimeOffset(1);
                else
                    _runner.Session.ApplyTimeOffset(targetShiftTime - curSimulationTime);
            }
            /// 진행이 모두 완료 되었는지 체크
            else if (!applyTimeInfo.isEffectTimeGoing && curSimulationTime >= targetShiftTime)
            {
                applyTimeInfo.isEffectTimeGoing = true;
                G.MsgBroker.Publish(new RxMsg.QuantumMsg.SetActiveEntityView() { isActive = true });
            }
            /// 진행이 완료가 되면 이펙트 진행 시간까지 지난뒤 종료처리한다.
            else if (applyTimeInfo.isEffectTimeGoing && curSimulationTime >= targetShiftTime + applyTimeInfo.effectTime)
            {
                isPlay = applyTimeInfo.isCurPlay;
                //ReviewSpeedChange(isPlay ? applyTimeInfo.speed : 0);
                Time.timeScale = isPlay ? applyTimeInfo.speed : 0;

                applyTimeInfo.isOnGoing = false;

                G.MsgBroker.Publish(new RxMsg.ReviewMsg.SetReviewUI_curTime() { curTime = curTimeSec });
                G.MsgBroker.Publish(new RxMsg.ReviewMsg.TimeShiftCompletion());
                UpdateScoreInfo(_runner.Game.Frames.Verified);
            }

            yield return null;
        } while (applyTimeInfo.isOnGoing);

        applyTimeGoingCo = null;
        yield return null;
    }

    float UpdateCurTimeSec(int _curFrameNum)
    {
        curFrameNum = _curFrameNum;
        int playFrameLength = curFrameNum - startFrameNum + 1;

        ///진행 프레임에서 골리플레이 구간은 제외
        if (null != grSectionList && 0 < grSectionList.Count)
        {
            for (int i = 0; i < grSectionList.Count; i++)
            {
                if (curFrameNum >= grSectionList[i].start && curFrameNum <= grSectionList[i].end)
                {
                    playFrameLength -= (curFrameNum - grSectionList[i].start + 1);
                    break;
                }
                else if (curFrameNum > grSectionList[i].end)
                    playFrameLength -= (grSectionList[i].end - grSectionList[i].start + 1);
            }
        }

        curTimeSec = playFrameLength / reviewFps;
        return curTimeSec;
    }

    private unsafe void UpdateScoreInfo(Frame f)
    {
        int redScore = 0; int blueScore = 0;

        if(f.Global->teamStateList.Ptr.Offset != 0)
        {
            var list = f.ResolveList<TeamStateInfo>(f.Global->teamStateList);
            for (int i = 0; i < list.Count; i++)
            {
                var state = list.GetPointer(i);
                if (state->type == TeamIndex.Red)
                    redScore = state->score.AsInt;
                else if (state->type == TeamIndex.Blue)
                    blueScore = state->score.AsInt;
            }
        }
        
        G.MsgBroker.Publish(new RxMsg.Multiplay.SetGameScoreForUI(TeamIndex.Red, redScore, blueScore));
    }
}
