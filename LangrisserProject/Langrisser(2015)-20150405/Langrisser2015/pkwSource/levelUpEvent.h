#pragma once

#include "battleEvent.h"

class levelUpEvent : public battleEvent
{
private:
	PkwCharactorPlay*		_pCharactorPlay;
	PkwCharactor*			_levelUpCommander;
	bool					_isOn;
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();
public:
	levelUpEvent();
	~levelUpEvent();
};

