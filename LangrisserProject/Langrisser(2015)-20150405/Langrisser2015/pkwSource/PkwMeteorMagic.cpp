#include "../stdafx.h"
#include "PkwMeteorMagic.h"


PkwMeteorMagic::PkwMeteorMagic()
{
}


PkwMeteorMagic::~PkwMeteorMagic()
{
}

void PkwMeteorMagic::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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
	//--------------------------------------------------------메테오 클래스 변수 초기화
	_alpaValue = 0.0f;

	for (int i = 0; i < 10; i++){
		_meteorObj[i].status = METEOR_WAIT;
		_meteorObj[i].aniTime = 0.08f;
		_meteorObj[i].lifeTime = 0.29f;
		_meteorObj[i].elapseTime = 0.0f;
		_meteorObj[i].waitTime = 0.2 * i;
		_meteorObj[i]._frameX = 0;

		if (i == 0){
			_meteorObj[i].pos.x = (_targetPos.x + 1) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 10;
		}
		else if (i == 1){
			_meteorObj[i].pos.x = (_targetPos.x) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 1 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 11;
		}
		else if (i == 2){
			_meteorObj[i].pos.x = (_targetPos.x - 1) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 10;
		}
		else if (i == 3){
			_meteorObj[i].pos.x = (_targetPos.x) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y + 1 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 9;
		}
		else if (i == 4){
			_meteorObj[i].pos.x = (_targetPos.x + 2) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 2 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 12;
		}
		else if (i == 5){
			_meteorObj[i].pos.x = (_targetPos.x - 2) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 2 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 12;
		}
		else if (i == 6){
			_meteorObj[i].pos.x = (_targetPos.x - 2) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y + 2 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 8;
		}
		else if (i == 7){
			_meteorObj[i].pos.x = (_targetPos.x + 2) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y + 2 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 8;
		}
		else if (i == 8){
			_meteorObj[i].pos.x = (_targetPos.x - 3) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y - 3  - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 13;
		}
		else if (i == 9){
			_meteorObj[i].pos.x = (_targetPos.x + 3) * UNITSIZEX;
			_meteorObj[i].pos.y = (_targetPos.y + 3 - 10) * UNITSIZEY;
			_meteorObj[i]._distance = UNITSIZEY * 13;
		}
	}
}
void PkwMeteorMagic::release()
{

}
void PkwMeteorMagic::update()
{
	_elapsedTime = TIMEMANAGER->getElapsedTime();

	magicScaleUpdate(_elapsedTime);

	int meteorCnt = 0;
	for (int i = 0; i < 10; i++){
		_meteorObj[i].elapseTime += _elapsedTime;

		if (_meteorObj[i].status == METEOR_WAIT && _meteorObj[i].elapseTime >= _meteorObj[i].waitTime){
			_meteorObj[i].status = METEOR_START;
			_meteorObj[i].elapseTime = 0.0f;
			
		}
		else if (_meteorObj[i].status == METEOR_START){
			if (_meteorObj[i]._distance <= 0.0){
			//if (_meteorObj[i].elapseTime >= _meteorObj[i].lifeTime){
				_meteorObj[i].status = METEOR_COLLISION;
				_meteorObj[i].elapseTime = 0.0f;
			}
			else{
				_meteorObj[i].pos.x += cosf(1.5 * PI) * 130.0f;
				_meteorObj[i].pos.y -= sinf(1.5 * PI) * 130.0f;
				_meteorObj[i]._distance += sinf(1.5 * PI) * 130.0f;
			}
		}
		else if (_meteorObj[i].status == METEOR_COLLISION && _meteorObj[i].elapseTime >= _meteorObj[i].aniTime){
			_meteorObj[i].elapseTime = 0.0f;
			_meteorObj[i]._frameX++;
			if (_meteorObj[i]._frameX >= _frameMax) _meteorObj[i].status = METEOR_END;
		}

		if (_meteorObj[i].status == METEOR_END) meteorCnt++;
	}

	if (meteorCnt == 10)
		_isPlay = false;
}
void PkwMeteorMagic::render(HDC memDC)
{
	magicScaleRender(memDC);

	for (int i = 0; i < 10; i++){
		if (_meteorObj[i].status == METEOR_START || _meteorObj[i].status == METEOR_COLLISION)
			IMAGEMANAGER->findImage(_imageKey)->alphaRender(memDC, _meteorObj[i].pos.x - _frameWidth / 2, _meteorObj[i].pos.y - _frameHeight / 2
			, _meteorObj[i]._frameX * _frameWidth, 0, _frameWidth, _frameHeight,200.0f);
	}
}
void PkwMeteorMagic::magicEffect()
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
