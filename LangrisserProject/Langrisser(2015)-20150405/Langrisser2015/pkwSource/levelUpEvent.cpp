#include "../stdafx.h"
#include "levelUpEvent.h"


levelUpEvent::levelUpEvent()
{
}


levelUpEvent::~levelUpEvent()
{
}

void levelUpEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();

	_levelUpCommander = NULL;

	_isOn = false;

	_eventStatus = EVENT_DEACTIVE;
}
void levelUpEvent::release(void)
{
	battleEvent::release();

	_levelUpCommander = NULL;
}
void levelUpEvent::update(float elapsedTime)
{
	_delayTime += elapsedTime;
	if (_delayTime > 1.0){
		_delayTime = 0.0;
		_isOn = !_isOn;
	}

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		_levelUpCommander = NULL;
		_isOn = false;
		_eventStatus = EVENT_DEACTIVE;
	}
}
void levelUpEvent::render(HDC memDC)
{
	if (_levelUpCommander->_armyType == 0)
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
	else if (_levelUpCommander->_armyType == 1)
		IMAGEMANAGER->findImage("UI_GREEN_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
	else 
		IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);

	IMAGEMANAGER->findImage("UI(576X160)")->render(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128);

	IMAGEMANAGER->findImage(_levelUpCommander->_talkImageKey)->render(memDC, _pMapStartPos->x + 16, _pMapStartPos->y + 64);

	SelectObject(memDC, _font[1]);
	SetTextColor(memDC, RGB(255, 255, 255));

	//선택된 지휘관 이름 출력
	TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 144 + 5, _levelUpCommander->_name, strlen(_levelUpCommander->_name));
	TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 176 + 5, "레벨이 올라갔다", strlen("레벨이 올라갔다"));
	TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 208 + 5, "AT가 1 올라갔다", strlen("AT가 1 올라갔다"));
	TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 240 + 5, "DF가 1 올라갔다", strlen("DF가 1 올라갔다"));

	if (_isOn)
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 560, _pMapStartPos->y + 240, 208, 144, 24, 24);
	else
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 560, _pMapStartPos->y + 240, 208 + 24, 144, 24, 24);
}
void levelUpEvent::checkEvent()
{
	//아군 체크
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (_pCharactorPlay->_vCoCommander[i]._curExp >= _pCharactorPlay->_vCoCommander[i]._maxExp){
			_pCharactorPlay->_vCoCommander[i]._atk += 1;
			_pCharactorPlay->_vCoCommander[i]._def += 1;
			_pCharactorPlay->_vCoCommander[i]._level += 1;
			_pCharactorPlay->_vCoCommander[i]._curExp -= _pCharactorPlay->_vCoCommander[i]._maxExp;

			_levelUpCommander = &_pCharactorPlay->_vCoCommander[i];

			_eventStatus = EVENT_ACTIVE;
			return;
		}
	}

	//지원군 체크
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (_pCharactorPlay->_vSupportCommander[i]._curExp >= _pCharactorPlay->_vSupportCommander[i]._maxExp){
			_pCharactorPlay->_vSupportCommander[i]._atk += 1;
			_pCharactorPlay->_vSupportCommander[i]._def += 1;
			_pCharactorPlay->_vSupportCommander[i]._level += 1;
			_pCharactorPlay->_vSupportCommander[i]._curExp -= _pCharactorPlay->_vSupportCommander[i]._maxExp;

			_levelUpCommander = &_pCharactorPlay->_vSupportCommander[i];

			_eventStatus = EVENT_ACTIVE;
			return;
		}
	}

	//적군 체크
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (_pCharactorPlay->_vEnemyCommander[i]._curExp >= _pCharactorPlay->_vEnemyCommander[i]._maxExp){
			_pCharactorPlay->_vEnemyCommander[i]._atk += 1;
			_pCharactorPlay->_vEnemyCommander[i]._def += 1;
			_pCharactorPlay->_vEnemyCommander[i]._level += 1;
			_pCharactorPlay->_vEnemyCommander[i]._curExp -= _pCharactorPlay->_vEnemyCommander[i]._maxExp;

			_levelUpCommander = &_pCharactorPlay->_vEnemyCommander[i];

			_eventStatus = EVENT_ACTIVE;
			return;
		}
	}
}