#pragma once
#include "PkwMagicEffect.h"

struct prayerEffect{
	POINTF				_barPos;
	float				_angle;
	float				_barElaspedTime;
	int					_barFrameX;
};
class PkwPrayerSummon : public PkwMagicEffect
{
private:
	prayerEffect		_barEffect[4];
	POINTF				_centerPos;
	float				_elaspedTime;
	float				_delayTime;

	int					_squareFrameX;
	float				_squareElaspedTime;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	PkwPrayerSummon();
	~PkwPrayerSummon();
};

