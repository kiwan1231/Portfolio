using System;
using UnityEngine;
using Photon.Deterministic;
using Quantum.XNode;
using System.Collections.Generic;
using CharacterInfo = Quantum.AiHelper.CharacterInfo;

namespace Quantum
{
    public unsafe partial class AiSettingConfig : AssetObject
    {
		[Serializable]
		public class RandomRatio
		{
			public int ratio;
			public int maxRatio;
		}

		[Serializable]
		public class BaseSetting
		{
			public AiGrade grade;

			public AIAttributeTag attributeTag;

			public QBoolean enabledMoveHorizontal;
			public QBoolean enabledMoveVertical;

			public FPVector2 rotateDelayTimeRange;

			public FPVector2 spaceDist;

			public QBoolean enabledAvoidGround;
		}

		[Serializable]
		public class TargetSetting
		{
			public FP nearTargetRange;
			public int sightTickInterval;
			public FpRect sightRange;
			public int targetTickInterval;
			public FpRect targetRange;
			
			public FPVector2 targetAttackDelayTime;

			public FP targetAlertRange;

			public FP attackStayTime;

			public FP angleMaxRotate;

			public bool CheckNearTarget(FPVector2 pos, FPVector2 targetPos)
			{
				return FPVector2.Distance(pos, targetPos) <= nearTargetRange;
			}
			public bool CheckSightRange(FPVector2 pos, FPVector2 targetPos, short facingDirection)
			{
				var localPos = targetPos - pos;

				return facingDirection > 0 ? sightRange.InValue(localPos) : sightRange.InValueByReverseX(localPos);
			}
			public bool CheckTargetRange(FPVector2 pos, FPVector2 targetPos, short facingDirection)
			{
				var localPos = targetPos - pos;

				return facingDirection > 0 ? targetRange.InValue(localPos) : targetRange.InValueByReverseX(localPos);
			}
		}

		[Serializable]
		public class JumpSetting
		{
			public bool enabledJump;

			public FP jumpNeedHeight;

			public bool enabledWallJump;
			[DrawIf("enabledWallJump", 1, mode: DrawIfMode.Hide)]
			public FP groundToWallJumpDist;
		}

		[Serializable]
		public class AbilitySetting
		{
			[Serializable]
			public class AbilityInfo
			{
				public AssetRef abilityRef;
				public int ratio;
				public FP addTiredScore;
				public bool enableRatioDelayTime;
			}

			public bool enabledRatio;
			public int abilityMaxRatio = 100;

			public List<AbilityInfo> abilityList;
		}

		[Serializable]
		public class GuardSetting
		{
			public FP guardCheckRange;
			public FP guardDurationTime;
			public FP guardDelayTime;
			public int guardeRatio;
		}

		[Serializable]
		public class DashSetting
		{
			public RandomRatio dashRatio;
			public FPVector2 dashCheckRange;
			public FP dashDelayTime;
			
			public int Ratio => dashRatio.ratio;
			public int MaxRatio => dashRatio.maxRatio;

			public bool Check(FPVector2 aiPos, FPVector2 targetPos)
			{
				var diff = targetPos - aiPos;

				return FPMathHelper.Abs(diff.X) >= dashCheckRange.X
						&& FPMathHelper.Abs(diff.Y) <= dashCheckRange.Y;
			}
		}

		[Serializable]
		public class AggroSetting
		{
			public FP aggroMaxLimit;

			public FP nearestAggroUpRate;

			public FP targetDistAggroUp;
			public FP targetOutAggroDown;

			public int aggroPerDamageUnit;

			public FP aggroGainPerUnit;
		}

		[Serializable]
		public class ActionDelaySetting
		{
			[Serializable]
			public class ActionDelayInfo
			{
				public AIActionType aiActionType;
				public FP delayTime;
			}
			[Serializable]
			public class HurtActionDelayInfo
			{
				public HurtType hurtType;
				public List<ActionDelayInfo> actionDelayList;
			}
			[Tooltip("Whether to apply an action delay when the character is hurt")]
			public bool enableHurtActionDelay;
            [DrawIf("enableHurtActionDelay", 1, mode: DrawIfMode.Hide)]
            public bool enableHurtDefaultActionDelay;
            [DrawIf("enableHurtDefaultActionDelay", 1, mode: DrawIfMode.Hide)]
            public List<ActionDelayInfo> hurtDefaultActionDelayList;
			[DrawIf("enableHurtActionDelay", 1, mode: DrawIfMode.Hide)]
			public List<HurtActionDelayInfo> hurtActionDelayList;
		}
		[Serializable]
		public class BackStepSetting
		{
			public int backstepMaxCount;
			public FPVector2 backstepCheckDist;
			public AssetRef backStepRef;
		}

		[Serializable]
		public class TeleportSetting
		{
			public bool enabledTeleport;

			[DrawIf("enabledTeleport", 1, mode: DrawIfMode.Hide)]
			public FP teleportDelayTime;

			public bool enableReturnPatrolPoint; /// ���� ����� ���� ���, ���� aiArea�� ������ ������ ������ ��Ʈ�� ����Ʈ�� ���ư���
			[DrawIf("enableReturnPatrolPoint", 1, mode: DrawIfMode.Hide)]
			public FP returnPatrolPointCheckTime;

			public bool enabledTargetTrace; /// ���� ����� �߰� �ڷ���Ʈ
		}

		[Serializable]
		public class PhaseEntryCondition
		{
			public enum PhaseEntryType
			{
				InitBoss = 0,
				HealthLessThanPercent = 1,
				PlayTimeElapsed = 2,
				PhaseTimeElapsed = 3,
			}

			public int phaseOrder;

			public PhaseEntryType entryType;

			[DrawIf("entryType", 1, mode: DrawIfMode.Hide)]
			public FP healthLessThanPercent;

			[DrawIf("entryType", 2, mode: DrawIfMode.Hide)]
			public FP playElapsedTime;

			[DrawIf("entryType", 3, mode: DrawIfMode.Hide)]
			public int phaseIndex;
			[DrawIf("entryType", 3, mode: DrawIfMode.Hide)]
			public FP phaseElapsedTime;

			public bool Check(Frame f, CharacterInfo aiInfo, PhasePatternSet currentPhase, AiSettingConfig config)
			{
				/// ���� ������ ���� �켱���� �Ʒ��� �н�
				if (currentPhase != null
					&& currentPhase.phaseIndex == aiInfo.commonAi->phaseIndex
					&& currentPhase.entryCondition.phaseOrder >= phaseOrder)
					return false;

				if (entryType == PhaseEntryType.HealthLessThanPercent)
				{
					var ratio = StatHelper.GetHealthRatio(f, aiInfo.entity);
					return ratio <= healthLessThanPercent;
				}

				else if (entryType == PhaseEntryType.PlayTimeElapsed)
				{
					return f.ElapsedTime >= aiInfo.commonAi->playElapsedTime + playElapsedTime;
				}

				else if (entryType == PhaseEntryType.PhaseTimeElapsed)
				{
					return (currentPhase.phaseIndex == phaseIndex
								&& f.ElapsedTime >= aiInfo.commonAi->phaseElapsedTime + phaseElapsedTime);
				}

				return false;
			}
		}

		[Serializable]
		public class PhaseEntryAbility
		{
			public AssetRef abilityRef;
			public FP delayTime;
		}
		
		[Serializable]
		public class AiHurtSetting
		{
			public bool enableHurtAction;
		}

		[Serializable]
		public class PhasePatternSet
		{
			public int phaseIndex;

			public bool disable;

			public PhaseEntryCondition entryCondition;

			public FP phaseEntryDelayTime;
			public List<PhaseEntryAbility> entryAbilityList;

			public List<AbilitySetting.AbilityInfo> abilityList;
		}

		public BaseSetting baseStting;
		[Space]
		public TargetSetting targetSetting;
		[Space]
		public JumpSetting jumpSetting;
		[Space]
		public AbilitySetting abilitySetting;
		[Space]
		public GuardSetting guardStting;
		[Space]
		public DashSetting dashSetting;
		[Space]
		public AggroSetting aggroSetting;
		[Space]
		public ActionDelaySetting actionDelaySetting;
		[Space]
		public BackStepSetting backstepSetting;
		[Space]
		public TeleportSetting teleportSetting;
		[Space]
		public List<PhasePatternSet> phasePatternList;
		[Space]
		public AiHurtSetting aiHurtSetting;

		#region[BaseSetting]
		public AiGrade Grade => baseStting.grade;
		
		#endregion

		#region[TargetSetting]
		public FP AngleMaxRotate  => targetSetting.angleMaxRotate;
		public FP TargetRangeLength => targetSetting.targetRange.Length;
		public FP AttackStayTime => targetSetting.attackStayTime;
		#endregion

		#region[JumpSetting]
		public bool EnabledJump => jumpSetting.enabledJump;
		#endregion

		#region[AbilitySetting]
		public bool EnabledRatio => abilitySetting.enabledRatio;
		public int AbilityMaxRatio => abilitySetting.abilityMaxRatio;
		public List<AbilitySetting.AbilityInfo> AbilityList => abilitySetting.abilityList;
		public bool CheckAbilityRatio(AbilitySetting.AbilityInfo abilityInfo, int ratio)
		{
			if (false == abilitySetting.enabledRatio)
				return true;

			return abilityInfo.ratio >= ratio;
		}
		#endregion

		#region[AggroSetting]
		public FP UpdateNearestAggro(FP curAggro, FP deltaTime)
		{
			var addAggro = deltaTime * aggroSetting.nearestAggroUpRate;
			return FPMath.Clamp(curAggro + addAggro, FP._0, aggroSetting.aggroMaxLimit);
		}
		public FP UpdateTargetDistAggroUp(FP curAggro, FP ratio, FP deltaTime) => FPMathHelper.ClampMax(curAggro + (ratio * deltaTime * aggroSetting.targetDistAggroUp), aggroSetting.aggroMaxLimit);

		public FP UpdateAggroTargetOut(FP curAggro, FP deltaTime) => FPMathHelper.Clamp0(curAggro - (deltaTime * aggroSetting.targetOutAggroDown));
		public FP UpdateDemageAggro(FP curAggro, FP demage)
		{
			if (demage <= 0 || aggroSetting.aggroPerDamageUnit <= 0)
				return curAggro;

			var addAggro = (demage / aggroSetting.aggroPerDamageUnit) * aggroSetting.aggroGainPerUnit;
			return FPMath.Clamp(curAggro + addAggro, FP._0, aggroSetting.aggroMaxLimit);
		}

		public FP AggroMaxLimit => aggroSetting.aggroMaxLimit;
		public FP NearestAggroUpRate => aggroSetting.nearestAggroUpRate;
		#endregion

		#region[BackstepSetting]
		public int BackstepMaxCount => backstepSetting.backstepMaxCount;
		public FPVector2 BackstepCheckDist => backstepSetting.backstepCheckDist;
		public AssetRef BackStepRef => backstepSetting.backStepRef;
		#endregion

		#region[TeleportSetting]
		public bool EnabledTeleport => teleportSetting.enabledTeleport;
		public FP TeleportDelayTime => teleportSetting.teleportDelayTime;
		public bool EnableReturnPatrolPoint => teleportSetting.enableReturnPatrolPoint;
		public FP ReturnPatrolPointCheckTime => teleportSetting.returnPatrolPointCheckTime;
		public bool EnabledTargetTrace => teleportSetting.enabledTargetTrace;
		#endregion

		#region[PhasePatternSet]
		public bool GetPhasePatternSet(int index, out PhasePatternSet pattern)
		{
			pattern = phasePatternList.Find(x => x.phaseIndex == index);

			return pattern != null;
		}

		public bool CheckPhaseEntryCondition(Frame f, CharacterInfo aiInfo, out PhasePatternSet newPhase)
		{
			newPhase = null;

			GetPhasePatternSet(aiInfo.commonAi->phaseIndex, out var currentPhase);
			if (null == currentPhase)
				return false;

			foreach (var phase in phasePatternList)
			{
				if (phase.disable)
					continue;

				if (currentPhase.phaseIndex == phase.phaseIndex
					|| currentPhase.entryCondition.phaseOrder >= phase.entryCondition.phaseOrder
					|| (null != newPhase && newPhase.entryCondition.phaseOrder <= phase.entryCondition.phaseOrder))
					continue;

				if (false == phase.entryCondition.Check(f, aiInfo, currentPhase, this))
					continue;

				newPhase = phase;
			}

			return newPhase != null;
		}

		public List<AbilitySetting.AbilityInfo> GetBossAbilityList(int phaseIndex)
		{
			var list = abilitySetting.abilityList;

			var checkIndex = 0;
			foreach (var phase in phasePatternList)
			{
				if (phase.disable)
					continue;
				/// üũ�ҷ��� phase���� �� ���� �ܰ� phase�� üũ���� �ʴ´�.
				if (phase.phaseIndex > phaseIndex)
					continue;
				/// �̹� üũ�� phase���� �Ʒ� �ܰ�� üũ ���� �ʴ´�.
				if (checkIndex > phase.phaseIndex)
					continue;

				if (null != phase.abilityList && phase.abilityList.Count > 0)
				{
					list = phase.abilityList;
					checkIndex = phase.phaseIndex;
				}
			}

			return list;
		}
		#endregion
	}
}
