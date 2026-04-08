using System;
using Photon.Deterministic;
using Hit3D = Quantum.Physics3D.Hit3D;

namespace Quantum
{
	public unsafe partial class HoverboardSystem
	{
		/// <summary>
		/// 호버링 처리
		/// </summary>
		void UpdateHovering(PlayData data)
		{
			if (false == IsEnabledHovering(data.pHoverboard))
				return;

			// 지면이나 물체에 닿았을때
			if (false == data.pHoverboard->Rsc.belowHitIsTrigger
				&& false == data.pHoverboard->Rsc.dontHoverDetected)
			{
				var groundCheckInfo = GetGroundCheckOffset(data, false);
				var rayStart = groundCheckInfo.Item1;
				var groundNormal = data.pHoverboard->Rsc.currentGroundNormal.Normalized;

				var pidForcePercent = data.spec.pid.Seek(
					GetHoverHeight(data),
					FPVector3.Distance(data.pHoverboard->Rsc.currentGroundedPoint, rayStart),
					data.f.DeltaTime,
					ref data.pHoverboard->Rsc.pidIntegral,
					ref data.pHoverboard->Rsc.pidLastProportional);

				var force = groundNormal * data.spec.hovering.hoverForce * pidForcePercent;
				data.pPhysicsBody->AddForce(force);

				// 지면과 정렬
				var forward = FPVector3Extension.ProjectOnPlane(data.pTransform->Forward, groundNormal);
				var lookRot = FPQuaternion.LookRotation(forward, groundNormal);
				if (FPMathExtension.Abs(data.pTransform->Rotation.X - lookRot.X) > FP.Epsilon
					|| FPMathExtension.Abs(data.pTransform->Rotation.Y - lookRot.Y) > FP.Epsilon
					|| FPMathExtension.Abs(data.pTransform->Rotation.Z - lookRot.Z) > FP.Epsilon
					|| FPMathExtension.Abs(data.pTransform->Rotation.W - lookRot.W) > FP.Epsilon)
				{
					// 지면 각도 정렬
					data.pTransform->Rotation = FPQuaternion.Slerp(data.pTransform->Rotation, lookRot, data.spec.hovering.groundAlignSpeed * data.f.DeltaTime);
				}
			}
		}

		void UpdateRotation(PlayData data)
		{
			UpdateSteer(data);

			UpdateTilt(data);
		}

		/// <summary>
		/// 좌우 회전 처리
		/// </summary>
		void UpdateSteer(PlayData data)
		{
			data.pPhysicsBody->AngularVelocity = FPVector3.Zero;

			if (false == IsEnabledSteer(data.f, data.entity))
				return;

			var steer = data.pHoverboard->Rsc.state.steerDir;
			var isSteer = FPMathExtension.Abs(steer) > FP.Epsilon;

			if (isSteer && steer != 0)
			{
				// 선회 입력 시간에 따라 회전량 다르게.
				var steerT = data.pHoverboard->Rsc.state.steerTime / data.spec.steer.maxSteerReachTime;
				steerT = data.spec.steer.steerCurve.Evaluate(steerT);

				var steerSpeed = data.spec.steer.steerSpeed.LerpFromXY(steerT) * GetSteerSpeedMultiply(data);

				var up = data.pHoverboard->Rsc.state.isGrounded ? data.pTransform->Up : FPVector3.Up;
				data.pPhysicsBody->AngularVelocity = up * steerSpeed * steer * data.f.DeltaTime;
			}
		}

		/// <summary>
		/// 틸트 업데이트
		/// </summary>
		void UpdateTilt(PlayData data)
		{
			if (false == IsEnabledTilt(data.f, data.entity))
				return;

			/// 틸트 입력 시간에 따른 틸트 속도 계산
			var fbT = data.spec.tilt.tiltFB_Curve.Evaluate(data.pHoverboard->Rsc.state.tiltFB_Time / data.spec.tilt.maxTiltFB_ReachTime);
			var fbStartSpd = data.spec.tilt.tiltFB_Speed.X + (data.pInput->fbTiltStartSpeed * data.spec.tilt.fbStartAddSpd);
			var tiltFB_Spd = FPMath.Lerp(fbStartSpd, data.spec.tilt.tiltFB_Speed.Y, fbT);
			tiltFB_Spd = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.TiltSpd, tiltFB_Spd);


			var lrT = data.spec.tilt.tiltLR_Curve.Evaluate(data.pHoverboard->Rsc.state.tiltLR_Time / data.spec.tilt.maxTiltLR_ReachTime);
			var lrStartSpd = data.spec.tilt.tiltLR_Speed.X + (data.pInput->lrTiltStartSpeed * data.spec.tilt.lrStartAddSpd);
			var tiltLR_Spd = FPMath.Lerp(lrStartSpd, data.spec.tilt.tiltLR_Speed.Y, lrT);
			tiltLR_Spd = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.TiltSpd, tiltLR_Spd);


			var fb       = data.pHoverboard->Rsc.state.tiltDir_FB * tiltFB_Spd;
			var lr       = FP._0 < data.pInput->lrTiltAngle ? data.pHoverboard->Rsc.state.tiltDir_LR * tiltLR_Spd : FP._0;
			var rotAngle = data.pHoverboard->Rsc.state.tiltRotAngle;

			/// 틸트 각도 제한 계산
			if (AiType.None == data.pAiData->aiType)
			{
				if (fb > FP._0 && data.pInput->fTiltAngle >= FP._0)
				{
					if (data.pHoverboard->Rsc.state.tiltRotAngle.X > data.pInput->fTiltAngle)
						fb = FP._0;
				}
				else if (fb < FP._0 && data.pInput->bTiltAngle >= FP._0)
				{
					if (data.pHoverboard->Rsc.state.tiltRotAngle.X < -data.pInput->bTiltAngle)
						fb = FP._0;
				}

				if (data.pInput->lrTiltAngle == FP._0)
					lr = FP._0;
				else if (data.pInput->lrTiltAngle >= 0)
				{
					if (lr > 0 && data.pHoverboard->Rsc.state.tiltRotAngle.Z > data.pInput->lrTiltAngle)
						lr = FP._0;
					else if (lr < 0 && data.pHoverboard->Rsc.state.tiltRotAngle.Z < -data.pInput->lrTiltAngle)
						lr = FP._0;
				}
			}

			if (fb != FP._0)
			{
				data.pHoverboard->Rsc.state.tiltRotAngle.X += (fb * data.f.DeltaTime);
				var w = data.pTransform->TransformDirection(FPVector3.Right) * fb;
				var torque = FPVector3.Scale(data.pPhysicsBody->GetInertiaTensor(), w);
				data.pPhysicsBody->AddTorque(torque);
			}

			if (lr != FP._0)
			{
				data.pHoverboard->Rsc.state.tiltRotAngle.Z += (lr * data.f.DeltaTime);
				var w = data.pTransform->TransformDirection(FPVector3.Forward) * lr;
				var torque = FPVector3.Scale(data.pPhysicsBody->GetInertiaTensor(), w);
				data.pPhysicsBody->AddTorque(torque);
			}
		}

		/// <summary>
		/// 경사로에서 추가 가속 or 감속을 계산
		/// </summary>
		void UpdateMaxSpeed(PlayData data)
		{
			/// 스윙중일떄 최대 속도 감소 설정
			if (data.pDribble->Swing.IsSwing == true)
			{
				/// 스윙중일때 (현재상태 -> swingingMaxSpeedFactor)으로 차츰 최대속도 비율이 변경
				data.pHoverboard->Rsc.spd.swingSpdRestoreDelay = 0;
				data.pHoverboard->Rsc.spd.maxSpeedRateInSwing
					= FPMath.Lerp(
						data.pHoverboard->Rsc.spd.maxSpeedRateInSwing,
						data.spec.speed.swingingMaxSpeedFactor,
						data.spec.speed.swingingSpeedDecreaseLerp * data.f.DeltaTime);
			}
			else
			{
				/// 스윙중이 아닐때 (현재상태 -> 1)으로 차츰 최대속도 비율이 변경
				data.pHoverboard->Rsc.spd.swingSpdRestoreDelay += data.f.DeltaTime;

				if (data.pHoverboard->Rsc.spd.swingSpdRestoreDelay > FP._0_10 + FP._0_05)
				{
					data.pHoverboard->Rsc.spd.maxSpeedRateInSwing
						= FPMath.Lerp(
						data.pHoverboard->Rsc.spd.maxSpeedRateInSwing,
						1,
						data.spec.speed.swingingSpeedIncreaseLerp * data.f.DeltaTime
					);
				}
			}

			/// 경사로에서 추가 가속 업데이트
			UpdateSlopeMaxSpeed(data);
		}
		void UpdateSlopeMaxSpeed(PlayData data)
		{
			/// 경사로이면 추가 최대속도를 재설정한다.
			if (data.pHoverboard->Rsc.state.isGrounded && data.pHoverboard->Rsc.state.isSlope)
			{
				FP yVel = data.pPhysicsBody->Velocity.Y;
				FP yAddMaxSpeedDir = yVel > FP._0 ? -FP._1 : FP._1;
				FP yVelForce = FPMathExtension.Abs(yVel);

				/// y속도와 최대속도(0.83)의 비율에 따라 추가가속 값을 설정한다.
				FP baseMaxSpeed = GetBaseMaxSpeed(data) * (FP._0_50 + FP._0_33);
				FP curveT = yVelForce < baseMaxSpeed ? yVelForce / baseMaxSpeed : FP._1;
				FP curveValue = data.spec.speed.maxSpdConfigWithGroundCurve.Evaluate(curveT);
				FPVector2 slopeMaxSpeedSection = yAddMaxSpeedDir > FP._0 ? data.spec.speed.slopeUpMaxSpped : data.spec.speed.slopeDownMaxSpped;
				FP newAddMaxSpped = slopeMaxSpeedSection.LerpFromXY(curveValue) * yAddMaxSpeedDir;

				/// 같은 방향으로 추가최대속도값이 된경d우
				if ((newAddMaxSpped > 0 && data.pHoverboard->Rsc.spd.moreSpeedFromDegree > 0)
					|| (newAddMaxSpped < 0 && data.pHoverboard->Rsc.spd.moreSpeedFromDegree < 0))
				{
					bool IsMoreNewSpeed = FPMathExtension.Abs(newAddMaxSpped) >= FPMathExtension.Abs(data.pHoverboard->Rsc.spd.moreSpeedFromDegree);
					/// 추가 최대속도
					if (newAddMaxSpped > 0)
					{
						if (IsMoreNewSpeed)
							data.pHoverboard->Rsc.spd.moreSpeedFromDegree = newAddMaxSpped;
						/// 경사로 내려갈때 추가 최대속도 감소를 적게 하여 평지로 내려갔을때 최대속도 값이 유지가 되게 하여준다.
						else
							data.pHoverboard->Rsc.spd.moreSpeedFromDegree = (data.pHoverboard->Rsc.spd.moreSpeedFromDegree * (FP._0_99) + (newAddMaxSpped * FP._0_01));
					}
					/// 감소 최대속도
					else
					{
						data.pHoverboard->Rsc.spd.moreSpeedFromDegree = newAddMaxSpped;
					}
				}
				/// 다른 방향으로 추가 최대속도 값이 된경우
				else
				{
					data.pHoverboard->Rsc.spd.moreSpeedFromDegree = newAddMaxSpped;
				}

				data.pHoverboard->Rsc.spd.remainGroundDegreeTime = data.spec.speed.maxSpdConfigWithGroundDura;
			}
			/// 경사로가 아니지만 추가최대속도 지속시간이 남아있다면 값을 유지한다.
			else if (data.pHoverboard->Rsc.spd.remainGroundDegreeTime > FP._0)
			{
				data.pHoverboard->Rsc.spd.remainGroundDegreeTime -= data.f.DeltaTime;
			}
			/// 경사로도 아니고 지속시간도 모두 지났다면 추가최대속도값은 적용되어지지 않는다.
			else
			{
				data.pHoverboard->Rsc.spd.remainGroundDegreeTime = FP._0;
				data.pHoverboard->Rsc.spd.moreSpeedFromDegree = FP._0;
			}
		}

		/// <summary>
		/// Accel업데이트 (지면 이동)
		/// </summary>
		void UpdateAccel(PlayData data)
		{
			var _isEnabledAccelFB = IsEnabledAccelFB(data.f, data.entity);
			var _isEnabledAccelLR = IsEnabledAccelLR(data.f, data.entity);
			
			var forward = _isEnabledAccelFB ? data.pHoverboard->Rsc.state.accelDir_FB : FP._0;
			var left    = _isEnabledAccelLR ? data.pHoverboard->Rsc.state.accelDir_LR : FP._0;

			var fbTorque = FP._0;
			if (_isEnabledAccelFB)
			{
				var accelTime = data.pHoverboard->Rsc.state.accelFB_Time;
				var accelTorque = (forward >= 0) ? data.spec.accel.accelF_Torque : data.spec.accel.accelB_Torque;

				fbTorque = FPMath.Lerp(accelTorque.X, accelTorque.Y, accelTime / data.spec.accel.accelFB_MaxTime);
			}

			var lrTorque = FP._0;
			if (_isEnabledAccelLR)
			{
				var accelTime = data.pHoverboard->Rsc.state.accelLR_Time;
				var accelTorque = FPVector2.Zero;

				if (data.pHoverboard->Rsc.state.isGrounded)
				{
					accelTorque = data.spec.accel.accelLR_Torque;
				}
				else if (FP._0 >= data.pInput->lrTiltAngle)
				{
					accelTorque = data.spec.accel.accelLR_AirTorque;
				}

				lrTorque = FPMath.Lerp(accelTorque.X, accelTorque.Y, accelTime / data.spec.accel.accelLR_MaxTime);
			}

			//FP wallDegree = data.pHoverboard->Rsc.wallDegree;

			/// 보드가 지상 판정일때 기울기에 따라 토크값 설정
			{
				var torqueFactor = GetCurrentWallTorqueFactor(data);
				torqueFactor *= data.f.RuntimeConfig.accelRatio;

				if (forward > 0)
					fbTorque *= Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.FrontMoveSpd, torqueFactor);
				else
					fbTorque *= Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.BackMoveSpd, torqueFactor);

				lrTorque *= Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.SideMoveSpd, torqueFactor);
			}

			/// 보드 이동 방향이 전환되어질때 더 강한 토크값을 준다.
			if (false == data.pHoverboard->Rsc.state.isSlope)
			{
				/// 뒤로 가는중 전진키 or 앞으로 가는중 후진키
				FP forwardSpeed = FPVector3.Dot(data.pPhysicsBody->Velocity, data.pTransform->Forward);
				if (_isEnabledAccelFB && 
				    (forward > FP.Epsilon && forwardSpeed < 0 || // 강체가 뒤로 가고 있는데 전진
				    forward < -FP.Epsilon && forwardSpeed > 0)) // 강체가 앞으로 가고 있는데 후진
				{
					var forwardForce = (data.pTransform->Forward * forward) * FPMathExtension.Abs(forwardSpeed);
					
					data.pPhysicsBody->AddForce(forwardForce.DumpY() * data.spec.accel.accelFB_DirChangeFactor);
				}

				/// 오른쪽 가는중 왼쪽키 or 왼쪽 가는중 오른쪽키
				FP leftSpeed = FPVector3.Dot(data.pPhysicsBody->Velocity, data.pTransform->Left);
				if (_isEnabledAccelLR &&
				    (left > FP.Epsilon && leftSpeed < 0 || // 오른쪽으로 가고 있는데 왼쪽이동
				    left < -FP.Epsilon && leftSpeed > 0)) // 왼쪽으로 가고 있는데 오른쪽 이동
				{
					if (data.pHoverboard->Rsc.state.isGrounded || 
					    FP._0 >= data.pInput->lrTiltAngle)
					{
						var leftForce               = (data.pTransform->Left * left) * FPMathExtension.Abs(leftSpeed);
						data.pPhysicsBody->AddForce(leftForce.DumpY() * data.spec.accel.accelLR_DirChangeFactor);
					}
				}
			}

			/// 이동 토크값 적용
			{
				/// 방향벡터 설정
				FPVector3 dirForce = FPVector3.Zero;
				if (forward != 0)
					dirForce += data.pTransform->Forward * forward;
				if (left != 0)
					dirForce += data.pTransform->Left * left;
				
				/// 벡터 크기 설정
				FP dirTorque = FP._0;
				if (forward != 0 && left != 0)
					dirTorque = (fbTorque + lrTorque) / 2;
				else if (forward != 0)
					dirTorque = fbTorque;
				else if (left != 0)
					dirTorque = lrTorque;

				/// 경사로에서 내려가는 방향으로 액셀을 주고 있는 상황이면 추가 가속을 준다.
				{
					if (data.pHoverboard->Rsc.state.isSlope
						&& data.pHoverboard->Rsc.spd.moreSpeedFromDegree > 0
						&& data.pPhysicsBody->Velocity.Y < 0
						&& FPVector3.Dot(dirForce, FPVector3.Down) > 0)
					{
						/// 경사로 아래방향에 따른 fbTorque 추가값 계산
						var downDirForce = FPVector3.Project(dirForce, FPVector3.Down);
						var downRatio = downDirForce.SqrMagnitude / dirForce.SqrMagnitude;
						dirTorque += data.spec.accel.sloopAddAccelTorque.Evaluate(downRatio);
					}
				}

				if (dirTorque > 0)
				{
					data.pPhysicsBody->AddLinearImpulse(dirForce.Normalized * dirTorque);

					//dirForce = dirForce.Normalized;
					//dirForce = FPVector3Extension.ProjectOnPlane(dirForce, data.pHoverboard->Rsc.currentGroundNormal);
					//data.pPhysicsBody->AddLinearImpulse(dirForce * dirTorque);
				}
			}
		}

		/// <summary>
		/// 점프 처리
		/// </summary>
		void UpdateJump(PlayData data)
		{
			if (false == IsEnabledJump(data.f, data.entity))
				return;

			if (false == data.pHoverboard->Rsc.state.isJumpInput) return;
		
			/// 대쉬키가 입력되었을때 지상대쉬를 할수 있는 상태라면, 점프를 하지 못하게 막는다.
			if (data.pHoverboard->Rsc.dashState.isDashInputState == InputState.Down 
				&& IsEnabledGroundDash(data.f, data.entity))
				return;
			
			/// 상태값 업데이트
			if (data.pHoverboard->Rsc.state.isSlope)
				data.pHoverboard->Rsc.state.isSlopeJump = true;
			else if (data.pHoverboard->Rsc.state.isWall)
				data.pHoverboard->Rsc.state.isWallJump = true;
			else
				data.pHoverboard->Rsc.state.isBaseJump = true;

			data.pHoverboard->Rsc.state.needJumpRelease = true;
			data.pHoverboard->Rsc.dontGroundCheck = true;

			/// 점프 시도시 Velocity.Y 값을 초기화
			FPVector3 localVel = data.pTransform->InverseTransformDirection(data.pPhysicsBody->Velocity);
			localVel.Y = 0;
			data.pPhysicsBody->Velocity = data.pTransform->TransformDirection(localVel);

			/// 점프값 계산
			var upVector = data.pTransform->Up;
			if (data.pHoverboard->Rsc.state.isSlopeJump)
			{
				/// 90(최대경사로)~180(평지)
				FP angle = FPVector3.Angle(data.pTransform->Up, FPVector3.Down);
				if (angle < 90) angle = 90;
				else if (angle > 180) angle = 180;

				/// 경사로쪽으로 갈수록 vel.y값은 낮추고 vel.x,z값은 높인다.
				/// (경사로에서 부스터로 올라올떄 점프를 하면 너무 큰yVel을 얻기 때문.)
				FP changeValue = (180 - angle) / 90;
				upVector.X *= (1 + changeValue);
				upVector.Z *= (1 + changeValue);
				upVector.Y *= (1 - changeValue);

				if (upVector.X > FP._1) upVector.X = FP._1;
				else if (upVector.X < -FP._1) upVector.X = -FP._1;
				if (upVector.Z > FP._1) upVector.Z = FP._1;
				else if (upVector.Z < -FP._1) upVector.Z = -FP._1;

				var _axis = FPVector3.Cross(data.pTransform->Up, FPVector3.Up);
				var _angle = FPVector3.Angle(data.pTransform->Up, FPVector3.Up);
				
				data.pHoverboard->Rsc.airAlignRot    = FPQuaternion.FromToRotation(data.pTransform->Up, FPVector3.Up) * data.pTransform->Rotation;
				data.pHoverboard->Rsc.airAlignTime.X = data.pHoverboard->Rsc.airAlignTime.Y = data.spec.align.alignTime * (_angle / 180);
			}

			var force = (data.pHoverboard->Rsc.state.isSlopeJump) ? data.spec.jump.slopeJumpForce : data.spec.jump.groundSmallJumpForce;
			force = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.JumpPower, force);
			data.pPhysicsBody->AddLinearImpulse(upVector * force);

			data.f.Signals.OnJumpHoverboard(data.entity, data.pTransform->Position);
			data.f.Events.OnSmallJump(data.entity);
		}

		/// <summary>
		/// 대쉬 업데이트
		/// </summary>
		void UpdateDash(PlayData data)
		{
			var forwardSpeed = GetCurrentForwardSpeed(data);
			var maxSpd = GetBaseMaxSpeed(data);

			UpdateGroundDash();
			UpdateAirDash();
			UpdateJumpDash();

			/// 지상 대쉬
			void UpdateGroundDash()
			{
				/// 지상 대쉬 상태일 경우 업데이트
				if (data.pHoverboard->Rsc.dashState.groundDashState > 0)
				{
					var groundDashSpeed = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.GroundDashSpd, data.spec.dash.groundDashSpeed);
					var temp = (data.pTransform->Forward * groundDashSpeed) + (data.pTransform->Down * data.spec.dash.groundDownFouce);
					temp.Y *= data.spec.dash.dashYweight;
					data.pPhysicsBody->Velocity = temp;
				}

				/// 지상 대쉬 키 입력 체크
				if (data.pHoverboard->Rsc.dashState.isDashInputState != InputState.Down)
					return;

				if (IsEnabledGroundDash(data.f, data.entity))
				{
					if (false == data.f.Global->cheatOption.unlimitNitro)
						data.pHoverboard->Rsc.currentNitro -= data.spec.dash.dashNitroValue;

					data.pHoverboard->Rsc.dashState.groundDashState = data.spec.dash.groundDashStateTime;

					var groundDashCoolTime = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.DashCoolTime, data.spec.dash.groundDashCoolTime);

					data.pHoverboard->Rsc.nitroCoolTime = groundDashCoolTime;
					data.pHoverboard->Rsc.nitroAction = NitroAction.GroundDash;
					data.pHoverboard->Rsc.dashState.groundDashBeforeSpeed = FPMath.Clamp(forwardSpeed, 0, maxSpd);
					data.f.Signals.OnDashHoverboard(data.entity, DashType.GroundDash);
					data.f.Events.OnGroundDash(data.entity, data.spec.dash.dashNitroValue);
				}
				else
				{
					if (data.pHoverboard->Rsc.state.isGrounded)
					{
						data.f.Events.OnDashNotPossible(data.entity);
						data.f.Events.OnNotEnoughNitro(data.entity, NitroUse.GroundDash);
					}
				}
			}

			/// 공중 대쉬
			void UpdateAirDash()
			{
				/// 공중 대쉬 상태일 경우 업데이트
				if (data.pHoverboard->Rsc.dashState.airDashState > 0)
				{
					var airDashSpeed = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.AirDashSpd, data.spec.dash.airDashSpeed);
					FPVector3 temp = data.pTransform->Forward * airDashSpeed;
					temp.Y *= data.spec.dash.dashYweight;
					data.pPhysicsBody->Velocity = temp;
				}

				/// 공중 대쉬 키 입력 체크
				if (data.pHoverboard->Rsc.dashState.isDashInputState != InputState.Down)
					return;

				if (IsEnabledAirDash(data.f, data.entity))
				{
					if (false == data.f.Global->cheatOption.unlimitNitro)
						data.pHoverboard->Rsc.currentNitro -= data.spec.dash.dashNitroValue;

					data.pHoverboard->Rsc.dashState.airDashState = data.spec.dash.airDashStateTime;

					var airDashCoolTime = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.DashCoolTime, data.spec.dash.airDashCoolTime);
					data.pHoverboard->Rsc.nitroCoolTime = airDashCoolTime;
					data.pHoverboard->Rsc.nitroAction = NitroAction.AirDash;
					data.pHoverboard->Rsc.dashState.airDashBeforeSpeed = FPMath.Clamp(forwardSpeed, 0, maxSpd);
					data.f.Signals.OnDashHoverboard(data.entity, DashType.AirDash);
					data.f.Events.OnAirDash(data.entity, data.spec.dash.dashNitroValue);
				}
				else
				{
					if (false == data.pHoverboard->Rsc.state.isGrounded)
					{
						data.f.Events.OnDashNotPossible(data.entity);
						data.f.Events.OnNotEnoughNitro(data.entity, NitroUse.AirDash);
					}
				}
			}

			/// 점프 대쉬
			void UpdateJumpDash()
			{
				if (data.pHoverboard->Rsc.dashState.isJumpDashInputState == InputState.Down)
				{
					if (IsEnabledJumpDash(data.f, data.entity))
					{
						if (false == data.f.Global->cheatOption.unlimitNitro)
							data.pHoverboard->Rsc.currentNitro -= data.spec.dash.dashNitroValue;

						data.pHoverboard->Rsc.dashState.jumpDashState = data.spec.dash.JumpDashStateTime;

						var JumpDashCoolTime = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.DashCoolTime, data.spec.dash.JumpDashCoolTime);
						data.pHoverboard->Rsc.nitroCoolTime = JumpDashCoolTime;
						data.pHoverboard->Rsc.nitroAction = NitroAction.JumpDash;
						data.f.Signals.OnDashHoverboard(data.entity, DashType.JumpDash);
						data.f.Events.OnJumpDash(data.entity, data.spec.dash.dashNitroValue);
					}
					else
					{
						if (false == data.pHoverboard->Rsc.state.isGrounded)
						{
							data.f.Events.OnDashNotPossible(data.entity);
							data.f.Events.OnNotEnoughNitro(data.entity, NitroUse.JumpDash);
						}
					}
				}

				/// 점프 대쉬 상태일 경우 업데이트
				if (data.pHoverboard->Rsc.dashState.jumpDashState > 0)
				{
					var _Up = data.pTransform->Up;
					if (FP._0 >= data.pInput->lrTiltAngle && FP._0_75 <= FPMath.Abs(data.pInput->tilt_LR))
					{
						//var front  = FPVector3Extension.ProjectOnPlane(data.pPhysicsBody->Velocity, data.pTransform->Up);
						
						var       _angle = (FP._0 <= data.pInput->tilt_LR) ? data.spec.jump.lrJumpBoostAngle : -data.spec.jump.lrJumpBoostAngle;
						_Up = data.pTransform->TransformDirection(FPQuaternion.AngleAxis(_angle, FPVector3.Forward) * FPVector3.Up);
						//var JumpDashSpeed = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.JumpDashSpd, data.spec.dash.JumpDashSpeed);
						//var temp          = _Up * JumpDashSpeed;
						//temp.Y                      *= data.spec.dash.dashYweight;
						//data.pPhysicsBody->Velocity =  front + temp;
					}
					
					var force = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.JumpPower, data.spec.jump.groundBigJumpForce);
					data.pPhysicsBody->AddLinearImpulse(_Up * force);	
				}
			}
		}

		/// <summary>
		/// 마찰력 업데이트
		/// </summary>
		void UpdateFriction(PlayData data)
		{
			var stepBack = data.f.Unsafe.GetPointer<StepBackComponent>(data.entity);
			if (false == data.pHoverboard->Rsc.state.isGrounded
				|| IsPlayingJump(data)
				|| FP._0 < data.pHoverboard->Rsc.dashState.groundDashState
				|| FP._0 < stepBack->stateTime)
				return;

			var playerVel = data.pPhysicsBody->Velocity;

			/// 앞뒤 마찰
			var forwardVel = FPVector3.Project(playerVel, data.pTransform->Forward);
			var sqForwardSpd = forwardVel.SqrMagnitude;
			var fdRange = data.pHoverboard->Rsc.state.isSlope ? data.spec.drag.fdSlopeFrictionRange : data.spec.drag.fdFrictionRange;
			var fdSpdLimit = data.pHoverboard->Rsc.state.isSlope ? data.spec.drag.fdSlopeSpdLimit : data.spec.drag.fdSpdLimit;
			forwardVel = forwardVel * FPMath.Lerp(fdRange.Y, fdRange.X, sqForwardSpd / fdSpdLimit * fdSpdLimit);

			/// 좌우 마찰
			var leftVel = FPVector3.Project(playerVel, data.pTransform->Left);
			var sqLeftSpd = leftVel.SqrMagnitude;
			var lrRange = data.pHoverboard->Rsc.state.isSlope ? data.spec.drag.lrSlopeFrictionRange : data.spec.drag.lrFrictionRange;
			var lrSpdLimit = data.pHoverboard->Rsc.state.isSlope ? data.spec.drag.lrSlopeSpdLimit : data.spec.drag.lrSpdLimit;
			leftVel = leftVel * FPMath.Lerp(lrRange.Y, lrRange.X, sqLeftSpd / lrSpdLimit * lrSpdLimit);

			/// 마찰력 적용
			var frictionVelocity = (forwardVel + leftVel).ClampVector(FPVector3Extension.Abs(playerVel));

			/// 마찰력이 최소마찰력보다 적으면 움직임을 0으로 만든다.
			var absFriction = FPVector3Extension.Abs(frictionVelocity);
			frictionVelocity.X = (absFriction.X <= data.spec.drag.minFrictionZeroVelocity.X) ? playerVel.X : frictionVelocity.X;
			frictionVelocity.Z = (absFriction.Z <= data.spec.drag.minFrictionZeroVelocity.Z) ? playerVel.Z : frictionVelocity.Z;

			data.pPhysicsBody->Velocity = playerVel - frictionVelocity;
		}

		/// <summary>
		/// 호버보드 속도 제한 업데이트
		/// </summary>
		void UpdateSpeedLimit(PlayData data)
		{
			var curSqSpd = GetCurrentSqSpeed(data);
			var maxSpd = GetMaxSpeed(data);
			if (curSqSpd <= maxSpd * maxSpd) return;

			/// 대쉬 중인지 여부
			bool isDash = (data.pHoverboard->Rsc.dashState.groundDashState > 0 || data.pHoverboard->Rsc.dashState.airDashState > 0);
			bool isJumpDash = data.pHoverboard->Rsc.dashState.jumpDashState > 0;
			bool isStepBack = IsPlayingStepBack(data);

			if (isDash) return;
			if (isJumpDash) return;
			if (isStepBack) return;

			var _velocity = data.pTransform->InverseTransformDirection(data.pPhysicsBody->Velocity);
			var ySpeed    = _velocity.Y;
			data.pPhysicsBody->Velocity = data.pTransform->TransformDirection(FPVector3.ClampMagnitude(_velocity.DumpY(), maxSpd).SetY(ySpeed));
		}

		/// <summary>
		/// 중력 처리
		/// </summary>
		void UpdateGravity(PlayData data)
		{
			FP gravityForce = data.spec.gravity.gravity;
			/// 공중에서 중력
			if (data.pHoverboard->Rsc.state.isGrounded == false)
			{
				/// 대쉬 사용중에는 중력 작용 없음
				if (data.pHoverboard->Rsc.dashState.airDashState > 0) return;
				if (data.pHoverboard->Rsc.dashState.jumpDashState > 0) return;

				data.pPhysicsBody->AddForce(FPVector3.Down * gravityForce);
			}
			/// 지상에서 중력은 호버링때문에 필요하다.
			else
			{
				/// 경사로에서는 경사로에 붙게 하기 위해서 중력값을 월드down과 호버보드down으로 나누어서 힘을 준다.
				FP boardDownForce = gravityForce * data.spec.gravity.boardDownforce;
				FP worldDownForce = gravityForce - boardDownForce;

				data.pPhysicsBody->AddForce(FPVector3.Down * worldDownForce);
				data.pPhysicsBody->AddLinearImpulse(data.pTransform->Down * boardDownForce);
			}
		}

		/// <summary>
		/// 스턴 상태 업데이트.
		/// </summary>
		void UpdateStun(PlayData data)
		{
			if (data.pHoverboard->Rsc.state.isStun == false) return;

			data.pHoverboard->Rsc.stunElapsed += data.f.DeltaTime;

			// 시간이 지나면 스턴 풀림
			if (data.pHoverboard->Rsc.stunElapsed > data.spec.stun.duration)
			{
				data.pHoverboard->Rsc.state.isStun = false;
				data.pHoverboard->Rsc.stunElapsed = 0;
				data.pBallOwner->PickBallState &= ~PickBallState.Stun;
			}
			else
			{
				/// 스턴중 대쉬 취소
				data.f.Signals.OnEndDash(data.entity);
				/// 스턴중 스텝백 취소
				data.f.Signals.OnEndStepBack(data.entity);

				data.pBallOwner->PickBallState |= PickBallState.Stun;
			}
		}

		/// <summary>
		/// 지면과 정렬 업데이트
		/// </summary>
		void UpdateAlignWithGround(PlayData data)
		{
			if (data.pHoverboard->Rsc.groundAlignTime > FP._0)
			{
				/// 각속도 계산
				var alignAngleSpd = data.spec.align.alignSpd * data.f.DeltaTime;
				/// 정렬 각도 차이
				var angleDist = FPQuaternion.Angle(data.pTransform->Rotation, data.pHoverboard->Rsc.groundAlignRot);

				var t = (angleDist > alignAngleSpd) ? alignAngleSpd / angleDist : 1;
				data.pTransform->Rotation = FPQuaternion.Slerp(
																data.pTransform->Rotation,
																data.pHoverboard->Rsc.groundAlignRot,
																t);
				
				FPExtension.ZeroDeduction(ref data.pHoverboard->Rsc.groundAlignTime, data.f.DeltaTime);
			}
		}
		
		void UpdateAlignWithAir(PlayData data)
		{
			var _time     = data.pHoverboard->Rsc.airAlignTime.X;
			var _duration = data.pHoverboard->Rsc.airAlignTime.Y;
			
			if (FP._0 < _time && 
			    _time != _duration &&
			    data.pHoverboard->Rsc.state.isGrounded)
			{
				data.pHoverboard->Rsc.airAlignTime = FPVector2.Zero;
			}
			
			if (data.pHoverboard->Rsc.airAlignTime.X > FP._0)
			{
				_time                                = FPMath.Max(FP._0, _time - data.f.DeltaTime);
				data.pTransform->Rotation            = FPQuaternion.Slerp(data.pHoverboard->Rsc.airAlignRot, data.pTransform->Rotation, FPMath.Clamp01(_time / _duration));
				data.pHoverboard->Rsc.airAlignTime.X = _time;
			}
		}

		/// <summary>
		/// 백스텝 업데이트
		/// </summary>
		void UpdateBackStep(PlayData data)
		{
			var stepBack = data.f.Unsafe.GetPointer<StepBackComponent>(data.entity);

			/// 인풋 상태 업데이트
			stepBack->inputState = stepBack->inputState.GetInputState(data.pInput->StepBack);

			/// 백스텝 업데이트
			if (stepBack->stateTime > 0)
			{
				FPExtension.ZeroDeduction(ref stepBack->stateTime, data.f.DeltaTime);
				
				/// 종료처리
				if (stepBack->stateTime == 0)
				{
					/// 지상 백스텝 종료때 Velocity 처리
					if (stepBack->type == 0)
					{
						var maxSpd = GetMaxSpeed(data);
						var spd = FPMath.Lerp(stepBack->beforeSpeed, maxSpd, data.spec.backStep.groundAfterRate);
						var afterVelocity = data.pTransform->Back * spd;

						afterVelocity.Y *= data.spec.backStep.afterSpeedYweight;
						data.pPhysicsBody->Velocity = afterVelocity;
						data.f.Events.OnEndStepBack(data.entity);
					}
				}
				/// 진행중 업데이트
				else
				{
					var speed = data.pTransform->Back * (stepBack->type == 0 ? data.spec.backStep.groundSpeed : data.spec.backStep.airSpeed);
					speed = Ability.AbilityMultipleValue(data.f, data.entity, *data.pAiData, *data.pInput, AbilityType.BackStepSpeed, speed);
					speed.Y *= data.spec.backStep.speedYweight;

					data.pPhysicsBody->Velocity = speed;
				}
			}
			/// 백스텝 실행 체크
			else
			{
				if (stepBack->inputState != InputState.Down)
					return;

				if (IsEnabledStepBack(data.f, data.entity))
				{
					/// 백스텝 실행
					var isGround = data.pHoverboard->Rsc.state.isGrounded;
					stepBack->type = isGround ? 0 : 1;

					stepBack->stateTime = isGround ? data.spec.backStep.groundStateTime : data.spec.backStep.airStateTime;
					data.pHoverboard->Rsc.nitroCoolTime = isGround ? data.spec.backStep.groundCoolTime : data.spec.backStep.airCoolTime;
					data.pHoverboard->Rsc.nitroAction = NitroAction.BackStep;
					stepBack->beforeSpeed = GetCurrentSpeed(data);

					if (false == data.f.Global->cheatOption.unlimitNitro)
						data.pHoverboard->Rsc.currentNitro -= data.spec.backStep.nitroValue;

					data.f.Signals.OnPlayStepBack(data.entity);
					data.f.Events.OnActionAniStepBack(data.entity);
					data.f.Events.OnActionStepBack(data.entity);
				}
				else
				{
					data.f.Events.OnStepBackNotPossible(data.entity);
					data.f.Events.OnNotEnoughNitro(data.entity, NitroUse.StepBack);
				}
			}
		}

		/// <summary>
		///  상황에 따른 호버보드 velocity 추가 처리 
		/// </summary>
		void UpdateVelocity(PlayData data)
		{
			/// 공중에서 호버보드 velocity상한선 두기
			if (false == data.pHoverboard->Rsc.state.isGrounded && data.pHoverboard->Rsc.state.airVelLimitTime > 0)
			{
				var stepBack = data.f.Unsafe.GetPointer<StepBackComponent>(data.entity);
				if (stepBack->stateTime <= 0 && false == IsPlayingDash(data))
					data.pPhysicsBody->Velocity = FPVector3Extension.ClampVector(data.pPhysicsBody->Velocity, data.spec.airVelocityLimit);
				else
					data.pHoverboard->Rsc.state.airVelLimitTime = FP._0;
			}

			FPExtension.ZeroDeduction(ref data.pHoverboard->Rsc.state.airVelLimitTime, data.f.DeltaTime);
		}
	}
}