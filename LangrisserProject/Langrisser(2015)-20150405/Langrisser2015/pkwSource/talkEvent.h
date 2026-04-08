#pragma once

#include "battleEvent.h"

enum TALK_MODE{
	COMMANDER_SERCH,
	COMMANDER_TALK,
	TALK_END
};

struct talkData{
	//자동 초기화 되는 값
	TALK_MODE	mode;
	PkwCharactor* commander;
	POINT pos;
	int curTalkLine;
	int maxTalkLine;
	//받아야할 값
	char commanderName[128];
	char** talk;
};

enum TALK_TERMS{
	COMMANDER_DEATH,
	TURN
};

class talkEvent : public battleEvent
{
public:
	//배틀 맵 씬의 상태 정보 다음 구조체 데이터
	BattleInfo*				_pBattleInfo;

	PkwCharactorPlay*		_pCharactorPlay;
	vector<talkData*>		_vtalkData;
	//이벤트 발동 조건 변수
	TALK_TERMS				_terms;
	char					_deathCommanderName[128];
	int						_nturn;
	BATTLE_STATUS			_battleStatus;
public://기타
	bool					_isOn;
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight, BattleInfo* pBattleInfo);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();

	void setTalkCommanderPos();
	void readTalkTerms(char* str, int nCnt);
	void createTalkData(char* str);
	void readTalkData(int talkNumber, char* str, int nCnt);
public:
	talkEvent();
	~talkEvent();
};

