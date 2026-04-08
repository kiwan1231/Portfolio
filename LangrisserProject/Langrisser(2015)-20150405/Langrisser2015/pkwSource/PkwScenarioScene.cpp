#include "../stdafx.h"
#include "PkwScenarioScene.h"


PkwScenarioScene::PkwScenarioScene()
{
}


PkwScenarioScene::~PkwScenarioScene()
{
}

HRESULT PkwScenarioScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	//-----SCENARIO_MOVE 변수 초기화
	_startPos.x = 419;  _startPos.y = 622;
	_destPos.x = 342;	_destPos.y = 604;
	_curPosX = 419.0;	_curPosY = 622;

	float distance = MY_UTIL::getDistance(_startPos.x, _startPos.y, _destPos.x, _destPos.y);
	_speed = distance / 1.5;

	//-----SCENARIO_SHOW 변수 초기화
	_pageMode = START_PAGE;
	_vScenarioTXT.clear();
	memset(_txtFilePath, 0, 128);
	memcpy(_txtFilePath, "scenario\\scenario-01.txt", strlen("scenario\\scenario-01.txt"));
	_txtFile.open(_txtFilePath);

	//----- 공용 변수 초기화
	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;
	_imageSour.x = 0; _imageSour.y = 0;

	_backImageWidth = IMAGEMANAGER->findImage("scenarioScene(ori)")->getWidth();
	_backImageHeight = IMAGEMANAGER->findImage("scenarioScene(ori)")->getHeight();

	_scenarioMode = SCENARIO_MOVE;
	_modeDelay = 0.0;
	_scenarioNum = DATAMANAGER->_stageNumber;

	//캐릭터 데이터 클래스
	for (int i = 0; i <DATAMANAGER->_DataBase._vCommandData.size(); i++)
		_CharactorPlay.CreateCharactor(&DATAMANAGER->_DataBase._vCommandData[i]);

	_backBuffer = NULL;
	_backBuffer = new image;
	if (FAILED(_backBuffer->init(_backImageWidth, _backImageHeight))){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
		return E_FAIL;
	}

	//dc 얻어오기
	_MemDC = _backBuffer->getMemDC();
	//글자 배경모드(TRANSPARENT : 투명 QPAQUEL : 불투명)
	SetBkMode(_MemDC, TRANSPARENT);

	// 폰트 set
	HFONT				font;
	font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "Times New Roman");
	SelectObject(_MemDC, font);
	SetTextColor(_MemDC, RGB(255,255, 255));
	DeleteObject(font);

	SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
	memset(SOUNDMANAGER->_curBGSound, 0, 128);

	memcpy(SOUNDMANAGER->_curBGSound, "2_scenario", strlen("2_scenario"));
	SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);

	return S_OK;
}
void PkwScenarioScene::release(void)
{
	_vScenarioTXT.clear();

	_CharactorPlay.CommanderRelease();

	_txtFile.close();

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
	}
}
void PkwScenarioScene::update(void)
{
	_elapsed = TIMEMANAGER->getElapsedTime();
	_modeDelay += _elapsed;

	// 주인공 시나리오에 의한 맵이동 보여주기
	if (_scenarioMode == SCENARIO_MOVE){
		_CharactorPlay._vCoCommander[0]._AniStatus = BOTTOM_WALK;
		_CharactorPlay._vCoCommander[0]._pos.x = _curPosX;
		_CharactorPlay._vCoCommander[0]._pos.y = _curPosY;
		_CharactorPlay._vCoCommander[0].update(_elapsed);

		MapMove();
		if (_modeDelay > 4.0){
			_scenarioMode = SCENARIO_COMMANDER_SHOW;
			_modeDelay = 0.0;
		}
	}
	//이번 스테이지 출격하는 지휘관 보여주기
	else if (_scenarioMode == SCENARIO_COMMANDER_SHOW){
		int startX = WINSIZEX / 2 - UNITSIZEX / 2;
		for (int i = 0; i < _CharactorPlay._vCoCommander.size(); i++){
			if (i == 0)
				_CharactorPlay._vCoCommander[i]._pos.x = _imageSour.x + startX;
			else if (i % 2 == 0)
				_CharactorPlay._vCoCommander[i]._pos.x = _imageSour.x + startX + (i / 2)* UNITSIZEX;
			else if (i % 2 == 1)
				_CharactorPlay._vCoCommander[i]._pos.x = _imageSour.x + startX - (i / 2 + 1)* UNITSIZEX;
			_CharactorPlay._vCoCommander[i]._AniStatus = BOTTOM_WALK;
			_CharactorPlay._vCoCommander[i]._pos.y = _imageSour.y + (WINSIZEY / 2) - (UNITSIZEY / 2);
			_CharactorPlay._vCoCommander[i].update(_elapsed*3);
		}
		if (_modeDelay > 2.0){
			_scenarioMode = SCENARIO_SHOW;
			_modeDelay = 0.0;
		}
	}
	// 이번 시나리오 보여주기
	else if (_scenarioMode == SCENARIO_SHOW){
		/*if (_modeDelay > 1.5){
			_modeDelay = 0.0;
			loadScenarioTXT();
		}*/
		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			loadScenarioTXT();
		}
	}
	//이번 시나리오 승리조건 패배조건을 보여주면고 마우스 클릭 입력 받으면 시나리오 씬 종료 한다
	else if (_scenarioMode == SCENARIO_EXIT){
		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			//버튼 눌르면 씬 종료
			_callbackFunctionParameter(_obj);
		}
	}
	
}
void PkwScenarioScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	//배경 이미지 그리기
	if (_scenarioMode == SCENARIO_MOVE || _scenarioMode == SCENARIO_COMMANDER_SHOW)
		IMAGEMANAGER->findImage("scenarioScene(ori)")->render(_MemDC, 0, 0);
	else
		IMAGEMANAGER->findImage("scenarioScene(black)")->render(_MemDC, 0, 0);

	if (_scenarioMode == SCENARIO_MOVE){
		_CharactorPlay._vCoCommander[0].render(_MemDC, false);
	}
	else if (_scenarioMode == SCENARIO_COMMANDER_SHOW){
		for (int i = 0; i < _CharactorPlay._vCoCommander.size(); i++){
			_CharactorPlay._vCoCommander[i].render(_MemDC, false);
		}
	}
	else if (_scenarioMode == SCENARIO_SHOW || _scenarioMode == SCENARIO_EXIT){
		scenarioTXTRender();
	}

	//백 이미지 그려주기
	BackImageRender();
}

void PkwScenarioScene::BackImageRender()
{
	int temp;

	_imageSour.x = _curPosX - 640 / 2;
	_imageSour.y = _curPosY - 480 / 2;

	//출력 위치 x 정하기
	if (_imageSour.x + 640 > _backImageWidth){
		temp = _imageSour.x + 640 - _backImageWidth;
		_imageSour.x = _imageSour.x - temp;
	}

	//출력 위치 y 정하기
	if (_imageSour.y + 480 > _backImageHeight){
		temp = _imageSour.y + 480 - _backImageHeight;
		_imageSour.y = _imageSour.y - temp;
	}
	
	_backBuffer->render(DATAMANAGER->_mainDc, 0, 0, _imageSour.x, _imageSour.y, _nWindowWidth, _nWindowHeight);
}

void PkwScenarioScene::scenarioTXTRender()
{
	int length;
	POINT pos;
	
	for (int i = 0; i < _vScenarioTXT.size(); i++){
		length = strlen(_vScenarioTXT[i].data);

		if (_pageMode == START_PAGE){
			if (i == 0)
				pos.x = 240;
			else if (i == 1)
				pos.x = 288;
		}
		else if (_pageMode == NEXT_PAGE)
			pos.x = _imageSour.x + 160;
		else if (_pageMode == FINAL_PAGE)
			pos.x = _imageSour.x + 70;

		pos.y = _imageSour.y + 100 + (40 * i);
		TextOut(_MemDC, pos.x, pos.y, _vScenarioTXT[i].data, length);
	}
}

void PkwScenarioScene::MapMove()
{
	float distance = MY_UTIL::getDistance(_curPosX, _curPosY, _destPos.x, _destPos.y);
	float angel = MY_UTIL::getAngle(_curPosX, _curPosY, _destPos.x, _destPos.y);

	if (distance > 1){
		_curPosX += cosf(angel) * _speed * _elapsed;
		_curPosY += -sinf(angel) * _speed * _elapsed;
	}
}
void PkwScenarioScene::loadScenarioTXT()
{
	scenarioData data;
	char temp[128];
	
	_vScenarioTXT.clear();

	while (1){

		_txtFile.getline(temp, 128);

		if (strcmp(temp, "[START_PAGE]") == 0){
			_pageMode = START_PAGE;
			break;
		}
		else if (strcmp(temp, "[NEXT_PAGE]") == 0){
			_pageMode = NEXT_PAGE;
			break;
		}
		else if (strcmp(temp, "[FINAL_PAGE]") == 0){
			_pageMode = FINAL_PAGE;
			_scenarioMode = SCENARIO_EXIT;
			break;
		}
		else{
			memset(data.data, 0, 128);
			memcpy(data.data, temp, strlen(temp));
			_vScenarioTXT.push_back(data);
		}
	}
}