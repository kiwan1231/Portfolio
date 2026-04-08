#include "../stdafx.h"
#include "PkwProtectionMagic.h"


PkwProtectionMagic::PkwProtectionMagic()
{
}


PkwProtectionMagic::~PkwProtectionMagic()
{
}

void PkwProtectionMagic::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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
	_defUpValue = 3;
	_centerPos.x = (_targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
	_centerPos.y = (_targetPos.y * UNITSIZEY) + UNITSIZEY / 2;

	_distance = 300.0f;
	_angle[0] = 0.0;
	_shieldPos[0].x = _centerPos.x + _distance;
	_shieldPos[0].y = _centerPos.y;

	_angle[1] = PI * 0.5;
	_shieldPos[1].x = _centerPos.x;
	_shieldPos[1].y = _centerPos.y + _distance;

	_angle[2] = PI;
	_shieldPos[2].x = _centerPos.x - _distance;
	_shieldPos[2].y = _centerPos.y;


	_angle[3] = PI * 1.5;
	_shieldPos[3].x = _centerPos.x;
	_shieldPos[3].y = _centerPos.y - _distance;
}
void PkwProtectionMagic::release()
{

}
void PkwProtectionMagic::update()
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
		_angle[i] += _elapsedTime * 10;
		_shieldPos[i].x = _centerPos.x + (cosf(_angle[i]) * _distance);
		_shieldPos[i].y = _centerPos.y - (sinf(_angle[i]) * _distance);
	}
}
void PkwProtectionMagic::render(HDC memDC)
{
	for (int i = 0; i < 4; i++){
		IMAGEMANAGER->findImage(_imageKey)->render(memDC, _shieldPos[i].x - _frameWidth / 2, _shieldPos[i].y - _frameHeight / 2, _frameX * _frameWidth, 0, _frameWidth, _frameHeight);
	}
}
void PkwProtectionMagic::magicEffect()
{
	PkwCharactor* commander;

	if (_targetUnit->_number < 1000){
		commander = _targetUnit;
	}
	else{
		commander = _targetUnit->_commander;
	}

	commander->_def += _defUpValue;
	commander->_commandDef += _defUpValue;

	/*for (int i = 0; i < commander->_vArms.size(); i++){
		commander->_vArms[i]._curDef.value += _defUpValue;
		commander->_vArms[i]._curDef.duration += 2;
	}*/
}

