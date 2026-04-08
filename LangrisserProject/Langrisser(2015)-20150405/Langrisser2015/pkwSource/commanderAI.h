#pragma once

#include "unitAI_Algorism.h"

class commanderAI : public unitAI_Algorism
{
private:
	bool isMagic;
	bool isSummon;
	//자기가 할 유닛(ai 예정된 유닛)
	PkwCharactor* _pTargetUnit;
	//char*			_targetUnitName; 나중에 폭발할수 있으니 이름으로 저장할수도 있음
	//자기가 이동할 지점
	POINT		_targetPos;

	//마법 위치
	POINT		_magicPos;
public:
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, PkwCharactor* pTargetUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, POINT targetPos, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void release();
	virtual void update(PkwCharactor* pUnit);
public://update 관련 함수들
	void updateStart();
	virtual void updateMove(float elapsed);
	void updateAtk();
	void updateMagic();
public://기능 함수들
	virtual void searchTargetUnit();
	void searchTargetPos();
	void thisHeal();
	void searchMagicPos();
public:
	commanderAI();
	~commanderAI();
};

