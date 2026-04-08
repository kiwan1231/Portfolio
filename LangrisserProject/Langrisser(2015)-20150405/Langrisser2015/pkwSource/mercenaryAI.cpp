#include "../stdafx.h"
#include "mercenaryAI.h"

mercenaryAI::mercenaryAI()
{
}
mercenaryAI::~mercenaryAI()
{
}

void mercenaryAI::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	unitAI_Algorism::init(pBttleMapScene, pUnit, action, pActionProcess, pEffectPlay);
}
void mercenaryAI::release()
{
	unitAI_Algorism::release();
}
void mercenaryAI::update(PkwCharactor* pUnit)
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
	else if (_unitControl == CONTROL_END){
		//행동이 종료되면 값을 바꾸고 다음 유닛으로 넘겨준다
		_pUnit->_action = CHARACTOR_ACTION_OFF;

		//초기화 해준다
		_unitControl = CONTROL_START;

		if (_pUnit->_armyType == 1)
			_pUnit->_AniStatus = LEFT_WALK;
		else
			_pUnit->_AniStatus = RIGHT_WALK;
	}
}

//update 관렴 함수들
void mercenaryAI::updateStart()
{
	if (_unitAction == AI_STOP){
		_unitControl = CONTROL_END;
	}
	else if (_unitAction == AI_MOVE){
		_pActionProcess->SearchPos(_pUnit);
		searchCommanderRangePos();
	}
	else if (_unitAction == AI_ATK){
		//이동 가능한 지점 저장
		_pActionProcess->SearchPos(_pUnit);

		if( isInCommanderRange())//지휘범위 안에 있으면 공격 상대 저장
			searchTargetUnit();
		else
			searchCommanderRangePos();
	}
	else if (_unitAction == AI_DEF){
		_unitControl = CONTROL_END;
	}
}
void mercenaryAI::updateMove(float elapsed)
{
	unitAI_Algorism::updateMove(elapsed);
}
void mercenaryAI::updateAtk()
{
	if (_unitAction != AI_ATK) return;

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

//기능 함수들
void mercenaryAI::searchTargetUnit()
{
	unitAI_Algorism::searchTargetUnit();

	//공격 가능 유닛이 없을 경우 
	if (_vTargetUnit.size() == 0){
		_unitControl = CONTROL_END;
		return;
	}

	int xGap, yGap;
	int minDistance = 100;
	POINT commanderPos = _pUnit->_commander->_pos;
	int commanderRange = _pUnit->_commander->_commandRange;

	//가장 피가 없는 유닛을 공격 대상으로 삼는다
	_targetUnit = &_vTargetUnit[0];
	for (int i = 1; i < _vTargetUnit.size(); i++){
		
		//지휘범위 안이 아니면 거기로 가지 않는다
		xGap = abs(_vTargetUnit[i].movePos.x - commanderPos.x);
		yGap = abs(_vTargetUnit[i].movePos.y - commanderPos.y);

		if (xGap + yGap > commanderRange) continue;

		else{
			//더 피자 적으면 그걸 공격
			if (_targetUnit->pTargetUnit->_curHp >= _vTargetUnit[i].pTargetUnit->_curHp)

				_targetUnit = &_vTargetUnit[i];
		}
	}

	//이동을 시킨다
	_pActionProcess->movingPosSave(_pUnit, _pUnit->_pos, _targetUnit->movePos);
	_unitControl = CONTROL_MOVE;
}
void mercenaryAI::searchCommanderRangePos()
{
	int xGap, yGap;
	int minDistance = 100;
	POINT minPos;
	POINT targetPos;

	//지휘관 위치 저장
	targetPos = _pUnit->_commander->_pos;

	//이동 가능 지점이 없으면 행동 종료로 한다
	if (_pActionProcess->_vMoveRange.size() == 0){
		_unitControl = CONTROL_END;
		return;
	}

	//위에 저장한 지점고 가장 가까운 이동 가능 지점으로 이동
	for (int i = 0; i < _pActionProcess->_vMoveRange.size(); i++){
		xGap = abs(targetPos.x - _pActionProcess->_vMoveRange[i].x);
		yGap = abs(targetPos.y - _pActionProcess->_vMoveRange[i].y);

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
bool mercenaryAI::isInCommanderRange()
{
	int xGap, yGap;
	POINT commanderPos = _pUnit->_commander->_pos;
	int commanderRange = _pUnit->_commander->_commandRange;

	xGap = abs(_pUnit->_pos.x - commanderPos.x);
	yGap = abs(_pUnit->_pos.y - commanderPos.y);

	if (xGap + yGap > commanderRange) return false;
	else								return true;
}