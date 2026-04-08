using Photon.Deterministic;
using System;
using System.Collections.Generic;
namespace Quantum
{
	public unsafe partial class DribbleSystem : SystemMainThread, ISignalOnOwnerBallCollisionEnterWithFloor
	{
		private List<IDribbleSystemInternal> internalSystems;

		PlayerEntityInfo pInfo = new PlayerEntityInfo();
		List<BallEntityInfo> bInfoList = new List<BallEntityInfo>();

		BallEntityInfo bInfo = new BallEntityInfo();

		public DribbleSystem()
		{
			internalSystems = new List<IDribbleSystemInternal>();

			internalSystems.Add(new SwingUpdater());
			internalSystems.Add(new AutoGrabUpdater());			
			internalSystems.Add(new ReleaseUpdater());
			internalSystems.Add(new JointUpdater());
			internalSystems.Add(new CollisionProcessor());
		}

		public override void Update (Frame f)
		{
			var all = f.Filter<DribbleComponent, BallOwnerComponent>();
			//f.Filter<BallComponent>().Next(out var ballEntity, out var ballComponent);

			while (all.NextUnsafe(out var playerRef, out var Drb, out var Bown))
			{
				pInfo.SetFromFrame(f, playerRef);
				EntityUtil.GetBallList_Near(f, pInfo.transform->Position, ref bInfoList);

				/// 자동 스윙슛 업데이트 처리
				UpdateAutoSwingRelase(f, pInfo);

				/// 인풋 상태 업데이트
				UpdateDribbleInputChange(f, pInfo);

				/// lockShieldTime 업데이트
				pInfo.playerFunction->lockShieldTime = FPMathExtension.Clamp0(pInfo.playerFunction->lockShieldTime - f.DeltaTime);

				if (pInfo.dribble->LeftPickCoolTime > 0)
					pInfo.owner->PickBallState |= PickBallState.DribbleCoolTime;
				else
					pInfo.owner->PickBallState &= ~PickBallState.DribbleCoolTime;

				UpdateShield(f, pInfo);

				for (int i = 0; i < internalSystems.Count; i++)
					internalSystems[i].Update(f, pInfo, bInfoList);

				pInfo.dribble->LeftPickCoolTime = FPMath.Clamp(pInfo.dribble->LeftPickCoolTime - f.DeltaTime, 0, FP.UseableMax);

				/// 볼을 소유하고 있을시 플레이어와의 최대 거리 체크
				if(Bown->Ball != EntityRef.None)
				{
					var ballRef = Bown->Ball;
					var ballTrs = f.Get<Transform3D>(ballRef);
					var distPTob = FPVector3.DistanceSquared(pInfo.transform->Position, ballTrs.Position);

					var dribbleLength = Ability.AbilityMultipleValue(f, pInfo.entity, *pInfo.aiData, *pInfo.input, AbilityType.ChainLength, pInfo.spec.dribbleSetting.DribbleLength.Z);
					if (distPTob >= dribbleLength * dribbleLength)
					{
						f.Signals.OnReleaseBall(new ReleaseInfo() { Ball = ballRef, OldOwner = pInfo.entity, ReleaseBy = ReleaseBy.Distance, swingAngle = FP._0, swingTime = FP._0 });
						var ballBody = f.Unsafe.GetPointer<PhysicsBody3D>(ballRef);
						ballBody->Velocity = (ballBody->Velocity).Normalized * pInfo.spec.dribbleSetting.releaseForceByDribbleMaxLength;
					}
				}
			}

		}

		public void OnOwnerBallCollisionEnterWithFloor(Frame f, EntityRef ballEntity)
		{
			var all = f.Filter<DribbleComponent, BallOwnerComponent>();

			while (all.NextUnsafe(out var playerRef, out var Drb, out var Bown))
			{
				pInfo.SetFromFrame(f, playerRef);

				bInfo.SetFromFrame(f, ballEntity);
				for (int i = 0; i < internalSystems.Count; i++)
					internalSystems[i].OnOwnerBallCollisionEnterWithFloor(f, pInfo, bInfo);
			}
		}

		static bool TryGetSystemInfo(Frame f, EntityRef player, EntityRef ball, ref PlayerEntityInfo dInfo, ref BallEntityInfo bInfo )
		{
			return dInfo.SetFromFrame(f, player) && bInfo.SetFromFrame(f, ball);
		}

		public static bool GetSwingInput(Frame f, PlayerEntityInfo pInfo)
		{
			return (pInfo.input->Swing 
				|| pInfo.input->rightSwing 
				|| pInfo.input->leftSwing);
		}

		void UpdateDribbleInputChange(Frame f, PlayerEntityInfo pInfo)
		{
			pInfo.dribble->Swing.IsInputStateFDswing = pInfo.dribble->Swing.IsInputStateFDswing.GetInputState(pInfo.input->Swing);

			pInfo.dribble->Swing.IsInputStateRswing = pInfo.dribble->Swing.IsInputStateRswing.GetInputState(pInfo.input->rightSwing);

			pInfo.dribble->Swing.IsInputStateLswing = pInfo.dribble->Swing.IsInputStateLswing.GetInputState(pInfo.input->leftSwing);

			pInfo.dribble->Release.releaseInputState = pInfo.dribble->Release.releaseInputState.GetInputState(pInfo.input->Release);
		}

		/// <summary>
		/// 쉴드 기능 업데이트
		/// 쿨타임 기능 비활성화, shieldUseGauge 값 아래에서는 쉴드를 활성화 할수 없다.
		/// </summary>
		void UpdateShield(Frame f, PlayerEntityInfo _pInfo)
		{
			/// 쉴드 상태가 아닐떄
			if (false == _pInfo.owner->PickBallState.HasFlag(PickBallState.Punching))
			{
				/// 쉴드키 입력시, 쉴드를 사용할수 있는 상태이면 쉴드 실행
				if (_pInfo.dribble->Release.releaseInputState == InputState.Press
					&& pInfo.playerFunction->lockShieldTime == FP._0
					&& _pInfo.playerFunction->playerFunctionFlag.HasFlag(PlayerFunctionFlag.Punching)
					&& _pInfo.board->Rsc.state.isStun == false
					&& _pInfo.dribble->shield.guage > _pInfo.spec.collisionSetting.shieldUseGauge
					&& false == _pInfo.owner->HasBall())
				{
					_pInfo.owner->PickBallState |= PickBallState.Punching;
				}
				else
				{
					var maxGauge = _pInfo.spec.collisionSetting.shieldMaxGauge;
					var recovery = _pInfo.spec.collisionSetting.ShieldRecovery * f.DeltaTime;

					/// 게이지 회복
					if (_pInfo.dribble->ReleaseInputNone())
						_pInfo.dribble->shield.guage = FPMath.Clamp(_pInfo.dribble->shield.guage + recovery, FP._0, maxGauge);
					/// 릴리즈키 입력시 쉴드게이지 소모
					else
					{
						var use = _pInfo.spec.collisionSetting.ShieldUse * f.DeltaTime;
						_pInfo.dribble->shield.guage = FPMathExtension.Clamp0(_pInfo.dribble->shield.guage - use);
					}

					/// 쉴드 사용불가 상태일때 쉴드 입력이 들어온 경우
					if (false == _pInfo.owner->HasBall()
						&& (_pInfo.dribble->Release.releaseInputState == InputState.Down || _pInfo.dribble->Release.releaseInputState == InputState.Press)
						&& _pInfo.dribble->shield.guage <= _pInfo.spec.collisionSetting.shieldUseGauge)
					{
						f.Events.OnUnavailableShield(_pInfo.entity);
					}
				}
			}
			/// 쉴드 상태 일 경우
			else
			{
				/// 쉴드키 입력안했거나, 쉴드를 사용할수 없는 상태이면 쉴드 종료
				if (_pInfo.dribble->Release.releaseInputState != InputState.Press
					|| _pInfo.playerFunction->playerFunctionFlag.HasFlag(PlayerFunctionFlag.Punching) == false
					|| _pInfo.board->Rsc.state.isStun
					|| _pInfo.dribble->shield.guage == FP._0)
				{
					_pInfo.owner->PickBallState &= ~PickBallState.Punching;
					//_pInfo.dribble->shield.coolTime = _pInfo.spec.collisionSetting.shieldCoolTime;
				}
				else
				{
					/// 게이지 소모
					var use = _pInfo.spec.collisionSetting.ShieldUse * f.DeltaTime;
					_pInfo.dribble->shield.guage = FPMathExtension.Clamp0(_pInfo.dribble->shield.guage - use);
				}
			}
		}

		/// <summary>
		/// 슛모드에 따른 자동 스윙슛 처리
		/// </summary>
		void UpdateAutoSwingRelase(Frame f, PlayerEntityInfo _pinfo)
		{
			if (_pinfo.input->PlayerRef == PlayerRef.None)
				return;

			var player = f.GetPlayerData(_pinfo.input->PlayerRef);
			if (null == player)
				return;

			if (player.shootMode == 0)
			{
				/// 뒤로 볼이 정렬되어있는 상태에서 릴리즈 하면, 자동 스윙릴리즈 처리
				if (_pinfo.owner->HasBall()
					&& true == _pinfo.input->Release
					&& false == IsTotalSwinging(_pinfo.dribble)
					&& false == _pinfo.dribble->Joint.IsFowardAlign)
				{
					/// 아카데미 스윙, 덩크슛 단계에서는 기능 잠금
					if (f.RuntimeConfig.gameType == GameType.Academy
						&& f.RuntimeConfig.gameSubType ==(int)AcademyType.Swing)
					{
						return;
					}

					_pinfo.input->Swing = true;
					pInfo.dribble->Swing.IsInputStateFDswing = InputState.None;
					_pinfo.dribble->playAutoSwingRelease = true;
				}
			}
		}

		public static void InitShield(DribbleComponent* dribble, DribbleSpec spec)
		{
			if (null == dribble)
				return;

			dribble->shield.guage = spec.collisionSetting.shieldMaxGauge;
			dribble->shield.coolTime = FP._0;
		}

		public static bool IsEnabledShield(Hoverboard* hoverboard, DribbleComponent* dribble, PlayerFunctionComponent* func, DribbleSpec spec)
		{
			 return (func->lockShieldTime == FP._0
							&& func->playerFunctionFlag.HasFlag(PlayerFunctionFlag.Punching)
							&& hoverboard->Rsc.state.isStun == false
							&& dribble->shield.guage > spec.collisionSetting.shieldUseGauge);
		}
		public static bool IsEnabledShield(Hoverboard hoverboard, DribbleComponent dribble, PlayerFunctionComponent func, DribbleSpec spec)
		{
			return (func.lockShieldTime == FP._0
						   && func.playerFunctionFlag.HasFlag(PlayerFunctionFlag.Punching)
						   && hoverboard.Rsc.state.isStun == false
						   && dribble.shield.guage > spec.collisionSetting.shieldUseGauge);
		}
		protected abstract class IDribbleSystemInternal
		{
			public abstract int Order { get; }
			public virtual void Update(Frame f, PlayerEntityInfo pInfo, List<BallEntityInfo> bInfoList) { }
			public virtual void CollisionProcess(Frame f, B2PCollision col, PlayerEntityInfo pInfo, BallEntityInfo bInfo, CollisionInfo3D colInfo) { }
			public virtual void OnSwingFinish(Frame f, FinishInfo finishInfo, PlayerEntityInfo pInfo, BallEntityInfo bInfo) { }
			public virtual void OnReleaseBall(Frame f, ReleaseInfo finishInfo, PlayerEntityInfo pInfo, BallEntityInfo bInfo) { }
			public virtual void OnPushBall(Frame f, PushInfo finishInfo, PlayerEntityInfo pInfo, BallEntityInfo bInfo) { }
			public virtual void OnPickBall(Frame f, PickInfo finishInfo, PlayerEntityInfo pInfo, BallEntityInfo bInfo) { }
			public virtual void OnOwnerBallCollisionEnterWithFloor(Frame f, PlayerEntityInfo pInfo, BallEntityInfo bInfo) { }
			public virtual void OnSwingStart(Frame f, EntityRef ownerRef, EntityRef ballRef, SwingType swingType) { }
		}
		
	}
}