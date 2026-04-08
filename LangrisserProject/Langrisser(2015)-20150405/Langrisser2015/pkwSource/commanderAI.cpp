
#include "../stdafx.h"
#include "commanderAI.h"

commanderAI::commanderAI()
{
}
commanderAI::~commanderAI()
{
}

void commanderAI::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	unitAI_Algorism::init(pBttleMapScene, pUnit, action, pActionProcess, pEffectPlay);

	if (pUnit->_vSkill.size() == 0)
		isMagic = false;
	else
		isMagic = true;

	if (pUnit->_vSummon.size() == 0)
		isSummon = false;
	else
		isSummon = true;
}
void commanderAI::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, PkwCharactor* pTargetUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	unitAI_Algorism::init(pBttleMapScene, pUnit, action, pActionProcess, pEffectPlay);

	_pTargetUnit = pTargetUnit;

	if (pUnit->_vSkill.size() == 0)
		isMagic = false;
	else
		isMagic = true;

	if (pUnit->_vSummon.size() == 0)
		isSummon = false;
	else
		isSummon = true;
}
void commanderAI::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, POINT targetPos, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	unitAI_Algorism::init(pBttleMapScene, pUnit, action, pActionProcess, pEffectPlay);

	_targetPos = targetPos;

	if (pUnit->_vSkill.size() == 0)
		isMagic = false;
	else
		isMagic = true;

	if (pUnit->_vSummon.size() == 0)
		isSummon = false;
	else
		isSummon = true;
}
void commanderAI::release()
{
	unitAI_Algorism::release();
}
void commanderAI::update(PkwCharactor* pUnit)
{
	float elasped = TIMEMANAGER->getElapsedTime();

	unitAI_Algorism::update(pUnit);

	if (_unitControl == CONTROL_START){
		updateStart();
	}
	else if (_unitControl == CONTROL_MOVE){
		updateMove(elasped);
	}
	else if (_unitControl == CONTROL_ATK){
		updateAtk();
	}
	else if (_unitControl == CONTROL_MAGIC){
		updateMagic();
	}
	else if (_unitControl == CONTROL_END){
		//행동이 종료되면 값을 바꾸고 다음 유닛으로 넘겨준다
		_pUnit->_action = CHARACTOR_ACTION_OFF;
		//초기화 해준다
		_unitControl = CONTROL_START;
		if (_pUnit->_armyType == 1){
			_pUnit->_AniStatus = LEFT_WALK;
			_pUnit->_AniNum = 1;
		}
		else{
			_pUnit->_AniStatus = RIGHT_WALK;
			_pUnit->_AniNum = 0;
		}
	}
}

//update 관련 함수들
void commanderAI::updateStart()
{
	if (_unitAction == AI_ATK || _unitAction == AI_TARGET_ATK){
		//피가 7 이하면 회복을 한다
		if (_pUnit->_curHp <= 7){
			thisHeal();
			_unitControl = CONTROL_END;
		}
		//피가 7초과 이면 공격 상대를 찾는다
		else{
			//이동 가능한 지점 저장
			_pActionProcess->SearchPos(_pUnit);
			//공격 상대 저장
			searchTargetUnit();
		}
	}
	else if (_unitAction == AI_DEF){
		thisHeal();
		_unitControl = CONTROL_END;
	}
	else if (_unitAction == AI_MAGIC){
		searchMagicPos();
	}
	else if (_unitAction == AI_POS_MOVE || _unitAction == AI_TARGET_MOVE){
		//피가 7 이하면 회복을 한다
		if (_pUnit->_curHp <= 7){
			thisHeal();
			_unitControl = CONTROL_END;
		}
		//피가 7초과 이면 공격 상대를 찾는다
		else{
			//이동 가능한 지점 저장
			_pActionProcess->SearchPos(_pUnit);
			//저장된 지점중에 제일 가까운 지점으로 지점이동을 시킨다
			searchTargetPos();
		}
	}
}
void commanderAI::updateMove(float elapsed)
{
	unitAI_Algorism::updateMove(elapsed);
}
void commanderAI::updateAtk()
{
	if (_unitAction == AI_ATK || _unitAction == AI_TARGET_ATK){
		
		//공격 상대를 찾지 못하고 근처로 이동만 했을 경우(AI_ATK 일경우는 무조건 공격 대상자가 있을 것이다)
		if (_targetUnit == NULL){
			_unitControl = CONTROL_END;
			return;
		}
		//일반 공격
		BattleInfo			battleInfo;
		battleInfo.charactorControl = CO_CONTROL_ATTACK;
		DATAMANAGER->setBmSceneData(battleInfo, _pUnit, NULL, NULL, _targetUnit->pTargetUnit);
		_pBttleMapScene->_callbackFunctionParameter1(_pBttleMapScene->_obj);

		_unitControl = CONTROL_END;
	}
	/*else if (_unitAction == AI_TARGET_ATK){
		
	}*/
	else{//들어오면 안됨 바로 행동종료 시킴
		_unitControl = CONTROL_END;
	}
}
void commanderAI::updateMagic()
{
	//아직 여기는 안들어옴 코딩 안했음
	if (_unitAction == AI_ATK){

	}
	else if (_unitAction == AI_DEF){

	}
	else if (_unitAction == AI_MAGIC){

	}
	else if (_unitAction == AI_POS_MOVE){

	}
	else if (_unitAction == AI_TARGET_MOVE){

	}
	else if (_unitAction == AI_TARGET_ATK){

	}
}

//기능 함수들
void commanderAI::searchTargetUnit()
{
	unitAI_Algorism::searchTargetUnit();

	//공격 가능 유닛이 없을 경우 
	if (_vTargetUnit.size() == 0){

		if (_unitAction == AI_ATK)
			_unitControl = CONTROL_END;
		else if (_unitAction == AI_TARGET_ATK)
			searchTargetPos();
		else//여기는 들어오면 안됨
			_unitControl = CONTROL_END;

		return;
	}

	//가장 피가 없는 유닛을 공격 대상으로 삼는다
	_targetUnit = &_vTargetUnit[0];
	for (int i = 1; i < _vTargetUnit.size(); i++){
		if (_targetUnit->pTargetUnit->_curHp > _vTargetUnit[i].pTargetUnit->_curHp)
			_targetUnit = &_vTargetUnit[i];
	}

	//이동을 시킨다
	_pActionProcess->movingPosSave(_pUnit, _pUnit->_pos, _targetUnit->movePos);
	_unitControl = CONTROL_MOVE;
}
void commanderAI::searchTargetPos()
{
	int xGap,yGap;
	int minDistance = 100;
	POINT minPos;
	POINT targetPos;

	//자기가 가야할 위치 저장
	if (_unitAction == AI_POS_MOVE || _unitAction == AI_MAGIC){
		targetPos = _targetPos;
	}
	//자기 목표인 지휘관 위치 저장
	else if (_unitAction == AI_TARGET_MOVE){
		targetPos = _pTargetUnit->_pos;
	}
	//자기 목표인 적군 지휘관 위치 저장
	else if (_unitAction == AI_TARGET_ATK){
		targetPos = _pTargetUnit->_pos;
	}
	else{//들어 오면 안됨
		_unitControl = CONTROL_END;
		return;
	}

	//이동 가능 지점이 없으면 행동 종료로 한다
	if (_pActionProcess->_vMoveRange.size() == 0){
		_unitControl = CONTROL_END;
		return;
	}

	//위에 저장한 지점고 가장 가까운 이동 가능 지점으로 이동
	for (int i = 0; i < _pActionProcess->_vMoveRange.size(); i++){
		xGap = abs( targetPos.x - _pActionProcess->_vMoveRange[i].x);
		yGap = abs( targetPos.y - _pActionProcess->_vMoveRange[i].y);

		if (minDistance > xGap + yGap){
			minDistance = xGap + yGap;
			minPos = _pActionProcess->_vMoveRange[i];
		}
	}

	//제자리이면 이동을 안시킨다
	if (_pUnit->_pos.x == minPos.x && _pUnit->_pos.y == minPos.y){
		_unitControl = CONTROL_END;
	}
	//이동을 시킨다
	else{
		_pActionProcess->movingPosSave(_pUnit, _pUnit->_pos, minPos);
		_unitControl = CONTROL_MOVE;
	}
}
void commanderAI::thisHeal()
{
	SkillData skill;
	skill._number = -1;
	_pEffectPlay->AddEffet(NULL, _pUnit, _pUnit->_pos, skill);
}
void commanderAI::searchMagicPos()
{
	int range = 5;
	int xGap, yGap;

	_magicPos.x = -1; _magicPos.y = -1;

	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (_magicPos.x != -1 && _magicPos.y != -1) break;

		xGap = abs(_pUnit->_pos.x - _pCharactorPlay->_vSupportCommander[i]._pos.x);
		yGap = abs(_pUnit->_pos.y - _pCharactorPlay->_vSupportCommander[i]._pos.y);
		if (xGap + yGap <= range && _pCharactorPlay->_vSupportCommander[i]._curHp < 10){
			_magicPos = _pCharactorPlay->_vSupportCommander[i]._pos;
			break;
		}

		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			xGap = abs(_pUnit->_pos.x - _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.x);
			yGap = abs(_pUnit->_pos.y - _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.y);
			if (xGap + yGap <= range && _pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp < 10){
				_magicPos = _pCharactorPlay->_vSupportCommander[i]._pos;
				break;
			}
		}
	}

	if (_magicPos.x != -1 && _magicPos.y != -1){
		//마법 발사

		SkillData skill;
		skill._number = 2000;
		skill._atkRange = 5;
		skill._atkScale = 5;
		_pEffectPlay->AddEffet(_pUnit, NULL, _magicPos, skill);
		
		BattleInfo			battleInfo;
		battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
		DATAMANAGER->setBmSceneData(battleInfo, _pUnit, NULL, NULL, NULL);
		_pBttleMapScene->_callbackFunctionParameter1(_pBttleMapScene->_obj);
		
		_unitControl = CONTROL_END;
	}
	else{
		//이동 가능한 지점 저장
		_pActionProcess->SearchPos(_pUnit);
		//저장된 지점중에 제일 가까운 지점으로 지점이동을 시킨다
		searchTargetPos();
	}
}

