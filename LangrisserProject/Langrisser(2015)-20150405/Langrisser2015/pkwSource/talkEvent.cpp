#include "../stdafx.h"
#include "talkEvent.h"


talkEvent::talkEvent()
{
}
talkEvent::~talkEvent()
{
}

void talkEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight, BattleInfo* pBattleInfo)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_pBattleInfo = pBattleInfo;

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();
	
	_vtalkData.clear();

	_isOn = false;

	_eventStatus = EVENT_DEACTIVE;
}
void talkEvent::release(void)
{
	for (int i = 0; i < _vtalkData.size(); i++){
		for (int j = 0; j < _vtalkData[i]->maxTalkLine; j++){
			SAFE_DELETE_ARRAY(_vtalkData[i]->talk[j]);
		}
		SAFE_DELETE_ARRAY(_vtalkData[i]->talk);
		SAFE_DELETE(_vtalkData[i]);
	}
	_vtalkData.clear();
}
void talkEvent::update(float elapsedTime)
{
	//이벤트 데이터가 없으면 이벤트 종료
	if (_vtalkData.size() == 0){
		_eventStatus = EVENT_END;
		return;
	}

	//가장 첫번째 이벤트를 처리 한다
	if (_vtalkData[0]->mode == COMMANDER_SERCH){
		int xPos, yPos;
		bool isXset = false, isYset = false;

		xPos = _pMapStartPos->x / UNITSIZEX;
		yPos = _pMapStartPos->y / UNITSIZEY;

		//지휘관 존재 않으면 바로 토크로~
		if (_vtalkData[0]->pos.x == -1 && _vtalkData[0]->pos.y == -1){
			_vtalkData[0]->mode = COMMANDER_TALK;
			return;
		}
		
		if (_vtalkData[0]->pos.x - xPos < 1){
			_pMapStartPos->x -= elapsedTime * 1000;
		}
		else if (_vtalkData[0]->pos.x - xPos > 11){
			_pMapStartPos->x += elapsedTime * 1000;
		}
		else{
			isXset = true;
		}

		if (_vtalkData[0]->pos.y - yPos < 1){
			_pMapStartPos->y -= elapsedTime * 1000;
		}
		else if (_vtalkData[0]->pos.y - yPos > 8){
			_pMapStartPos->y += elapsedTime * 1000;
		}
		else{
			isYset = true;
		}
		//지휘관 있는데까지 찾아갔으면 토크로 ㄱ~
		if (isXset && isYset)  _vtalkData[0]->mode = COMMANDER_TALK;
	}
	else if (_vtalkData[0]->mode == COMMANDER_TALK){
		//0.5초마다 버튼 이미지 갱신
		_delayTime += elapsedTime;
		if (_delayTime > 0.5) !_isOn;

		//마지막 줄같이 출력했으면 다음 토크데이터를 종료한다
		if (_vtalkData[0]->curTalkLine >= _vtalkData[0]->maxTalkLine){
			_vtalkData[0]->mode = TALK_END;
			return;
		}
		//3줄씩 출력함으로 +3
		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			_vtalkData[0]->curTalkLine += 3;
		}
	}
	else if (_vtalkData[0]->mode == TALK_END){
		//종료가 된 데이터는 삭제하고 다음 데이터를 가장 앞으로 보낸다
		for (int i = 0; i < _vtalkData[0]->maxTalkLine; i++){
			SAFE_DELETE_ARRAY(_vtalkData[0]->talk[i]);
		}
		SAFE_DELETE_ARRAY(_vtalkData[0]->talk);
		SAFE_DELETE(_vtalkData[0]);

		_vtalkData.erase(_vtalkData.begin());
	}
}
void talkEvent::render(HDC memDC)
{
	//이벤트 데이터가 없으면 이벤트 종료
	if (_vtalkData.size() == 0){
		_eventStatus = EVENT_END;
		return;
	}

	//첫번째 이벤트가 토크 상태 일때만 그려준다
	if (_vtalkData[0]->mode != COMMANDER_TALK) return;
	
	if (_vtalkData[0]->commander != NULL){
		if (_vtalkData[0]->commander->_armyType == 0)
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
		else if (_vtalkData[0]->commander->_armyType == 1)
			IMAGEMANAGER->findImage("UI_GREEN_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
		else
			IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
	}
	else
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);

	IMAGEMANAGER->findImage("UI(576X160)")->render(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128);

	//지휘관이 존재하면 얼굴 출력
	if (_vtalkData[0]->commander != NULL)
		IMAGEMANAGER->findImage(_vtalkData[0]->commander->_talkImageKey)->render(memDC, _pMapStartPos->x + 16, _pMapStartPos->y + 64);

	SelectObject(memDC, _font[1]);
	SetTextColor(memDC, RGB(255, 255, 255));

	//선택된 지휘관 이름 출력

	TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 144 + 5, _vtalkData[0]->commanderName, strlen(_vtalkData[0]->commanderName));

	if (_isOn)
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 560, _pMapStartPos->y + 240, 208, 144, 24, 24);
	else
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 560, _pMapStartPos->y + 240, 208 + 24, 144, 24, 24);

	//여기가 대사 출력
	int nCnt = 0;
	for (int i = _vtalkData[0]->curTalkLine; i < _vtalkData[0]->maxTalkLine; i++){
		if (nCnt == 3) break;
		TextOut(memDC, _pMapStartPos->x + 128 + 10, _pMapStartPos->y + 176 + 5 + (nCnt * 32), _vtalkData[0]->talk[i], strlen(_vtalkData[0]->talk[i]));
		nCnt++;
	}
}
void talkEvent::checkEvent()
{
	if (_terms == COMMANDER_DEATH){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			if (strcmp(_pCharactorPlay->_vCoCommander[i]._name, _deathCommanderName) == 0) return;
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, _deathCommanderName) == 0) return;
		}
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, _deathCommanderName) == 0) return;
		}
		//찾고 있는 지휘관이 없다면 죽은것이므로 이벤트 활성화
		setTalkCommanderPos();
		_eventStatus = EVENT_ACTIVE;
	}
	else if (_terms == TURN){
		if (_nturn == _pBattleInfo->nTurn && _battleStatus == _pBattleInfo->battleStatus){
			//조건 상태의 턴일경우 이벤트 활성화
			setTalkCommanderPos();
			_eventStatus = EVENT_ACTIVE;
		}
	}
}
void talkEvent::setTalkCommanderPos()
{
	//해당 지휘관을 찾으면 true해준다.
	bool isSearch;

	for (int i = 0; i < _vtalkData.size(); i++){
		isSearch = false;
		for (int j = 0; j < _pCharactorPlay->_vCoCommander.size(); j++){
			if (strcmp(_pCharactorPlay->_vCoCommander[j]._name, _vtalkData[i]->commanderName) == 0){
				_vtalkData[i]->commander = &_pCharactorPlay->_vCoCommander[j];
				_vtalkData[i]->pos = _pCharactorPlay->_vCoCommander[j]._pos;
				isSearch = true;
				break;
			}
		}

		if (isSearch) continue;

		for (int j = 0; j < _pCharactorPlay->_vSupportCommander.size(); j++){
			if (strcmp(_pCharactorPlay->_vSupportCommander[j]._name, _vtalkData[i]->commanderName) == 0){
				_vtalkData[i]->commander = &_pCharactorPlay->_vSupportCommander[j];
				_vtalkData[i]->pos = _pCharactorPlay->_vSupportCommander[j]._pos;
				isSearch = true;
				break;
			}
		}

		if (isSearch) continue;

		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander.size(); j++){
			if (strcmp(_pCharactorPlay->_vEnemyCommander[j]._name, _vtalkData[i]->commanderName) == 0){
				_vtalkData[i]->commander = &_pCharactorPlay->_vEnemyCommander[j];
				_vtalkData[i]->pos = _pCharactorPlay->_vEnemyCommander[j]._pos;
				isSearch = true;
				break;
			}
		}
		//해당 지휘관이 죽었음으로 -1로 넣어준다. 이때는 지휘관 쪽으로 가지 않고 바로 토크 박스를 출력한다
		if (isSearch == false){
			_vtalkData[i]->commander = NULL;
			_vtalkData[i]->pos.x = -1;
			_vtalkData[i]->pos.y = -1;
		}
	}
}
void talkEvent::readTalkTerms(char* str, int nCnt)
{
	int length = strlen(str);
	int value;
	
	//무슨 조건인지 보기
	if (nCnt == 0){
		if (strcmp(str, "COMMANDER_DEATH") == 0){
			_terms = COMMANDER_DEATH;
		}
		else if (strcmp(str, "TURN") == 0){
			_terms = TURN;
		}
	}
	//조건 저장
	else if (nCnt == 1){
		if (_terms == COMMANDER_DEATH){
			memset(_deathCommanderName, 0, 128);
			memcpy(_deathCommanderName, str, length);
		}
		else if (_terms == TURN){
			value = atoi(str);
			_nturn = value;
		}
	}
	//조건 저장
	else if (nCnt == 2){
		if (strcmp(str, "CO_TURN") == 0){
			_battleStatus = CO_TURN;
		}
		else if (strcmp(str, "SUPPORT_TURN") == 0){
			_battleStatus = SUPPORT_TURN;
		}
		else if (strcmp(str, "ENEMY_TURN") == 0){
			_battleStatus = ENEMY_TURN;
		}
	}
}
void talkEvent::createTalkData(char* str)
{
	//토크 갯수 만큼 토크 이벤트 생성
	int number = atoi(str);
	for (int i = 0; i < number; i++){
		talkData* temp;
		temp = new talkData();
		temp->mode = COMMANDER_SERCH;
		_vtalkData.push_back(temp);
	}
}
void talkEvent::readTalkData(int talkNumber, char* str, int nCnt)
{
	int length = strlen(str);
	
	//커맨더 이름 저장
	if (nCnt == 0){
		memset(_vtalkData[talkNumber]->commanderName, 0, 128);
		memcpy(_vtalkData[talkNumber]->commanderName, str, length);
	}
	else if (nCnt == 1){
		//토크 데이터만큼 동적할당 해준다
		int number = atoi(str);
		_vtalkData[talkNumber]->talk = new char*[number];
		for (int i = 0; i < number; i++){
			_vtalkData[talkNumber]->talk[i] = new char[128];
		}
		_vtalkData[talkNumber]->curTalkLine = 0;
		_vtalkData[talkNumber]->maxTalkLine = number;
	}
	//대사 저장
	else{
		memset(_vtalkData[talkNumber]->talk[_vtalkData[talkNumber]->curTalkLine], 0, 128);
		memcpy(_vtalkData[talkNumber]->talk[_vtalkData[talkNumber]->curTalkLine], str, length);
		_vtalkData[talkNumber]->curTalkLine++;
		//모든 대사를 다 넣으면 다시 0으로 바꾸어 주어 처음부터 출력되게 한다
		if (_vtalkData[talkNumber]->curTalkLine >= _vtalkData[talkNumber]->maxTalkLine)
			_vtalkData[talkNumber]->curTalkLine = 0;
	}
}

