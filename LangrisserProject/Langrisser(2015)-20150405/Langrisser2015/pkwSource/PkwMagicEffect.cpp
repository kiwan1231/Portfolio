#include "../stdafx.h"
#include "PkwMagicEffect.h"


PkwMagicEffect::PkwMagicEffect()
{
}


PkwMagicEffect::~PkwMagicEffect()
{
}

void PkwMagicEffect::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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
}
void PkwMagicEffect::release()
{

}
void PkwMagicEffect::update()
{
	_freameTime += TIMEMANAGER->getElapsedTime();

	if (_freameTime > _frameDelay){
		_curFrame++;
		if (_curFrame >= _frameMax){
			_isPlay = false;
			_curFrame = _frameMax - 1;
		}
		_freameTime = 0;
	}
}
void PkwMagicEffect::render(HDC memDC)
{
	IMAGEMANAGER->findImage(_imageKey)->render(memDC);
}

void PkwMagicEffect::magicEffect()
{

}
//-----------------------------------------
// get, set 함수들
//-----------------------------------------
bool PkwMagicEffect::getIsPlay()
{
	return _isPlay;
}

bool PkwMagicEffect::isMagicAction(POINT pos)
{
	int maigcSize = _skill._atkScale;
	POINT gap;
	gap.x = _targetPos.x - pos.x;
	gap.y = _targetPos.y - pos.y;
	if (gap.x < 0) gap.x *= -1;
	if (gap.y < 0) gap.y *= -1;

	if (maigcSize >= gap.x + gap.y)
		return true;
	else
		return false;
}
void PkwMagicEffect::magicScaleInit(POINT pos, int range)
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
	magicScaleInit(temp, range - 1);
	//오른쪽 검색
	temp = pos; temp.x += 1;
	magicScaleInit(temp, range - 1);
	//위 검색
	temp = pos; temp.y -= 1;
	magicScaleInit(temp, range - 1);
	//아래 검색
	temp = pos; temp.y += 1;
	magicScaleInit(temp, range - 1);
}
void PkwMagicEffect::magicScaleUpdate(float elaspedTime)
{
	if (_alpaValue == 0.0){
		_alpaValue = 100.0;
	}
	else if (_alpaValue == 100.0){
		_alpaValue = 0.0;
	}
}
void PkwMagicEffect::magicScaleRender(HDC memDC)
{
	for (int i = 0; i < _vMigScalePos.size(); i++){
		IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(memDC, _vMigScalePos[i].x * UNITSIZEX, _vMigScalePos[i].y * UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY, _alpaValue);
	}
}

void PkwMagicEffect::explosionInit()
{
	_explosionCnt = 0;

	for (int i = 0; i < 9; i++){
		if (i == 0){
			_explisionObj[i].pos.x = (_targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = (_targetPos.y * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 1){
			_explisionObj[i].pos.x = ( (_targetPos.x +1) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ( (_targetPos.y -1) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 2){
			_explisionObj[i].pos.x = ((_targetPos.x - 1) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y - 1) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 3){
			_explisionObj[i].pos.x = ((_targetPos.x - 1) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y + 1) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 4){
			_explisionObj[i].pos.x = ((_targetPos.x + 1) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y + 1) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 5){
			_explisionObj[i].pos.x = ((_targetPos.x + 2) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 6){
			_explisionObj[i].pos.x = ((_targetPos.x) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y - 2) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 7){
			_explisionObj[i].pos.x = ((_targetPos.x - 2) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y) * UNITSIZEY) + UNITSIZEY / 2;
		}
		else if (i == 8){
			_explisionObj[i].pos.x = ((_targetPos.x) * UNITSIZEX) + UNITSIZEX / 2;
			_explisionObj[i].pos.y = ((_targetPos.y + 2) * UNITSIZEY) + UNITSIZEY / 2;
		}
		_explisionObj[i].curFrameY = 0;
		_explisionObj[i].elaspedTime = 0.0;
		_explisionObj[i].status = EXPLOSION_WAIT;
		_explisionObj[i].waitTime = i*0.1f;
	}
}
void PkwMagicEffect::explosionUpdate(float elaspedTime)
{
	_explosionCnt = 0;

	for (int i = 0; i < 9; i++){
		if (_explisionObj[i].status == EXPLOSION_WAIT){
			if (_explisionObj[i].waitTime < 0.0)
				_explisionObj[i].status = EXPLOSION_START;
			else
				_explisionObj[i].waitTime -= elaspedTime;
		}
		else if (_explisionObj[i].status == EXPLOSION_START){
			if (_explisionObj[i].curFrameY >= _frameMax)
				_explisionObj[i].status = EXPLOSION_END;
			else{
				_explisionObj[i].elaspedTime += elaspedTime;
				if (_explisionObj[i].elaspedTime > 0.1){
					_explisionObj[i].elaspedTime = 0.0;
					_explisionObj[i].curFrameY++;
				}
			}
		}
		else if (_explisionObj[i].status == EXPLOSION_END){
			_explosionCnt++;
		}
	}
}
void PkwMagicEffect::explisionRender(HDC memDC)
{
	image* explision = IMAGEMANAGER->findImage("effectExplosion");
	int frameWidth = explision->getFrameWidth();
	int frameHeight = explision->getFrameHeight();
	POINT pos;

	for (int i = 0; i < 9; i++){
		if (_explisionObj[i].status == EXPLOSION_START){
			pos.x = _explisionObj[i].pos.x - frameWidth/2;
			pos.y = _explisionObj[i].pos.y - frameHeight/2;
			explision->render(memDC, pos.x, pos.y, 0, _explisionObj[i].curFrameY * frameHeight, frameWidth, frameHeight);
		}
	}
}