#pragma once
#include"../common.h"
#include "battleEvent.h"
#include "turnChangeEvent.h"
#include "unitDeathEvent.h"
#include "levelUpEvent.h"
#include "classChangeEvent.h"
#include "talkEvent.h"
#include "battleEndEvent.h"

class PkwBattleMapScene;

class PkwEventManager
{
public:
	//배틀 맵 씬의 전반성언 포인터 ㅠㅠ 구조 망함
	PkwBattleMapScene*			_pBttleMapScene;
	//배틀 맵 씬의 상태 정보 다음 구조체 데이터
	BattleInfo*					_pBattleInfo;
	//이벤트 정보 저장 벡터
	vector<battleEvent*>		_vBattleEvent;
	//이벤트가 활성화 되어있는지 여부 변수
	bool _isEventManagerAction;
public:
	HRESULT init(PkwBattleMapScene* pBttleMapScene);
	void release(void);
	void update(float elapsedTime);
	void render(HDC memDC);
	
	//데이터 매니져에서 talk이벤트를 이걸로 넣어준다.
	//배틀 맵이 이걸 통해서 턴 지날 때마다 턴 변환 이벤트를 넣어준다
	void addEvent(int eventType);
	void talkEventCreate();
	void addTalkEvent();
public:
	PkwEventManager();
	~PkwEventManager();
};

