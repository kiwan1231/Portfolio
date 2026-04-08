#include "../stdafx.h"
#include "unitAI_Algorism.h"

unitAI_Algorism::unitAI_Algorism()
{
}
unitAI_Algorism::~unitAI_Algorism()
{
}
void unitAI_Algorism::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, PkwCharactor* pTargetUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	//안씀
}
void unitAI_Algorism::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, POINT targetPos, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	//안씀
}
void unitAI_Algorism::init(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pUnit, AI_UNIT_ACTION action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	_pBttleMapScene = pBttleMapScene;
	_pUnit = pUnit;
	_unitAction = action;
	_pActionProcess = pActionProcess;
	_pEffectPlay = pEffectPlay;

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();

	_unitControl = CONTROL_START;
}
void unitAI_Algorism::release()
{
	_vTargetUnit.clear();
}
void unitAI_Algorism::update(PkwCharactor* pUnit)
{
	_pUnit = pUnit;
}

//update 관렴 함수들
void unitAI_Algorism::updateMove(float elapsed)
{
	POINT temp;
	if (_pActionProcess->_curPosNum < 0){

		//이동하고 ai  상태에 따라 처리 해준다.
		if (_unitAction == AI_MOVE || _unitAction == AI_POS_MOVE || _unitAction == AI_TARGET_MOVE || _unitAction == AI_MAGIC)
			_unitControl = CONTROL_END;
		else if (_unitAction == AI_ATK || _unitAction == AI_TARGET_ATK){
			_unitControl = CONTROL_ATK;
		}
	}
	else{
		_delayTime += elapsed;

		if (_delayTime > 0.1){

			//이동 할 위치를 가져온다
			temp = _pActionProcess->_aStar._vMovePos[_pActionProcess->_curPosNum];

			//이동 위치에 대한 애니메이션 상태를 바꿔준다
			if (temp.x < _pUnit->_pos.x)		_pUnit->_AniStatus = LEFT_WALK;
			else if (temp.x > _pUnit->_pos.x)	_pUnit->_AniStatus = RIGHT_WALK;
			else if (temp.y < _pUnit->_pos.y)	_pUnit->_AniStatus = TOP_WALK;
			else if (temp.y > _pUnit->_pos.y)	_pUnit->_AniStatus = BOTTOM_WALK;

			//이동 시켜준다
			_pUnit->_pos = temp;

			//다음 이동할 위치로 간다
			_pActionProcess->_curPosNum--;

			_delayTime = 0.0;
		}
	}
}

//기능 함수들
void unitAI_Algorism::searchTargetUnit()
{
	POINT pos[4];

	//값 초기화
	_vTargetUnit.clear();
	_targetUnit = NULL;

	//앞뒤 좌우 공격 체크
	pos[0].x = -1 * _pUnit->_curAtkRange.value; pos[0].y = 0;
	pos[1].x = _pUnit->_curAtkRange.value;		pos[1].y = 0;
	pos[2].x = 0;								pos[2].y = -1 * _pUnit->_curAtkRange.value;
	pos[3].x = 0;								pos[3].y = _pUnit->_curAtkRange.value;

	//해당 인원이 지원군 이면 적군을 찾는다
	if (_pUnit->_armyType == 1){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			//저장된 이동 지점에서 공격이 가능한지 체크
			for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
				for (int k = 0; k < 4; k++){
					if (_pCharactorPlay->_vEnemyCommander[i]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
						&& _pCharactorPlay->_vEnemyCommander[i]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
					{
						unitAIaction temp;
						temp.isSkill = false;
						temp.pSkill = NULL;
						temp.movePos = _pActionProcess->_vMoveRange[j];
						temp.pTargetUnit = &_pCharactorPlay->_vEnemyCommander[i];
						_vTargetUnit.push_back(temp);
					}
				}
			}
			for (int i2 = 0; i2 < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); i2++){
				for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
					for (int k = 0; k < 4; k++){
						if (_pCharactorPlay->_vEnemyCommander[i]._vArms[i2]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
							&& _pCharactorPlay->_vEnemyCommander[i]._vArms[i2]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
						{
							unitAIaction temp;
							temp.isSkill = false;
							temp.pSkill = NULL;
							temp.movePos = _pActionProcess->_vMoveRange[j];
							temp.pTargetUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[i2];
							_vTargetUnit.push_back(temp);
						}
					}
				}
			}
		}
	}
	//해당 인원이 적군 이면 아군이나 지원군을 찾는다
	else if (_pUnit->_armyType == 2){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			//저장된 이동 지점에서 공격이 가능한지 체크
			for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
				for (int k = 0; k < 4; k++){
					if (_pCharactorPlay->_vCoCommander[i]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
						&& _pCharactorPlay->_vCoCommander[i]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
					{
						unitAIaction temp;
						temp.isSkill = false;
						temp.pSkill = NULL;
						temp.movePos = _pActionProcess->_vMoveRange[j];
						temp.pTargetUnit = &_pCharactorPlay->_vCoCommander[i];
						_vTargetUnit.push_back(temp);
					}
				}
			}
			for (int i2 = 0; i2 < _pCharactorPlay->_vCoCommander[i]._vArms.size(); i2++){
				for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
					for (int k = 0; k < 4; k++){
						if (_pCharactorPlay->_vCoCommander[i]._vArms[i2]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
							&& _pCharactorPlay->_vCoCommander[i]._vArms[i2]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
						{
							unitAIaction temp;
							temp.isSkill = false;
							temp.pSkill = NULL;
							temp.movePos = _pActionProcess->_vMoveRange[j];
							temp.pTargetUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[i2];
							_vTargetUnit.push_back(temp);
						}
					}
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name,"리아나") == 0) continue;
			if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "시스터") == 0) continue;
			//저장된 이동 지점에서 공격이 가능한지 체크
			for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
				for (int k = 0; k < 4; k++){
					if (_pCharactorPlay->_vSupportCommander[i]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
						&& _pCharactorPlay->_vSupportCommander[i]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
					{
						unitAIaction temp;
						temp.isSkill = false;
						temp.pSkill = NULL;
						temp.movePos = _pActionProcess->_vMoveRange[j];
						temp.pTargetUnit = &_pCharactorPlay->_vSupportCommander[i];
						_vTargetUnit.push_back(temp);
					}
				}
			}
			for (int i2 = 0; i2 < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); i2++){
				for (int j = 0; j < _pActionProcess->_vMoveRange.size(); j++){
					for (int k = 0; k < 4; k++){
						if (_pCharactorPlay->_vSupportCommander[i]._vArms[i2]._pos.x == _pActionProcess->_vMoveRange[j].x + pos[k].x
							&& _pCharactorPlay->_vSupportCommander[i]._vArms[i2]._pos.y == _pActionProcess->_vMoveRange[j].y + pos[k].y)
						{
							unitAIaction temp;
							temp.isSkill = false;
							temp.pSkill = NULL;
							temp.movePos = _pActionProcess->_vMoveRange[j];
							temp.pTargetUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[i2];
							_vTargetUnit.push_back(temp);
						}
					}
				}
			}
		}
	}
}
