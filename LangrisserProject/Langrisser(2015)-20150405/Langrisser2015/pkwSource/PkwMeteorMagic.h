#pragma once
#include "PkwMagicEffect.h"

enum METEOR_STATUS{
	METEOR_WAIT,
	METEOR_START,
	METEOR_COLLISION,
	METEOR_END
};
struct  meteorObj{
	POINTF			pos;
	float			lifeTime;
	float			waitTime;
	float			aniTime;
	float			elapseTime;
	METEOR_STATUS	status;
	int				_frameX;

	float			_distance;
};

class PkwMeteorMagic : public PkwMagicEffect
{
private:
	meteorObj		_meteorObj[10];
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	PkwMeteorMagic();
	~PkwMeteorMagic();
};

