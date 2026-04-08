#pragma once
#include "PkwMagicEffect.h"

class PkwAtkMagic : public PkwMagicEffect
{
private:
	POINTF	_centerPos;
	int		_atkUpValue;

	POINTF	_swordPos[4];
	float	_angle[4];
	float	_distance;
	
	int		_frameX;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	PkwAtkMagic();
	~PkwAtkMagic();
};

