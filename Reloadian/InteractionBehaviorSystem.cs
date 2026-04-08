using System;
using UnityEngine.Scripting;
using Photon.Deterministic;
using Quantum.Collections;
using Quantum.Physics2D;

namespace Quantum
{
    public unsafe partial class InteractionBehaviorSystem : SystemMainThread, ISignalOnKCC2DTrigger, ISignalOnKCC2DExit, ISignalOnKCC2DEnter, ISignalOnComponentAdded<Death>, ISignalOnPreHitTask
            ,ISignalOnKCC2DSolverCollision, ISignalOnKCC2DPreCollision, ISignalOnKCC2DPostSolverCollision
            , ISignalOnActiveTeleportPortal, ISignalOnActiveTeamTeleportPortal, ISignalOnSpawnInteractionNpc, ISignalOnEnterBossRoom
    {
        public override void OnInit(Frame f)
		{

		}

        public unsafe override void Update(Frame f)
		{
            var checkFilter = f.Filter<InteractionCheckComponent>();
            while(checkFilter.NextUnsafe(out var checkRef, out var checkComponent))
			{
				var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
				{
                    if (false == f.GetCheckBehavior(checkList[i].checkBehavior, out var _checkBehavior))
                        continue;

                    var check = checkList.GetPointer(i);
                    if (false == check->init)
                    {
                        _checkBehavior.InitCheck(f, checkRef, check);
                    }

                    if (_checkBehavior.Check(f, checkRef, check))
                    {
                        _checkBehavior.OnAction(f, checkRef, check);
                    }
                }
            }

            var actionFilter = f.Filter<InteractionActionComponent>();
            while (actionFilter.NextUnsafe(out var actionRef, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for(int i = 0; i < actionList.Count; i++)
				{
                    if (false == f.GetActionBehavior(actionList[i].actionBehavior, out var _actionBehavior))
                        continue;

                    var action = actionList.GetPointer(i);
                    
                    if (false == action->init)
                    {
                        _actionBehavior.InitAction(f, actionRef, action);
                    }

                    if (action->enabledUpdate)
                    {
                        _actionBehavior.UpdateAction(f, actionRef, action);
                    }
                }
			}
		}

        public void OnKCC2DTrigger(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, Hit trigger)
        {
            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(trigger.Entity, out var checkComponent))
			{
                var checkList = f.ResolveList(checkComponent->checkList);
                for(int i = 0; i < checkList.Count; i++)
				{
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerStay(f, entity, trigger.Entity, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(trigger.Entity, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerStay(f, entity, trigger.Entity, actionList.GetPointer(i));
                }
			}
		}
        public void OnKCC2DEnter(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, EntityRef other)
        {
            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(other, out var checkComponent))
			{
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerEnter(f, entity, other, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(other, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerEnter(f, entity, other, actionList.GetPointer(i));
                }
            }
		}
        public void OnKCC2DExit(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, EntityRef other)
        {
            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(other, out var checkComponent))
			{
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerExit(f, entity, other, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(other, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DTriggerExit(f, entity, other, actionList.GetPointer(i));
                }
            }
		}
        public void OnAdded(Frame f, EntityRef entity, Death* component)
        {
            var checkFilter = f.Filter<InteractionCheckComponent>();
            while (checkFilter.NextUnsafe(out var checkRef, out var checkComponent))
            {
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnNpcDeath(f, checkRef, entity, checkList.GetPointer(i));
                }
            }
        }
        public void OnPreHitTask(Frame f, ref HitTask hitTask)
        {
            if (!f.Unsafe.TryGetPointer<InteractionCheckComponent>(hitTask.target, out var checkComponent))
            {
                return;
            }

            var checkList = f.ResolveList(checkComponent->checkList);
            for (var i = 0; i < checkList.Count; i++)
            {
                if (f.TryCheckBehavior<IC_OnHit>(checkList[i].checkBehavior.Id, out var _hitAsset))
                {
                    _hitAsset.OnEntityHit(f, hitTask.originator, hitTask.target, checkList.GetPointer(i));
                }
            }

            hitTask.ignore = true;
        }

        public void OnKCC2DSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, KCCQueryResult* collision, Int32 iteration)
        {
            if (collision->Contact.IsStatic || collision->Contact.Entity == EntityRef.None)
                return;

            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(collision->Contact.Entity, out var checkComponent))
            {
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DSolverCollision(f, entity, kcc, settings, collision, iteration, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(collision->Contact.Entity, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DSolverCollision(f, entity, kcc, settings, collision, iteration, actionList.GetPointer(i));
                }
            }
		}
        public void OnKCC2DPreCollision(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, KCCQueryResult* collision)
        {
            if (collision->Contact.IsStatic || collision->Contact.Entity == EntityRef.None)
                return;

            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(collision->Contact.Entity, out var checkComponent))
            {
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DPreCollision(f, entity, kcc, settings, collision, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(collision->Contact.Entity, out var actionComponent))
			{
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DPreCollision(f, entity, kcc, settings, collision, actionList.GetPointer(i));
                }
            }
		}
        public void OnKCC2DPostSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, ref KCC2DSettings settings, KCCQueryResult* collision)
        {
            if (collision->Contact.IsStatic || collision->Contact.Entity == EntityRef.None)
                return;

            if (f.Unsafe.TryGetPointer<InteractionCheckComponent>(collision->Contact.Entity, out var checkComponent))
            {
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnKCC2DPostSolverCollision(f, entity, kcc, settings, collision, checkList.GetPointer(i));
                }
            }

			if (f.Unsafe.TryGetPointer<InteractionActionComponent>(collision->Contact.Entity, out var actionComponent))
            {
                var actionList = f.ResolveList(actionComponent->actionList);
                for (int i = 0; i < actionList.Count; i++)
                {
                    if (f.GetActionBehavior(actionList[i].actionBehavior, out var _behavior))
                        _behavior.OnKCC2DPostSolverCollision(f, entity, kcc, settings, collision, actionList.GetPointer(i));
                }
            }
		}

        public void OnActiveTeleportPortal(Frame f, EntityRef portalRef, EntityRef playerRef)
		{
            if (f.Unsafe.TryGetPointer<TeleportPortal>(portalRef, out var teleportPortal))
            {
                teleportPortal->activePortal = true;

                teleportPortal->isTeamProtal = false;

                var accessList = f.ResolveList(teleportPortal->portalAccessPlayers);
                var playerID = f.Get<PlayerID>(playerRef);

                if (false == accessList.Contains(playerID.PlayerRef))
				{
                    accessList.Add(playerID.PlayerRef);
                }
            }
        }
        public void OnActiveTeamTeleportPortal(Frame f, EntityRef portalRef, Byte teamIndex)
		{
            if(f.Unsafe.TryGetPointer<TeleportPortal>(portalRef, out var teleportPortal))
			{
                teleportPortal->activePortal = true;
                teleportPortal->isTeamProtal = true;
                teleportPortal->teamIndex = teamIndex;
            }
		}

        public void OnSpawnInteractionNpc(Frame f, EntityRef interactionRef, EntityRef spawnNpcRef)
		{
            var checkFilter = f.Filter<InteractionCheckComponent>();
            while (checkFilter.NextUnsafe(out var checkRef, out var checkComponent))
            {
                var checkList = f.ResolveList(checkComponent->checkList);
                for (int i = 0; i < checkList.Count; i++)
                {
                    if (f.GetCheckBehavior(checkList[i].checkBehavior, out var _behavior))
                        _behavior.OnSpawnInteractionNpc(f, checkRef, interactionRef, spawnNpcRef, checkList.GetPointer(i));
                }
            }
		}

        public void OnEnterBossRoom(Frame f, Int32 bossRoomIndex, EntityRef enterPlayerRef)
		{
            
		}

        public static void PlayActionByInteractionCheck(Frame f, EntityRef checkEntity, InteractionCheck* interactionCheck)
		{
            var actionArgumentList = f.ResolveList(interactionCheck->actionArgumentList);
            for (int i = 0; i < actionArgumentList.Count; i++)
            {
                if (false == f.Unsafe.TryGetPointer<InteractionActionComponent>(actionArgumentList[i].actionRef, out var actionComponent))
                    continue;

                var actionList = f.ResolveList(actionComponent->actionList);
                for (int actionCnt = 0; actionCnt < actionList.Count; actionCnt++)
				{
                    if (actionArgumentList[i].actionNumber != actionCnt)
                        continue;
                    if (false == f.GetActionBehavior(actionList[actionCnt].actionBehavior, out var actionBehavior))
                        continue;

                    actionBehavior.PlayAction(f, actionArgumentList[i], actionList.GetPointer(actionCnt));
                }
            }
        }
    }
}
