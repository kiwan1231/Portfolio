using System;
using System.Collections.Generic;
using Photon.Deterministic;
using Quantum.XNode.Ability;
using UnityEngine;
using Sirenix.OdinInspector;

using CharacterInfo = Quantum.AiHelper.CharacterInfo;

namespace Quantum.XNode
{
	public unsafe partial class AbilityGraph
	{
		public enum AbilityAttribute
		{
			None = 0,
			MeleeAttack = 1,
			RangedAttack = 2,
			Tired = 4,
		}

		#region[AbilityRangeShape]
		/// <summary>
		/// x,y축에 정렬 되어있는 사각형
		/// </summary>
		[Serializable]
		public struct AlignedRectangleRange
		{
			public FPVector2 min;

			public FPVector2 max;

			public bool IsAlignedRectangleOverlapping(FPVector2 rectMin, FPVector2 rectMax, bool reverseX, out FP overlapRatio)
			{
				overlapRatio = FP._0;

				var _min = min;
				var _max = max;
				if (reverseX)
				{
					_min.X = -max.X;
					_max.X = -min.X;
				}

				var overlapWidth = FPMath.Min(_max.X, rectMax.X) - FPMath.Max(_min.X, rectMin.X);
				var overlapHeight = FPMath.Min(_max.Y, rectMax.Y) - FPMath.Max(_min.Y, rectMin.Y);
				
				if (overlapWidth > FP._0 && overlapHeight > FP._0)
				{
					var overlapSize = overlapWidth * overlapHeight;
					var tempSize = (rectMax.X - rectMin.X) * (rectMax.Y - rectMin.Y);
					overlapRatio = FPMath.Clamp(overlapSize / tempSize, FP._0, FP._1);
				}

				return overlapWidth > FP._0 && overlapHeight > FP._0;
			}
			public bool IsColliderOverlapping(FPVector2 rectPos, PhysicsCollider2D* collider, bool reverseX, out FP overlapRatio)
			{
				if (collider->Shape.Type == Shape2DType.Box)
				{
					var tempMin = rectPos + collider->Shape.Centroid - (collider->Shape.Box.Extents / 2);
					var tempMax = rectPos + collider->Shape.Centroid + (collider->Shape.Box.Extents / 2);
					return IsAlignedRectangleOverlapping(tempMin, tempMax, reverseX, out overlapRatio);
				}
				else if (collider->Shape.Type == Shape2DType.Capsule)
				{
					var extent = FPVector2Helper.Set(collider->Shape.Capsule.Diameter, collider->Shape.Capsule.Height);
					var tempMin = rectPos + collider->Shape.Centroid - (extent / 2);
					var tempMax = rectPos + collider->Shape.Centroid + (extent / 2);
					return IsAlignedRectangleOverlapping(tempMin, tempMax, reverseX, out overlapRatio);
				}
				else
				{
					overlapRatio = FP._0;
					return false;
				}
			}
		}

		[Serializable]
		public struct CircleRange
		{
			public FPVector2 center;
			public FP radius;

			public bool IsAlignedRectangleOverlapping(FPVector2 tempMin, FPVector2 tempMax, bool reverseX, out FP overlapRatio)
			{
				overlapRatio = FP._0;

				var _center = center.Reverse(reverseX, false);

				var closestX = FPMath.Clamp(_center.X, tempMin.X, tempMax.X);
				var closestY = FPMath.Clamp(_center.Y, tempMin.Y, tempMax.Y);

				var dx = _center.X - closestX;
				var dy = _center.Y - closestY;
				var sqDist = (dx * dx) + (dy * dy);
				var sqRadius = (radius * radius);

				/// 거리 제곱이 반지름 제곱보다 작거나 같으면 겹침
				if (sqDist <= sqRadius)
				{
					overlapRatio = FP._1;
					return true;
				}
				else
					return false;
			}
			public bool IsColliderOverlapping(FPVector2 rectPos, PhysicsCollider2D* collider, bool reverseX, out FP overlapRatio)
			{
				if (collider->Shape.Type == Shape2DType.Box)
				{
					var tempMin = rectPos + collider->Shape.Centroid - (collider->Shape.Box.Extents / 2);
					var tempMax = rectPos + collider->Shape.Centroid + (collider->Shape.Box.Extents / 2);
					return IsAlignedRectangleOverlapping(tempMin, tempMax, reverseX, out overlapRatio);
				}
				else if (collider->Shape.Type == Shape2DType.Capsule)
				{
					var extent = FPVector2Helper.Set(collider->Shape.Capsule.Diameter, collider->Shape.Capsule.Height);
					var tempMin = rectPos + collider->Shape.Centroid - (extent / 2);
					var tempMax = rectPos + collider->Shape.Centroid + (extent / 2);
					return IsAlignedRectangleOverlapping(tempMin, tempMax, reverseX, out overlapRatio);
				}
				else
				{
					overlapRatio = FP._0;
					return false;
				}
			}
		}

		[Serializable]
		public struct CurveRange
		{
			public FPAnimationCurve curve;
			public FP curveSamplingCount;
			public FP circleRadius;

			public bool IsColliderOverlapping(FPVector2 targetPos, PhysicsCollider2D* collider, bool reverseX, out FP overlapRatio)
			{
				overlapRatio = FP._0;

				var targetRectMin = FPVector2.Zero;
				var targetRectMax = FPVector2.Zero;

				if (collider->Shape.Type == Shape2DType.Box)
				{
					targetRectMin = targetPos + collider->Shape.Centroid - (collider->Shape.Box.Extents / 2);
					targetRectMax = targetPos + collider->Shape.Centroid + (collider->Shape.Box.Extents / 2);
				}
				else if (collider->Shape.Type == Shape2DType.Capsule)
				{
					var extent = FPVector2Helper.Set(collider->Shape.Capsule.Diameter, collider->Shape.Capsule.Height);
					targetRectMin = targetPos + collider->Shape.Centroid - (extent / 2);
					targetRectMax = targetPos + collider->Shape.Centroid + (extent / 2);
				}
				else
				{
					return false;
				}

				for (FP i = 0; i <= curveSamplingCount; i += FP._1)
				{
					var xCurve = (i / curveSamplingCount) * curve.EndTime;
					var yCurve = curve.Evaluate(xCurve);
					var circleCenter =  FPVector2Helper.Set(xCurve, yCurve).Reverse(reverseX, false);

					if (FPMathHelper.IsCollisionRectAndCircle(targetRectMin, targetRectMax, circleCenter, circleRadius))
					{
						overlapRatio = FP._1;
						return true;
					}
				}

				return false;
			}
		}
		#endregion

		/// <summary>
		/// 어빌리티의 요소들을 저장
		/// </summary>
		[Serializable]
		public class AbilityMetaData
		{
			public AbilityMetaType metaType;
			
			public FPVector2 scoreWeightRange;

			//[ShowIf("metaType", AbilityMetaType.Attack)]
			[DrawIf("metaType", 10, mode: DrawIfMode.Hide)]
			public AttackMetaData attackMetaData;

			[DrawIf("metaType", 20, mode: DrawIfMode.Hide)]
			public MoveMetaData moveMetaData;
			[DrawIf("metaType", 21, mode: DrawIfMode.Hide)]
			public RotationMetaData rotationMetaData;

			[DrawIf("metaType", 30, mode: DrawIfMode.Hide)]
			public StatusMetaData statusMetaData;
			[DrawIf("metaType", 31, mode: DrawIfMode.Hide)]
			public HurtMetaData hurtMetaData;

			[DrawIf("metaType", 100, mode: DrawIfMode.Hide)]
			public TiredMetaData tiredMetaData;

			[Serializable]
			public class AttackMetaData
			{
				public short attackDirection;

				[Tooltip("Instead of a single target, it checks the entire enemy team as attack targets.")]
				public bool enableAllTargetCheck;

				//public bool isPlatformBlocked;

				public AbilityRangeShapeType rangeShapeType;
				[DrawIf("rangeShapeType", 1, mode: DrawIfMode.Hide)]
				public FP lineRange;
				[DrawIf("rangeShapeType", 1, mode: DrawIfMode.Hide)]
				public FP lineAngleLimit;

				[DrawIf("rangeShapeType", 2, mode: DrawIfMode.Hide)]
				public CircleRange circleRangel;
				[DrawIf("rangeShapeType", 3, mode: DrawIfMode.Hide)]
				public AlignedRectangleRange alignedRectangleRange;
				[DrawIf("rangeShapeType", 4, mode: DrawIfMode.Hide)]
				public List<AlignedRectangleRange> alignedRectangleList;

				[DrawIf("rangeShapeType", 5, mode: DrawIfMode.Hide)]
				public CurveRange curveRange;

				public FP GetAreaHoldingMetaScore(AbilityMetaData data, Frame f, CharacterInfo info, CharacterInfo targetInfo, List<AiHelper.CharacterInfo> characterList)
				{
					var targetLocalPos = info.transform->InverseTransformPoint(targetInfo.transform->Position);
					var reverseX = attackDirection != info.movement->facingDirection;
					var ratio = FP._0;

					if (rangeShapeType == AbilityRangeShapeType.Line)
					{
						var forward = info.movement->facingDirection > 0 ? info.transform->Right : info.transform->Left;

						if (enableAllTargetCheck)
						{
							foreach(var _characterInfo in characterList)
							{
								if (_characterInfo.team->index == info.team->index)
									continue;

								var tempRatio = GetRatioByLine(_characterInfo);
								ratio = ratio < tempRatio ? tempRatio : ratio;
							}
						}
						else
						{
							ratio = GetRatioByLine(targetInfo);
						}

						FP GetRatioByLine(CharacterInfo _targetInfo)
						{
							var targetDir = (_targetInfo.transform->Position - info.transform->Position).Normalized;
							var angle = FPVector2.Angle(forward, targetDir);
							var dist = (_targetInfo.transform->Position - info.transform->Position).Magnitude;

							return (angle <= lineAngleLimit && dist <= lineRange) ? FP._1 : FP._0;
						}
					}

					else if (rangeShapeType == AbilityRangeShapeType.Circle)
					{
						if (enableAllTargetCheck)
						{
							foreach (var _characterInfo in characterList)
							{
								if (_characterInfo.team->index == info.team->index)
									continue;

								var _targetLocalPos = info.transform->InverseTransformPoint(_characterInfo.transform->Position);
								circleRangel.IsColliderOverlapping(_targetLocalPos, _characterInfo.collider, reverseX, out var tempRatio);
								ratio = ratio < tempRatio ? tempRatio : ratio;
							}
						}
						else
						{
							circleRangel.IsColliderOverlapping(targetLocalPos, targetInfo.collider, reverseX, out ratio);
						}
					}

					else if (rangeShapeType == AbilityRangeShapeType.AlignedRectangle)
					{
						if (enableAllTargetCheck)
						{
							foreach (var _characterInfo in characterList)
							{
								if (_characterInfo.team->index == info.team->index)
									continue;

								var _targetLocalPos = info.transform->InverseTransformPoint(_characterInfo.transform->Position);
								alignedRectangleRange.IsColliderOverlapping(_targetLocalPos, _characterInfo.collider, reverseX, out var tempRatio);

								ratio = ratio < tempRatio ? tempRatio : ratio;
							}
						}
						else
						{
							alignedRectangleRange.IsColliderOverlapping(targetLocalPos, targetInfo.collider, reverseX, out ratio);
						}
					}

					else if (rangeShapeType == AbilityRangeShapeType.AlignedRectangleList)
					{
						if (enableAllTargetCheck)
						{
							foreach (var _characterInfo in characterList)
							{
								if (_characterInfo.team->index == info.team->index)
									continue;

								var _targetLocalPos = info.transform->InverseTransformPoint(_characterInfo.transform->Position);
								foreach (var rectangle in alignedRectangleList)
								{
									if (false == rectangle.IsColliderOverlapping(_targetLocalPos, _characterInfo.collider, reverseX, out var tempRatio))
										continue;

									ratio = ratio < tempRatio ? tempRatio : ratio;
								}
							}
						}
						else
						{
							foreach (var rectangle in alignedRectangleList)
							{
								if (false == rectangle.IsColliderOverlapping(targetLocalPos, targetInfo.collider, reverseX, out var tempRatio))
									continue;

								ratio = ratio < tempRatio ? tempRatio : ratio;
							}
						}
					}

					else if (rangeShapeType == AbilityRangeShapeType.Curve)
					{
						if (enableAllTargetCheck)
						{
							foreach (var _characterInfo in characterList)
							{
								if (_characterInfo.team->index == info.team->index)
									continue;

								var _targetLocalPos = info.transform->InverseTransformPoint(_characterInfo.transform->Position);
								curveRange.IsColliderOverlapping(_targetLocalPos, _characterInfo.collider, reverseX, out var tempRatio);

								ratio = ratio < tempRatio ? tempRatio : ratio;
							}
						}
						else
						{
							curveRange.IsColliderOverlapping(targetLocalPos, targetInfo.collider, reverseX, out ratio);
						}
					}

					return FPMath.Lerp(data.scoreWeightRange.X, data.scoreWeightRange.Y, ratio);
				}

				public FP MaxRange()
				{
					if (rangeShapeType == AbilityRangeShapeType.Line)
					{
						return lineRange;
					}

					else if (rangeShapeType == AbilityRangeShapeType.Circle)
					{
						return circleRangel.radius;
					}

					else if (rangeShapeType == AbilityRangeShapeType.AlignedRectangle)
					{
						return alignedRectangleRange.max.X;
					}

					else if (rangeShapeType == AbilityRangeShapeType.AlignedRectangleList)
					{
						var maxRange = FP._0;
						foreach (var rectangle in alignedRectangleList)
						{
							if (maxRange == FP._0 || maxRange < rectangle.max.X)
								maxRange = rectangle.max.X;
						}
						return maxRange;
					}

					else if (rangeShapeType == AbilityRangeShapeType.Curve)
					{
						var maxRange = FP._0;
						for (FP i = 0; i <= curveRange.curveSamplingCount; i += FP._1)
						{
							var xCurve = (i / curveRange.curveSamplingCount) * curveRange.curve.EndTime;
							var yCurve = curveRange.curve.Evaluate(xCurve);
							
							if (yCurve >= FP._0 && maxRange < xCurve)
								maxRange = xCurve;
						}
						return maxRange;
					}

					else
						return FP._0;
				}
			}

			[Serializable]
			public class MoveMetaData
			{
				public enum MoveType
				{
					AreaCenter = 0,
					EnemyAvoid = 1,
					CornerEscape = 2,
					MovePosition = 3,
				}

				public MoveType moveType;
				public FPVector2 translation;

				public bool enableFloorCheck;
				[DrawIf("enableFloorCheck", 1, mode: DrawIfMode.Hide)]
				public FP floorCheckDist;

				[DrawIf("moveType", 1, mode: DrawIfMode.Hide)]
				public FPVector2 avoidDist;

				[DrawIf("moveType", 2, mode: DrawIfMode.Hide)]
				public FPVector2 targetDist;
				[DrawIf("moveType", 2, mode: DrawIfMode.Hide)]
				public FP cornerDist;

				[DrawIf("moveType", 3, mode: DrawIfMode.Hide)]
				public FPAnimationCurve postionScoreCorve;

				/// <summary>
				/// 타겟을 잡는것보다 특정 위치에 계속 있는것에 초점을 맞춤
				/// </summary>
				public FP GetAreaHoldingMetaScore(AbilityMetaData data, Frame f, CharacterInfo info, CharacterInfo targetInfo)
				{
					var _ScoreT = FP._0;

					var _aiPos = info.CentorPos;
					var _targetPos = targetInfo.CentorPos;

					var _translation = info.movement->facingDirection >= 0 ? translation : translation.Reverse(true, false);
					var _translationDist = _translation.Magnitude;

					var _hitCollection = PhysicsHelper.PhysicsRayCast(f, _aiPos, _translation.Normalized, _translationDist, f.GetLayerStaticMap());

					/// 벽 등에 가로 막혔을 경우
					var _movePos = _aiPos + _translation;
					if (PhysicsHelper.HitNearPos(_aiPos, _hitCollection, out var _nearPos))
					{
						_movePos.X = _nearPos.X >= _aiPos.X ? _nearPos.X - info.collider->Shape.Capsule.Diameter : _nearPos.X + info.collider->Shape.Capsule.Diameter;
						_movePos.Y = _nearPos.Y >= _aiPos.Y ? _nearPos.Y - info.collider->Shape.Capsule.Height : _nearPos.Y + info.collider->Shape.Capsule.Height;
					}

					if (enableFloorCheck)
					{
						var floorHitCollection = PhysicsHelper.PhysicsRayCast(f, _movePos, FPVector2.Down, floorCheckDist, f.GetLayerStaticMap());
						if (floorHitCollection.Count == 0)
						{
							return 0;
						}
					}

					if (MoveType.AreaCenter == moveType)
					{
						var _curPosScore = AiHelper.GetAreaPositionScore(f, info.commonAi, _aiPos);
						var _movePosScore = AiHelper.GetAreaPositionScore(f, info.commonAi, _movePos);
						var _diffPosScore = FPMath.Clamp(_movePosScore - _curPosScore, 0, AiHelper.MaxPositionScore);

						_ScoreT = _diffPosScore / 200;
					}

					else if (MoveType.EnemyAvoid == moveType)
					{
						var targetDist = (targetInfo.transform->Position - info.transform->Position).Abs();
						if (targetDist.X <= avoidDist.X && targetDist.Y <= avoidDist.Y)
						{
							var curDist = (targetInfo.transform->Position - _aiPos).Magnitude;
							var moveDist = (targetInfo.transform->Position - _movePos).Magnitude;
							var diffDist = FPMath.Clamp(moveDist - curDist, FP._0, _translationDist);

							_ScoreT = diffDist / _translationDist;
						}
						else
						{
							_ScoreT = FP._0;
						}
					}
					
					else if (MoveType.CornerEscape == moveType)
					{
						var dist = (_targetPos - _aiPos).Abs();
						if (dist.X > targetDist.X || dist.Y > targetDist.Y)
							_ScoreT = FP._0;
						else
						{
							var dir = _targetPos.X <= _aiPos.X ? FPVector2.Right : FPVector2.Left;
							var cornerHit = PhysicsHelper.PhysicsRayCast(f, _aiPos, dir, cornerDist, f.GetLayerStaticMap());

							_ScoreT = (cornerHit.Count == 0) ? FP._0 : FP._1;
						}
					}

					else if (MoveType.MovePosition == moveType)
					{
						var actualTranslationDist = (_movePos - _aiPos).Magnitude;
						
						if (_translationDist <= 0)
							_ScoreT = 1;
						else
							_ScoreT = FPMath.Lerp(0, 1, postionScoreCorve.Evaluate(FPMath.Clamp01(actualTranslationDist / _translationDist)));
					}

					return FPMath.Lerp(data.scoreWeightRange.X, data.scoreWeightRange.Y, _ScoreT);
				}
			}

			[Serializable]
			public class RotationMetaData
			{
				public bool isReverseDirection;
				public FP GetAreaHoldingMetaScore(AbilityMetaData data, Frame f, CharacterInfo info, CharacterInfo targetInfo)
				{
					var aiFacingRight = info.movement->facingDirection == 1;
					var targetRight = targetInfo.transform->Position.X >= info.transform->Position.X;
					
					return (aiFacingRight == targetRight) ? data.scoreWeightRange.X : data.scoreWeightRange.Y;
				}
			}

			[Serializable]
			public class StatusMetaData
			{
				public StatusType targetStatusType;

				public FP GetMetaScore(AbilityMetaData data, Frame f, CharacterInfo info, CharacterInfo targetInfo)
				{
					return Status.Contains(f, targetInfo.entity, targetStatusType) ? data.scoreWeightRange.Y : FP._0;
				}
			}

			[Serializable]
			public class HurtMetaData
			{
				public HurtType targetHurtType;

				public FP GetMetaScore(AbilityMetaData data, Frame f, CharacterInfo info, CharacterInfo targetInfo)
				{
					if (f.TryGet<Hurt>(targetInfo.entity, out var targetHurt))
					{
						return (targetHurt.hurtType == targetHurtType) ? data.scoreWeightRange.Y : FP._0;
					}
					else
						return FP._0;
				}
			}

			[Serializable]
			public class TiredMetaData
			{
				public FP limitTiredScore;
				public FP GetgMetaScore(AbilityMetaData data, Frame f, CharacterInfo info)
				{
					return info.commonAi->tiredScore >= limitTiredScore ? data.scoreWeightRange.Y : FP._0;
				}
			}

			public FP GetMetaScore(Frame f, CharacterInfo info, CharacterInfo targetInfo, List<AiHelper.CharacterInfo> characterList, AbilityUsageContext usageContext)
			{
				if (AbilityMetaType.Attack == metaType)
				{
					if (usageContext == AbilityUsageContext.AreaHolding)
						return attackMetaData.GetAreaHoldingMetaScore(this, f, info, targetInfo, characterList);
					else
						return FP._0;
				}

				else if (AbilityMetaType.Move == metaType)
				{
					if (usageContext == AbilityUsageContext.AreaHolding)
						return moveMetaData.GetAreaHoldingMetaScore(this, f, info, targetInfo);
					else
						return FP._0;
				}
				else if (AbilityMetaType.Rotation == metaType)
				{
					if (usageContext == AbilityUsageContext.AreaHolding)
						return rotationMetaData.GetAreaHoldingMetaScore(this, f, info, targetInfo);
					else
						return FP._0;
				}

				else if (AbilityMetaType.Status == metaType)
				{
					return statusMetaData.GetMetaScore(this, f, info, targetInfo);
				}
				else if (AbilityMetaType.Hurt == metaType)
				{
					return hurtMetaData.GetMetaScore(this, f, info, targetInfo);
				}

				else if (AbilityMetaType.Tired == metaType)
				{
					return tiredMetaData.GetgMetaScore(this, f, info);
				}
				else
					return FP._0;
			}
		}

		[Serializable]
		public struct AbilityVariableData
		{
			public enum AbilityVariableType
			{
				None = 0,
				FPVector2 = 1,
				Entity = 2,
			}
			public enum AbilityVariableOption
			{
				None = 0,

				TargetPos = 10,
				TargetEntity = 11,

				AreaMaximum = 100,
				AreaMinimum = 101,
				AreaCenter = 102,
				AreaFarthest = 103,
				AreaNearest = 104,
			}

			public AssetRef<VariableObject> variableObject;

			public AbilityVariableType variableType;

			[DrawIf("variableType", 1, mode: DrawIfMode.Hide)]
			public AbilityVariableOption vectorXOption;
			[DrawIf("variableType", 1, mode: DrawIfMode.Hide)]
			public AbilityVariableOption vectorYOption;
			[DrawIf("variableType", 1, mode: DrawIfMode.Hide)]
			public FPVector2 variableFPVector2Shift;
			[DrawIf("variableType", 1, mode: DrawIfMode.Hide)]
			public bool enabledLimitFPVector2;
			[DrawIf("enabledLimitFPVector2", 1, mode: DrawIfMode.Hide)]
			public FPVector2 variableFPVector2Limit;

			[DrawIf("variableType", 2, mode: DrawIfMode.Hide)]
			public AbilityVariableOption entityOption;

			public FPVector2 GetFPVector2Variable(CharacterInfo aiInfo, CharacterInfo targetInfo)
			{
				var variable = FPVector2.Zero;

				var areaMax = aiInfo.commonAi->areaInfo.maxArea;
				var areaMin = aiInfo.commonAi->areaInfo.minArea;
				var areaCenter = (areaMax + areaMin) * FP._0_50;

				if (variableType != AbilityVariableType.FPVector2)
					return variable;

				if (vectorXOption == AbilityVariableOption.TargetPos)
					variable.X = (targetInfo != null) ? targetInfo.transform->Position.X + variableFPVector2Shift.X : FP._0;

				else if (vectorXOption == AbilityVariableOption.AreaMaximum)
					variable.X = areaMax.X - variableFPVector2Shift.X;
				else if (vectorXOption == AbilityVariableOption.AreaMinimum)
					variable.X = areaMin.X + variableFPVector2Shift.X;
				else if (vectorXOption == AbilityVariableOption.AreaCenter)
					variable.X = areaCenter.X;
				else if (vectorXOption == AbilityVariableOption.AreaFarthest)
					variable.X = aiInfo.transform->Position.X >= areaCenter.X ? areaMin.X + variableFPVector2Shift.X : areaMax.X - variableFPVector2Shift.X;
				else if (vectorXOption == AbilityVariableOption.AreaNearest)
					variable.X = aiInfo.transform->Position.X >= areaCenter.X ? areaMax.X - variableFPVector2Shift.X : areaMin.X + variableFPVector2Shift.X;

				if (vectorYOption == AbilityVariableOption.TargetPos)
					variable.Y = (targetInfo != null) ? targetInfo.transform->Position.Y + variableFPVector2Shift.Y : FP._0;

				else if (vectorYOption == AbilityVariableOption.AreaMaximum)
					variable.Y = areaMax.Y - variableFPVector2Shift.Y;
				else if (vectorYOption == AbilityVariableOption.AreaMinimum)
					variable.Y = areaMin.Y + variableFPVector2Shift.Y;
				else if (vectorYOption == AbilityVariableOption.AreaCenter)
					variable.Y = areaCenter.Y;
				else if (vectorYOption == AbilityVariableOption.AreaFarthest)
					variable.Y = aiInfo.transform->Position.Y >= areaCenter.Y ? areaMin.Y + variableFPVector2Shift.Y : areaMax.Y - variableFPVector2Shift.Y;
				else if (vectorYOption == AbilityVariableOption.AreaNearest)
					variable.Y = aiInfo.transform->Position.Y >= areaCenter.Y ? areaMax.Y - variableFPVector2Shift.Y : areaMin.Y + variableFPVector2Shift.Y;

				if (enabledLimitFPVector2)
				{
					variable.X = (variable.X >= aiInfo.transform->Position.X) ? FPMathHelper.ClampMax(variable.X, aiInfo.transform->Position.X + variableFPVector2Limit.X)
																				: FPMathHelper.ClampMin(variable.X, aiInfo.transform->Position.X - variableFPVector2Limit.X);

					variable.Y = (variable.Y >= aiInfo.transform->Position.Y) ? FPMathHelper.ClampMax(variable.Y, aiInfo.transform->Position.Y + variableFPVector2Limit.Y)
																				: FPMathHelper.ClampMin(variable.Y, aiInfo.transform->Position.Y - variableFPVector2Limit.Y);
				}

				return variable;
			}
			public EntityRef GetEntityVariable(CharacterInfo aiInfo, CharacterInfo targetInfo)
			{
				var variable = EntityRef.None;

				if (variableType != AbilityVariableType.Entity)
					return variable;

				if (entityOption == AbilityVariableOption.TargetEntity)
				{
					variable = targetInfo.entity;
				}

				return variable;
			}

		}

		/// <summary>
		/// 현재 상황에서 어빌리티 사용에 대한 우선순위를 지정하는 기능
		/// </summary>
		[Serializable]
		public struct AbilitySelector
        {
            public AttackCategory        attackCategory;

			public AbilityAttribute		 abilityAttribute;

			public List<AbilityMetaData> metaDatList;
			[Space]
			public AbilityVariableData variableData;
			[Space]
			public FPVector2 enabledScoreRange;
			[Space]
			public FP abilityActionDelay; /// 어빌리티 액션 실행 딜레이
			public FP abilityActionComboDelay;
			public FP abilityDelay; /// 해당 어빌리티 딜레이

			public bool enabledAimRotate;
			[DrawIf("enabledAimRotate", 1, mode: DrawIfMode.Hide)]
			public bool enabledAbilityRotate;

			[Header("SameAbilityCombo")]
			public FP sameAbilityComboDelayTime;
			public int sameAbilityComboCount; /// 다름 어빌리티로 콤보를 실행하는게 아닌 같은 어빌리티를 연속으로 사용하는 콤보 횟수, 0이면 비활성화
			public FP sameAbilityComboValidTime;
		}

		public bool HasCombo => (null != aiComboList && 0 < aiComboList.Count);
		public FP AbilityActionDelay => (HasCombo || SameAbilityComboCount > 0) ? abilitySelector.abilityActionComboDelay : abilitySelector.abilityActionDelay;
		public FP AbilityDelayTime => abilitySelector.abilityDelay;

		public bool EnabledAimRotate => abilitySelector.enabledAimRotate;
		public bool EnabledAbilityRotate => abilitySelector.enabledAbilityRotate;

		public FP SameAbilityComboDelayTime => abilitySelector.sameAbilityComboDelayTime;
		public int SameAbilityComboCount => abilitySelector.sameAbilityComboCount;
		public FP SameAbilityComboValidTime => abilitySelector.sameAbilityComboValidTime;

		public bool CanUseAbilityGraph(Frame f, KCC2D* kcc, CommonAIComponent* commonAi, KCC2DConfig kccConfig)
		{
			if (landOrAir == LandOrAir.Land && AiHelper.IsAir(kcc))
				return false;
			else if (landOrAir == LandOrAir.Air && AiHelper.IsGround(kcc))
				return false;

			return true;
		}
		public bool GetHoldingMetaScore(Frame f, CharacterInfo info, CharacterInfo targetInfo, List<AiHelper.CharacterInfo> characterList, out FP score)
		{
			score = 0;

			if (null == abilitySelector.metaDatList || 0 == abilitySelector.metaDatList.Count)
				return false;

			foreach (var meta in abilitySelector.metaDatList)
				score += meta.GetMetaScore(f, info, targetInfo, characterList, AbilityUsageContext.AreaHolding);

			return abilitySelector.enabledScoreRange.InValue(score);
		}
		public bool CheckAttribute(AbilityAttribute checkAttribute)
		{
			return abilitySelector.abilityAttribute.HasFlag(checkAttribute);
		}
	}
}