using UnityEngine;
using System.Collections.Generic;
using Photon.Deterministic;
using System;

namespace Quantum
{
    public abstract unsafe partial class InteractionCheckBehavior : AssetObject
    {
		public InteractionCheckPlayEventType playEventType;

		public virtual void OnKCC2DTriggerEnter(Frame f, EntityRef entity, EntityRef triggerRef, InteractionCheck* interactionCheck)
		{
		}
		public virtual void OnKCC2DTriggerStay(Frame f, EntityRef entity, EntityRef triggerRef, InteractionCheck* interactionCheck)
		{
		}
		public virtual void OnKCC2DTriggerExit(Frame f, EntityRef entity, EntityRef triggerRef, InteractionCheck* interactionCheck)
		{
		}

		public virtual void OnKCC2DSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, Int32 iteration, InteractionCheck* interactionCheck)
		{
		}
		public virtual void OnKCC2DPreCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, InteractionCheck* interactionCheck)
		{
		}
		public virtual void OnKCC2DPostSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, InteractionCheck* interactionCheck)
		{
		}
		public unsafe virtual void InitCheck(Frame f, EntityRef checkRef, InteractionCheck* interactionCheck)
		{
			interactionCheck->init = true;
		}

		public virtual bool Check(Frame f, EntityRef checkRef, InteractionCheck* interactionCheck)
		{
			/// 비활성화 여부
			if (interactionCheck->disableInteraction)
				return false;

			/// 발동횟수 부족
			if (interactionCheck->curCheckCount < interactionCheck->maxCheckCount)
				return false;

			/// 실행 횟수 오버 
			if (false == interactionCheck->isRepeat && interactionCheck->curPlayCount >= interactionCheck->maxPlayCount)
				return false;

			return true;
		}

		public virtual void OnAction(Frame f, EntityRef checkRef, InteractionCheck* interactionCheck)
		{
			InteractionBehaviorSystem.PlayActionByInteractionCheck(f, checkRef, interactionCheck);

			interactionCheck->curCheckCount = 0;
			interactionCheck->curPlayCount = (interactionCheck->isRepeat) ? 0 : interactionCheck->curPlayCount + 1;

			if (playEventType != InteractionCheckPlayEventType.None)
				f.Events.OnInteractionCheckPlayEvent(checkRef, playEventType);
		}

		public unsafe virtual void SetDisableInteraction(Frame f, EntityRef checkRef, InteractionCheck* interactionCheck, bool disableInteraction)
		{
			interactionCheck->disableInteraction = disableInteraction;
		}


		public virtual void OnSpawnInteractionNpc(Frame f, EntityRef checkRef, EntityRef spawnActionRef, EntityRef npcRef, InteractionCheck* interactionCheck)
		{
		}
		public virtual void OnNpcDeath(Frame f, EntityRef checkRef, EntityRef deathRef, InteractionCheck* interactionCheck) { }
	}
}

