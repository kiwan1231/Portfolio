#pragma once

#include "battleEvent.h"

struct deathEffect{
	POINTF	pos;
	int		frameX;
	float	delay;
	bool	isEnd;
};

class unitDeathEvent : public battleEvent
{
private:
	PkwCharactorPlay*			_pCharactorPlay;
	vector<POINT>				_vDeathUnit;
	vector<deathEffect>			_vDeathEffect;
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();

	void addEffect(POINT pos);
public:
	unitDeathEvent();
	~unitDeathEvent();
};

