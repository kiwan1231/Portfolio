#include "../stdafx.h"
#include "PkwFireBallMagic.h"


PkwFireBallMagic::PkwFireBallMagic()
{
}


PkwFireBallMagic::~PkwFireBallMagic()
{
}

void PkwFireBallMagic::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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

	_vMigScalePos.clear();
	magicScaleInit(_targetPos, _skill._atkScale);
	explosionInit();


	_isAttack = false;
	for (int i = 0; i < 8; i++){
		_fireballObj[i].status = FIREBALL_UPSPEED;
		//_fireballObj[i].pos.x = (attackUnit->_pos.x * UNITSIZEX) + UNITSIZEX / 2;
		//_fireballObj[i].pos.y = (attackUnit->_pos.y * UNITSIZEY) + UNITSIZEY / 2;
		_fireballObj[i].angle = PI4 * i;
		_fireballObj[i].speed = 0.0f;
		_fireballObj[i].waitTime = 0.5f;
		_fireballObj[i].curDistance = 0.0f;
		_fireballObj[i].maxDistance = 180.0f;
	}
}
void PkwFireBallMagic::release()
{

}
void PkwFireBallMagic::update()
{
	POINTF start,target;
	int cnt = 0;

	start.x = (_attackUnit->_pos.x * UNITSIZEX) + UNITSIZEX / 2;
	start.y = (_attackUnit->_pos.y * UNITSIZEY) + UNITSIZEY / 2;

	target.x = (_targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
	target.y = (_targetPos.y * UNITSIZEY) + UNITSIZEY / 2;

	float elaspedTime = TIMEMANAGER->getElapsedTime();

	for (int i = 0; i < 8; i++){
		if (_fireballObj[i].status == FIREBALL_UPSPEED){
			if (_fireballObj[i].curDistance >= _fireballObj[i].maxDistance){
				_fireballObj[i].status = FIREBALL_DOWNSPEED;
			}
			else{
				_fireballObj[i].curDistance += elaspedTime * 50.0f;
				_fireballObj[i].speed += elaspedTime;
				if (_fireballObj[i].speed > 0.1) _fireballObj[i].speed = 0.1;
				_fireballObj[i].angle -= _fireballObj[i].speed;
			}
		}
		else if (_fireballObj[i].status == FIREBALL_DOWNSPEED){
			if (_fireballObj[i].speed <= 0.0){
				_fireballObj[i].speed = 0.0f;
				_fireballObj[i].status = FIREBALL_WAIT;
			}
			else{
				_fireballObj[i].speed -= elaspedTime;
				if (_fireballObj[i].speed < 0)		_fireballObj[i].speed = 0.0;
				_fireballObj[i].angle -= _fireballObj[i].speed;
			}
		}
		else if (_fireballObj[i].status == FIREBALL_WAIT){
			if (_fireballObj[i].waitTime <= 0.0){
				_fireballObj[i].speed = 30.0f;
				_fireballObj[i].status = FIREBALL_ATTACK;
			}
			else{
				_fireballObj[i].waitTime -= elaspedTime;
			}
		}
		else if (_fireballObj[i].status == FIREBALL_ATTACK){
			
			_fireballObj[i].curDistance = MY_UTIL::getDistance(_fireballObj[i].pos[0].x, _fireballObj[i].pos[0].y, target.x, target.y);
			
			if (_fireballObj[i].curDistance <= 20){
				_fireballObj[i].status = FIREBALL_END;
			}
			else{
				_fireballObj[i].angle = MY_UTIL::getAngle(_fireballObj[i].pos[0].x, _fireballObj[i].pos[0].y, target.x, target.y);
				for (int j = 0; j < 4; j++){
				
					if (j == 0){
						_fireballObj[i].pos[j].x += cosf(_fireballObj[i].angle) * _fireballObj[i].speed;
						_fireballObj[i].pos[j].y -= sinf(_fireballObj[i].angle) * _fireballObj[i].speed;
					}
					else{
						_fireballObj[i].pos[j].x = _fireballObj[i].pos[0].x - cosf(_fireballObj[i].angle) * _fireballObj[i].speed * j;
						_fireballObj[i].pos[j].y = _fireballObj[i].pos[0].y + sinf(_fireballObj[i].angle) * _fireballObj[i].speed * j;
					}
				}
			}
		}
		else if (_fireballObj[i].status == FIREBALL_END){
			cnt++;
		}

		if (_fireballObj[i].status != FIREBALL_ATTACK && _fireballObj[i].status != FIREBALL_END){
			for (int j = 0; j < 4; j++){
				if (j == 0){
					_fireballObj[i].pos[j].x = start.x + cosf(_fireballObj[i].angle) * _fireballObj[i].curDistance;
					_fireballObj[i].pos[j].y = start.y - sinf(_fireballObj[i].angle) * _fireballObj[i].curDistance;
				}
				else {
					_fireballObj[i].pos[j].x = start.x + cosf( _fireballObj[i].angle + (_fireballObj[i].speed * j) ) * _fireballObj[i].curDistance;
					_fireballObj[i].pos[j].y = start.y - sinf( _fireballObj[i].angle + (_fireballObj[i].speed * j) ) * _fireballObj[i].curDistance;
				}
			}
		}

	}
	
	if (cnt == 8){
		_isAttack = true;
		magicScaleUpdate(_elapsedTime);
		explosionUpdate(elaspedTime);
		if (_explosionCnt == 9)
			_isPlay = false;
	}
}
void PkwFireBallMagic::render(HDC memDC)
{
	if (_isAttack){
		magicScaleRender(memDC);
	}
	explisionRender(memDC);

	for (int i = 0; i < 8; i++){
		if (_fireballObj[i].status == FIREBALL_END) continue;

		for (int j = 0; j < 4; j++){
			IMAGEMANAGER->findImage(_imageKey)->alphaRender(memDC, _fireballObj[i].pos[j].x - _frameWidth / 2, _fireballObj[i].pos[j].y - _frameHeight / 2
				, j * _frameWidth, 0, _frameWidth, _frameHeight, 200.0f);
		}
	}
}
void PkwFireBallMagic::magicEffect()
{
	POINT pos;

	if (_attackUnit->_armyType == 0 || _attackUnit->_armyType == 1){

		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vEnemyCommander[i]._pos)){
				_pCharactorPlay->_vEnemyCommander[i]._curHp -= _skill._demage;
				if (_pCharactorPlay->_vEnemyCommander[i]._curHp < 0) _pCharactorPlay->_vEnemyCommander[i]._curHp = 0;
			}
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos)){
					_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._curHp -= _skill._demage;
					if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._curHp < 0)
						_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._curHp = 0;
				}
			}
		}

	}
	else if (_attackUnit->_armyType == 2){

		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vCoCommander[i]._pos)){
				_pCharactorPlay->_vCoCommander[i]._curHp -= _skill._demage;
				if (_pCharactorPlay->_vCoCommander[i]._curHp < 0) _pCharactorPlay->_vCoCommander[i]._curHp = 0;
			}
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos)){
					_pCharactorPlay->_vCoCommander[i]._vArms[j]._curHp -= _skill._demage;
					if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._curHp < 0)
						_pCharactorPlay->_vCoCommander[i]._vArms[j]._curHp = 0;
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vSupportCommander[i]._pos)){
				_pCharactorPlay->_vSupportCommander[i]._curHp -= _skill._demage;
				if (_pCharactorPlay->_vSupportCommander[i]._curHp < 0) _pCharactorPlay->_vSupportCommander[i]._curHp = 0;
			}
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos)){
					_pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp -= _skill._demage;
					if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp < 0)
						_pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp = 0;
				}
			}
		}
	}
}
