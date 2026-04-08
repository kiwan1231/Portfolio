#pragma once
#include "PkwMagicEffect.h"

enum TELEPORT_STATUS{
	TELEPORT_MEMORIZE,
	TELEPORT_WAIT,
	TELEPORT_CASTING,
	TELEPORT_END
};

class PkwTeleportMagic : public PkwMagicEffect
{
private:
	vector<POINT>		_vTeleportPos;
	TELEPORT_STATUS		_status;
	float				_elaspedTime;
	int					_frameX;
	int					_waitTime;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();

	void teleportStart();
	void teleportDest();
	void unitPosSet();
public:
	PkwTeleportMagic();
	~PkwTeleportMagic();
};

