#include "../stdafx.h"
#include "PkwHealEffect.h"

//------------------------------------------------------------------
// heal 클래스 
//------------------------------------------------------------------
PkwHealEffect::PkwHealEffect()
{
}
PkwHealEffect::~PkwHealEffect()
{
}

void PkwHealEffect::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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
void PkwHealEffect::release()
{

}
void PkwHealEffect::update()
{
	_freameTime += TIMEMANAGER->getElapsedTime();

	if (_freameTime >= _frameDelay){
		_curFrame++;
		if (_curFrame >= _frameMax){
			_isPlay = false;
			_curFrame = _frameMax - 1;
		}
		_freameTime = 0.0;
	}
}
void PkwHealEffect::render(HDC memDC)
{
	POINT pos = _targetUnit->_pos;

	if (_isAlpa)
		IMAGEMANAGER->findImage(_imageKey)->alphaRender(memDC, pos.x * UNITSIZEX, pos.y * UNITSIZEY, 0, _curFrame * _frameHeight, _frameWidth, _frameHeight, 150.0);
	else
		IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x * UNITSIZEX, pos.y * UNITSIZEY, 0, _curFrame * _frameHeight, _frameWidth, _frameHeight);
}
void PkwHealEffect::magicEffect()
{
	_targetUnit->_curHp += 3;
	if (_targetUnit->_curHp > _targetUnit->_maxHp)	_targetUnit->_curHp = _targetUnit->_maxHp;

	_targetUnit->_curMp += 2;
	if (_targetUnit->_curMp > _targetUnit->_maxMp)	_targetUnit->_curMp = _targetUnit->_maxMp;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
// heal 1 클래스 
//------------------------------------------------------------------
//------------------------------------------------------------------
PkwHeal_1Effect::PkwHeal_1Effect()
{
}
PkwHeal_1Effect::~PkwHeal_1Effect()
{
}

void PkwHeal_1Effect::init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa)
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

	_centerPos.x = (_targetPos.x * UNITSIZEX) + UNITSIZEX / 2;
	_centerPos.y = (_targetPos.y * UNITSIZEY) + UNITSIZEY / 2;
}
void PkwHeal_1Effect::release()
{

}
void PkwHeal_1Effect::update()
{
	_freameTime += TIMEMANAGER->getElapsedTime();

	if (_freameTime >= _frameDelay){
		_curFrame++;
		if (_curFrame >= _frameMax){
			_isPlay = false;
			_curFrame = _frameMax - 1;
		}
		_freameTime = 0.0;
	}
}
void PkwHeal_1Effect::render(HDC memDC)
{
	POINTF pos;
	int num;
	float speed = 0.3 * (_curFrame+1);

	for (int i = 0; i < 12; i++){
		pos.x = _centerPos.x + (cosf(PI6*i) * ((_curFrame + 1) * _frameWidth * speed));
		pos.y = _centerPos.y - (sinf(PI6*i) * ((_curFrame + 1) * _frameHeight * speed));
		IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x, pos.y, 0, _curFrame * _frameHeight, _frameWidth, _frameHeight);
		if (_curFrame - 1 >= 0){
			pos.x = _centerPos.x + (cosf(PI6*i) * ((_curFrame + 1) * _frameWidth * speed * 0.8));
			pos.y = _centerPos.y - (sinf(PI6*i) * ((_curFrame + 1) * _frameHeight * speed * 0.8));
			IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x, pos.y, 0, (_curFrame-1) * _frameHeight, _frameWidth, _frameHeight);
		}
		if (_curFrame - 2 >= 0){
			pos.x = _centerPos.x + (cosf(PI6*i) * ((_curFrame + 1) * _frameWidth * speed * 0.6));
			pos.y = _centerPos.y - (sinf(PI6*i) * ((_curFrame + 1) * _frameHeight* speed * 0.6));
			IMAGEMANAGER->findImage(_imageKey)->render(memDC, pos.x, pos.y, 0, (_curFrame - 2) * _frameHeight, _frameWidth, _frameHeight);
		}
	}
}
void PkwHeal_1Effect::magicEffect()
{
	POINT pos;
	if (_attackUnit->_armyType == 0 || _attackUnit->_armyType == 1){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vCoCommander[i]._pos)){
				PkwHealEffect* temp = new PkwHealEffect;
				pos = _pCharactorPlay->_vCoCommander[i]._pos;
				temp->init(NULL, NULL, &_pCharactorPlay->_vCoCommander[i], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
				_vpEffect->push_back(temp);
			}
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos)){
					PkwHealEffect* temp = new PkwHealEffect;
					pos = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
					temp->init(NULL, NULL, &_pCharactorPlay->_vCoCommander[i]._vArms[j], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
					_vpEffect->push_back(temp);
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vSupportCommander[i]._pos)){
				PkwHealEffect* temp = new PkwHealEffect;
				pos = _pCharactorPlay->_vSupportCommander[i]._pos;
				temp->init(NULL, NULL, &_pCharactorPlay->_vSupportCommander[i], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
				_vpEffect->push_back(temp);
			}
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos)){
					PkwHealEffect* temp = new PkwHealEffect;
					pos = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
					temp->init(NULL, NULL, &_pCharactorPlay->_vSupportCommander[i]._vArms[j], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
					_vpEffect->push_back(temp);
				}
			}
		}
	}
	else if (_attackUnit->_armyType == 2){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			if (isMagicAction(_pCharactorPlay->_vEnemyCommander[i]._pos)){
				PkwHealEffect* temp = new PkwHealEffect;
				pos = _pCharactorPlay->_vEnemyCommander[i]._pos;
				temp->init(NULL, NULL, &_pCharactorPlay->_vEnemyCommander[i], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
				_vpEffect->push_back(temp);
			}
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				if (isMagicAction(_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos)){
					PkwHealEffect* temp = new PkwHealEffect;
					pos = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
					temp->init(NULL, NULL, &_pCharactorPlay->_vEnemyCommander[i]._vArms[j], pos, _skill, "effectHeal", 48, 48, 10, 0.15, false);
					_vpEffect->push_back(temp);
				}
			}
		}
	}
}
//bool PkwHeal_1Effect::isMagicAction(POINT pos)
//{
//	int maigcSize = _skill._atkScale;
//	POINT gap;
//	gap.x = _targetPos.x - pos.x;
//	gap.y = _targetPos.y - pos.y;
//	if (gap.x < 0) gap.x *= -1;
//	if (gap.y < 0) gap.y *= -1;
//
//	if (maigcSize >= gap.x + gap.y)
//		return true;
//	else
//		return false;
//}