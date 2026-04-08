#include "../stdafx.h"
#include "PkwActionProcess.h"
#include "PkwMap.h"
#include "PkwCharactorPlay.h"
PkwActionProcess::PkwActionProcess()
{
}
PkwActionProcess::~PkwActionProcess()
{
}

void PkwActionProcess::Init(PkwCharactorPlay* pCharactorPlay, MapData* mapData)
{
	_pCharactorPlay = pCharactorPlay;
	_mapData = mapData;
	_aStar.init();

	_charactor = NULL;
	_alpaAni = ALPA_UP;
	_alFrame = 0.0f;

	_vCommandPos.clear();
	_vMoveRange.clear();
	_vMovePos.clear();
	_vAtkRangePos.clear();
	_vMigRangePos.clear();
	_vMigScalePos.clear();
	_vSummonRangePos.clear();
}
void PkwActionProcess::release()
{
	_aStar.release();

	_vCommandPos.clear();
	_vMoveRange.clear();
	_vMovePos.clear();
	_vAtkRangePos.clear();
	_vMigRangePos.clear();
	_vMigScalePos.clear();
	_vSummonRangePos.clear();
}
void PkwActionProcess::render(HDC MemDC, CHARACTOR_CONTROL control, RECT gameView)
{
	if (_alpaAni == ALPA_UP){
		_alFrame +=10.0;
		if (_alFrame >= 120.0){ _alFrame = 120.0; _alpaAni = ALPA_DOWN; }
	}
	else if (_alpaAni == ALPA_DOWN){
		_alFrame -= 10.0;
		if (_alFrame <= 0.0){ _alFrame = 0.0; _alpaAni = ALPA_UP; }
	}
	
	_control = control;

	if (_control == CO_CONTROL_OFF){
	}
	else if (_control == CO_CONTROL_CHOICE){
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == CO_CONTROL_MOVE){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vMoveRange.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vMoveRange[i].x * UNITSIZEX, _vMoveRange[i].y * UNITSIZEY, _vMoveRange[i].x * UNITSIZEX, _vMoveRange[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == CO_CONTROL_ATTACK){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vAtkRangePos.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vAtkRangePos[i].x * UNITSIZEX, _vAtkRangePos[i].y * UNITSIZEY, _vAtkRangePos[i].x * UNITSIZEX, _vAtkRangePos[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
	}
	else if (_control == CO_CONTROL_MAGIC){
	}
	else if (_control == CO_CONTROL_MAGIC_CHOICE){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vMigRangePos.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vMigRangePos[i].x * UNITSIZEX, _vMigRangePos[i].y * UNITSIZEY, _vMigRangePos[i].x * UNITSIZEX, _vMigRangePos[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == CO_CONTROL_MAGIC_TARGET_CHOICE){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vMigScalePos.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vMigScalePos[i].x * UNITSIZEX, _vMigScalePos[i].y * UNITSIZEY, _vMigScalePos[i].x * UNITSIZEX, _vMigScalePos[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == CO_CONTROL_MAGIC_POS_CHOICE){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vMigScalePos.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vMigScalePos[i].x * UNITSIZEX, _vMigScalePos[i].y * UNITSIZEY, _vMigScalePos[i].x * UNITSIZEX, _vMigScalePos[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == CO_CONTROL_SUMMON){
	}
	else if (_control == CO_CONTROL_SUMMON_CHOICE){
		IMAGEMANAGER->findImage("backImage")->alphaRender(MemDC, 100);
		for (int i = 0; i < _vSummonRangePos.size(); i++){
			IMAGEMANAGER->findImage("map1")->render(MemDC, _vSummonRangePos[i].x * UNITSIZEX, _vSummonRangePos[i].y * UNITSIZEY, _vSummonRangePos[i].x * UNITSIZEX, _vSummonRangePos[i].y * UNITSIZEY, UNITSIZEX, UNITSIZEY);
		}
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	/*else if (_control == CO_CONTROL_SUMMON_POS_CHOICE){
	}*/
	else if (_control == CO_CONTROL_ORDER){
	}
	else if (_control == SUPPORT_CHOICE){
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_GREEN_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
	else if (_control == ENEMY_CHOICE){
		for (int i = 0; i < _vCommandPos.size(); i++){
			IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(MemDC, _vCommandPos[i].x * UNITSIZEX, _vCommandPos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alFrame);
		}
	}
}
void PkwActionProcess::SearchPos(PkwCharactor* charactor)
{
	_vCommandPos.clear();
	_vMoveRange.clear();

	_vMovePos.clear();
	_vAtkRangePos.clear();
	_vMigRangePos.clear();
	_vMigScalePos.clear();
	_vSummonRangePos.clear();

	_charactor = charactor;
	//_mapData = mapData;
	_alFrame = 0.0f;

	if (_charactor == NULL) return;

	SearchCommandPos(charactor, _charactor->_pos, _charactor->_curCommandRange.value + 1);
	SearchMovePos(charactor, _charactor->_pos, _charactor->_curMove.value);
}
void PkwActionProcess::SearchCommandPos(PkwCharactor* charactor,POINT pos, int range)
{
	POINT temp;
	int num;
	//범위를 지났으면 해당 서치 종료
	if (range <= 0)	return;

	num = pos.x + (pos.y*_mapData->width);
	// 맵을 벗어났을때
	if (pos.x < 0 || pos.x >= _mapData->width){}
	else if (pos.y < 0 || pos.y >= _mapData->height){}
	// 갈수 없는 지역일때
	//else if (_mapData->map[num] == 0){}

	else{
		//벡터에 아무것도 없으면 바로 저장
		if (_vCommandPos.size() == 0)
			_vCommandPos.push_back(pos);
		else{
			//해당 위치 벡터에 저장
			for (int i = 0; i < _vCommandPos.size(); i++){
				//같은 위치가 이미 들어가 있으면 저장을 안한다.
				if (_vCommandPos[i].x == pos.x && _vCommandPos[i].y == pos.y) break;
				//같은 위치가 없으면 해당 서치 저장
				if (i == _vCommandPos.size() - 1)	_vCommandPos.push_back(pos);
			}
		}
		
		int moveVlue = 1;
		//왼쪽 검색
		temp = pos; temp.x -= 1;
		SearchCommandPos(charactor,temp, range - moveVlue);
		//오른쪽 검색
		temp = pos; temp.x += 1;
		SearchCommandPos(charactor,temp, range - moveVlue);
		//위 검색
		temp = pos; temp.y -= 1;
		SearchCommandPos(charactor,temp, range - moveVlue);
		//아래 검색
		temp = pos; temp.y += 1;
		SearchCommandPos(charactor,temp, range - moveVlue);
	}
}

void PkwActionProcess::SearchMovePos(PkwCharactor* charactor, POINT pos, int move)
{
	POINT temp;
	int num;
	bool isPushBack = true;//이게 false이면 pushback 안한다
	// 범위를 지났으면 해당 서치 종료
	if (move <= 0)	return;

	num = pos.x + (pos.y*_mapData->width);

	// 맵을 벗어났을때
	if (pos.x < 0 || pos.x >= _mapData->width){}
	else if (pos.y < 0 || pos.y >= _mapData->height){}
	// 갈수 없는 지역일때
	else if (_mapData->map[num] == 0){}

	//선택된 캐릭터가 보병이고 보병이 갈수 없는 지역일떄
	else if (_charactor->_type == 0 && (_mapData->map[num] == 1 || _mapData->map[num] == 3)){}
	//선택된 캐릭터가 기병이고 기병이 갈수 없는 지역일떄
	else if (_charactor->_type == 1 && (_mapData->map[num] == 1 || _mapData->map[num] == 3)){}
	//선택된 캐릭터가 수병이고 수병이 갈수 없는 지역일떄
	else if (_charactor->_type == 2 && (_mapData->map[num] == 2 || _mapData->map[num] == 3)){}
	//선택된 캐릭터가 비병이고 비병이 갈수 없는 지역일떄
	else if (_charactor->_type == 3 && (_mapData->map[num] == 1 || _mapData->map[num] == 2)){}
	else{
		//선택된 캐릭터가 적군이면 아군이나 지원군 있는 지역은 지나쳐 갈수 없다
		if (_charactor->_armyType == 0 || _charactor->_armyType == 1){
			for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
				if (isPushBack == false) break;
				
				for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						isPushBack = false;
						break;
					}
				}

				temp = _pCharactorPlay->_vCoCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		isPushBack = false;
			}
			for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
				if (isPushBack == false) break;

				for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						isPushBack = false;
						break;
					}
				}

				temp = _pCharactorPlay->_vSupportCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		isPushBack = false;
			}
			for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
				//적군 병력 체크
				for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						return;
					}
				}
				//적군 지휘관 체크
				temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		return;
			}
		}
		//선택된 캐릭터가 아군이나 지원군이면 적군이 있는 지역은 지나쳐 갈수 없다
		else if (_charactor->_armyType == 2){
			for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
				//적군 병력 체크
				for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						return;
					}
				}
				//적군 지휘관 체크
				temp = _pCharactorPlay->_vCoCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		return;
			}
			for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
				//적군 병력 체크
				for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						return;
					}
				}
				//적군 지휘관 체크
				temp = _pCharactorPlay->_vSupportCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		return;
			}
			for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
				if (isPushBack == false) break;

				for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						isPushBack = false;
						break;
					}
				}

				temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
				if (pos.x == temp.x && pos.y == temp.y)		isPushBack = false;
			}
		}

		if (isPushBack == true){
			//벡터에 아무것도 없으면 바로 저장
			if (_vMoveRange.size() == 0){
				_vMoveRange.push_back(pos);
			}
			else{
				//해당 위치 벡터에 저장
				for (int i = 0; i < _vMoveRange.size(); i++){
					//같은 위치가 이미 들어가 있으면 저장 안함
					if (_vMoveRange[i].x == pos.x && _vMoveRange[i].y == pos.y) break;
					//같은 위치가 없으면 해당 서치 저장
					if (i == _vMoveRange.size() - 1) _vMoveRange.push_back(pos);
				}
			}
		}

		int moveVlue = MapMoveTable(pos);

		//왼쪽 검색
		temp = pos; temp.x -= 1;
		SearchMovePos(charactor, temp, move - moveVlue);
		//오른쪽 검색
		temp = pos; temp.x += 1;
		SearchMovePos(charactor, temp, move - moveVlue);
		//위 검색
		temp = pos; temp.y -= 1;
		SearchMovePos(charactor, temp, move - moveVlue);
		//아래 검색
		temp = pos; temp.y += 1;
		SearchMovePos(charactor, temp, move - moveVlue);
	}
}
void PkwActionProcess::movingPosSave(PkwCharactor* choiceUnit, POINT startPos, POINT destPos)
{
	_aStar.setUnitStartEndPos(choiceUnit, startPos, destPos);
	_curPosNum = _aStar._vMovePos.size() - 1;
	_maxPosNum = _aStar._vMovePos.size();
	
	return;
}

void PkwActionProcess::SearchAtkPos(PkwCharactor* charactor, POINT pos, int range)
{
	POINT temp;
	int num;

	// 범위를 지났으면 해당 서치 종료
	if (range < 0)	return;

	num = pos.x + (pos.y*_mapData->width);

	// 맵을 벗어났을때
	if (pos.x < 0 || pos.x >= _mapData->width){}
	else if (pos.y < 0 || pos.y >= _mapData->height){}
	// 갈수 없는 지역일때
	else if (_mapData->map[num] == 0){}

	else{
		//아군이나 지원군이 적군 공격할떄
		if (charactor->_armyType == 0 || charactor->_armyType == 1){
			for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
				if (pos.x == _pCharactorPlay->_vEnemyCommander[i]._pos.x && pos.y == _pCharactorPlay->_vEnemyCommander[i]._pos.y){
					atkPosSave(pos);
				}
				for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						atkPosSave(pos);
					}
				}
			}
		}
		//적군이 아군이나 지원군 공격할떄
		else if (charactor->_armyType == 2){
			for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
				if (pos.x == _pCharactorPlay->_vCoCommander[i]._pos.x && pos.y == _pCharactorPlay->_vCoCommander[i]._pos.y){
					atkPosSave(pos);
				}
				for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						atkPosSave(pos);
					}
				}
			}
			for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
				if (pos.x == _pCharactorPlay->_vSupportCommander[i]._pos.x && pos.y == _pCharactorPlay->_vSupportCommander[i]._pos.y){
					atkPosSave(pos);
				}
				for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
					temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
					if (pos.x == temp.x && pos.y == temp.y){
						atkPosSave(pos);
					}
				}
			}
		}
		
		//선택된 캐릭터의 위치는 저장한다.
		if (charactor->_pos.x == pos.x && charactor->_pos.y == pos.y)
			_vAtkRangePos.push_back(pos);

		int moveVlue = 1;
		//왼쪽 검색
		temp = pos; temp.x -= 1;
		SearchAtkPos(charactor, temp, range - moveVlue);
		//오른쪽 검색
		temp = pos; temp.x += 1;
		SearchAtkPos(charactor, temp, range - moveVlue);
		//위 검색
		temp = pos; temp.y -= 1;
		SearchAtkPos(charactor, temp, range - moveVlue);
		//아래 검색
		temp = pos; temp.y += 1;
		SearchAtkPos(charactor, temp, range - moveVlue);
	}
}
void PkwActionProcess::atkPosSave(POINT pos)// 공격위치 벡터에 이미 있는지 여부 확인하고 넣어주는 함수
{
	if (_vAtkRangePos.size() == 0){
		_vAtkRangePos.push_back(pos);
	}
	else{
		for (int i = 0; i < _vAtkRangePos.size(); i++){
			//같은 위치가 이미 들어가 있으면 저장 안함
			if (_vAtkRangePos[i].x == pos.x && _vAtkRangePos[i].y == pos.y)
				break;
			//같은 위치가 없으면 해당 서치 저장
			if (i == _vAtkRangePos.size() - 1){
				_vAtkRangePos.push_back(pos);
				break;
			}
		}
	}
}

void PkwActionProcess::magicRangePosSave(POINT pos, int range)
{
	POINT temp;

	// 범위를 지났으면 해당 서치 종료
	if (range < 0)	return;

	if (_vMigRangePos.size() == 0){
		_vMigRangePos.push_back(pos);
	}
	else{
		for (int i = 0; i < _vMigRangePos.size(); i++){
			//같은 위치가 이미 들어가 있으면 저장 안함
			if (_vMigRangePos[i].x == pos.x && _vMigRangePos[i].y == pos.y)
				break;
			//같은 위치가 없으면 해당 서치 저장
			if (i == _vMigRangePos.size() - 1){
				_vMigRangePos.push_back(pos);
				break;
			}
		}
	}
	//왼쪽 검색
	temp = pos; temp.x -= 1;
	magicRangePosSave(temp, range - 1);
	//오른쪽 검색
	temp = pos; temp.x += 1;
	magicRangePosSave(temp, range - 1);
	//위 검색
	temp = pos; temp.y -= 1;
	magicRangePosSave(temp, range - 1);
	//아래 검색
	temp = pos; temp.y += 1;
	magicRangePosSave(temp, range - 1);
}
void PkwActionProcess::magicScalePosSave(PkwCharactor* targetUnit)
{
	PkwCharactor* commander;

	if (targetUnit->_number < 1000){
		commander = targetUnit;
	}
	else{
		commander = targetUnit->_commander;
	}

	_vMigScalePos.push_back(commander->_pos);
	for (int i = 0; i < commander->_vArms.size(); i++){
		_vMigScalePos.push_back(commander->_vArms[i]._pos);
	}
}
void PkwActionProcess::magicScalePosSave(POINT pos, int range)
{
	POINT temp;

	// 범위를 지났으면 해당 서치 종료
	if (range < 0)	return;

	if (_vMigScalePos.size() == 0){
		_vMigScalePos.push_back(pos);
	}
	else{
		for (int i = 0; i < _vMigScalePos.size(); i++){
			//같은 위치가 이미 들어가 있으면 저장 안함
			if (_vMigScalePos[i].x == pos.x && _vMigScalePos[i].y == pos.y)
				break;
			//같은 위치가 없으면 해당 서치 저장
			if (i == _vMigScalePos.size() - 1){
				_vMigScalePos.push_back(pos);
				break;
			}
		}
	}
	//왼쪽 검색
	temp = pos; temp.x -= 1;
	magicScalePosSave(temp, range - 1);
	//오른쪽 검색
	temp = pos; temp.x += 1;
	magicScalePosSave(temp, range - 1);
	//위 검색
	temp = pos; temp.y -= 1;
	magicScalePosSave(temp, range - 1);
	//아래 검색
	temp = pos; temp.y += 1;
	magicScalePosSave(temp, range - 1);
}
void PkwActionProcess::summonPosSave(PkwCharactor* choiceUnit)
{
	_vSummonRangePos.clear();

	POINT pos[4],temp;
	int num;

	pos[0].x = choiceUnit->_pos.x - 1;
	pos[0].y = choiceUnit->_pos.y;
	pos[1].x = choiceUnit->_pos.x + 1;
	pos[1].y = choiceUnit->_pos.y;
	pos[2].x = choiceUnit->_pos.x;
	pos[2].y = choiceUnit->_pos.y - 1;
	pos[3].x = choiceUnit->_pos.x;
	pos[3].y = choiceUnit->_pos.y + 1;
	
	bool isFlag;
	for (int i = 0; i < 4; i++){
		
		isFlag = true;

		num = pos[i].x + (pos[i].y*_mapData->width);

		// 맵을 벗어났을때
		if (pos[i].x < 0 || pos[i].x >= _mapData->width){ isFlag = false; }
		else if (pos[i].y < 0 || pos[i].y >= _mapData->height){ isFlag = false; }
		// 갈수 없는 지역일때
		else if (_mapData->map[num] == 0){ isFlag = false; }
		//선택된 캐릭터가 보병이고 보병이 갈수 없는 지역일떄
		else if (choiceUnit->_type == 0 && (_mapData->map[num] == 1 || _mapData->map[num] == 3)){ isFlag = false; }
		//선택된 캐릭터가 기병이고 기병이 갈수 없는 지역일떄
		else if (choiceUnit->_type == 1 && (_mapData->map[num] == 1 || _mapData->map[num] == 3)){ isFlag = false; }
		//선택된 캐릭터가 수병이고 수병이 갈수 없는 지역일떄
		else if (choiceUnit->_type == 2 && (_mapData->map[num] == 2 || _mapData->map[num] == 3)){ isFlag = false; }
		//선택된 캐릭터가 비병이고 비병이 갈수 없는 지역일떄
		else if (choiceUnit->_type == 3 && (_mapData->map[num] == 1 || _mapData->map[num] == 2)){ isFlag = false; }
		else{
			//아군 유닛 있으면 소환 못하는 지역으로
			for (int j = 0; j < _pCharactorPlay->_vCoCommander.size(); j++){
				if (pos[i].x == _pCharactorPlay->_vCoCommander[j]._pos.x && pos[i].y == _pCharactorPlay->_vCoCommander[j]._pos.y){
					isFlag = false;
				}
				for (int k = 0; k < _pCharactorPlay->_vCoCommander[j]._vArms.size(); k++){
					temp = _pCharactorPlay->_vCoCommander[j]._vArms[k]._pos;
					if (pos[i].x == temp.x && pos[i].y == temp.y){
						isFlag = false;
					}
				}
			}
			//지원군 유닛 있으면 소환 못하는 지역으로
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander.size(); j++){
				if (pos[i].x == _pCharactorPlay->_vSupportCommander[j]._pos.x && pos[i].y == _pCharactorPlay->_vSupportCommander[j]._pos.y){
					isFlag = false;
				}
				for (int k = 0; k < _pCharactorPlay->_vSupportCommander[j]._vArms.size(); k++){
					temp = _pCharactorPlay->_vSupportCommander[j]._vArms[k]._pos;
					if (pos[i].x == temp.x && pos[i].y == temp.y){
						isFlag = false;
					}
				}
			}
			//적군 유닛 있으면 소환 못하는 지역으로
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander.size(); j++){
				if (pos[i].x == _pCharactorPlay->_vEnemyCommander[j]._pos.x && pos[i].y == _pCharactorPlay->_vEnemyCommander[j]._pos.y){
					isFlag = false;
				}
				for (int k = 0; k < _pCharactorPlay->_vEnemyCommander[j]._vArms.size(); k++){
					temp = _pCharactorPlay->_vEnemyCommander[j]._vArms[k]._pos;
					if (pos[i].x == temp.x && pos[i].y == temp.y){
						isFlag = false;
					}
				}
			}
		}//else문 종료
		if (isFlag) _vSummonRangePos.push_back(pos[i]);
	}//for문 종료
}
int PkwActionProcess::MapMoveTable(POINT pos)
{
	/*
	_type 0 : 보병, 1 : 기병, 2 : 수병, 3 : 비병
	*/
	int num = pos.x + (pos.y*_mapData->width);
	if (_mapData->map[num] == 1){// 바다 유닛만 갈수 있는 지역
		if (_charactor->_type == 0) return 100;
		else if (_charactor->_type == 1) return 100;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 100;
	}
	else if (_mapData->map[num] == 2){//대지 유닛만 갈수 있는 지역
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 1;
		else if (_charactor->_type == 2) return 100;
		else if (_charactor->_type == 3) return 100;
	}
	else if (_mapData->map[num] == 3){//하늘 유닛만 갈수 있는 지역
		if (_charactor->_type == 0) return 100;
		else if (_charactor->_type == 1) return 100;
		else if (_charactor->_type == 2) return 100;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 10){//초원
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 1;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 11){//자갈 지역
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 1;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 12){//도로
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 1;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 13){//숲
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 2;
		else if (_charactor->_type == 2) return 2;
		else if (_charactor->_type == 3) return 2;
	}
	else if (_mapData->map[num] == 14){//산
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 2;
		else if (_charactor->_type == 2) return 2;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 15){//건물 안
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 2;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 2;
	}
	else if (_mapData->map[num] == 16){//성안
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 2;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 17){//성벽
		if (_charactor->_type == 0) return 1;
		else if (_charactor->_type == 1) return 2;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	else if (_mapData->map[num] == 18){//물지역
		if (_charactor->_type == 0) return 2;
		else if (_charactor->_type == 1) return 3;
		else if (_charactor->_type == 2) return 1;
		else if (_charactor->_type == 3) return 1;
	}
	//여기에 들어오면 안되지만 예외처리 ㄷㄷ
	else return 1;
}
