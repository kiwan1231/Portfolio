#pragma once

#include "battleEvent.h"

enum BATTLEEND_MODE{
	BATTLE_GAME_ING,
	BATTLE_GAME_CLEAR,
	BATTLE_GAME_OVER
};

class battleEndEvent : public battleEvent
{
private:
	PkwCharactorPlay*		_pCharactorPlay;
	//해당 이름의 유닛이 죽으면 게임 클리어
	char					_clearCommanderName[128];
	//해당 이름의 유닛이 죽으면 게임 오버
	char					_overCommanderName[128];

	BATTLEEND_MODE			_mode;

	BattleInfo*				_pBattleInfo;

	bool					_isStart;
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight, BattleInfo* pBattleInfo);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();
public:
	battleEndEvent();
	~battleEndEvent();
};

