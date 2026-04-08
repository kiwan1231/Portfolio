#include "../stdafx.h"
#include "PkwTeleportMagic.h"


PkwTeleportMagic::PkwTeleportMagic()
{
}
PkwTeleportMagic::~PkwTeleportMagic()
{
}

void PkwTeleportMagic::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
{
	_pCharactorPlay = DATAMANAGER->getCharactorPlay();
	//받은 매개변수 넣어주기
	_vpEffect = vpEffect;
	_attackUnit = attackUnit;
	_targetUnit = targetUnit;
	_targetPos = targetPos;
	_skill = skill;

	memset(_imageKey, 0, 128);
	memcpy(_imageKey, imageKey, strlen(imageKey));

	_frameWidth = frameWidth;
	_frameHeight = frameHeight;
	_frameMax = frameMax;
	_frameDelay = frameDelay;
	_isAlpa = isAlpa;

	//기본 변수 초기화
	_isPlay = true;
	_curFrame = 0;
	_freameTime = 0.0;

	//텔레포트 클래스 변수 초기화
	_vTeleportPos.clear();
	_status = TELEPORT_MEMORIZE;
	_elaspedTime = 0.0f;
	_frameX = 0;
	_waitTime = 1.0f;
}
void PkwTeleportMagic::release()
{
	
}
void PkwTeleportMagic::update()
{
	_elaspedTime += TIMEMANAGER->getElapsedTime();

	if (_status == TELEPORT_MEMORIZE){
		if (_elaspedTime > 0.2){
			_elaspedTime = 0.0;
			_frameX++;
		}
		if (_frameX >= _frameMax){
			_elaspedTime = 0.0;
			_frameX=0;
			teleportStart();
			_status = TELEPORT_WAIT;
		}
	}
	else if (_status == TELEPORT_WAIT){
		if (_elaspedTime > _waitTime){
			_elaspedTime = 0.0;
			unitPosSet();
			_status = TELEPORT_CASTING;
		}
	}
	else if (_status == TELEPORT_CASTING){
		if (_elaspedTime > 0.2){
			_elaspedTime = 0.0;
			_frameX++;
		}
		if (_frameX >= _frameMax){
			_elaspedTime = 0.0;
			_frameX = 0;
			_status = TELEPORT_END;
			_isPlay = false;
		}
	}
}
void PkwTeleportMagic::render(HDC memDC)
{
	PkwCharactor* commander;
	POINT pos;

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}

	if (_status == TELEPORT_MEMORIZE){
		pos.x = (commander->_pos.x * UNITSIZEX) + UNITSIZEX / 2;
		pos.y = (commander->_pos.y * UNITSIZEY) + UNITSIZEY / 2;
		IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x - _frameWidth / 2, pos.y - _frameHeight / 2, _frameX * _frameWidth, 0, _frameWidth, _frameHeight);

		for (int i = 0; i < commander->_vArms.size(); i++){
			pos.x = (commander->_vArms[i]._pos.x * UNITSIZEX) + UNITSIZEX / 2;
			pos.y = (commander->_vArms[i]._pos.y * UNITSIZEY) + UNITSIZEY / 2;
			IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x - _frameWidth / 2, pos.y - _frameHeight / 2, _frameX * _frameWidth, 0, _frameWidth, _frameHeight);
		}
	}
	else if (_status == TELEPORT_CASTING){
		for (int i = 0; i < _vTeleportPos.size(); i++){
			pos.x = (_vTeleportPos[i].x * UNITSIZEX) + UNITSIZEX / 2;
			pos.y = (_vTeleportPos[i].y * UNITSIZEY) + UNITSIZEY / 2;
			IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x - _frameWidth / 2, pos.y - _frameHeight / 2, _frameX * _frameWidth, 0, _frameWidth, _frameHeight);
		}
	}
}
void PkwTeleportMagic::magicEffect()
{
	teleportDest();
}

void PkwTeleportMagic::teleportStart()
{
	PkwCharactor* commander;

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}

	//-1로 둬서 잠시 출력이안되게 한다
	commander->_pos.x = -1;
	commander->_pos.y = -1;

	for (int i = 0; i < commander->_vArms.size(); i++){
		commander->_vArms[i]._pos.x = -1;
		commander->_vArms[i]._pos.y = -1;
	}
}
void PkwTeleportMagic::teleportDest()
{
	PkwCharactor* commander;

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}

	//해당 위치로 세팅
	commander->_pos = _vTeleportPos[0];

	for (int i = 0; i < commander->_vArms.size(); i++){
		commander->_vArms[i]._pos = _vTeleportPos[i+1];
	}
}
void PkwTeleportMagic::unitPosSet()
{
	POINT arrPos[8];
	POINT pos ,temp;
	PkwCharactor* commander;
	int cnt;
	MapData* mapData = DATAMANAGER->_MapData.GetMap();
	//------------------변수 초기화
	arrPos[0].x = -1;	arrPos[0].y = -1;
	arrPos[1].x =  0;	arrPos[1].y = -1;
	arrPos[2].x =  1;	arrPos[2].y = -1;
	arrPos[3].x = -1;	arrPos[3].y =  0;
	arrPos[4].x =  1;	arrPos[4].y =  0;
	arrPos[5].x = -1;	arrPos[5].y =  1;
	arrPos[6].x =  0;	arrPos[6].y =  1;
	arrPos[7].x =  1;	arrPos[7].y =  1;
	
	_vTeleportPos.clear();

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}


	//-----------------위치저장
	bool isAdd;
	int tile;

	_vTeleportPos.push_back(_targetPos);

	for (int i = 0; i < commander->_vArms.size(); i++){//유닛 갯수 만큼 돌림

		cnt = 1;

		while (1){
			for (int j = 0; j < 9; j++){
				isAdd = true;

				pos.x = _targetPos.x + arrPos[j].x * cnt;
				pos.y = _targetPos.y + arrPos[j].y * cnt;

				tile = pos.x + (pos.y * mapData->width);

				// 맵을 벗어났을때
				if (pos.x < 0 || pos.x >= mapData->width){ isAdd = false; }
				else if (pos.y < 0 || pos.y >= mapData->height){ isAdd = false; }
				// 갈수 없는 지역일때
				else if (mapData->map[tile] == 0 || mapData->map[tile] == 1 || mapData->map[tile] == 2 || mapData->map[tile] == 3){ isAdd = false; }
				
				else{
					//벡터에 해당 타일과 같은 값이 있을 경우
					for (int vPos = 0; vPos < _vTeleportPos.size(); vPos++){
						if (pos.x == _vTeleportPos[vPos].x && pos.y == _vTeleportPos[vPos].y){
							isAdd = false;
						}
					}

					// 해당 타일에 아군 유닛이 있을 경우
					for (int coCommander = 0; coCommander < _pCharactorPlay->_vCoCommander.size(); coCommander++){
						temp = _pCharactorPlay->_vCoCommander[coCommander]._pos;
						if (pos.x == temp.x && pos.y == temp.y){
							isAdd = false;
						}
						for (int army = 0; army < _pCharactorPlay->_vCoCommander[coCommander]._vArms.size(); army++){
							temp = _pCharactorPlay->_vCoCommander[coCommander]._vArms[army]._pos;
							if (pos.x == temp.x && pos.y == temp.y){
								isAdd = false;
							}
						}
					}

					// 해당 타일에 지원군 유닛이 있을 경우
					for (int suportCommander = 0; suportCommander < _pCharactorPlay->_vSupportCommander.size(); suportCommander++){
						temp = _pCharactorPlay->_vSupportCommander[suportCommander]._pos;
						if (pos.x == temp.x && pos.y == temp.y){
							isAdd = false;
						}
						for (int army = 0; army < _pCharactorPlay->_vSupportCommander[suportCommander]._vArms.size(); army++){
							temp = _pCharactorPlay->_vSupportCommander[suportCommander]._vArms[army]._pos;
							if (pos.x == temp.x && pos.y == temp.y){
								isAdd = false;
							}
						}
					}

					// 해당 타일에 적군 유닛이 있을 경우
					for (int enemyCommander = 0; enemyCommander < _pCharactorPlay->_vEnemyCommander.size(); enemyCommander++){
						temp = _pCharactorPlay->_vEnemyCommander[enemyCommander]._pos;
						if (pos.x == temp.x && pos.y == temp.y){
							isAdd = false;
						}
						for (int army = 0; army < _pCharactorPlay->_vEnemyCommander[enemyCommander]._vArms.size(); army++){
							temp = _pCharactorPlay->_vEnemyCommander[enemyCommander]._vArms[army]._pos;
							if (pos.x == temp.x && pos.y == temp.y){
								isAdd = false;
							}
						}
					}
				}
				if (isAdd) break;
			}//for(arrPos)
			if (isAdd) break;
			else cnt++;
		}//while문
		_vTeleportPos.push_back(pos);
	}//이동되는 유닛 fo문
}
