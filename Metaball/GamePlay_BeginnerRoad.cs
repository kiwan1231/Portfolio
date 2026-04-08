using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Quantum;
using Photon.Deterministic;

/// <summary>
/// RuntimeConfig.GameOptConfig.intOptDic
/// 0 : 비기너로드 스테이지 타입
/// </summary>

public unsafe partial class GamePlaySystem
{
	protected class BeginnerRoadPlay : IGamePlaySystemInternal
	{
		BeginnerRoadComponent* beginnerRoad = null;
		HalfCourtComponent* halfCourt = null;

		BeginnerRoadSpec beginnerRoadSpec = null;
		HalfCourtSpec halfCourtSpec = null;

		TB_OnSetInGameUI.SetInGameUI setUI = new TB_OnSetInGameUI.SetInGameUI();

		public override void Init(Frame f)
		{
			/// 생성
			var protoType = f.FindAsset<EntityPrototype>(ProtoTypePath.GamePlayProtoType);
			var entityRef = f.Create(protoType);


			if (f.Unsafe.TryGetPointer<GamePlayComponent>(entityRef, out var component))
			{
				InitGamePlayComponent(f, component);
			}

			if (f.Unsafe.TryGetPointer<GameRoundComponent>(entityRef, out var gameRound))
			{
				gameRound->resultList = f.AllocateList<RoundResult>();
				InitGameRoundComponent(f, gameRound);
			}

			if (f.RuntimeConfig.mapModeType == MapModeType.HalfCourt)
			{
				if (false == f.TryGetSpec(SpecType.HalfCourtSpec, out halfCourtSpec))
					return;

				var halfCourtComponent = new HalfCourtComponent();
				halfCourtComponent.currentSetIndex = 0;
				halfCourtComponent.abstentioTeam = TeamIndex.None;
				halfCourtComponent.setResultList = f.AllocateList<HalfCourtSetResult>();
				var list = f.ResolveList(halfCourtComponent.setResultList);
				for (int i = 0; i < halfCourtSpec.setCount; i++)
				{
					var result = new HalfCourtSetResult();
					list.Add(result);
				}

				f.Add(entityRef, halfCourtComponent);

				f.RuntimeConfig.matchTime = halfCourtSpec.atkTime;
			}
			else
			{
				f.RuntimeConfig.matchTime = 120;
			}
			var beginnerRoadComponent = new BeginnerRoadComponent();
			f.Add(entityRef, beginnerRoadComponent);

			SetBeginnerRoadStage(f, f.RuntimeConfig.gameOptConfig.intList[0]);

			f.Events.QuantumGameSystemInitEnd(component->currentState);
		}

		public override void Update(Frame f)
		{
			if (false == GetComponent(f, out gamePlayRef, out gamePlay, out gameRound, out beginnerRoad, out halfCourt))
				return;
			if (false == f.TryGetSpec(SpecType.BeginnerRoadSpec, out beginnerRoadSpec))
				return;
			if (false == f.TryGetSpec(SpecType.HalfCourtSpec, out halfCourtSpec))
				return;

			base.Update(f);
		}

		public override void UpdateState(Frame f)
		{
			base.UpdateState(f);
		}

		public override void UpdatePlayTime(Frame f)
		{
			/// 비기너 로드 2단계는 게임의 무제한이면 유저가 골을 넣으면 게임종료, ai가 볼을 소유하면 위치 리셋을 한다.
			if (beginnerRoad->type == BeginnerRoadType.Stage1)
			{
				if (gamePlay->currentState == GameState.Playing)
				{
					var filter = f.Filter<Hoverboard, BallOwnerComponent, TeamComponent, AIData>();
					while (filter.NextUnsafe(out var boardRef, out var board, out var boardBallOwner, out var boardTeam, out var boardAi))
					{
						/// 수비팀이 볼을 소유시 공격 종료
						if (halfCourt->currentAtkTeam != boardTeam->TeamIndex
							&& boardBallOwner->HasBall())
						{
							ChangeGameState(f, GameState.Pause);
							return;
						}
					}
				}
				return;
			}

			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				if (gamePlay->currentState == GameState.Playing)
				{         
					/// elapsedTime 업데이트
					gamePlay->elapsedTime += f.DeltaTime;

					var filter = f.Filter<Hoverboard, BallOwnerComponent, TeamComponent, AIData>();
					while (filter.NextUnsafe(out var boardRef, out var board, out var boardBallOwner, out var boardTeam, out var boardAi))
					{
						/// 수비팀이 볼을 소유시 공격 종료
						if (halfCourt->currentAtkTeam != boardTeam->TeamIndex
							&& boardBallOwner->HasBall())
						{
							f.TryGetSpec<BallSpec>(SpecType.Ball, out var ballSpec);
							//QuantumUtility.DespawnBall(f, EntityRef.None);
							QuantumUtility.DelayDespawnBall(f, EntityRef.None, FP._0_50);
							ChangeGameState(f, GameState.Pause);
							return;
						}
					}

					/// 공격 시간 종료
					if (halfCourtSpec.atkTime <= gamePlay->elapsedTime)
					{
						f.TryGetSpec<BallSpec>(SpecType.Ball, out var ballSpec);
						//QuantumUtility.DespawnBall(f, EntityRef.None);
						QuantumUtility.DelayDespawnBall(f, EntityRef.None, FP._0_50);
						ChangeGameState(f, GameState.Pause);
					}
				}
			}
			else
			{
				base.UpdatePlayTime(f);
			}
		}

		public override void UpdateCommand(Frame f)
		{
			var userList = f.ResolveList(f.Global->userList);
			for (int i = 0; i < userList.Count; i++)
			{
				if (userList[i] == PlayerRef.None) continue;

				var command = f.GetPlayerCommand(userList[i]);
				if (null == command)
					continue;

				UpdateTestCommand(f, command);

				UpdateGamePlayCommand(f, userList[i], command);
			}
		}

		public override void UpdateUI(Frame f)
		{
			base.UpdateUI(f);
		}

		#region[UpdateState]
		public override void PlayerReadyState(Frame f)
		{
			if (gamePlay->stateStep == StateStep.Step_Enter)
			{
				SetActiveInGameUI(f, false);

				f.Events.OnGameBgm();

				/// GamePlayHfsm 초기화
				f.Signals.OnRefreshGamePlayHfsm(0);

				f.Events.OnSetActiveAllPlayer(false);
				f.Events.OnPlayerReadyStart();

				gamePlay->stateStep = StateStep.Step_1;
			}

			else if (gamePlay->stateStep == StateStep.Step_1)
			{
				
			}
		}
		public override void InitState(Frame f)
		{
			if (gamePlay->stateStep == StateStep.Step_Enter)
			{
				f.Events.OnSetActiveAllPlayer(true);

				f.TryGetSpec<DribbleSpec>(SpecType.Dribble, out var spec);

				var filter = f.Filter<Hoverboard, DribbleComponent>();
				while (filter.NextUnsafe(out EntityRef e, out var board, out var dribble))
				{
					DribbleSystem.InitAutoGrab(dribble, spec);
					DribbleSystem.InitShield(dribble, spec);
				}

				if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
				{
					/// 선공팀 레드 고정
					halfCourt->firstAtkTeam = TeamIndex.Red;
					halfCourt->secondAtkTeam = TeamIndex.Blue;
					halfCourt->currentAtkTeam = halfCourt->firstAtkTeam;

					/// 골대 팀변경
					ChangeGoalPostTeam(f, halfCourt);

					f.Events.OnSetHalfCourtInfo(halfCourtSpec.setCount, halfCourt->firstAtkTeam, halfCourt->secondAtkTeam);
				}

				ChangeGameState(f, GameState.Countdown);
			}
		}

		public override void CountState(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				/// 카운트 다운 이벤트
				OnCountDownUI(f, gamePlaySpec.OffenseConversionDelay, gamePlay->stateElapsedTime, halfCourt->currentAtkTeam);

				var isFirstStart = halfCourt->currentSetIndex == 0 && halfCourt->currentAtkTeam == TeamIndex.Red;

				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					/// 스테이지1 처음 시작에서는 hfsmplay에서 이미 설정되어있음
					var isOneStageFirstStart = beginnerRoad->type == BeginnerRoadType.Stage1 && isFirstStart;

					f.TryGetSpec<HalfCourtPlayerSpawnSpec>(SpecType.PlayerSpawnSpec, out var spec);

					/// 공격자 수비자 배치, 경기 재개 하기 전에 니트로 full
					f.TryGetSpec<HoverboardSpec>(SpecType.HoverBoard, out var boardSpec);
					var filter = f.Filter<Hoverboard, TeamComponent, Transform3D, PhysicsBody3D>();
					while (filter.NextUnsafe(out var e, out var board, out var team, out var trs, out var body))
					{
						if (false == isOneStageFirstStart)
							spec.AtkSpawnPlayer(f, e, team->TeamIndex, (team->TeamIndex == halfCourt->currentAtkTeam));
						board->Rsc.currentNitro = Ability.AbilityAddValue(f, e, AbilityType.AddNitro, boardSpec.nitro.boosterNitroMax);
					}

					f.Events.VirtualCameraPlay(VirtualCameraType.None, 0);
					f.Events.InitEntityEffect();
					f.Events.GamePlayChangeStateCountdown(f.RuntimeConfig.gameType, true, gamePlaySpec.OffenseConversionDelay);

					if (false == isOneStageFirstStart)
						f.Events.OnScreenFadeUI(FP._0_33, 1, 0, true);
					gamePlay->stateStep = StateStep.Step_1;
				}

				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					f.Signals.OnPlayerBlow(halfCourt->currentAtkTeam == TeamIndex.Red ? TeamIndex.Blue : TeamIndex.Red);

					/// 하프코트 처음 시작 카운트는 입력 락을 시작할때 풀어주는것으로 한다.
					if (false == isFirstStart)
						f.Signals.OnPlayerInputLock(false);
					f.Events.OnPrePlaylHalfCourt(halfCourt->currentSetIndex, halfCourt->currentAtkTeam);

					gamePlay->stateStep = StateStep.Step_2;
				}
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay - FP._0_50)
					{
						AtkBallSpawn(f, halfCourt, halfCourtSpec);

						gamePlay->stateStep = StateStep.Step_End;
					}

				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay)
					{
						if (isFirstStart)
							f.Signals.OnPlayerInputLock(false);

						ChangeGameState(f, GameState.Playing);
					}
				}
			}
			else
			{
				var first = gamePlay->playStateFlags.HasFlag(GamePlayStateFlag.First);
				var normalGame = gamePlay->playStateFlags.HasFlag(GamePlayStateFlag.NormalGame);
				var countTime = (first && normalGame) ? gamePlaySpec.firstCountdown : gamePlaySpec.gamingCountdown;

				/// 카운트 다운 이벤트
				var tempStateElapsedTime = FPMathExtension.Clamp0(gamePlay->stateElapsedTime - f.DeltaTime);
				if (FPMath.FloorToInt(tempStateElapsedTime) != FPMath.FloorToInt(gamePlay->stateElapsedTime))
				{
					var gamePlay = EntityUtil.GetGamePlayComponent(f);
					f.Events.ActionCountdown(PlayerRef.None,
											 (countTime - FPMath.FloorToInt(gamePlay->stateElapsedTime)).AsInt,
											 TeamIndex.None,
											 gamePlay->playStateFlags);
				}

				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					SetActiveInGameUI(f, true);
					f.Events.VirtualCameraPlay(VirtualCameraType.None, 0);

					if (first && normalGame)
						f.Events.VirtualCameraPlay(VirtualCameraType.Countdown, 0);

					f.Events.GamePlayChangeStateCountdown(f.RuntimeConfig.gameType
														, (first && normalGame)
														, countTime);
					f.Events.InitEntityEffect();

					gamePlay->stateStep = StateStep.Step_1;
				}

				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					f.Signals.OnAllPlayerRePosition(0);
					gamePlay->stateStep = StateStep.Step_2;
				}

				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					var _ballRegenEffectTime = countTime - gamePlaySpec.ballRegenTimeOffset - FP._0_33;
					if (_ballRegenEffectTime < gamePlay->stateElapsedTime)
					{
						gamePlay->stateStep = StateStep.Step_3;
					}
				}

				else if (gamePlay->stateStep == StateStep.Step_3)
				{
					var _ballRegenTime = countTime - gamePlaySpec.ballRegenTimeOffset;
					if (_ballRegenTime < gamePlay->stateElapsedTime)
					{
						var tableIndex = 0;
						if (f.Global->lastGoalInfo.scoredTeam == TeamIndex.Red)
							tableIndex = 2;
						else if (f.Global->lastGoalInfo.scoredTeam == TeamIndex.Blue)
							tableIndex = 1;

						f.Signals.OnBallSpawn(tableIndex);
						gamePlay->stateStep = StateStep.Step_4;
					}
				}

				else if (gamePlay->stateStep == StateStep.Step_4)
				{
					if (first && normalGame)
					{
						if (countTime - gamePlaySpec.ballRegenTimeOffset + FP._0_10 < gamePlay->stateElapsedTime)
						{
							f.Events.VirtualCameraPlay(VirtualCameraType.None, 0);
							gamePlay->stateStep = StateStep.Step_End;
						}
					}
					else
						gamePlay->stateStep = StateStep.Step_End;
				}

				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (countTime < gamePlay->stateElapsedTime)
					{
						/// 점프볼
						var ballInfo = EntityUtil.GetFirstBall(f);
						if (ballInfo != null)
							ballInfo.body->Velocity += FPVector3.Up * gamePlaySpec.jumpBallForce;

						ChangeGameState(f, GameState.Playing);
						f.Events.OnJumpBall(ballInfo.entity, ballInfo.transform->Position, ballInfo.spec.shape.SphereRadius);
					}
				}
			}
		}
		public override void PlayState(Frame f)
		{
			/// 시작 상태 초기화
			if (gamePlay->stateStep == StateStep.Step_Enter)
			{
				var filter = f.Filter<Hoverboard, AIData>();
				while (filter.NextUnsafe(out var e, out var board, out var aiData))
				{
					/// 시작할떄 라운드 statics 정보 생성 or 초기화
					f.Signals.OnCreateRoundStatistics(e);
				}

				f.Signals.OnPlayerInputLock(false);

				f.Events.GamePlayChangeStatePlaying(gamePlay->playStateFlags);

				gamePlay->stateStep = StateStep.Step_1;
				gamePlay->stateElapsedTime = 0;
			}
			/// 게임 플레이중
			else if (gamePlay->stateStep == StateStep.Step_1)
			{

			}
		}
		public override void PauseState(Frame f)
		{
			/// 비기너 로드 2단계에 전용 처리
			if (beginnerRoad->type == BeginnerRoadType.Stage1)
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					f.Signals.OnPlayerInputLock(true);
					f.Events.OnScreenFadeUI(FP._0_50, 0, 1, false);
					gamePlay->stateStep = StateStep.Step_1;
				}
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime > FP._0_50)
					{
						var ballFilter = f.Filter<BallComponent, Transform3D, PhysicsBody3D>();
						while (ballFilter.NextUnsafe(out var ballRef, out var ball, out var ballTrs, out var ballBody))
						{
							if (ball->Owner != EntityRef.None)
								f.Signals.OnReleaseBall(new ReleaseInfo() { Ball = ballRef, OldOwner = ball->Owner, ReleaseBy = ReleaseBy.Init, swingAngle = FP._0, swingTime = FP._0 });

							ballTrs->Position = new FPVector3(65, 0, 0);
							ballBody->Velocity = FPVector3.Zero;
							ballBody->AngularVelocity = FPVector3.Zero;
						}


						var boardFilter = f.Filter<Hoverboard, Transform3D, BallOwnerComponent, TeamComponent, AIData, PhysicsBody3D>();
						while (boardFilter.NextUnsafe(out var boardRef, out var board, out var boardTrs, out var boardOwner, out var boardTeam, out var boardAi, out var boardBody))
						{
							if (boardTeam->TeamIndex == TeamIndex.Red)
							{
								boardTrs->Position = new FPVector3(60, 5, 0);
								boardTrs->Rotation = FPQuaternion.Euler(0, 270, 0);
								boardBody->Velocity = FPVector3.Zero;
								boardBody->AngularVelocity = FPVector3.Zero;
							}
							else if (boardTeam->TeamIndex == TeamIndex.Blue)
							{
								boardTrs->Position = new FPVector3(-10, 5, 0);
								boardTrs->Rotation = FPQuaternion.Euler(0, 90, 0);
								boardBody->Velocity = FPVector3.Zero;
								boardBody->AngularVelocity = FPVector3.Zero;
							}
							boardOwner->PickBallState &= ~PickBallState.BeforeStart;
						}
						gamePlay->stateStep = StateStep.Step_2;
					}	
				}
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime > FP._0_75)
					{
						f.Events.OnScreenFadeUI(FP._0_33, 1, 0, true);
						gamePlay->stateStep = StateStep.Step_End;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (gamePlay->stateElapsedTime > FP._1_20)
					{
						f.Signals.OnPlayerInputLock(false);
						gamePlay->stateElapsedTime = 0;
						ChangeGameState(f, GameState.Playing, StateStep.Step_1);
					}
				}
				return;
			}

			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					f.Signals.OnPlayerInputLock(true);
					f.Events.GamePlayChangeStatePause();

					var list = f.ResolveList(halfCourt->setResultList);
					var resultInfo = list[halfCourt->currentSetIndex];
					f.Events.OnFinishAttackHalfCourt(halfCourt->currentSetIndex, halfCourt->currentAtkTeam, resultInfo.firstScore, resultInfo.secondScore);

					gamePlay->stateStep = StateStep.Step_1;
				}
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.pauseDelay)
					{
						gamePlay->stateStep = StateStep.Step_End;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					/// 공격 대기 시간이 지나면 게임 종료 체크로 다음 시퀀스 설정
					FinishAttack(f, gamePlay, halfCourt, halfCourtSpec.setCount);
				}
			}

			else
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					f.Events.GamePlayChangeStatePause();

					gamePlay->stateStep = StateStep.Step_End;
					gamePlay->stateElapsedTime = 0;
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{

				}
			}
		}
		public override void FinishState(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				/// 종료 처리
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					if (halfCourt->isFlagFadeUI)
					{
						halfCourt->isFlagFadeUI = false;
						f.Events.OnScreenFadeUI(FP._0_50, 1, 0, true);
					}

					f.Signals.OnPlayerInputLock(true);

					/// 라운드 승자 팀 결정
					QuantumUtility.GetTeamScore(f, out var redScore, out var blueScore);
					var winTeam = (redScore > blueScore) ? TeamIndex.Red : (redScore < blueScore) ? TeamIndex.Blue : TeamIndex.None;
					var redMvpPoint = GetRoundTeamMvpPoint(f, TeamIndex.Red, gameRound->roundNumber);
					var blueMvpPoint = GetRoundTeamMvpPoint(f, TeamIndex.Blue, gameRound->roundNumber);

					if (winTeam == TeamIndex.Red)
						gameRound->redWinRound++;
					else if (winTeam == TeamIndex.Blue)
						gameRound->blueWinRound++;

					QuantumUtility.SaveRoundResult(f, gameRound, 0, winTeam);
					f.Events.OnFinishRound(f.RuntimeConfig.gameRound
											, gameRound->roundNumber + 1, gameRound->redWinRound, gameRound->blueWinRound
											, winTeam, 0
											, redScore, blueScore
											, redMvpPoint, blueMvpPoint
											, gameRound->firstGoalTime, true);

					/// 해당 라운드 mvp플레이어 설정
					SetRoundMvpPlayer(f, winTeam, gameRound->roundNumber);

					gamePlay->stateStep = StateStep.Step_2;
					f.Events.GamePlayChangeStateFinish(f.RuntimeConfig.gameType, winTeam, gamePlay->elapsedTime, redScore, blueScore, halfCourt->abstentioTeam);

					if (beginnerRoad->type == BeginnerRoadType.Stage1)
						f.Events.SetTutorialTimerUI(true, true, 0, false, false, false, true, false);
				}

				/// 정상적인 게임 종료 상태
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime > gamePlaySpec.gameFinishDelay)
					{
						f.Events.GameEndUnityPlay();
						gamePlay->stateStep = StateStep.Step_End;
					}
				}

				/// 정상적인 게임 종료 상태
				else if (gamePlay->stateStep == StateStep.Step_End)
				{

				}
			}

			else
			{
				/// 종료 처리
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					f.Signals.OnPlayerInputLock(true);

					/// 라운드 승자 팀 결정
					var winType = 0;
					
					QuantumUtility.GetTeamScore(f, out var redScore, out var blueScore);

					var winTeam = (redScore > blueScore) ? TeamIndex.Red : (redScore < blueScore) ? TeamIndex.Blue : TeamIndex.None;
					var redMvpPoint = GetRoundTeamMvpPoint(f, TeamIndex.Red, gameRound->roundNumber);
					var blueMvpPoint = GetRoundTeamMvpPoint(f, TeamIndex.Blue, gameRound->roundNumber);

					if (winTeam == TeamIndex.Red)
						gameRound->redWinRound++;
					else if (winTeam == TeamIndex.Blue)
						gameRound->blueWinRound++;

					QuantumUtility.SaveRoundResult(f, gameRound, winType, winTeam);

					f.Events.OnFinishRound(f.RuntimeConfig.gameRound
											, gameRound->roundNumber + 1, gameRound->redWinRound, gameRound->blueWinRound
											, winTeam, winType
											, redScore, blueScore
											, redMvpPoint, blueMvpPoint
											, gameRound->firstGoalTime, true);

					/// 해당 라운드 mvp플레이어 설정
					SetRoundMvpPlayer(f, winTeam, gameRound->roundNumber);

					gamePlay->stateStep = StateStep.Step_1;
					gamePlay->stateElapsedTime = 0;
					f.Events.GamePlayChangeStateFinish(f.RuntimeConfig.gameType, winTeam, gamePlay->elapsedTime, redScore, blueScore, TeamIndex.None);
				}

				/// 정상적인 게임 종료 상태
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime > FP._1)
					{
						f.Events.GameEndUnityPlay();
						gamePlay->stateStep = StateStep.Step_End;
					}
				}

				/// 정상적인 게임 종료 상태
				else if (gamePlay->stateStep == StateStep.Step_End)
				{

				}
			}
			
		}

		public override void GoalReplayWait(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				base.GoalReplayWait(f);
			}

			else
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					gamePlay->stateStep = StateStep.Step_1;
					gamePlay->stateElapsedTime = 0;
				}
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.replayStartDelay - gamePlaySpec.replayStartFadeTime)
					{
						f.Signals.OnPlayerInputLock(true);
						f.Events.OnScreenFadeUI(gamePlaySpec.replayStartFadeTime, 0, 1, false);
						gamePlay->stateStep = StateStep.Step_2;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.replayStartDelay)
					{
						gamePlay->stateStep = StateStep.Step_End;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					ChangeGameState(f, GameState.GoalReplaying);
				}
			}
		}
		public override void GoalReplaying(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					gamePlay->stateStep = StateStep.Step_1;
					f.Global->skipPlayerCount = 0;
					f.Events.GoalReplayStart(1, gamePlaySpec.replayTime, f.Global->playerUserCount, f.Global->lastGoalInfo);
				}

				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if ((f.Global->skipPlayerCount > 0 && f.Global->skipPlayerCount >= f.Global->playerUserCount) || gamePlay->stateElapsedTime >= gamePlaySpec.replayTime)
					{
						gamePlay->stateElapsedTime = 0;
						halfCourt->isFlagFadeUI = true;
						gamePlay->stateStep = StateStep.Step_End;

						f.Events.OnScreenFadeUI(FP._0_50, 0, 1, false);
					}
				}

				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (gamePlay->stateElapsedTime >= FP._0_75)
					{
						/// 골리플레이 중에 탈주처리로 인한 승리 여부 체크
						if (halfCourt->abstentioTeam == TeamIndex.None)
							FinishAttack(f, gamePlay, halfCourt, halfCourtSpec.setCount);
						else
							ChangeGameState(f, GameState.Finish);

						f.Events.GoalReplayEnd();
						f.Events.RefreshPlayerStatistics(EntityRef.None);
					}
				}
			}

			else
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					gamePlay->stateStep = StateStep.Step_End;
					gamePlay->stateElapsedTime = 0;
					f.Global->skipPlayerCount = 0;
					f.Events.GoalReplayStart(1, gamePlaySpec.replayTime, f.Global->userCount, f.Global->lastGoalInfo);
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if ((f.Global->skipPlayerCount > 0 && f.Global->skipPlayerCount >= f.Global->userCount) || gamePlay->stateElapsedTime >= gamePlaySpec.replayTime)
					{
						/// 골든볼 경기일경우, 득점시 바로 경기 종료를 한다.
						if (gamePlay->playStateFlags.HasFlag(GamePlayStateFlag.GoldenBallGame))
							ChangeGameState(f, GameState.Finish);
						else
							ChangeGameState(f, GameState.OffenseConversion);

						f.Events.GoalReplayEnd();
						f.Events.RefreshPlayerStatistics(EntityRef.None);
					}
				}
			}
			
		}
		public override void GoldenBallEnterWait(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				base.GoldenBallEnterWait(f);
			}

			else
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					gamePlay->stateStep = StateStep.Step_1;
					gamePlay->stateElapsedTime = 0;

					f.Global->lastGoalInfo.scoredTeam = TeamIndex.None;

					f.Signals.OnPlayerInputLock(true);
				}
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.goldenballEnterDelay)
					{
						f.Events.OnScreenFadeUI(1, 0, 1, true);

						///볼 디스폰
						QuantumUtility.DespawnBall(f, EntityRef.None);

						gamePlay->stateStep = StateStep.Step_2;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.goldenballEnterDelay + 1)
					{
						///플레이어 재배치
						f.Signals.OnAllPlayerRePosition(0);

						gamePlay->stateStep = StateStep.Step_3;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_3)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.goldenballEnterDelay + 2)
					{
						f.Events.OnNotiUI(EntityRef.None, "ui_0013", 3, false, 0);
						gamePlay->stateStep = StateStep.Step_End;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.goldenballEnterDelay + 3)
					{
						ChangeGameState(f, GameState.Countdown);
					}
				}
			}
			
		}

		public override void OffenseConversion(Frame f)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					f.Signals.OnPlayerInputLock(true);

					if (false == halfCourt->isFlagFadeUI)
						f.Events.OnScreenFadeUI(FP._0_50, 0, 1, false);

					halfCourt->isFlagFadeUI = false;

					gamePlay->stateStep = StateStep.Step_1;
				}

				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime >= FP._0_50)
					{
						/// 공격 시간 초기화
						gamePlay->elapsedTime = FP._0;

						f.TryGetSpec<HalfCourtPlayerSpawnSpec>(SpecType.PlayerSpawnSpec, out var spec);

						/// 공격자 수비자 배치
						var filter = f.Filter<Hoverboard, TeamComponent, Transform3D, PhysicsBody3D>();
						while (filter.NextUnsafe(out var e, out var board, out var team, out var trs, out var body))
						{
							spec.AtkSpawnPlayer(f, e, team->TeamIndex, (team->TeamIndex == halfCourt->currentAtkTeam));
						}

						f.Events.GamePlayChangeStateOffenseConversion();

						gamePlay->stateStep = StateStep.Step_2;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_2)
				{
					if (gamePlay->stateElapsedTime >= FP._1)
					{
						f.Events.OnScreenFadeUI(FP._0_50, 1, 0, true);
						gamePlay->stateStep = StateStep.Step_3;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_3)
				{
					if (gamePlay->stateElapsedTime >= FP._1 + FP._0_50)
					{
						gamePlay->stateElapsedTime = 0;

						/// 경기 재개 하기 전에 니트로 full
						f.TryGetSpec<HoverboardSpec>(SpecType.HoverBoard, out var boardSpec);
						var filter = f.Filter<Hoverboard>();
						while (filter.NextUnsafe(out EntityRef e, out Hoverboard* board))
						{
							board->Rsc.currentNitro = Ability.AbilityAddValue(f, e, AbilityType.AddNitro, boardSpec.nitro.boosterNitroMax);
						}

						f.Signals.OnPlayerInputLock(false);
						f.Signals.OnPlayerBlow(halfCourt->currentAtkTeam == TeamIndex.Red ? TeamIndex.Blue : TeamIndex.Red);
						f.Events.OnPrePlaylHalfCourt(halfCourt->currentSetIndex, halfCourt->currentAtkTeam);

						gamePlay->stateStep = StateStep.Step_4;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_4 || gamePlay->stateStep == StateStep.Step_End)
				{
					/// 카운트 다운 이벤트
					OnCountDownUI(f, gamePlaySpec.OffenseConversionDelay, gamePlay->stateElapsedTime, halfCourt->currentAtkTeam);

					if (gamePlay->stateStep == StateStep.Step_4)
					{
						if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay - FP._0_50)
						{
							AtkBallSpawn(f, halfCourt, halfCourtSpec);

							gamePlay->stateStep = StateStep.Step_End;
						}
					}
					else if (gamePlay->stateStep == StateStep.Step_End)
					{
						if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay)
						{
							ChangeGameState(f, GameState.Playing);
						}
					}
				}
			}

			else
			{
				if (gamePlay->stateStep == StateStep.Step_Enter)
				{
					gamePlay->stateStep = StateStep.Step_1;
					gamePlay->stateElapsedTime = 0;
					f.Events.GamePlayChangeStateOffenseConversion();
				}
				else if (gamePlay->stateStep == StateStep.Step_1)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay - FP._0_33)
					{
						f.Events.OnBallSpawnEffect(true, GetBallRegenPos(f));
						gamePlay->stateStep = StateStep.Step_End;
					}
				}
				else if (gamePlay->stateStep == StateStep.Step_End)
				{
					if (gamePlay->stateElapsedTime >= gamePlaySpec.OffenseConversionDelay)
					{
						/// 경기 재개 하기 전에 니트로 초기화
						f.TryGetSpec<HoverboardSpec>(SpecType.HoverBoard, out var boardSpec);
						var es = f.Filter<Hoverboard>();
						while (es.NextUnsafe(out EntityRef e, out Hoverboard* board))
						{
							if (f.Global->cheatOption.unlimitNitro)
								board->Rsc.currentNitro = Ability.AbilityAddValue(f, e, AbilityType.AddNitro, boardSpec.nitro.boosterNitroMax);
							else
								board->Rsc.currentNitro = boardSpec.nitro.boosterNitroInit;
						}

						/// 볼 스폰
						int tableIndex = 0;
						if (f.Global->lastGoalInfo.scoredTeam == TeamIndex.Red)
							tableIndex = 2;
						else if (f.Global->lastGoalInfo.scoredTeam == TeamIndex.Blue)
							tableIndex = 1;

						f.Signals.OnBallSpawn(tableIndex);

						/// 경기 플레이 전환
						ChangeGameState(f, GameState.Playing);
					}
				}
			}
			
		}
		#endregion

		#region[UpdateCommand]
		public override void UpdateTestCommand(Frame f, DeterministicCommand command)
		{
			base.UpdateTestCommand(f, command);
		}
		public override void UpdateGamePlayCommand(Frame f, PlayerRef playerRef, DeterministicCommand command)
		{
			if (null == command as GamePlayCommand)
				return;

			var gamePlayCommand = command as GamePlayCommand;

			if (gamePlayCommand.type == GamePlayCommandType.ReStartGamePlay)
			{
				OnReStartGamePlay(f, playerRef, gamePlayCommand);
			}

			else if (gamePlayCommand.type == GamePlayCommandType.StopGamePlay)
			{
				if (false == GetComponent(f, out var _gamePlayRef, out var _gamePlay, out var _gameRound, out var _beginnerRoad, out var _halfCourt))
					return;

				_gamePlay->currentState = GameState.Finish;
				_gamePlay->stateStep = StateStep.Step_End;
				_gamePlay->playStateFlags = 0;

				QuantumUtility.AllDespawnBall(f);
				f.Signals.OnPlayerInputLock(true);

				f.Events.OnGamePlayCommand(playerRef, GamePlayCommandType.StopGamePlay);
			}
		}

		public override void OnReStartGamePlay(Frame f, PlayerRef playerRef, GamePlayCommand command)
		{
			if (false == GetComponent(f, out var _gamePlayRef, out var _gamePlay, out var _gameRound, out var _beginnerRoad, out var _halfCourt))
				return;

			var stage = command.intDic[IntVariable.Index0];
			var state = (GameState)command.intDic[IntVariable.Index2];

			InitGamePlayComponent(f, _gamePlay, state);
			InitGameRoundComponent(f, _gameRound);
			if (null != _halfCourt)
				InitHalfCourtComponent(f, _halfCourt);
			SetBeginnerRoadStage(f, stage);

			/// 호버보드 Statistics 클리어
			f.Signals.OnAllClearStatistics();

			QuantumUtility.InitlastGoalInfo(f);
			QuantumUtility.SetTeamScore(f, FP._0, FP._0);

			QuantumUtility.AllDespawnBall(f);
			QuantumUtility.AllDespawnObject(f);

			/// PlayerReady 부터 재시작하면 삭제하고 다시 재생성 한다.
			if (state == GameState.PlayerReady)
				QuantumUtility.AllDespawnAiPlayer(f);

			/// ui 및 mouseLock 초기화
			if (state != GameState.PlayerReady)
				f.Events.OnPlayerReadyStart();

			/// PlayerReady, Init 아닐경우 플레이어 오브젝트 활성화
			if (state != GameState.PlayerReady && state != GameState.Init)
				f.Events.OnSetActiveAllPlayer(true);

			f.SystemEnable<GoalPostSystem>();
			f.SystemEnable<RefreeSystem>();

			f.Events.RefreshPlayerStatistics(EntityRef.None);
			f.Events.OnGamePlayCommand(playerRef, GamePlayCommandType.ReStartGamePlay);
			
		}
		#endregion

		#region[Signal]
		public override void OnReStartGamePlay(Frame f, Int32 index0, Int32 index1)
		{
			base.OnReStartGamePlay(f, index0, index1);

			QuantumUtility.AllDespawnAiPlayer(f);
			QuantumUtility.AllDespawnBall(f);
			QuantumUtility.AllDespawnObject(f);

			if (MapModeType.HalfCourt != f.RuntimeConfig.mapModeType)
				SetBeginnerRoadStage(f, index0);
		}
		public override void OnSwingDunkStart(Frame f)
		{
			base.OnSwingDunkStart(f);
		}
		public override void OnContactUserPlayer(Frame f, ContactPlayerType contactType, PlayerRef playerRef, TeamIndex team, QBoolean isCreatePlayer, GameState gameState)
		{
			EntityInitSystem.CreatePlayer(f, false, false, playerRef, team, gameState, out var entityRef);

			if (f.Unsafe.TryGetPointer<EntityTagComponent>(entityRef, out var tagComponent))
			{
				tagComponent->Tag = EntityTag.Player;
				tagComponent->index = 0;
			}
			else
			{
				var newTagComponent = new EntityTagComponent();
				newTagComponent.Tag = EntityTag.Player;
				newTagComponent.index = 0;
				f.Add(entityRef, newTagComponent);
			}
		}
		public override void OnDisContactUserPlayer(Frame f, PlayerRef playerRef, EntityRef playerEntity, TeamIndex team)
		{

		}
		public override void OnBallCollisionWithFloor(Frame f, FPVector3 ballPos, FPVector3 colPos, FPVector3 colNormal)
		{
			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				/// 시간 종료되면 바로 공격전환이 이루어진다.
			}

			else
			{
				base.OnBallCollisionWithFloor(f, ballPos, colPos, colNormal);
			}
		}
		public override void OnGoal(Frame f, GoalInfo goalInfo)
		{
			if (false == GetComponent(f, out var _gamePlayRef, out var _gamePlay, out var _gameRound, out var _beginnerRoad, out var _halfCourt))
				return;
			if (false == f.TryGetSpec<BeginnerRoadSpec>(SpecType.BeginnerRoadSpec, out var _spec))
				return;

			/// 비기너 로드 2단계 전용 처리
			if (_beginnerRoad->type == BeginnerRoadType.Stage1)
			{
				/// 골 플레이어 팀
				var goalPlayerTeam = (false == f.Has<TeamComponent>(goalInfo.Shooter)) ? TeamIndex.None : f.Get<TeamComponent>(goalInfo.Shooter).TeamIndex;

				/// 골 득점 처리
				var teamScore = QuantumUtility.AddTeamScore(f, _halfCourt->currentAtkTeam, FP._1);

				/// 하프코트 세트 득점 처리
				var list = f.ResolveList(_halfCourt->setResultList);
				for (int i = 0; i < list.Count; i++)
				{
					var result = list.GetPointer(i);
					result->firstScore = 1;
					result->secondScore = 0;
				}

				/// 골이 들어가면 골 디스폰
				f.TryGetSpec<BallSpec>(SpecType.Ball, out var ballSpec);
				QuantumUtility.DelayDespawnBall(f, goalInfo.Ball, ballSpec.goalDelayDespawnTime);

				f.Events.OnGoalEffect(goalInfo);
				ChangeGameState(f, GameState.Finish);
				return;
			}

			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				/// 골 플레이어 팀
				var goalPlayerTeam = (false == f.Has<TeamComponent>(goalInfo.Shooter)) ? TeamIndex.None : f.Get<TeamComponent>(goalInfo.Shooter).TeamIndex;

				/// 골 득점 처리
				var teamScore = QuantumUtility.AddTeamScore(f, _halfCourt->currentAtkTeam, FP._1);

				/// 득점 정보를 저장
				f.Global->lastGoalInfo.goalPlayer = goalInfo.Shooter;
				f.Global->lastGoalInfo.scoredTeam = _halfCourt->currentAtkTeam;
				f.Global->lastGoalInfo.goalFrameNum = f.Number;
				f.Global->lastGoalInfo.isDunk = goalInfo.isDunk;
				f.Global->lastGoalInfo.isOwnGoal = (goalPlayerTeam != _halfCourt->currentAtkTeam);
				f.Global->lastGoalInfo.elapsedTime = _gamePlay->elapsedTime;

				/// 하프코트 세트 득점 처리
				var list = f.ResolveList(_halfCourt->setResultList);
				var result = list.GetPointer(_halfCourt->currentSetIndex);
				if (_halfCourt->currentAtkTeam == _halfCourt->firstAtkTeam)
					result->firstScore += 1;
				else if (_halfCourt->currentAtkTeam == _halfCourt->secondAtkTeam)
					result->secondScore += 1;

				/// 골이 들어가면 골 디스폰
				f.TryGetSpec<BallSpec>(SpecType.Ball, out var ballSpec);
				QuantumUtility.DelayDespawnBall(f, goalInfo.Ball, ballSpec.goalDelayDespawnTime);

				/// 득점 ui, effect 이벤트 실행
				QuantumUtility.GetTeamScore(f, out var redScore, out var blueScore);
				f.Events.OnGoalAchievenent(goalInfo.Shooter, goalInfo.Assist, FP._1, redScore, blueScore, goalInfo, f.Global->lastGoalInfo, _gamePlay->playStateFlags, _gamePlay->elapsedTime);
				f.Events.OnGoalEffect(goalInfo);
				f.Events.GameScoreChanged(_halfCourt->currentAtkTeam, teamScore, goalInfo.Shooter, goalInfo.Assist, f.Global->lastGoalInfo);

				var firstScore = _halfCourt->firstAtkTeam == TeamIndex.Red ? redScore : blueScore;
				var secondScore = _halfCourt->secondAtkTeam == TeamIndex.Red ? redScore : blueScore;
				f.Events.OnGoalHalfCourt(goalInfo.Shooter, _halfCourt->currentSetIndex, _halfCourt->currentAtkTeam, firstScore.AsInt, secondScore.AsInt);

				/// 하프코트 타입일때는 골 리플레이를 하지 않는다.
				ChangeGameState(f, GameState.Pause);
			}

			else
			{
				base.OnGoal(f, goalInfo);

				/// 레드팀이 득점했을 경우 확률에 따라 ai플레이어 stamp 실행
				if (f.Global->lastGoalInfo.scoredTeam == TeamIndex.Red)
				{
					var filter = f.Filter<Hoverboard, PlayerInput>();
					while (filter.Next(out var e, out var board, out var input))
					{
						if (PlayerRef.None != input.PlayerRef)
							continue;

						if (f.RNG->Next(0, 100) <= _spec.goalStampRatio)
						{
							var stampIndex = 200 + f.RNG->NextInclusive(0, 3);
							f.Events.PlayerChatData(e, stampIndex);
						}
					}
				}
			}
		}
		#endregion

		void InitHalfCourtComponent(Frame f, HalfCourtComponent* halfCourt)
		{
			halfCourt->currentSetIndex = 0;
			halfCourt->currentAtkTeam = TeamIndex.Red;
			halfCourt->abstentioTeam = TeamIndex.None;
			halfCourt->isFlagFadeUI = false;

			var list = f.ResolveList(halfCourt->setResultList);
			for (int i = 0; i < list.Count; i++)
			{
				var score = list.GetPointer(i);
				score->firstScore = 0;
				score->secondScore = 0;
			}
		}
		void SetBeginnerRoadStage(Frame f, Int32 index0)
		{
			if (false == GetComponent(f, out var _gamePlayRef, out var _gamePlay, out var _gameRound, out var _beginnerRoad, out var _halfCourt))
				return;
			if (false == f.TryGetSpec<BeginnerRoadSpec>(SpecType.BeginnerRoadSpec, out var spec))
				return;

			f.RuntimeConfig.gameSubType = index0;

			/// 스테이지 설정
			_beginnerRoad->type = (BeginnerRoadType)index0;

			var stageInfo = spec.GetStage((BeginnerRoadType)index0);
			f.RuntimeConfig.gameOptConfig.intList[0] = index0;
			f.RuntimeConfig.isActiveGoalReplay = stageInfo.isGoalReplay;
			f.RuntimeConfig.goldenBallCnt = stageInfo.goldenBallGameCount;
		}
		
		void SetActiveInGameUI(Frame f, bool isActive)
		{
			setUI.isSetNitroUI = true;
			setUI.isOnNitroUI = isActive;

			setUI.isSetAutoGrabUI = true;
			setUI.isOnAutoGrabUI = isActive;

			setUI.isSetOtherPlayerUI = true;
			setUI.isOnOtherPlayerUI = isActive;

			setUI.isSetNaviUI = true;
			setUI.isOnNaviUI = isActive;

			f.Events.OnSetInGameUI(setUI);
		}

		public bool GetComponent(Frame f, out EntityRef gamePlayRef, out GamePlayComponent* gamePlay, out GameRoundComponent* round, out BeginnerRoadComponent* beginnerRoad, out HalfCourtComponent* halfCourt)
		{
			gamePlayRef = EntityRef.None;
			gamePlay = null;
			round = null;
			beginnerRoad = null;
			halfCourt = null;

			if (MapModeType.HalfCourt == f.RuntimeConfig.mapModeType)
			{
				var filter = f.Filter<GamePlayComponent, GameRoundComponent, BeginnerRoadComponent, HalfCourtComponent>();
				while (filter.NextUnsafe(out gamePlayRef, out gamePlay, out round, out beginnerRoad, out halfCourt))
				{
					break;
				}
			}
			else
			{
				var filter = f.Filter<GamePlayComponent, GameRoundComponent, BeginnerRoadComponent>();
				while (filter.NextUnsafe(out gamePlayRef, out gamePlay, out round, out beginnerRoad))
				{
					break;
				}
			}
			
			return (EntityRef.None != gamePlayRef);
		}

		#region[하프코트]
		/// <summary>
		/// 공격종료 처리
		/// </summary>
		void FinishAttack(Frame f, GamePlayComponent* _gamePlay, HalfCourtComponent* _halfCourt, int _setMaxCount)
		{
			var list = f.ResolveList(_halfCourt->setResultList);
			var resultInfo = list[_halfCourt->currentSetIndex];

			f.Events.OnUpdateHalfCourt(_halfCourt->currentSetIndex, _halfCourt->currentAtkTeam, resultInfo.firstScore, resultInfo.secondScore);

			/// 하프코트 종료 여부 체크
			if (IsFinishHalfCourt(f, _halfCourt))
			{
				ChangeGameState(f, GameState.Finish);
			}

			/// 선공에서 후공으로 변경
			else if (_halfCourt->currentAtkTeam == _halfCourt->firstAtkTeam)
			{
				ChangeAtkTeam(f, gamePlay, _halfCourt);
			}

			/// 다음 세트로 변경
			else if (_halfCourt->currentAtkTeam == _halfCourt->secondAtkTeam && _halfCourt->currentSetIndex + 1 < _setMaxCount)
			{
				_halfCourt->currentSetIndex += 1;
				ChangeAtkTeam(f, gamePlay, _halfCourt);
			}

			/// 모든 게임 진행되어서 게임 종료
			else
			{
				ChangeGameState(f, GameState.Finish);
			}
		}

		/// <summary>
		/// 공격이 끝나고 하프코트경기가 종료되어야 하는지 여부 체크
		/// </summary>
		bool IsFinishHalfCourt(Frame f, HalfCourtComponent* _halfCourt)
		{
			var list = f.ResolveList(_halfCourt->setResultList);
			var firstScore = 0;
			var firstAtkChance = 0;
			var secondScore = 0;
			var secondAtkChance = 0;

			for (int i = 0; i < list.Count; i++)
			{
				/// 지난 세트
				if (i < _halfCourt->currentSetIndex)
				{
					firstScore += list[i].firstScore;
					secondScore += list[i].secondScore;
				}
				/// 현재 세트, 선공팀 공격이 끝났을 경우 선공팀은 공격점수 후공팀은 횟수를 올려준다. 후공팀 공격이 끝났을 경우 둘다 공격점수를 올려준다.
				else if (i == _halfCourt->currentSetIndex)
				{
					firstScore += list[i].firstScore;

					if (_halfCourt->currentAtkTeam == _halfCourt->firstAtkTeam)
						secondAtkChance += 1;
					else
						secondScore += list[i].secondScore;
				}
				/// 진행이 아직 안된 세트
				else
				{
					firstAtkChance += 1;
					secondAtkChance += 1;
				}
			}

			/// 남은 세트 결과와 상관없이 이긴팀이 정해졌다면 
			return (firstScore > secondScore + secondAtkChance) || (secondScore > firstScore + firstAtkChance);
		}

		/// <summary>
		/// 공격팀 전환
		/// </summary>
		void ChangeAtkTeam(Frame f, GamePlayComponent* _gamePlay, HalfCourtComponent* _halfCourt)
		{
			/// 공격 팀 변경
			_halfCourt->currentAtkTeam = (_halfCourt->currentAtkTeam == _halfCourt->firstAtkTeam) ?
													_halfCourt->secondAtkTeam : _halfCourt->firstAtkTeam;

			/// 골대 팀 변경
			ChangeGoalPostTeam(f, _halfCourt);

			/// 공수 전환 상태로 변경
			ChangeGameState(f, GameState.OffenseConversion);
		}

		/// <summary>
		/// 골대 팀 변경
		/// </summary>
		void ChangeGoalPostTeam(Frame f, HalfCourtComponent* _halfCourt)
		{
			var changeGpTeam = _halfCourt->currentAtkTeam == TeamIndex.Red ? TeamIndex.Blue : TeamIndex.Red;

			var gpFilter = f.Filter<GoalPostComponent, TeamComponent>();
			while (gpFilter.NextUnsafe(out var gpRef, out var gp, out var gpTeam))
			{
				gpTeam->TeamIndex = changeGpTeam;
				break;
			}

			var dfzFilter = f.Filter<DefenseZoneComponent>();
			while (dfzFilter.NextUnsafe(out var dfzRef, out var dfz))
			{
				var list = f.ResolveList(dfz->dfzParmList);
				var dfzInfo = list.GetPointer(0);
				dfzInfo->Team = changeGpTeam;

				break;
			}
		}

		/// <summary>
		/// 공격 시작시 볼스폰
		/// </summary>
		bool AtkBallSpawn(Frame f, HalfCourtComponent* _halfCourt, HalfCourtSpec _spec)
		{
			f.Signals.OnBallSpawn(0);

			var ballRef = QuantumUtility.GetFirstBall(f);

			/// 공격자 위치 뒤로 볼 스폰위치 조정
			if (EntityRef.None != ballRef
					&& f.Unsafe.TryGetPointer<Transform3D>(ballRef, out var ballTrs)
					&& GetAtkPlayer(f, _halfCourt, out var atkRef, out var atkTrs))
			{
				ballTrs->Position = atkTrs->Position + (atkTrs->Back * _spec.ballSpawnBackPos);
			}

			return (EntityRef.None != ballRef);
		}

		/// 공격자 정보 가져오기
		bool GetAtkPlayer(Frame _f, HalfCourtComponent* _halfCourt, out EntityRef _playerRef, out Transform3D* _trs)
		{
			var filter = _f.Filter<Hoverboard, TeamComponent, Transform3D>();
			while (filter.NextUnsafe(out _playerRef, out var _board, out var team, out _trs))
			{
				if (team->TeamIndex != _halfCourt->currentAtkTeam)
					continue;
				return true;
			}

			return false;
		}
		#endregion

	}
}
