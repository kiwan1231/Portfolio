#pragma once

#include "unitAI_Algorism.h"

class mercenaryAI : public unitAI_Algorism
{
public:
	virtual void init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);
	virtual void release();
	virtual void update(PkwCharactor* pUnit);
public://update관련 함수들
	void updateStart();
	virtual void updateMove(float elapsed);
	void updateMoving();
	void updateAtk();
public://기능 함수들
	virtual void searchTargetUnit();
	void searchCommanderRangePos();
	bool isInCommanderRange();
public:
	mercenaryAI();
	~mercenaryAI();
};

