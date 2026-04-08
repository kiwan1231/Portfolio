#pragma once
#include "PkwMagicEffect.h"
class PkwProtectionMagic : public PkwMagicEffect
{
private:
	POINTF	_centerPos;
	int		_defUpValue;

	POINTF	_shieldPos[4];
	float	_angle[4];
	float	_distance;

	int		_frameX;
public:
	PkwProtectionMagic();
	~PkwProtectionMagic();
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
};

