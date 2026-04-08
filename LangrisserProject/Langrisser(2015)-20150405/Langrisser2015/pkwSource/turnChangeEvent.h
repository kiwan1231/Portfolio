#pragma once

#include "battleEvent.h"

class turnChangeEvent : public battleEvent
{
private:
	int				_turn;
	BATTLE_STATUS	_status;
public:
	void init(BATTLE_EVENT_TYPE eventType, int turn, BATTLE_STATUS status, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
public:
	turnChangeEvent();
	~turnChangeEvent();
};

