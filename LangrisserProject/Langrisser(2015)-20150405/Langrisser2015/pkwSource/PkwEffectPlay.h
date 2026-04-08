#pragma once
#include"../common.h"
#include "PkwMagicEffect.h"
#include "PkwHealEffect.h"
#include "PkwAtkMagic.h"
#include "PkwProtectionMagic.h"
#include "PkwMeteorMagic.h"
#include "PkwFireBallMagic.h"
#include "PkwTeleportMagic.h"
#include "PkwPrayerSummon.h"

class PkwEffectPlay
{
public:
	vector<PkwMagicEffect*>				_vEffect;
	vector<PkwMagicEffect*>::iterator	_viEffect;
public:
	void Init();
	void render(HDC memDC);
	void update();
	void release();

	void AddEffet(PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill);
	int getEffectSize();
public:
	PkwEffectPlay();
	~PkwEffectPlay();
};

