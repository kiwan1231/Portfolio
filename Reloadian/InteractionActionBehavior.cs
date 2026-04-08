

using UnityEngine;
using System.Collections.Generic;
using Photon.Deterministic;
using System;

namespace Quantum
{
    public abstract unsafe partial class InteractionActionBehavior : AssetObject
    {
        public unsafe virtual void OnKCC2DTriggerEnter(Frame f, EntityRef entity, EntityRef triggerRef, InteractionAction* interactionAction)
        {
        }
        public unsafe virtual void OnKCC2DTriggerStay(Frame f, EntityRef entity, EntityRef triggerRef, InteractionAction* interactionAction)
        {
        }
        public unsafe virtual void OnKCC2DTriggerExit(Frame f, EntityRef entity, EntityRef triggerRef, InteractionAction* interactionAction)
        {
        }

        public unsafe virtual void OnKCC2DSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, Int32 iteration, InteractionAction* interactionAction)
        {
        }
        public unsafe virtual void OnKCC2DPreCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, InteractionAction* interactionAction)
        {
        }
        public unsafe virtual void OnKCC2DPostSolverCollision(Frame f, EntityRef entity, KCC2D* kcc, KCC2DSettings settings, KCCQueryResult* collision, InteractionAction* interactionAction)
        {
        }

        public unsafe virtual void InitAction(Frame f, EntityRef actionRef, InteractionAction* interactionAction)
        {
            interactionAction->init = true;
        }
        public unsafe virtual void PlayAction(Frame f, InteractionActionArgument actionArgument, InteractionAction* interactionAction) { }
        public unsafe virtual void PlayAction(Frame f, EntityRef actionRef, InteractionAction* interactionAction) { }
        public unsafe virtual void PlayAction(Frame f, EntityRef actionRef, int actionNumber, int paramInt, EntityRef paramRef, InteractionAction* interactionAction) { }
        public unsafe virtual void UpdateAction(Frame f, EntityRef actionRef, InteractionAction* interactionAction) { }

        public unsafe virtual void SetDisableInteraction(Frame f, EntityRef actionRef, InteractionAction* interactionAction, bool disableInteraction)
        {
            interactionAction->disableInteraction = disableInteraction;
        }
    }
}

