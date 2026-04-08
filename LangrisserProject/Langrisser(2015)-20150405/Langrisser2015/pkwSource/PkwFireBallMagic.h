#pragma once
#include "PkwMagicEffect.h"

enum FIREBALL_STATUS{
	FIREBALL_UPSPEED,
	FIREBALL_DOWNSPEED,
	FIREBALL_WAIT,
	FIREBALL_ATTACK,
	FIREBALL_END
};

struct  fireballObj{
	FIREBALL_STATUS	status;
	POINTF			pos[4];
	float			angle;
	float			speed;
	float			curDistance;
	float			maxDistance;
	float			waitTime;
};

class PkwFireBallMagic : public PkwMagicEffect
{
private:
	fireballObj		_fireballObj[8];
	bool			_isAttack;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	PkwFireBallMagic();
	~PkwFireBallMagic();
};