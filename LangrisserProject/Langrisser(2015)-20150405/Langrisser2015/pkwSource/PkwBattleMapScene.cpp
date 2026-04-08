

#include "../stdafx.h"
#include "PkwBattleMapScene.h"
#include "unitAI_Algorism.h"
PkwBattleMapScene::PkwBattleMapScene()
{
}
PkwBattleMapScene::~PkwBattleMapScene()
{
}

HRESULT PkwBattleMapScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, CALLBACK_FUNCTION_PARAMETER cbFunction3, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter1 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction1);
	_callbackFunctionParameter2 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction2);
	_callbackFunctionParameter3 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction3);

	_battleInfo.nTurn = 1;
	_battleInfo.battleStatus = CO_TURN;
	_battleInfo.eventPlay = BATTLE_EVENT_OFF;
//	_battleInfo.effectPlay = BATTLE_EFFECT_STOP;
	_battleInfo.charactorControl = CO_CONTROL_OFF;
	_battleInfo.commandChoice = COMMAND_MOVE;
	_battleInfo.orderChoice = ORDER_BATTLE;

	/*_battleInfo.nTurn = 1;
	_battleInfo.battleStatus = CO_TURN;
	_battleInfo.eventPlay = BATTLE_EVENT_STOP;
	_battleInfo.effectPlay = BATTLE_EFFECT_STOP;
	_battleInfo.charactorControl = CO_CONTROL_OFF;
	_battleInfo.commandChoice = COMMAND_MOVE;
	_battleInfo.orderChoice = ORDER_BATTLE;*/

	//데이터매니져에게 받아오는 값들
	_mapStage = DATAMANAGER->_stageNumber;
	_mapImage = IMAGEMANAGER->findImage(DATAMANAGER->_stageDataFile[_mapStage].stageMapImageKey);
	_pCharactorPlay = DATAMANAGER->getCharactorPlay();
	_pMapData = &DATAMANAGER->_MapData;
	_pDataBase = &DATAMANAGER->_DataBase;

	// 업데이트 관련 변수
	_elapsed = 0.0;
	_nChoice = 0;
	_curPage = 0;
	_maxPage = 0;
	//화면 관련 변수
	_nWindowWidth	= _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight	= _winResolutionRc.bottom - _winResolutionRc.top;
	_gameView.left	= 0; 
	_gameView.top	= 0; 
	_gameView.right = _nWindowWidth;
	_gameView.bottom = _nWindowHeight;

	_MapStartPos.x = 1300;		_MapStartPos.y = 1300;
	_MapMaxWidth = _mapImage->getWidth();
	_MapManHeight = _mapImage->getHeight();
	
	_choiceUnit = NULL;
	//_choiceUnit = &_pCharactorPlay->_vCoCommander[0];
	_bufPos.x = 0; _bufPos.y = 0;
	
	//백dc 생성
	_backBuffer = new image;
	_backBuffer->init(_MapMaxWidth, _MapManHeight);
	_MemDC = _backBuffer->getMemDC();

	//글자 배경모드(TRANSPARENT : 투명 QPAQUEL : 불투명)
	SetBkMode(_MemDC, TRANSPARENT);

	//글자 간격
	SetTextCharacterExtra(_MemDC, 5);

	// 폰트 set
	_font[0] = CreateFont(28, 0, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[1] = CreateFont(22, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[2] = CreateFont(16, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");

	//클레스 변수 초기화
	_actionProcess.Init(_pCharactorPlay, _pMapData->GetMap());
	_effectPlay.Init();
	_eventManager.init(this);

	unitAIset();

	_pkwDelay = 2.0f;

	_mouseImageFrame.frameDelay = 0.1;
	_mouseImageFrame.elaspedTime = 0.0;
	_mouseImageFrame.frameDir = NORMAL_DIR;
	_mouseImageFrame.frameX = 0;
	_mouseImageFrame.maxFrameX = IMAGEMANAGER->findImage("titleChoice")->getMaxFrameX();

	soundPlay();

	return S_OK;
}
void PkwBattleMapScene::release(void)
{
	_actionProcess.release();
	_effectPlay.release();
	_eventManager.release();

	for (int i = 0; i < 10; i++){
		if (_font[i] != NULL)
			DeleteObject(_font[i]);
	}

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
	}
}
void PkwBattleMapScene::update(void)
{
	_elapsed = TIMEMANAGER->getElapsedTime();
	
	mouseImageFrameUpdate();

	if (_battleInfo.battleStatus == GAME_CLEAR || _battleInfo.battleStatus == GAME_OVER){
		_callbackFunctionParameter3(_obj);
		return;
	}
	
	_eventManager.update(_elapsed);

	//이벤트 매니져 활성화 중에는 이벤트매니져만 업데이트 한다
	if (_eventManager._isEventManagerAction == true) return;
	
	charactorUpdate();
	_effectPlay.update();
	
	//마법 실행중이에는 아무것도  못함
	if (_effectPlay._vEffect.size() != 0) return;

	if (_battleInfo.battleStatus == SUPPORT_TURN || _battleInfo.battleStatus == ENEMY_TURN){
		aiUpdate();
		return;
	}

	// 백 이미지 그려주기
	backImageDraw();

	if (_battleInfo.charactorControl == SYSTEM_ON){
		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			RECT rc;
			rc.left = 208; rc.right = 448;
			rc.top = 176; rc.bottom = 240;

			if (PtInRect(&rc, _ptMouse)){
				if (_battleInfo.battleStatus == CO_TURN){
					_battleInfo.battleStatus = SUPPORT_TURN;
					turnStarsetUnit(1);
					_eventManager.addEvent(0);
					soundPlay();
				}
				/*else if (_battleInfo.battleStatus == SUPPORT_TURN){
					_battleInfo.battleStatus = ENEMY_TURN;
					_eventManager.addEvent(0);
				}
				else if (_battleInfo.battleStatus == ENEMY_TURN){
					_battleInfo.nTurn++;
					_battleInfo.battleStatus = CO_TURN;
					turnStarsetUnit(0);
					_eventManager.addEvent(0);
				}*/
				_battleInfo.charactorControl = CO_CONTROL_OFF;
			}
		}
		else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_battleInfo.charactorControl = CO_CONTROL_OFF;
		}
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_OFF){
		controlOffUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_CHOICE){
		controlChoiceUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MOVE){
		controlMoveUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MOVING){
		controlMovingUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_ATTACK){
		controlAttackUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC){
		controlMagicUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_CHOICE){
		controlMagicChoiceUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_TARGET_CHOICE){
		controlMagicTargetChoiceUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_POS_CHOICE){
		controlMagicPosChoiceUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON){
		controlSummonUpdate();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON_CHOICE){
		controlSummonChoiceUpdate();
	}
	/*else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON_POS_CHOICE){
		
	}*/
	else if (_battleInfo.charactorControl == CO_CONTROL_ORDER){
		controlOrderUpdate();
	}
	else if (_battleInfo.charactorControl == SUPPORT_CHOICE || _battleInfo.charactorControl == ENEMY_CHOICE){
		if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			//_choiceUnit = NULL;
			_nChoice = 0;
			_battleInfo.charactorControl = CO_CONTROL_OFF;
		}
	}

	
}
void PkwBattleMapScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	// 맵 그리기
	_mapImage->render(_MemDC);

	//캐릭터 지휘범위,공격 범위등을 출력
	_actionProcess.render(_MemDC, _battleInfo.charactorControl, _gameView);

	// 캐릭터 그리기
	charactorRender();

	// 인터페이스 그리기
	InterfaceRender();

	//이펙트 그리기
	_effectPlay.render(_MemDC);

	//이벤트 매니져 그리기
	_eventManager.render(_MemDC);

	// 경과 시간 그리기 getHDC()
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(250, 250, 250));
	sprintf_s(str, "battleMap Scene - elapse time : %f  ", _elapsed);
	TextOut(_MemDC, _MapStartPos.x, _MapStartPos.y, str, strlen(str));

	//커서 그리기
	IMAGEMANAGER->findImage("titleCursor")->render(_MemDC, _MapStartPos.x + _ptMouse.x, _MapStartPos.y + _ptMouse.y, _mouseImageFrame.frameX * 46, 0, 46, 46);

	// 백 이미지 그려주기
	if (_pkwDelay > 0){
		_pkwDelay -= _elapsed;
		backImageDraw();
	}

	_backBuffer->render(DATAMANAGER->_mainDc, _gameView.left, _gameView.top, _MapStartPos.x, _MapStartPos.y, _gameView.right - _gameView.left, _gameView.bottom - _gameView.top);
}

//init관련함수
void PkwBattleMapScene::unitAIset()
{
	PkwCharactor* pTargetUnit = NULL;
	POINT targetPos;
	int action;

	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		
		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			_pCharactorPlay->_vSupportCommander[i]._vArms[j].aiSet(this, NULL, targetPos, _pCharactorPlay->_vSupportCommander[i]._mercenartOrder, &_actionProcess, &_effectPlay);
		}

		if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "리아나") == 0){
			pTargetUnit = NULL;
			targetPos.x = 12; targetPos.y = 42;
			action = 5;
		}
		else if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "로빈") == 0){
			pTargetUnit = NULL;
			targetPos.x = 12; targetPos.y = 4;
			action = 4;
		}
		else if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "에반젤") == 0){
			pTargetUnit = NULL;
			targetPos.x = 14; targetPos.y = 2;
			action = 4;
		}
		else if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "시스터") == 0){
			pTargetUnit = NULL;
			targetPos.x = 25; targetPos.y = 23;
			action = 5;
		}

		_pCharactorPlay->_vSupportCommander[i].aiSet(this, pTargetUnit, targetPos, action, &_actionProcess, &_effectPlay);
	}

	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			_pCharactorPlay->_vEnemyCommander[i]._vArms[j].aiSet(this, NULL, targetPos, _pCharactorPlay->_vEnemyCommander[i]._mercenartOrder, &_actionProcess, &_effectPlay);
		}

		if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, "레온") == 0){
			pTargetUnit = commanderNameSearch("로빈");
			targetPos.x = -1; targetPos.y = -1;
			action = 7;
		}
		else if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, "레아드") == 0){
			pTargetUnit = commanderNameSearch("로빈");
			targetPos.x = -1; targetPos.y = -1;
			action = 7;
		}
		else if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, "발드") == 0){
			pTargetUnit = NULL;
			targetPos.x = 12; targetPos.y = 42;
			action = 5;
		}
		else if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, "제국지휘관") == 0){
			pTargetUnit = NULL;
			targetPos.x = -1; targetPos.y = -1;
			action = 3;
		}

		_pCharactorPlay->_vEnemyCommander[i].aiSet(this, pTargetUnit, targetPos, action, &_actionProcess, &_effectPlay);
	}
}
PkwCharactor* PkwBattleMapScene::commanderNameSearch(char* name)
{
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vCoCommander[i]._name, name) == 0){
			return &_pCharactorPlay->_vCoCommander[i];
		}
	}
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, name) == 0){
			return &_pCharactorPlay->_vSupportCommander[i];
		}
	}
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vEnemyCommander[i]._name, name) == 0){
			return &_pCharactorPlay->_vEnemyCommander[i];
		}
	}
	return NULL;
}


//업데이트 관련 함수
void PkwBattleMapScene::aiUpdate()
{
	PkwCharactor* playUnit = NULL;
	bool isAction = false;
	int size;

	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (strcmp(_pCharactorPlay->_vSupportCommander[i]._name, "시스터") == 0){
			if (_pCharactorPlay->_vSupportCommander[i]._pos.x == 25 && _pCharactorPlay->_vSupportCommander[i]._pos.y == 23)
				_pCharactorPlay->_vSupportCommander.erase(_pCharactorPlay->_vSupportCommander.begin() + i);
		}
	}

	if (_battleInfo.battleStatus == SUPPORT_TURN){

		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (_pCharactorPlay->_vSupportCommander[i]._action == CHARACTOR_ACTION_ON){
				isAction = true;
				playUnit = &_pCharactorPlay->_vSupportCommander[i];
				break;
			}
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._action == CHARACTOR_ACTION_ON){
					isAction = true;
					playUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[j];
					break;
				}
			}
			if (isAction) break;
		}

		if (playUnit == NULL){
			_battleInfo.battleStatus = ENEMY_TURN;
			turnStarsetUnit(2);
			_eventManager.addEvent(0);
			soundPlay();
		}
		else{
			if (aiFocus(playUnit))
				playUnit->_AI->update(playUnit);
		}
	}
	else if (_battleInfo.battleStatus == ENEMY_TURN){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			if (_pCharactorPlay->_vEnemyCommander[i]._action == CHARACTOR_ACTION_ON){
				isAction = true;
				playUnit = &_pCharactorPlay->_vEnemyCommander[i];
				break;
			}
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._action == CHARACTOR_ACTION_ON){
					isAction = true;
					playUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[j];
					break;
				}
			}
			if (isAction) break;
		}

		if (playUnit == NULL){
			_battleInfo.nTurn++;
			_battleInfo.battleStatus = CO_TURN;
			turnStarsetUnit(0);
			_eventManager.addEvent(0);
			soundPlay();
		}
		else{
			if (aiFocus(playUnit))
				playUnit->_AI->update(playUnit);
		}
	}
}
bool PkwBattleMapScene::aiFocus(PkwCharactor* playUnit)
{
	int xPos, yPos;
	bool isXset = false, isYset = false;

	xPos = _MapStartPos.x / UNITSIZEX;
	yPos = _MapStartPos.y / UNITSIZEY;

	if (playUnit->_pos.x - xPos < 1){
		_MapStartPos.x -= _elapsed * 1000;
	}
	else if (playUnit->_pos.x - xPos > 11){
		_MapStartPos.x += _elapsed * 1000;
	}
	else{
		isXset = true;
	}

	if (playUnit->_pos.y - yPos < 1){
		_MapStartPos.y -= _elapsed * 1000;
	}
	else if (playUnit->_pos.y - yPos > 8){
		_MapStartPos.y += _elapsed * 1000;
	}
	else{
		isYset = true;
	}
	//모두 이동 했으면 true 이동 중이면 false
	if (isXset && isYset)	return true;
	else					return false;
}
void PkwBattleMapScene::charactorUpdate()
{
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._action == CHARACTOR_ACTION_ON) 
				_pCharactorPlay->_vCoCommander[i]._vArms[j].update(_elapsed);
		}
		if (_pCharactorPlay->_vCoCommander[i]._action == CHARACTOR_ACTION_ON)
			_pCharactorPlay->_vCoCommander[i].update(_elapsed);
	}

	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._action == CHARACTOR_ACTION_ON)
				_pCharactorPlay->_vSupportCommander[i]._vArms[j].update(_elapsed);
		}
		if (_pCharactorPlay->_vSupportCommander[i]._action == CHARACTOR_ACTION_ON)
			_pCharactorPlay->_vSupportCommander[i].update(_elapsed);
	}

	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._action == CHARACTOR_ACTION_ON)
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j].update(_elapsed);
		}
		
		if (_pCharactorPlay->_vEnemyCommander[i]._action == CHARACTOR_ACTION_ON)
			_pCharactorPlay->_vEnemyCommander[i].update(_elapsed);
	}
}
void PkwBattleMapScene::controlOffUpdate()
{
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){

		POINT pos = _ptMouse;
		pos.x += _MapStartPos.x;		pos.y += _MapStartPos.y;
		pos.x /= UNITSIZEX;				pos.y /= UNITSIZEY;

		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			if (_pCharactorPlay->_vCoCommander[i]._pos.x == pos.x && _pCharactorPlay->_vCoCommander[i]._pos.y == pos.y){
				_choiceUnit = &_pCharactorPlay->_vCoCommander[i];
				_battleInfo.charactorControl = CO_CONTROL_CHOICE;
				_actionProcess.SearchPos(_choiceUnit);
				_bufPos = _choiceUnit->_pos;//현재 위치를 저장해서 이동한것을 취소할떄 다시 돌아오는 용도로 쓴다
				break;
			}
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.x == pos.x && _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.y == pos.y){
					_choiceUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[j];
					_battleInfo.charactorControl = CO_CONTROL_CHOICE;
					_actionProcess.SearchPos(_choiceUnit);
					_bufPos = _choiceUnit->_pos;
					break;
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			if (_pCharactorPlay->_vSupportCommander[i]._pos.x == pos.x && _pCharactorPlay->_vSupportCommander[i]._pos.y == pos.y){
				_choiceUnit = &_pCharactorPlay->_vSupportCommander[i];
				_battleInfo.charactorControl = SUPPORT_CHOICE;
				_actionProcess.SearchPos(_choiceUnit);
				break;
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			if (_pCharactorPlay->_vEnemyCommander[i]._pos.x == pos.x && _pCharactorPlay->_vEnemyCommander[i]._pos.y == pos.y){
				_choiceUnit = &_pCharactorPlay->_vEnemyCommander[i];
				_battleInfo.charactorControl = ENEMY_CHOICE;
				_actionProcess.SearchPos(_choiceUnit);
				break;
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_battleInfo.charactorControl = SYSTEM_ON;
	}
}
void PkwBattleMapScene::controlChoiceUpdate()
{
	int commandNum;
	bool isCommander, isMagic, isSummon;

	// 선택된 유닛이 지휘관인지?
	if (_choiceUnit->_number < 1000){
		commandNum = 4;
		isCommander = true;
	}
	else{
		commandNum = 2;
		isCommander = false;
	}

	// 선택된 유닛이 마법이 있는가?
	if (_choiceUnit->_vSkill.size() > 0){
		commandNum++;
		isMagic = true;
	}
	else
		isMagic = false;

	// 선택된 유닛이 소환수가 있는가?
	if (_choiceUnit->_vSummon.size() > 0){
		commandNum++;
		isSummon = true;
	}
	else
		isSummon = false;

	RECT *rc = new RECT[commandNum];
	POINT pos = _ptMouse;
	for (int i = 0; i < commandNum; i++){
		rc[i].left = 48;
		rc[i].right = rc[i].left + 96;
		rc[i].top = 80 + (i * 48);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}

	//커맨드 버튼 눌렸을때 처리
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		//이미 행동한 애들은 명령 못한다
		if (_choiceUnit->_action == CHARACTOR_ACTION_OFF) return;

		for (int i = 0; i < commandNum; i++){
			if (PtInRect(&rc[i], pos)){
				if (i == 0)	{
					_battleInfo.charactorControl = CO_CONTROL_MOVE;
				}
				else if (i == 1){
					_actionProcess._vAtkRangePos.clear();
					_actionProcess.SearchAtkPos(_choiceUnit, _choiceUnit->_pos, _choiceUnit->_atkRange);
					_battleInfo.charactorControl = CO_CONTROL_ATTACK;
				}
				else if (i == 2){
					if (isMagic){
						_nChoice = 0;
						_curPage = 0;
						_maxPage = (_choiceUnit->_vSkill.size() - 1) / 5;
						_battleInfo.charactorControl = CO_CONTROL_MAGIC;
					}
					else if (isSummon){
						_nChoice = 0;
						_curPage = 0;
						_maxPage = (_choiceUnit->_vSummon.size() - 1) / 5;
						_battleInfo.charactorControl = CO_CONTROL_SUMMON;
					}
					else
						choiceUnitHeal();
				}
				else if (i == 3){
					if (isMagic && isSummon){
						_nChoice = 0;
						_curPage = 0;
						_maxPage = (_choiceUnit->_vSummon.size() - 1) / 5;
						_battleInfo.charactorControl = CO_CONTROL_SUMMON;
					}
					else if (isMagic || isSummon)	choiceUnitHeal();
					else							_battleInfo.charactorControl = CO_CONTROL_ORDER;
				}
				else if (i == 4){
					if (isMagic && isSummon)		choiceUnitHeal();
					else if (isMagic || isSummon)	_battleInfo.charactorControl = CO_CONTROL_ORDER;
				}
				else if (i == 5)	_battleInfo.charactorControl = CO_CONTROL_ORDER;

				_nChoice = 0;
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_choiceUnit = NULL;
		_nChoice = 0;
		_battleInfo.charactorControl = CO_CONTROL_OFF;
	}

	SAFE_DELETE_ARRAY(rc);
}
void PkwBattleMapScene::controlMoveUpdate()
{
	POINT pos, temp;
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;

		
		//아군 지휘관 및 용병 위치에 있는곳의 이동 타일을 눌렀을면 이동안되게 예외처리
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			temp = _pCharactorPlay->_vCoCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y) return;

			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y) return;
			}
		}
		//지원군 지휘관 및 용병 위치에 있는곳의 이동 타일을 눌렀을면 이동안되게 예외처리
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			temp = _pCharactorPlay->_vSupportCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y) return;

			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y) return;
			}
		}
		//적군 지휘관 및 용병 위치에 있는곳의 이동 타일을 눌렀을면 이동안되게 예외처리
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y) return;

			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y) return;
			}
		}
		//이동 지점 클릭했을때 처리
		for (int i = 0; i < _actionProcess._vMoveRange.size(); i++){
			if (pos.x == _actionProcess._vMoveRange[i].x && pos.y == _actionProcess._vMoveRange[i].y){
				//길 찾기 알고리즘 쓰기
				_actionProcess.movingPosSave(_choiceUnit, _choiceUnit->_pos, pos);
				
				_moveDelay = 0.0;
				_battleInfo.charactorControl = CO_CONTROL_MOVING;
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_battleInfo.charactorControl = CO_CONTROL_CHOICE;
	}
}
void PkwBattleMapScene::controlMovingUpdate()
{
	POINT temp;
	if (_actionProcess._curPosNum < 0){
		_actionProcess._vAtkRangePos.clear();
		_actionProcess.SearchAtkPos(_choiceUnit, _choiceUnit->_pos, _choiceUnit->_atkRange);

		_choiceUnit->_AniStatus = BOTTOM_WALK;
		_battleInfo.charactorControl = CO_CONTROL_ATTACK;
	}
	else{
		_moveDelay += _elapsed;

		if (_moveDelay > 0.1){

			//이동 할 위치를 가져온다
			temp = _actionProcess._aStar._vMovePos[_actionProcess._curPosNum];

			//이동 위치에 대한 애니메이션 상태를 바꿔준다
			if (temp.x < _choiceUnit->_pos.x)	_choiceUnit->_AniStatus = LEFT_WALK;
			else if (temp.x > _choiceUnit->_pos.x)	_choiceUnit->_AniStatus = RIGHT_WALK;
			else if (temp.y < _choiceUnit->_pos.y)	_choiceUnit->_AniStatus = TOP_WALK;
			else if (temp.y > _choiceUnit->_pos.y)	_choiceUnit->_AniStatus = BOTTOM_WALK;
			
			//이동 시켜준다
			_choiceUnit->_pos = temp;

			//다음 이동할 위치로 간다
			_actionProcess._curPosNum--;

			_moveDelay = 0.0;
		}
	}
}
void PkwBattleMapScene::controlAttackUpdate()
{
	POINT pos;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;
		for (int i = 0; i < _actionProcess._vAtkRangePos.size(); i++){
			if (pos.x == _actionProcess._vAtkRangePos[i].x && pos.y == _actionProcess._vAtkRangePos[i].y){
				//이동하고 종료
				if (_choiceUnit->_pos.x == pos.x && _choiceUnit->_pos.y == pos.y){

					_choiceUnit->_action = CHARACTOR_ACTION_OFF;
					_choiceUnit->_AniStatus = LEFT_WALK;
					_choiceUnit->_AniNum = 1;
					//_choiceUnit = NULL;

					_battleInfo.charactorControl = CO_CONTROL_OFF;
				}
				//공격하고 종료
				else{
					CharactorAttack(pos);
					DATAMANAGER->setBmSceneData(_battleInfo,_choiceUnit,_choiceSkill,_choiceSummon,_targetUnit);

					_choiceUnit->_action = CHARACTOR_ACTION_OFF;
					_choiceUnit->_AniStatus = LEFT_WALK;
					_choiceUnit->_AniNum = 1;
					//_choiceUnit = NULL;

					_battleInfo.charactorControl = CO_CONTROL_OFF;
					_callbackFunctionParameter1(_obj);
				}
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_choiceUnit->_pos = _bufPos;
		_battleInfo.charactorControl = CO_CONTROL_CHOICE;
	}
}
void PkwBattleMapScene::controlMagicUpdate()
{
	int showMagicNum;

	showMagicNum = _choiceUnit->_vSkill.size() - (_curPage * 5);
	if (showMagicNum > 5)	showMagicNum = 5;

	//마법 렉트 초기화
	RECT *rc = new RECT[showMagicNum + 2];
	for (int i = 0; i < showMagicNum; i++){
		rc[i].left = 80;
		rc[i].right = rc[i].left + 192;
		rc[i].top = 80 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}

	//앞 뒤 페이지 버튼 렉트 초기화
	rc[showMagicNum].left		= 272;		rc[showMagicNum].right		= rc[showMagicNum].left + 16;
	rc[showMagicNum].top		= 80;		rc[showMagicNum].bottom		= rc[showMagicNum].top + 16;
	rc[showMagicNum + 1].left	= 272;		rc[showMagicNum + 1].right	= rc[showMagicNum + 1].left + 16;
	rc[showMagicNum + 1].top	= 224;		rc[showMagicNum + 1].bottom = rc[showMagicNum + 1].top + 16;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showMagicNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showMagicNum){
					_curPage--;
					if (_curPage < 0)	_curPage++;
				}
				else if (i == showMagicNum + 1){
					_curPage++;
					if (_curPage > _maxPage)	_curPage--;
				}
				else{
					_choiceSkill = &_choiceUnit->_vSkill[ (_curPage * 5) + i];
					_actionProcess._vMigRangePos.clear();
					_actionProcess.magicRangePosSave(_choiceUnit->_pos, _choiceSkill->_atkRange);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_CHOICE;
				}
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_battleInfo.charactorControl = CO_CONTROL_CHOICE;
	}

	delete[] rc;
}
void PkwBattleMapScene::controlMagicChoiceUpdate()
{
	POINT pos, temp;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		
		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;

		for (int i = 0; i < _actionProcess._vMigRangePos.size(); i++){
			//클릭한 위치가 벡터 안에 있으면 다음으로 통과 
			if (_actionProcess._vMigRangePos[i].x == pos.x && _actionProcess._vMigRangePos[i].y == pos.y)
				break;
			// 모두 검색할동안 없었으면 함수를 종료한다
			if (i == _actionProcess._vMigRangePos.size() - 1)
				return;
		}

		//적 지정 마법
		if (_choiceSkill->_type == 0){
			targetAtkMagic(pos);
		}
		//아군 지정 마법
		else if (_choiceSkill->_type == 1){
			targetDefMagic(pos);
		}
		//지형 아무곳이나 다 되는 마법
		else if (_choiceSkill->_type == 2){
			_targetPos = pos;
			_actionProcess._vMigScalePos.clear();
			_actionProcess.magicScalePosSave(_targetPos, _choiceSkill->_atkScale);
			_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
		}
		//텔레포트 
		else if (_choiceSkill->_type == 3){
			targetDefMagic(pos);
		}

	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_curPage = 0;
		_maxPage = (_choiceUnit->_vSkill.size() - 1) / 5;
		_battleInfo.charactorControl = CO_CONTROL_MAGIC;
	}
}
void PkwBattleMapScene::controlMagicTargetChoiceUpdate()
{
	POINT pos;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;

		for (int i = 0; i < _actionProcess._vMigScalePos.size(); i++){
			if (_actionProcess._vMigScalePos[i].x == pos.x && _actionProcess._vMigScalePos[i].y == pos.y){

				_effectPlay.AddEffet(_choiceUnit, _targetUnit, pos, *_choiceSkill);
				_nChoice = 0;
				DATAMANAGER->setBmSceneData(_battleInfo, _choiceUnit, _choiceSkill, _choiceSummon, _targetUnit);

				_choiceUnit->_action = CHARACTOR_ACTION_OFF;
				_choiceUnit->_AniStatus = LEFT_WALK;
				_choiceUnit->_AniNum = 1;
				//_choiceUnit = NULL;

				_battleInfo.charactorControl = CO_CONTROL_OFF;

				_callbackFunctionParameter1(_obj);
			}
		}
		
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_battleInfo.charactorControl = CO_CONTROL_MAGIC_CHOICE;
	}
}
void PkwBattleMapScene::controlMagicPosChoiceUpdate()
{
	POINT pos, temp;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;
		//텔레포트 일경우
		if (_choiceSkill->_type == 3){
			teleportPos(pos);
		}
		else{
			for (int i = 0; i < _actionProcess._vMigScalePos.size(); i++){
				if (_actionProcess._vMigScalePos[i].x == pos.x && _actionProcess._vMigScalePos[i].y == pos.y){
					_effectPlay.AddEffet(_choiceUnit, NULL, pos, *_choiceSkill);
					_nChoice = 0;

					DATAMANAGER->setBmSceneData(_battleInfo, _choiceUnit, _choiceSkill, _choiceSummon, _targetUnit);

					_choiceUnit->_action = CHARACTOR_ACTION_OFF;
					_choiceUnit->_AniStatus = LEFT_WALK;
					_choiceUnit->_AniNum = 1;
					_choiceUnit = NULL;

					_battleInfo.charactorControl = CO_CONTROL_OFF;

					_callbackFunctionParameter1(_obj);
				}
			}
		}

	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_battleInfo.charactorControl = CO_CONTROL_MAGIC_CHOICE;
	}
}
void PkwBattleMapScene::controlSummonUpdate()
{
	int showSummonNum;

	showSummonNum = _choiceUnit->_vSkill.size() - (_curPage * 5);
	if (showSummonNum > 5)	showSummonNum = 5;

	//마법 렉트 초기화
	RECT *rc = new RECT[showSummonNum + 2];
	for (int i = 0; i < showSummonNum; i++){
		rc[i].left = 80;
		rc[i].right = rc[i].left + 192;
		rc[i].top = 80 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}

	//앞 뒤 페이지 버튼 렉트 초기화
	rc[showSummonNum].left = 272;		rc[showSummonNum].right = rc[showSummonNum].left + 16;
	rc[showSummonNum].top = 80;			rc[showSummonNum].bottom = rc[showSummonNum].top + 16;
	rc[showSummonNum + 1].left = 272;	rc[showSummonNum + 1].right = rc[showSummonNum + 1].left + 16;
	rc[showSummonNum + 1].top = 224;	rc[showSummonNum + 1].bottom = rc[showSummonNum + 1].top + 16;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showSummonNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showSummonNum){
					_curPage--;
					if (_curPage < 0)	_curPage++;
				}
				else if (i == showSummonNum + 1){
					_curPage++;
					if (_curPage > _maxPage)	_curPage--;
				}
				else{
					_choiceSummon = &_choiceUnit->_vSummon[(_curPage * 5) + i];
					_actionProcess.summonPosSave(_choiceUnit);
					_battleInfo.charactorControl = CO_CONTROL_SUMMON_CHOICE;
				}
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_battleInfo.charactorControl = CO_CONTROL_CHOICE;
	}

	delete[] rc;
}
void PkwBattleMapScene::controlSummonChoiceUpdate()
{
	POINT pos, temp;

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){

		pos = _ptMouse;
		pos.x += _MapStartPos.x - _gameView.left;		pos.y += _MapStartPos.y - _gameView.top;
		pos.x /= UNITSIZEX;					pos.y /= UNITSIZEY;

		for (int i = 0; i < _actionProcess._vSummonRangePos.size(); i++){
			if (_actionProcess._vSummonRangePos[i].x == pos.x && _actionProcess._vSummonRangePos[i].y == pos.y){
				//소환 발동!!!
				_effectPlay.AddEffet(_choiceUnit, NULL, pos, *_choiceSummon);
				_nChoice = 0;

				DATAMANAGER->setBmSceneData(_battleInfo, _choiceUnit, _choiceSkill, _choiceSummon, _targetUnit);

				_choiceUnit->_action = CHARACTOR_ACTION_OFF;
				_choiceUnit->_AniStatus = LEFT_WALK;
				_choiceUnit->_AniNum = 1;

				_battleInfo.charactorControl = CO_CONTROL_OFF;

				_callbackFunctionParameter1(_obj);
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_curPage = 0;
		_maxPage = (_choiceUnit->_vSummon.size() - 1) / 5;
		_battleInfo.charactorControl = CO_CONTROL_SUMMON;
	}
}
void PkwBattleMapScene::controlOrderUpdate()
{
	RECT rc[4];
	POINT pos = _ptMouse;
	for (int i = 0; i < 4; i++){
		rc[i].left = 144;
		rc[i].right = rc[i].left + 80;
		rc[i].top = 176 + (i * 48);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}

	//커맨드 버튼 눌렸을때 처리
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < 4; i++){
			if (PtInRect(&rc[i], pos)){
				if (i == 0){
					_choiceUnit->_mercenartOrder = MERCENARY_BATTLE;
				}
				else if (i == 1){
					_choiceUnit->_mercenartOrder = MERCENARY_ATTACK;
				}
				else if (i == 2){
					_choiceUnit->_mercenartOrder = MERCENARY_DEFENCE;
				}
				else if (i == 3){
					_choiceUnit->_mercenartOrder = MERCENARY_ANAUTO;
				}

				_nChoice = 0;
				_battleInfo.charactorControl = CO_CONTROL_CHOICE;
			}
		}
	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_nChoice = 0;
		_battleInfo.charactorControl = CO_CONTROL_CHOICE;
	}
}

//렌더 관련 함수
void PkwBattleMapScene::backImageDraw()
{
	//---------------------------------------------------카메라 위치에 맞는 이미지 구하기
	int loadWidth = _nWindowWidth;
	int	loadHeight = _nWindowHeight;
	int camaraSpeed = _elapsed * 1000;

	//전체 이미지(가로)가 윈도우 창보다 작을떄 가운데다 그린다 
	if (loadWidth >= _MapMaxWidth){

		_gameView.left = (loadWidth - _MapMaxWidth) / 2;
		_gameView.right = _MapMaxWidth + _gameView.left;

		_MapStartPos.x = 0;
		loadWidth = _MapMaxWidth;
	}
	//전체 이미지(가로)가 윈도우 창보다 클떄
	else{
		//마우스가 윈도우창 왼쪽,오른쪽 끝에 다았을때 처리
		if (_ptMouse.x <= 10)
			_MapStartPos.x -= camaraSpeed;
		else if (_ptMouse.x >= loadWidth - 10)
			_MapStartPos.x += camaraSpeed;

		if (_MapStartPos.x <= 0)
			_MapStartPos.x = 0;
		else if (_MapStartPos.x + loadWidth >= _MapMaxWidth)
			_MapStartPos.x -= (_MapStartPos.x + loadWidth) - _MapMaxWidth;
	}

	//전체 이미지(세로)가 윈도우 창보다 작을떄 가운데다 그린다 
	if (loadHeight >= _MapManHeight){

		_gameView.top = (loadHeight - _MapManHeight) / 2;
		_gameView.bottom = _MapManHeight + _gameView.top;

		_MapStartPos.y = 0;
		loadHeight = _MapManHeight;
	}
	//전체 이미지(세로)가 윈도우 창보다 클떄
	else{
		//마우스가 윈도우창 탑,바텀 끝에 다았을때 처리 
		if (_ptMouse.y <= 10)
			_MapStartPos.y -= camaraSpeed;
		else if (_ptMouse.y >= loadHeight - 10)
			_MapStartPos.y += camaraSpeed;

		if (_MapStartPos.y <= 0)
			_MapStartPos.y = 0;
		else if (_MapStartPos.y + loadHeight >= _MapManHeight)
			_MapStartPos.y -= (_MapStartPos.y + loadHeight) - _MapManHeight;
	}
//	_backBuffer->render(DATAMANAGER->_mainDc, _gameView.left, _gameView.top, _MapStartPos.x, _MapStartPos.y, _gameView.right - _gameView.left, _gameView.bottom - _gameView.top);
}
void PkwBattleMapScene::charactorRender()
{
	//아군 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (_pCharactorPlay->_vCoCommander[i]._pos.x != -1 && _pCharactorPlay->_vCoCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vCoCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vCoCommander[i].render(_MemDC, true);
		}
	}
	//지휘관 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (_pCharactorPlay->_vSupportCommander[i]._pos.x != -1 && _pCharactorPlay->_vSupportCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vSupportCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vSupportCommander[i].render(_MemDC, true);
		}
	}
	//적 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (_pCharactorPlay->_vEnemyCommander[i]._pos.x != -1 && _pCharactorPlay->_vEnemyCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vEnemyCommander[i].render(_MemDC, true);
		}
	}
}
void PkwBattleMapScene::InterfaceRender()
{
	if (_battleInfo.charactorControl == SYSTEM_ON){
		RECT rc;
		rc.left = 208; rc.right = 448;
		rc.top = 176; rc.bottom = 240;

		SelectObject(_MemDC, _font[0]);
		SetTextColor(_MemDC, RGB(255, 255, 255));

		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 208, _MapStartPos.y + 176, 0, 0, 240, 64, 150.0);
		IMAGEMANAGER->findImage("UI(240X64)")->render(_MemDC, _MapStartPos.x + 208, _MapStartPos.y + 176);

		TextOut(_MemDC, _MapStartPos.x + 208 + 48, _MapStartPos.y + 192 + 2, " 턴  종료", strlen(" 턴  종료"));
		if (PtInRect(&rc, _ptMouse))
			IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 224, _MapStartPos.y + 192, 160, 224, 15, 26);
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_OFF){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_CHOICE){
		if (_choiceUnit->_action == CHARACTOR_ACTION_ON)
			commandWindowRender();
		unitInfoWindowRender();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MOVE){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MOVING){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_ATTACK){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC){
		magicWindowRender();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_CHOICE){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_TARGET_CHOICE){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_POS_CHOICE){
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON){
		summonWindowRender();
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON_CHOICE){
	}
	/*else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON_POS_CHOICE){
	}*/
	else if (_battleInfo.charactorControl == CO_CONTROL_ORDER){
		commandWindowRender();
		unitInfoWindowRender();
		armyCommandWindowRender();
	}
	else if (_battleInfo.charactorControl == SUPPORT_CHOICE){
		unitInfoWindowRender();
	}
	else if (_battleInfo.charactorControl == ENEMY_CHOICE){
		unitInfoWindowRender();
	}

	downBarRender();
}
void PkwBattleMapScene::commandWindowRender()
{
	float alpa = 150.0;
	bool isCommander, isMagic, isSummon;

	//선택된 유닛이 지휘관인지?
	if (_choiceUnit->_number < 1000)
		isCommander = true;
	else
		isCommander = false;

	//선택된 유닛이 마법이 있는가?
	if (_choiceUnit->_vSkill.size() > 0)
		isMagic = true;
	else
		isMagic = false;

	//선택된 유닛이 소환수가 있는가?
	if (_choiceUnit->_vSummon.size() > 0)
		isSummon = true;
	else
		isSummon = false;

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	if (isCommander){//지휘관일 경우
		if (isMagic && isSummon){//마법과 소환창이 전부 있을떄
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 304, alpa);
			IMAGEMANAGER->findImage("UI(128X304)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);

			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 176 + 2, "마법", strlen("마법"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 224 + 2, "소환", strlen("소환"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 272 + 2, "치료", strlen("치료"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 320 + 2, "명령", strlen("명령"));
		}
		else if (isMagic){//마법만 있을떄
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 256, alpa);
			IMAGEMANAGER->findImage("UI(128X256)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);

			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 176 + 2, "마법", strlen("마법"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 224 + 2, "치료", strlen("치료"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 272 + 2, "명령", strlen("명령"));
		}
		else if (isSummon){//소환만 있을떄
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 256, alpa);
			IMAGEMANAGER->findImage("UI(128X256)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);

			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 176 + 2, "소환", strlen("소환"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 224 + 2, "치료", strlen("치료"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 272 + 2, "명령", strlen("명령"));
		}
		else{//모두 없을떄
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 208, alpa);
			IMAGEMANAGER->findImage("UI(128X208)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);

			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 176 + 2, "치료", strlen("치료"));
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 224 + 2, "명령", strlen("명령"));
		}
		TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 80 + 2, "이동", strlen("이동"));
		TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 128 + 2, "공격", strlen("공격"));
	}
	else{// 용병이나 소환수 일 경우
		if (isMagic){//마법이 있을때
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 160, alpa);
			IMAGEMANAGER->findImage("UI(128X160)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);
			
			TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 176 + 2, "마법", strlen("마법"));
		}
		else{
			IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64, 0, 0, 128, 112, alpa);
			IMAGEMANAGER->findImage("UI(128X112)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 64);
		}
		TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 80 + 2, "이동", strlen("이동"));
		TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 128 + 2, "공격", strlen("공격"));
	}

	//선택 되어지고 있는 용병 화살표 출력
	if (_battleInfo.charactorControl == CO_CONTROL_CHOICE)
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 80 - 3 + _nChoice * 48, 160, 224, 15, 26);
}
void PkwBattleMapScene::unitInfoWindowRender()
{
	//알파값 저장 변수
	float alpa = 150.0f;
	// 선택된 유닛이 지휘관인지 판별 하는 변수
	bool isCommander;
	//아이템 그려줄때 쓰는 변수
	int sourX, sourY;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	//퍼센티지 저장(경험치바 그릴때 씀)
	float per;
	float width;
	
	//선택된 유닛이 지휘관인지?
	if (_choiceUnit->_number < 1000)
		isCommander = true;
	else
		isCommander = false;

	//지휘관이 아니면 그리지 않는다
	if (!isCommander) return;

	if (_choiceUnit->_armyType == 0)
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 288, _MapStartPos.y + 64, 0, 0, 320, 208, alpa);
	else if (_choiceUnit->_armyType == 1)
		IMAGEMANAGER->findImage("UI_GREEN_BACK")->alphaRender(_MemDC, _MapStartPos.x + 288, _MapStartPos.y + 64, 0, 0, 320, 208, alpa);
	else if (_choiceUnit->_armyType == 2)
		IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(_MemDC, _MapStartPos.x + 288, _MapStartPos.y + 64, 0, 0, 320, 208, alpa);
	
	IMAGEMANAGER->findImage("UI(320X208)")->render(_MemDC, _MapStartPos.x + 288, _MapStartPos.y + 64);

	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	//선택된 지휘관 이름 출력
	TextOut(_MemDC, _MapStartPos.x + 368 + 5, _MapStartPos.y + 80 + 5, _choiceUnit->_name, strlen(_choiceUnit->_name));
	//선택된 지휘관 클레스 출력
	TextOut(_MemDC, _MapStartPos.x + 368 + 5, _MapStartPos.y + 112 + 5, _choiceUnit->_chClass, strlen(_choiceUnit->_chClass));
	//병종 출력
	if (_choiceUnit->_type == 0){
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 144 + 5, "보병", strlen("보병"));
	}
	else if (_choiceUnit->_type == 1){
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 144 + 5, "기병", strlen("기병"));
	}
	else if (_choiceUnit->_type == 2){
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 144 + 5, "수병", strlen("수병"));
	}
	else if (_choiceUnit->_type == 3){
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 144 + 5, "비병", strlen("비병"));
	}

	//공격 아이템 출력
	sourX = (_choiceUnit->_atkItem._imageNum % 8) * 32;
	sourY = (_choiceUnit->_atkItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + 464, _MapStartPos.y + 144, sourX, sourY, 32, 32);

	//방어 아이템 출력
	sourX = (_choiceUnit->_DefItem._imageNum % 8) * 32;
	sourY = (_choiceUnit->_DefItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + 512, _MapStartPos.y + 144, sourX, sourY, 32, 32);

	//레벨 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 320, _MapStartPos.y + 176, 160, 0, 16, 16);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 336, _MapStartPos.y + 176, 112, 0, 16, 16);
	numDigit = GetNumDigit(_choiceUnit->_level);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 384 - (i*16), _MapStartPos.y + 176, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//경험치 바 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 432, _MapStartPos.y + 176, 0, 144, 144, 16);
	per = (float)_choiceUnit->_curExp / (float)_choiceUnit->_maxExp;
	width = 112.0 * per;
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 176, 0, 160, (int)width, 16);

	//공격력 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 320, _MapStartPos.y + 192, 16, 0, 16, 16);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 336, _MapStartPos.y + 192, 32, 0, 16, 16);
	numDigit = GetNumDigit(_choiceUnit->_atk + _choiceUnit->_atkItem._atk + +_choiceUnit->_DefItem._atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 384 - (i*16), _MapStartPos.y + 192, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//방어력 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 320, _MapStartPos.y + 208, 48, 0, 16, 16);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 336, _MapStartPos.y + 208, 64, 0, 16, 16);
	numDigit = GetNumDigit(_choiceUnit->_def + +_choiceUnit->_atkItem._def + _choiceUnit->_DefItem._def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 384 - (i*16), _MapStartPos.y + 208, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//mp그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 432, _MapStartPos.y + 192, 80, 0, 16, 16);//m출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 192, 96, 0, 16, 16);//p출력
	numDigit = GetNumDigit(_choiceUnit->_curMp);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 496 - (i * 16), _MapStartPos.y + 192, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 512, _MapStartPos.y + 192, 32, 32, 16, 16);// '/'출력
	numDigit = GetNumDigit(_choiceUnit->_maxMp);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 544 - (i * 16), _MapStartPos.y + 192, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}

	//선택된 지휘관 이동력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 432, _MapStartPos.y + 208, 80, 0, 16, 16);//m출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 208, 112, 0, 16, 16);//v출력
	numDigit = GetNumDigit(_choiceUnit->_move + _choiceUnit->_DefItem._move);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 496 - (i * 16), _MapStartPos.y + 208, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}

	//지휘범위 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 336, _MapStartPos.y + 224, 0, 48, 64, 16);//'지휘범위'출력
	numDigit = GetNumDigit(_choiceUnit->_commandRange + _choiceUnit->_DefItem._commandRange);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 384 - (i*16), _MapStartPos.y + 240, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}

	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 224, 64, 48, 32, 16);//수정 출력
	//공격 수정치 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 432, _MapStartPos.y + 240, 16, 0, 16, 16);//'A'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 240, 0, 16, 16, 16);//'+'출력
	numDigit = GetNumDigit(_choiceUnit->_commandAtk + _choiceUnit->_atkItem._mercenaryAtk + _choiceUnit->_DefItem._mercenaryAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 480 - (i * 16), _MapStartPos.y + 240, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	//방어 수정치 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 512, _MapStartPos.y + 240, 48, 0, 16, 16);//'D'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 528, _MapStartPos.y + 240, 0, 16, 16, 16);//'+'출력
	numDigit = GetNumDigit(_choiceUnit->_commandDef + _choiceUnit->_atkItem._mercenaryDef + _choiceUnit->_DefItem._mercenaryDef);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 560 - (i * 16), _MapStartPos.y + 240, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}

	//캐릭터 얼굴창 출력
	IMAGEMANAGER->findImage("charactorfaceWin")->render(_MemDC, _MapStartPos.x + 224, _MapStartPos.y + 48);
	//캐릭터 얼굴 출력
	IMAGEMANAGER->findImage(_choiceUnit->_faceImageKey)->render(_MemDC, _MapStartPos.x + 240, _MapStartPos.y + 64);
}
void PkwBattleMapScene::armyCommandWindowRender()
{
	float alpa = 150.0;
	bool isCommander;

	//선택된 유닛이 지휘관인지?
	if (_choiceUnit->_number < 1000)
		isCommander = true;
	else
		isCommander = false;

	//지휘관이 아니면 안그린다
	if (!_choiceUnit)	return;

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 128, _MapStartPos.y + 160, 0, 0, 112, 208, alpa);
	IMAGEMANAGER->findImage("UI(112X208)")->render(_MemDC, _MapStartPos.x + 128, _MapStartPos.y + 160);

	TextOut(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 176 + 2, "전투", strlen("전투"));
	TextOut(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 224 + 2, "돌격", strlen("돌격"));
	TextOut(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 272 + 2, "방어", strlen("방어"));
	TextOut(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 320 + 2, "수동", strlen("수동"));

	//선택 되어지고 있는 용병 화살표 출력
	if (_battleInfo.charactorControl == CO_CONTROL_ORDER)
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 144, _MapStartPos.y + 176 - 3 + _nChoice * 48, 160, 224, 15, 26);
}
void PkwBattleMapScene::magicWindowRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 64, 0, 0, 256, 192, alpa);
	IMAGEMANAGER->findImage("UI(256X192)")->render(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 64);

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 240, 0, 0, 144, 64, alpa);
	IMAGEMANAGER->findImage("UI(144X64)")->render(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 240);

	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	int magicStart = _curPage * 5;

	for (int i = magicStart; i < magicStart + 5; i++){
		if (i >= _choiceUnit->_vSkill.size()) break;

		pos.x = _MapStartPos.x + 80; pos.y = _MapStartPos.y + 80 + (i - magicStart) * 32;
		//스킬 이름 출력
		TextOut(_MemDC, pos.x + 2, pos.y + 5, _choiceUnit->_vSkill[i]._name, strlen(_choiceUnit->_vSkill[i]._name));

		if (_nChoice == i - magicStart){

			//선택된 유닛의 현재 마나량
			numDigit = GetNumDigit(_choiceUnit->_vSkill[i]._mpSize);
			pos.x = _MapStartPos.x + 208; pos.y = _MapStartPos.y + 272 - 3;
			for (int i = 0; i < numDigit.strlen; i++){
				infoImage->render(_MemDC, pos.x,pos.y, 16 * numDigit.value[i], 224, 16, 19);
				pos.x -= 16;
			}
			
			infoImage->render(_MemDC, _MapStartPos.x + 224, _MapStartPos.y + 272, 32, 32, 16, 16);// '/'출력

			//선택되어진 마법의 마나 소모량 출력
			numDigit = GetNumDigit(_choiceUnit->_curMp);
			pos.x = _MapStartPos.x + 272; pos.y = _MapStartPos.y + 272;
			for (int i = 0; i < numDigit.strlen; i++){
				infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
				pos.x -= 16;
			}
		}
	}
	
	// 장비, 사기, 팔기 선택 화살표 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 80 + (_nChoice * 32) + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_curPage != 0){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 272, _MapStartPos.y + 80, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_curPage != _maxPage){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 272, _MapStartPos.y + 224, 224, 0, 16, 16);
	}
}
void PkwBattleMapScene::summonWindowRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 64, 0, 0, 256, 192, alpa);
	IMAGEMANAGER->findImage("UI(256X192)")->render(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 64);

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 240, 0, 0, 144, 64, alpa);
	IMAGEMANAGER->findImage("UI(144X64)")->render(_MemDC, _MapStartPos.x + 160, _MapStartPos.y + 240);

	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	int summonStart = _curPage * 5;

	for (int i = summonStart; i < summonStart + 5; i++){
		if (i >= _choiceUnit->_vSummon.size()) break;

		pos.x = _MapStartPos.x + 80; pos.y = _MapStartPos.y + 80 + (i - summonStart) * 32;
		//스킬 이름 출력
		TextOut(_MemDC, pos.x + 2, pos.y + 5, _choiceUnit->_vSummon[i]._name, strlen(_choiceUnit->_vSummon[i]._name));

		if (_nChoice == i - summonStart){

			//선택되어진 마법의 마나 소모량 출력
			numDigit = GetNumDigit(_choiceUnit->_vSummon[i]._mpSize);
			pos.x = _MapStartPos.x + 208; pos.y = _MapStartPos.y + 272 - 3;
			for (int i = 0; i < numDigit.strlen; i++){
				infoImage->render(_MemDC, pos.x, pos.y, 16 * numDigit.value[i], 224, 16, 19);
				pos.x -= 16;
			}

			infoImage->render(_MemDC, _MapStartPos.x + 224, _MapStartPos.y + 272, 32, 32, 16, 16);// '/'출력

			//선택된 유닛의 현재 마나량
			numDigit = GetNumDigit(_choiceUnit->_curMp);
			pos.x = _MapStartPos.x + 272; pos.y = _MapStartPos.y + 272;
			for (int i = 0; i < numDigit.strlen; i++){
				infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
				pos.x -= 16;
			}
		}
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 80 + (_nChoice * 32) + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_curPage != 0){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 272, _MapStartPos.y + 80, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_curPage != _maxPage){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 272, _MapStartPos.y + 224, 224, 0, 16, 16);
	}
}
void PkwBattleMapScene::downBarRender()
{
	PkwCharactor* mouseChoiceUnit = NULL;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	int groundValue, groundProperty;
	POINT pos = _ptMouse;

	pos.x += _MapStartPos.x;		pos.y += _MapStartPos.y;
	pos.x /= UNITSIZEX;				pos.y /= UNITSIZEY;

	IMAGEMANAGER->findImage("downBar")->render(_MemDC, _MapStartPos.x, _MapStartPos.y + 400);

	groundValue = _pMapData->GetMap()->map[ (_pMapData->GetMap()->width * pos.y) + pos.x];
	if (groundValue > 18) groundValue = 19;
	IMAGEMANAGER->findImage("groundIcon")->render(_MemDC, _MapStartPos.x + 16, _MapStartPos.y + 432, (groundValue-10)*48,0,48,32);

	if (groundValue == 10){
		groundProperty = 10;
	}
	else if (groundValue == 11){
		groundProperty = 10;
	}
	else if (groundValue == 12){
		groundProperty = 10;
	}
	else if (groundValue == 13){
		groundProperty = 20;
	}
	else if (groundValue == 15){
		groundProperty = 15;
	}
	else if (groundValue == 16){
		groundProperty = 20;
	}
	else if (groundValue == 17){
		groundProperty = 40;
	}
	else if (groundValue == 18){
		groundProperty = 10;
	}
	else{
		groundProperty = 0;
	}

	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 448, 0, 16, 16, 16);//'+'출력
	numDigit = GetNumDigit(groundProperty);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 96 - (i * 16), _MapStartPos.y + 448, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 112, _MapStartPos.y + 448, 16, 32, 16, 16);//'%'출력

	bool isSearch = false;
	//아군 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){

		if (isSearch) break;

		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.x == pos.x && _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.y == pos.y){
				mouseChoiceUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[j];
				isSearch = true;
				break;
			}
		}

		if (_pCharactorPlay->_vCoCommander[i]._pos.x == pos.x && _pCharactorPlay->_vCoCommander[i]._pos.y == pos.y){
			mouseChoiceUnit = &_pCharactorPlay->_vCoCommander[i];
			isSearch = true;
		}
	}
	//지휘관 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){

		if (isSearch) break;

		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.x == pos.x && _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.y == pos.y){
				mouseChoiceUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[j];
				isSearch = true;
				break;
			}
		}

		if (_pCharactorPlay->_vSupportCommander[i]._pos.x == pos.x && _pCharactorPlay->_vSupportCommander[i]._pos.y == pos.y){
			mouseChoiceUnit = &_pCharactorPlay->_vSupportCommander[i];
			isSearch = true;
		}
	}
	//적 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){

		if (isSearch) break;

		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos.x == pos.x && _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos.y == pos.y){
				mouseChoiceUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[j];
				isSearch = true;
				break;
			}
		}

		if (_pCharactorPlay->_vEnemyCommander[i]._pos.x == pos.x && _pCharactorPlay->_vEnemyCommander[i]._pos.y == pos.y){
			mouseChoiceUnit = &_pCharactorPlay->_vEnemyCommander[i];
			isSearch = true;
		}
	}
	if (mouseChoiceUnit == NULL) return;

	//지휘관 이미지 출력
	IMAGEMANAGER->findImage(mouseChoiceUnit->_imageKey)->render(_MemDC, _MapStartPos.x + +128, _MapStartPos.y + 416, 180, 0, 60, 64);

	SelectObject(_MemDC, _font[2]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	if (mouseChoiceUnit->_number < 1000){
		//선택된 지휘관 클레스 출력
		TextOut(_MemDC, _MapStartPos.x + 192, _MapStartPos.y + 432, mouseChoiceUnit->_chClass, strlen(mouseChoiceUnit->_chClass));
		//선택된 지휘관 이름 출력
		TextOut(_MemDC, _MapStartPos.x + 352, _MapStartPos.y + 432, mouseChoiceUnit->_name, strlen(mouseChoiceUnit->_name));
	}
	else{
		//선택된 지휘관 클레스 출력
		TextOut(_MemDC, _MapStartPos.x + 192, _MapStartPos.y + 432, mouseChoiceUnit->_name, strlen(mouseChoiceUnit->_name));
		//선택된 지휘관 이름 출력
		TextOut(_MemDC, _MapStartPos.x + 352, _MapStartPos.y + 432, mouseChoiceUnit->_commander->_name, strlen(mouseChoiceUnit->_commander->_name));
	}
	//레벨 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 192, _MapStartPos.y + 448, 160, 0, 16, 16);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 210, _MapStartPos.y + 448, 32 + 16 * mouseChoiceUnit->_level, 16, 16, 16);
	//공격 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 240, _MapStartPos.y + 448, 16, 0, 16, 16);//'A'출력
	numDigit = GetNumDigit(mouseChoiceUnit->_atk + mouseChoiceUnit->_atkItem._atk + mouseChoiceUnit->_DefItem._atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 272 - (i * 16), _MapStartPos.y + 448, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	//방어 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 304, _MapStartPos.y + 448, 48, 0, 16, 16);//'D'출력
	numDigit = GetNumDigit(mouseChoiceUnit->_def + mouseChoiceUnit->_atkItem._def + mouseChoiceUnit->_DefItem._def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 336 - (i * 16), _MapStartPos.y + 448, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	//HP 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 448, 128, 0, 16, 16);//'H'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 384, _MapStartPos.y + 448, 144, 0, 16, 16);//'P'출력
	numDigit = GetNumDigit(mouseChoiceUnit->_curHp);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 416 - (i * 16), _MapStartPos.y + 448, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	//MP 출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 432, _MapStartPos.y + 448, 80, 0, 16, 16);//'M'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 448, _MapStartPos.y + 448, 96, 0, 16, 16);//'P'출력
	numDigit = GetNumDigit(mouseChoiceUnit->_curMp);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 480 - (i * 16), _MapStartPos.y + 448, 32 + (16 * numDigit.value[i]), 16, 16, 16);
	}
	int sourX, sourY;
	if (mouseChoiceUnit->_number >= 1000) return;
	//공격 아이템 출력
	sourX = (mouseChoiceUnit->_atkItem._imageNum % 8) * 32;
	sourY = (mouseChoiceUnit->_atkItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + 512, _MapStartPos.y + 432, sourX, sourY, 32, 32);

	//방어 아이템 출력
	sourX = (mouseChoiceUnit->_DefItem._imageNum % 8) * 32;
	sourY = (mouseChoiceUnit->_DefItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + 560, _MapStartPos.y + 432, sourX, sourY, 32, 32);
	
}
//기능 함수
NumDigit PkwBattleMapScene::GetNumDigit(int num)
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	//카운트 변수
	int cnt;
	//여러 값들 임시 저장
	int temp1, temp2, temp3;

	int value = num;

	cnt = 0;
	temp1 = 10;

	while (1){
		if (cnt == 0)	numDigit.value[cnt++] = value % 10;

		temp2 = value / temp1;
		if (temp2 == 0){
			break;
		}
		else{
			numDigit.value[cnt++] = temp2 % 10;
			temp1 *= 10;
		}
	}
	numDigit.strlen = cnt;

	return numDigit;
}
void PkwBattleMapScene::choiceUnitHeal()
{
	SkillData skill;
	skill._number = -1;
	_effectPlay.AddEffet(NULL, _choiceUnit, _choiceUnit->_pos, skill);

	_nChoice = 0;

	_choiceUnit->_action = CHARACTOR_ACTION_OFF;
	_choiceUnit->_AniStatus = LEFT_WALK;
	_choiceUnit->_AniNum = 1;
	_battleInfo.charactorControl = CO_CONTROL_OFF;
}
void PkwBattleMapScene::CharactorAttack(POINT target)
{
	POINT temp;

	//아군이 공격할 경우
	if (_choiceUnit->_armyType == 0 || _choiceUnit->_armyType == 1){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
			if (temp.x == target.x && temp.y == target.y){
				_targetUnit = &_pCharactorPlay->_vEnemyCommander[i];
				//BattleProcess(_choiceUnit, &_pCharactorPlay->_vEnemyCommander[i]);
				return;
			}
			//용병 및 소환수
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
				if (temp.x == target.x && temp.y == target.y){
					_targetUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[j];
					//BattleProcess(_choiceUnit, &_pCharactorPlay->_vEnemyCommander[i]._vArms[j]);
					return;
				}
			}
		}
	}
	//적이 공격할 경우
	else if (_choiceUnit->_armyType == 2){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			temp = _pCharactorPlay->_vCoCommander[i]._pos;
			if (temp.x == target.x && temp.y == target.y){
				_targetUnit = &_pCharactorPlay->_vCoCommander[i];
				//BattleProcess(_choiceUnit, &_pCharactorPlay->_vCoCommander[i]);
				return;
			}
			//용병 및 소환수
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
				if (temp.x == target.x && temp.y == target.y){
					_targetUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[j];
					//BattleProcess(_choiceUnit, &_pCharactorPlay->_vCoCommander[i]._vArms[j]);
					return;
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			temp = _pCharactorPlay->_vSupportCommander[i]._pos;
			if (temp.x == target.x && temp.y == target.y){
				_targetUnit = &_pCharactorPlay->_vSupportCommander[i];
				//BattleProcess(_choiceUnit, &_pCharactorPlay->_vSupportCommander[i]);
				return;
			}
			//용병 및 소환수
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
				if (temp.x == target.x && temp.y == target.y){
					_targetUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[j];
					//BattleProcess(_choiceUnit, &_pCharactorPlay->_vSupportCommander[i]._vArms[j]);
					return;
				}
			}
		}
	}
}
void PkwBattleMapScene::BattleProcess(PkwCharactor* atkPlayer, PkwCharactor* defPlayer)
{
	int atkPlayerHp, defPlayerHp;
	int demage, atk, random;
	atkPlayerHp = atkPlayer->_curHp;
	defPlayerHp = defPlayer->_curHp;

	//공격자의 공격 처리
	atk = atkPlayer->_curAtk.value - defPlayer->_curDef.value;
	/*if (atk > 9) atk = 9;
	else if (atk < -9) atk = -9;*/

	demage = 0;
	for (int i = 0; i < atkPlayerHp; i++){
		random = RND->getInt(101) - 60;//공격테이블 -60 ~ 40
		if (random + atk>0) demage++;
	}
	defPlayer->_curHp -= demage;
	if (defPlayer->_curHp < 0) defPlayer->_curHp = 0;
	//방어자의 공격 처리
	atk = defPlayer->_curAtk.value - atkPlayer->_curDef.value;
	/*if (atk > 9) atk = 9;
	else if (atk < -9) atk = -9;*/

	demage = 0;
	for (int i = 0; i < defPlayerHp; i++){
		random = RND->getInt(101) - 60;//공격테이블 -60 ~ 40
		if (random + atk>0) demage++;
	}
	atkPlayer->_curHp -= demage;
	if (atkPlayer->_curHp < 0) atkPlayer->_curHp = 0;
}
void PkwBattleMapScene::mouseImageFrameUpdate()
{
	_mouseImageFrame.elaspedTime += _elapsed;
	if (_mouseImageFrame.elaspedTime < _mouseImageFrame.frameDelay) return;

	_mouseImageFrame.elaspedTime = 0.0;

	if (_mouseImageFrame.frameDir == NORMAL_DIR){
		_mouseImageFrame.frameX++;
		if (_mouseImageFrame.frameX >= _mouseImageFrame.maxFrameX){
			_mouseImageFrame.frameX = _mouseImageFrame.maxFrameX - 1;
			_mouseImageFrame.frameDir = REVERSE_DIR;
		}
	}
	else if (_mouseImageFrame.frameDir == REVERSE_DIR){
		_mouseImageFrame.frameX--;
		if (_mouseImageFrame.frameX < 0){
			_mouseImageFrame.frameX = 0;
			_mouseImageFrame.frameDir = NORMAL_DIR;
		}
	}
}
/*
마법 을 선택한뒤 그것을 어느곳에 쓰는지 처리 해주는 함수들
*/
void PkwBattleMapScene::targetAtkMagic(POINT pos)
{
	POINT temp;
	//선택 유닛이 아군유닛이나 지원군이떄 적 유닛을 공격했는지 여부 확인
	if (_choiceUnit->_armyType == 0 || _choiceUnit->_armyType == 1){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vEnemyCommander[i];

				_actionProcess._vMigScalePos.clear();
				_actionProcess.magicScalePosSave(_targetUnit);
				_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[j];

					_actionProcess._vMigScalePos.clear();
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					return;
				}
			}
		}
	}
	//선택 유닛이 적군 유닛이면 아군이나 지원군을 공격했는지 확인
	else if (_choiceUnit->_armyType == 2){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			temp = _pCharactorPlay->_vCoCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vCoCommander[i];

				_actionProcess._vMigScalePos.clear();
				_actionProcess.magicScalePosSave(_targetUnit);
				_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[j];

					_actionProcess._vMigScalePos.clear();
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					return;
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			temp = _pCharactorPlay->_vSupportCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vSupportCommander[i];

				_actionProcess._vMigScalePos.clear();
				_actionProcess.magicScalePosSave(_targetUnit);
				_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[j];

					_actionProcess._vMigScalePos.clear();
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					return;
				}
			}
		}
	}
}
void PkwBattleMapScene::targetDefMagic(POINT pos)
{
	POINT temp;

	//선택 유닛이 아군이나 지원군이면 아군이나 지원군 유닛을 선택 했는지 확인
	if (_choiceUnit->_armyType == 0 || _choiceUnit->_armyType == 1){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			temp = _pCharactorPlay->_vCoCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vCoCommander[i];
				_actionProcess._vMigScalePos.clear();
				
				if (_choiceSkill->_type == 3){
					_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
				}
				else{
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				}
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vCoCommander[i]._vArms[j];
					_actionProcess._vMigScalePos.clear();

					if (_choiceSkill->_type == 3){
						_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
					}
					else{
						_actionProcess.magicScalePosSave(_targetUnit);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					}
					return;
				}
			}
		}
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			temp = _pCharactorPlay->_vSupportCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vSupportCommander[i];
				_actionProcess._vMigScalePos.clear();

				if (_choiceSkill->_type == 3){
					_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
				}
				else{
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				}
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vSupportCommander[i]._vArms[j];
					_actionProcess._vMigScalePos.clear();

					if (_choiceSkill->_type == 3){
						_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
					}
					else{
						_actionProcess.magicScalePosSave(_targetUnit);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					}
					return;
				}
			}
		}
	}
	//선택 유닛이 적군이면 적 유닛을 선택 했는지 여부 확인
	else if (_choiceUnit->_armyType == 2){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
			if (temp.x == pos.x && temp.y == pos.y){
				_targetUnit = &_pCharactorPlay->_vEnemyCommander[i];
				_actionProcess._vMigScalePos.clear();

				if (_choiceSkill->_type == 3){
					_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
				}
				else{
					_actionProcess.magicScalePosSave(_targetUnit);
					_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
				}
				return;
			}
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
				if (temp.x == pos.x && temp.y == pos.y){
					_targetUnit = &_pCharactorPlay->_vEnemyCommander[i]._vArms[j];
					_actionProcess._vMigScalePos.clear();

					if (_choiceSkill->_type == 3){
						_actionProcess.magicScalePosSave(_targetUnit->_pos, _choiceSkill->_atkScale);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_POS_CHOICE;
					}
					else{
						_actionProcess.magicScalePosSave(_targetUnit);
						_battleInfo.charactorControl = CO_CONTROL_MAGIC_TARGET_CHOICE;
					}
					return;
				}
			}
		}
	}
}
void PkwBattleMapScene::teleportPos(POINT pos)
{
	POINT temp;
	int num;

	num = pos.x + (pos.y * _pMapData->_map->width);

	// 맵을 벗어났을때
	if (pos.x < 0 || pos.x >= _pMapData->_map->width)	return;
	else if (pos.y < 0 || pos.y >= _pMapData->_map->height)	return;
	// 갈수 없는 지역일때
	else if (_pMapData->_map->map[num] == 0)	return;
	else if (_pMapData->_map->map[num] == 1)	return;
	else if (_pMapData->_map->map[num] == 2)	return;
	else if (_pMapData->_map->map[num] == 3)	return;

	//아군 캐릭터와 겹치는 지역 안됨
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		temp = _pCharactorPlay->_vCoCommander[i]._pos;
		if (pos.x == temp.x && pos.y == temp.y) return;

		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
			temp = _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos;
			if (pos.x == temp.x && pos.y == temp.y) return;
		}
	}
	//지원군 캐릭터와 겹치는 지역 안됨
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		temp = _pCharactorPlay->_vSupportCommander[i]._pos;
		if (pos.x == temp.x && pos.y == temp.y) return;

		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			temp = _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos;
			if (pos.x == temp.x && pos.y == temp.y) return;
		}
	}
	//적군 캐릭터와 겹치는 지역 안됨
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		temp = _pCharactorPlay->_vEnemyCommander[i]._pos;
		if (pos.x == temp.x && pos.y == temp.y) return;

		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			temp = _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos;
			if (pos.x == temp.x && pos.y == temp.y) return;
		}
	}

	//마법 범위 안에 위치를 찍었는지 확인 
	for (int i = 0; i < _actionProcess._vMigScalePos.size(); i++){
		temp = _actionProcess._vMigScalePos[i];
		if (pos.x == temp.x && pos.y == temp.y){
			_effectPlay.AddEffet(_choiceUnit, _targetUnit, pos, *_choiceSkill);
			_nChoice = 0;

			DATAMANAGER->setBmSceneData(_battleInfo, _choiceUnit, _choiceSkill, _choiceSummon, _targetUnit);

			_choiceUnit->_action = CHARACTOR_ACTION_OFF;
			_choiceUnit->_AniStatus = LEFT_WALK;
			_choiceUnit->_AniNum = 1;
			_choiceUnit = NULL;

			_battleInfo.charactorControl = CO_CONTROL_OFF;

			_callbackFunctionParameter1(_obj);
			break;
		}
	}
}

void PkwBattleMapScene::turnStarsetUnit(int type)
{
	if (type == 0){
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				_pCharactorPlay->_vCoCommander[i]._vArms[j]._action = CHARACTOR_ACTION_ON;
				_pCharactorPlay->_vCoCommander[i]._vArms[j]._AniStatus = BOTTOM_WALK;
				_pCharactorPlay->_vCoCommander[i]._vArms[j]._AniNum = 0;
			}
			_pCharactorPlay->_vCoCommander[i]._action = CHARACTOR_ACTION_ON;
			_pCharactorPlay->_vCoCommander[i]._AniStatus = BOTTOM_WALK;
			_pCharactorPlay->_vCoCommander[i]._AniNum = 0;
		}
	}
	else if (type == 1){
		for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
				_pCharactorPlay->_vSupportCommander[i]._vArms[j]._action = CHARACTOR_ACTION_ON;
				_pCharactorPlay->_vSupportCommander[i]._vArms[j]._AniStatus = BOTTOM_WALK;
				_pCharactorPlay->_vSupportCommander[i]._vArms[j]._AniNum = 0;
			}
			_pCharactorPlay->_vSupportCommander[i]._action = CHARACTOR_ACTION_ON;
			_pCharactorPlay->_vSupportCommander[i]._AniStatus = BOTTOM_WALK;
			_pCharactorPlay->_vSupportCommander[i]._AniNum = 0;
		}
	}
	else if (type == 2){
		for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._action = CHARACTOR_ACTION_ON;
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._AniStatus = BOTTOM_WALK;
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._AniNum = 0;
			}
			_pCharactorPlay->_vEnemyCommander[i]._action = CHARACTOR_ACTION_ON;
			_pCharactorPlay->_vEnemyCommander[i]._AniStatus = BOTTOM_WALK;
			_pCharactorPlay->_vEnemyCommander[i]._AniNum = 0;
		}
	}
}

void PkwBattleMapScene::soundPlay()
{
	if (_battleInfo.battleStatus == CO_TURN){

		SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
		memset(SOUNDMANAGER->_curBGSound, 0, 128);

		if (_battleInfo.nTurn % 3 == 1)
			memcpy(SOUNDMANAGER->_curBGSound, "4_coTurn1", strlen("4_coTurn1"));
		else if (_battleInfo.nTurn % 3 == 2)
			memcpy(SOUNDMANAGER->_curBGSound, "5_coTurn2", strlen("5_coTurn2"));
		else
			memcpy(SOUNDMANAGER->_curBGSound, "10_ending", strlen("10_ending"));

		SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);
	}
	else if (_battleInfo.battleStatus == SUPPORT_TURN){
		SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
		memset(SOUNDMANAGER->_curBGSound, 0, 128);

		memcpy(SOUNDMANAGER->_curBGSound, "6_suportTurn", strlen("6_suportTurn"));
		SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);
	}
	else if (_battleInfo.battleStatus == ENEMY_TURN){
		SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
		memset(SOUNDMANAGER->_curBGSound, 0, 128);

		if (_battleInfo.nTurn % 2 == 1)
			memcpy(SOUNDMANAGER->_curBGSound, "7_enemyTurn1", strlen("7_enemyTurn1"));
		else
			memcpy(SOUNDMANAGER->_curBGSound, "8_enemyTurn2", strlen("8_enemyTurn2"));

		SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);
	}
}