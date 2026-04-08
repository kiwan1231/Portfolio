#include "../stdafx.h"
#include "battleEndEvent.h"


battleEndEvent::battleEndEvent()
{
}


battleEndEvent::~battleEndEvent()
{
}

void battleEndEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight, BattleInfo* pBattleInfo)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_pBattleInfo = pBattleInfo;

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();

	memset(_clearCommanderName, 0, 128);
	memset(_overCommanderName, 0, 128);
	if (DATAMANAGER->_stageNum == 0){
		memcpy(_clearCommanderName, "¹ßµå", strlen("¹ßµå"));
		memcpy(_overCommanderName, "¿¤À©", strlen("¿¤À©"));
	}
	else{
		memcpy(_clearCommanderName, "¹ßµå", strlen("¹ßµå"));
		memcpy(_overCommanderName, "¿¤À©", strlen("¿¤À©"));
	}
	_mode = BATTLE_GAME_ING;
	_eventStatus = EVENT_DEACTIVE;

	_isStart = false;
	
}
void battleEndEvent::release(void)
{
	battleEvent::release();
}
void battleEndEvent::update(float elapsedTime)
{
	_delayTime += elapsedTime;

	if (_isStart == false){
		SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
		memset(SOUNDMANAGER->_curBGSound, 0, 128);

		memcpy(SOUNDMANAGER->_curBGSound, "9_gameClear", strlen("9_gameClear"));
		SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);

		_isStart = true;
	}
	if (_delayTime > 5.0){
		if (_mode == BATTLE_GAME_CLEAR){
			_pBattleInfo->battleStatus = GAME_CLEAR;
			_eventStatus = EVENT_END;
		}
		else if (_mode == BATTLE_GAME_OVER){
			_pBattleInfo->battleStatus = GAME_OVER;
			_eventStatus = EVENT_END;
		}
	}
}
void battleEndEvent::render(HDC memDC)
{
	int posX, posY;

	float alpa = _delayTime * 100.0;
	if (alpa >= 250) alpa = 250.0;

	if (_mode == BATTLE_GAME_CLEAR){
		posX = (WINSIZEX - 448) / 2;
		posY = (WINSIZEY - 64) / 2;
		IMAGEMANAGER->findImage("scenarioClear")->alphaRender(memDC, _pMapStartPos->x + posX, _pMapStartPos->y + posY, 0, 0, 448, 64, alpa);
	}
	else if (_mode == BATTLE_GAME_OVER){
		posX = (WINSIZEX - 288) / 2;
		posY = (WINSIZEY - 64) / 2;
		IMAGEMANAGER->findImage("gameOver")->alphaRender(memDC, _pMapStartPos->x + posX, _pMapStartPos->y + posY, 0, 0, 288, 64, alpa);
	}
}
void battleEndEvent::checkEvent()
{
	bool isClear = true;
	bool isGameOver = true;

	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vCoCommander[i]._name, _clearCommanderName) == 0) isClear = false;
	}
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, _clearCommanderName) == 0) isClear = false;
	}
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, _clearCommanderName) == 0) isClear = false;
	}

	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vCoCommander[i]._name, _overCommanderName) == 0) isGameOver = false;
	}
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, _overCommanderName) == 0) isGameOver = false;
	}
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, _overCommanderName) == 0) isGameOver = false;
	}

	if (isClear){
		_mode = BATTLE_GAME_CLEAR;
		_eventStatus = EVENT_ACTIVE;

		SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
		memset(SOUNDMANAGER->_curBGSound, 0, 128);

		memcpy(SOUNDMANAGER->_curBGSound, "11_gameEvent1", strlen("11_gameEvent1"));
		SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);
	}
	else if (isGameOver){
		_mode = BATTLE_GAME_OVER;
		_eventStatus = EVENT_ACTIVE;
	}
}