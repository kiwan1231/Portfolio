#pragma once
#include "PkwMagicEffect.h"

class PkwHealEffect : public PkwMagicEffect
{
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	PkwHealEffect();
	~PkwHealEffect();
};

class PkwHeal_1Effect : public PkwMagicEffect
{
private:
	POINTF _centerPos;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();

	//bool isMagicAction(POINT pos);
public:
	PkwHeal_1Effect();
	~PkwHeal_1Effect();
};