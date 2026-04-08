#pragma once

enum BATTLE_EVENT_STATUS{
	EVENT_ACTIVE,
	EVENT_DEACTIVE,
	EVENT_END
};

enum BATTLE_EVENT_TYPE{
	TURN_CHANGE_EVENT,
	DEATH_EVENT,
	LEVEL_UP_EVENT,
	CLASS_CHANGE_EVENT,
	TALK_EVENT,
	GAMEOVER_EVENT,
	GAMECLEAR_EVENT
};

class battleEvent
{
public:
	BATTLE_EVENT_STATUS		_eventStatus;
	BATTLE_EVENT_TYPE		_eventType;
public://업데이트 변수
	float					_delayTime;
public://렌더 변수
	POINT*					_pMapStartPos;
	int						_MapMaxWidth;
	int						_MapManHeight;
	HFONT					_font[10];
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();
public:
	battleEvent();
	~battleEvent();
};

