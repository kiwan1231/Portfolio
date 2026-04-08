#include "../stdafx.h"
#include "PkwPrayerSummon.h"

PkwPrayerSummon::PkwPrayerSummon()
{
}
PkwPrayerSummon::~PkwPrayerSummon()
{
}

void PkwPrayerSummon::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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

	//프레이어 서몬 클래스 변수 초기화
	_centerPos.x = (targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
	_centerPos.y = (targetPos.y * UNITSIZEY) + UNITSIZEY / 2;
	_elaspedTime = 0.0;
	_delayTime = 4.0;
	_squareFrameX = 0;
	_squareElaspedTime = 0.0;

	for (int i = 0; i < 4; i++){
		_barEffect[i]._angle = PI / 2.0 * i;
		_barEffect[i]._barFrameX = 0;
		_barEffect[i]._barElaspedTime = 0.0;
	}
}
void PkwPrayerSummon::release()
{

}
void PkwPrayerSummon::update()
{
	float elaspedTime = TIMEMANAGER->getElapsedTime();

	_elaspedTime += elaspedTime;

	if (_elaspedTime > _delayTime)
		_isPlay = false;
	else{
		//4개의 바 업데이트
		for (int i = 0; i < 4; i++){
			_barEffect[i]._angle += elaspedTime*10;
			_barEffect[i]._barElaspedTime += elaspedTime;
			if (_barEffect[i]._barElaspedTime > 0.01){
				_barEffect[i]._barElaspedTime = 0.0;
				_barEffect[i]._barFrameX++;
			}
			if (_barEffect[i]._barFrameX >= 4) _barEffect[i]._barFrameX = 0;

			_barEffect[i]._barPos.x = _centerPos.x + cosf(_barEffect[i]._angle) * 32;
			_barEffect[i]._barPos.y = _centerPos.y - sinf(_barEffect[i]._angle) * 32;
		}
		//마법진 업데이트
		_squareElaspedTime += elaspedTime;
		if (_squareElaspedTime > 0.1){
			_squareElaspedTime = 0.0;
			_squareFrameX++;
		}
		if (_squareFrameX >= 6) _squareFrameX = 0;
	}
}
void PkwPrayerSummon::render(HDC memDC)
{
	//마법진 그리기
	IMAGEMANAGER->findImage("summonEffect2")->render(memDC, _centerPos.x - 32, _centerPos.y - 32, _squareFrameX * 64,0,64,64);
	
	//캐릭터 뒤에 바 그리기
	for (int i = 0; i < 4; i++){
		if (_barEffect[i]._barPos.y >= _centerPos.y) continue;

		IMAGEMANAGER->findImage("summonEffect3")->render(memDC, _barEffect[i]._barPos.x - 8, _barEffect[i]._barPos.y - 64, _barEffect[i]._barFrameX * 16, 0, 16, 64);
	}

	//캐릭터 그리기
	IMAGEMANAGER->findImage(_imageKey)->alphaRender(memDC, _centerPos.x - 30, _centerPos.y - 32, 180, 0, 60, 64, _elaspedTime * 62);
	
	//캐릭터 앞에 바 그리기
	for (int i = 0; i < 4; i++){
		if (_barEffect[i]._barPos.y < _centerPos.y) continue;

		IMAGEMANAGER->findImage("summonEffect3")->render(memDC, _barEffect[i]._barPos.x - 8, _barEffect[i]._barPos.y - 64, _barEffect[i]._barFrameX * 16, 0, 16, 64);
	}
}
void PkwPrayerSummon::magicEffect()
{
	PkwCharactor charactor;
	SummonData summon;

	for (int i = 0; i < DATAMANAGER->_DataBase._vSummonData.size(); i++){
		if (DATAMANAGER->_DataBase._vSummonData[i]._number == _skill._number){
			summon = DATAMANAGER->_DataBase._vSummonData[i];
			break;
		}
	}

	charactor.summonSet(_attackUnit, summon, _targetPos, _imageKey);
	_attackUnit->_vArms.push_back(charactor);
}
