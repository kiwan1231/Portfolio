#pragma once

#include "PkwActionProcess.h"
#include "PkwEffectPlay.h"
#include "PkwBattleMapScene.h"
enum AI_UNIT_ACTION{
	//일반 적인 AI
	AI_STOP,//아무것도 안하고 가만히 있는다(수동)
	AI_MOVE,//이동하고 멈춘다(전투)
	AI_ATK,//이동안하고 공격한다(돌격)
	AI_DEF,//방어만 한다
	//지휘관만 쓰는 AI
	AI_MAGIC,//가만히 있으면서 마법만 쓰고 마나가 다 달으면 치료 한다
	AI_POS_MOVE,//특정 지점으로 이동을 한다
	AI_TARGET_MOVE,//특정 지휘관을 향해 이동한다(char형으로 이름을 저장해 검색한다)
	AI_TARGET_ATK//특정 지휘관을 공격한다(char형으로 이름을 저장해 검색한다)
};

enum AI_UNIT_TYPE{
	AI_UNIT_COMMANDER,// 지휘관일 경우
	AI_UNIT_MERCENARY// 용병이나 소환수 일 경우
};

enum AI_UNIT_CONTROL{
	CONTROL_START,
	CONTROL_MOVE,
	CONTROL_ATK,
	CONTROL_MAGIC,
	CONTROL_END
};

struct unitAIaction{
	//스킬 관련 변수
	bool					isSkill;//스킬 쓰는지 여부
	SkillData*				pSkill;//쓰는 스킬 포인터
	//기본 변수
	PkwCharactor*			pTargetUnit;//공격 하거나 마법대상자 유닛
	POINT					pTargetPos;//마법 대상자 위치
	POINT					movePos;// 해당 유닛이 이동하는 위치
};
class unitAI_Algorism
{
protected:
	//배틀 맵 씬의 전반성언 포인터 ㅠㅠ 구조 망함
	PkwBattleMapScene*			_pBttleMapScene;

	PkwActionProcess*		_pActionProcess;
	PkwEffectPlay*			_pEffectPlay;
	PkwCharactorPlay*		_pCharactorPlay;

	AI_UNIT_ACTION			_unitAction;
	AI_UNIT_TYPE			_unitType;
	AI_UNIT_CONTROL			_unitControl;

	PkwCharactor*			_pUnit;
	//공격 가능한 유닛들 벡터
	vector<unitAIaction>	_vTargetUnit;
	//위에 중에 선택한 유닛
	unitAIaction*			_targetUnit;
	
	float					_delayTime;
public:
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, PkwCharactor* pTargetUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, POINT targetPos, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void release();
	virtual void update(PkwCharactor* pUnit);
public://update관련 함수들
	virtual void updateMove(float elapsed);
public:
	//기능 함수들
	virtual void searchTargetUnit();
public:
	
public:
	unitAI_Algorism();
	~unitAI_Algorism();
};

