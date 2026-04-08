#include "../stdafx.h"
#include "PkwAtkMagic.h"


PkwAtkMagic::PkwAtkMagic()
{
}


PkwAtkMagic::~PkwAtkMagic()
{
}

void PkwAtkMagic::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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
	_atkUpValue = 3;
	_centerPos.x = (_targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
	_centerPos.y = (_targetPos.y * UNITSIZEY) + UNITSIZEY / 2;

	_distance = 300.0f;
	_angle[0] = 0.0;
	_swordPos[0].x = _centerPos.x + _distance;
	_swordPos[0].y = _centerPos.y;

	_angle[1] = PI * 0.5;
	_swordPos[1].x = _centerPos.x;
	_swordPos[1].y = _centerPos.y + _distance;

	_angle[2] = PI;
	_swordPos[2].x = _centerPos.x - _distance;
	_swordPos[2].y = _centerPos.y;


	_angle[3] = PI * 1.5;
	_swordPos[3].x = _centerPos.x;
	_swordPos[3].y = _centerPos.y - _distance;
}
void PkwAtkMagic::release()
{

}
void PkwAtkMagic::update()
{
	_elapsedTime = TIMEMANAGER->getElapsedTime();

	//시간 지남에 따라 거리가 줄어든다	
	_distance -= _elapsedTime * 100;
	if (_distance < 3)
		_isPlay = false;
	else if (_distance < 5)
		_frameX = 0;
	else if (_distance < 10)
		_frameX = 1;
	else if (_distance < 15)
		_frameX = 2;
	else
		_frameX = 3;

	//타겟 유닛을 중심으로 계속 돈다
	for (int i = 0; i < 4; i++){
		_angle[i] += _elapsedTime*10;
		_swordPos[i].x = _centerPos.x + ( cosf(_angle[i]) * _distance );
		_swordPos[i].y = _centerPos.y - ( sinf(_angle[i]) * _distance );
	}
}
void PkwAtkMagic::render(HDC memDC)
{
	for (int i = 0; i < 4; i++){
		IMAGEMANAGER->findImage(_imageKey)->render(memDC, _swordPos[i].x - _frameWidth / 2, _swordPos[i].y - _frameHeight/2, _frameX * _frameWidth, 0, _frameWidth, _frameHeight);
	}
}
void PkwAtkMagic::magicEffect()
{
	PkwCharactor* commander;

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}

	commander->_atk += _atkUpValue;
	commander->_commandAtk += _atkUpValue;

	/*for (int i = 0; i < commander->_vArms.size(); i++){
		commander->_vArms[i]._curAtk.value += _atkUpValue;
		commander->_vArms[i]._curAtk.duration += 2;
	}*/
}