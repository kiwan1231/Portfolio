#include "../stdafx.h"
#include "Pkw2DUserInterface.h"
#include "PkwCharactorPlay.h"
#include "PkwActionProcess.h"
Pkw2DUserInterface::Pkw2DUserInterface()
{
}


Pkw2DUserInterface::~Pkw2DUserInterface()
{
}

void Pkw2DUserInterface::Init()
{
	// 게임 매니져 변수 초기화
	_control = CO_CONTROL_OFF;
	_commandChoice = COMMAND_MOVE;
	_orderChoice = ORDER_BATTLE;
	_mapStartPos.x = 0;
	_mapStartPos.y = 0;
	_charactor = NULL;

	//렉트 초기화
	_commandWinRc = RectMakeCenter(180, (_winResolutionRc.bottom - _winResolutionRc.top)/2, 160, 400);
	_orderWinRc = RectMakeCenter(180+160, (_winResolutionRc.bottom - _winResolutionRc.top) / 2 + 150, 160, 192);
	_charactorWinRc = RectMakeCenter(_winResolutionRc.right - 400, (_winResolutionRc.bottom - _winResolutionRc.top) / 2, 400, 320);
	for (int i = 0; i < 6; i++){
		_commandRc[i].left = _commandWinRc.left + 35;
		_commandRc[i].right = _commandWinRc.right - 35;
		_commandRc[i].top = _commandWinRc.top + 40 + (i * 60);
		_commandRc[i].bottom = _commandRc[i].top + 40;
	}
	for (int i = 0; i < 4; i++){
		_orderRc[i].left = _orderWinRc.left + 35;
		_orderRc[i].right = _orderWinRc.right - 35;
		_orderRc[i].top = _orderWinRc.top + 20 + (i * 40);
		_orderRc[i].bottom = _orderRc[i].top + 30;
	}

	_hFont[0] = CreateFont(40, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "Times New Roman");
	_hFont[1] = CreateFont(25, 0, 0, 0, 0, TRUE, 0, 0, ANSI_CHARSET, 3, 2, 1,VARIABLE_PITCH | FF_SWISS, "Arial Black");
	_hFont[2] = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 3, 2, 1,VARIABLE_PITCH | FF_ROMAN, "Wide Latin");

	_color[0] = RGB(255, 0, 0);
	_color[1] = RGB(0, 255, 0);
	_color[2] = RGB(255, 255, 255);

	
	//상태 출력
	_updateSpeed = 0;
}
void Pkw2DUserInterface::release()
{
	for (int i = 0; i < 3; i++)
		DeleteObject(_hFont[i]);
}
void Pkw2DUserInterface::update()
{

}
void Pkw2DUserInterface::SetValue(SCENE_MODE gameMode, CHARACTOR_CONTROL control, COMMAND_CHOICE choice, MERCENARY_ORDER_CHOICE orderChoice, PkwCharactor* charactor, POINT mapStartPos, RECT gameView)
{
	_gameMode = gameMode;
	_control = control;
	_orderChoice = orderChoice;
	_commandChoice = choice;
	_charactor = charactor;
	_mapStartPos = mapStartPos;
	_gameView = gameView;
}
void Pkw2DUserInterface::render(HDC MemDC)
{
	if (_control == CO_CONTROL_OFF){
	}
	else if (_control == CO_CONTROL_CHOICE){
		CommandWinDraw(MemDC);
		CharactorWinDraw(MemDC);
	}
	else if (_control == CO_CONTROL_MOVE){
	}
	else if (_control == CO_CONTROL_ATTACK){
	}
	else if (_control == CO_CONTROL_MAGIC){
	}
	else if (_control == CO_CONTROL_SUMMON){
	}
	else if (_control == CO_CONTROL_ORDER){
		CommandWinDraw(MemDC);
		CharactorWinDraw(MemDC);
		OrderWinDraw(MemDC);
	}
	else if (_control == SUPPORT_CHOICE){
		CharactorWinDraw(MemDC);
	}
	else if (_control == ENEMY_CHOICE){
		CharactorWinDraw(MemDC);
	}
}

void Pkw2DUserInterface::CommandWinDraw(HDC MemDC)
{
	RECT commandWinRc = _commandWinRc;
	int width, height;
	commandWinRc.left += _mapStartPos.x; commandWinRc.right += _mapStartPos.x;
	commandWinRc.top += _mapStartPos.y; commandWinRc.bottom += _mapStartPos.y;
	width = commandWinRc.right - commandWinRc.left;
	height = commandWinRc.bottom - commandWinRc.top;

	//커맨드 창 출력
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, commandWinRc.left, commandWinRc.top, 0, 0, width, height, 100);
	IMAGEMANAGER->findImage("UI(160X400)")->render(MemDC, commandWinRc.left, commandWinRc.top);

	//커맨드 보기 출력
	char str[6][30] = { "이 동", "공 격", "마 법","소 환","치 료","명 령" };
	
	SelectObject(MemDC, _hFont[0]);
	SetTextColor(MemDC, _color[2]);
	for (int i = 0; i < 6; i++){
		TextOut(MemDC, _commandRc[i].left + _mapStartPos.x, _commandRc[i].top + _mapStartPos.y, str[i], strlen(str[i]));
	}
	//커맨드 선택 화살표 출력
	IMAGEMANAGER->findImage("choiceArrow")->render(MemDC, _commandRc[_commandChoice].left + _mapStartPos.x - 15, _commandRc[_commandChoice].top + _mapStartPos.y);
}
void Pkw2DUserInterface::OrderWinDraw(HDC MemDC)
{
	RECT orderWinRc = _orderWinRc;
	int width, height;

	SelectObject(MemDC, _hFont[0]);
	SetTextColor(MemDC, _color[2]);

	orderWinRc.left += _mapStartPos.x; orderWinRc.right += _mapStartPos.x;
	orderWinRc.top += _mapStartPos.y; orderWinRc.bottom += _mapStartPos.y;
	width = orderWinRc.right - orderWinRc.left;
	height = orderWinRc.bottom - orderWinRc.top;

	//용병 명령 창 출력
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, orderWinRc.left, orderWinRc.top, 0, 0, width, height, 100);
	IMAGEMANAGER->findImage("UI(160X192)")->render(MemDC, orderWinRc.left, orderWinRc.top);
	///용병 명령 보기 출력
	char str[4][30] = { "전 투", "돌 격", "방 어", "수 동" };
	for (int i = 0; i < 4; i++)
		TextOut(MemDC, _orderRc[i].left + _mapStartPos.x, _orderRc[i].top + _mapStartPos.y, str[i], strlen(str[i]));
	
	//용병 명령 선택 화살표 출력
	IMAGEMANAGER->findImage("choiceArrow")->render(MemDC, _orderRc[_orderChoice].left + _mapStartPos.x - 15, _orderRc[_orderChoice].top + _mapStartPos.y);
}
void Pkw2DUserInterface::CharactorWinDraw(HDC MemDC)
{
	RECT temp;
	char chLine[128];
	int width, height, sourX, sourY;
	RECT charactorWinRc = _charactorWinRc;

	SelectObject(MemDC, _hFont[1]);
	SetTextColor(MemDC, _color[2]);

	charactorWinRc.left += _mapStartPos.x; charactorWinRc.right += _mapStartPos.x;
	charactorWinRc.top += _mapStartPos.y; charactorWinRc.bottom += _mapStartPos.y;
	width = charactorWinRc.right - charactorWinRc.left;
	height = charactorWinRc.bottom - charactorWinRc.top;

	//캐릭터 상태창 출력
	if (_charactor->_armyType == 0)
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(MemDC, charactorWinRc.left, charactorWinRc.top, 0, 0, width, height, 100);
	else if (_charactor->_armyType == 1)
		IMAGEMANAGER->findImage("UI_GREEN_BACK")->alphaRender(MemDC, charactorWinRc.left, charactorWinRc.top, 0, 0, width, height, 100);
	else if (_charactor->_armyType == 2)
		IMAGEMANAGER->findImage("UI_RED_BACK")->alphaRender(MemDC, charactorWinRc.left, charactorWinRc.top, 0, 0, width, height, 100);

	IMAGEMANAGER->findImage("UI(400X320)")->render(MemDC, charactorWinRc.left, charactorWinRc.top);
	
	//캐릭터 얼굴창 출력
	width = IMAGEMANAGER->findImage("charactorfaceWin")->getWidth();
	height = IMAGEMANAGER->findImage("charactorfaceWin")->getHeight();
	temp.left = charactorWinRc.left - width / 2;	temp.right = temp.left + width;
	temp.top = charactorWinRc.top - height / 2;		temp.bottom = temp.top + height;
	IMAGEMANAGER->findImage("charactorfaceWin")->render(MemDC, temp.left, temp.top);
	//캐릭터 얼굴 출력
	IMAGEMANAGER->findImage(_charactor->_faceImageKey)->render(MemDC, temp.left+16, temp.top+16);

	//캐릭터 정보 출력
	TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 30, _charactor->_name, strlen(_charactor->_name));
	TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 70, _charactor->_chClass, strlen(_charactor->_chClass));

	if (_charactor->_type == 0)
		TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 110, "보병", strlen("보병"));
	else if (_charactor->_type == 1)
		TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 110, "기병", strlen("기병"));
	else if (_charactor->_type == 2)
		TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 110, "수병", strlen("수병"));
	else if (_charactor->_type == 3)
		TextOut(MemDC, charactorWinRc.left + 100, charactorWinRc.top + 110, "비병", strlen("비병"));

	sourX = (_charactor->_atkItem._imageNum % 8) * 32;
	sourY = (_charactor->_atkItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(MemDC, charactorWinRc.left + 200, charactorWinRc.top + 105, sourX, sourY, 32, 32);
	sourX = (_charactor->_DefItem._imageNum % 8) * 32;
	sourY = (_charactor->_DefItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(MemDC, charactorWinRc.left + 250, charactorWinRc.top + 105, sourX, sourY, 32, 32);

	
	memset(chLine, 0, 128);

	wsprintf(chLine, "   LV   %d                 EXP %d / %d", _charactor->_level, _charactor->_curExp, _charactor->_maxExp);
	TextOut(MemDC, charactorWinRc.left + 20, charactorWinRc.top + 150, chLine, strlen(chLine));
	
	wsprintf(chLine, "   AT   %d               M P %d / %d", _charactor->_atk, _charactor->_curMp, _charactor->_maxMp);
	TextOut(MemDC, charactorWinRc.left + 20, charactorWinRc.top + 180, chLine, strlen(chLine));
	
	wsprintf(chLine, "   DF   %d               M V %d", _charactor->_def, _charactor->_move);
	TextOut(MemDC, charactorWinRc.left + 20, charactorWinRc.top + 210, chLine, strlen(chLine));

	wsprintf(chLine, "   지휘범위        지휘력    ");
	TextOut(MemDC, charactorWinRc.left + 20, charactorWinRc.top + 240, chLine, strlen(chLine));

	memset(chLine, 0, 128);
	if (_charactor->_mercenartOrder == MERCENARY_BATTLE)	memcpy(chLine, "전투", strlen("전투"));
	else if (_charactor->_mercenartOrder == MERCENARY_ATTACK)	memcpy(chLine, "돌격", strlen("돌격"));
	else if (_charactor->_mercenartOrder == MERCENARY_DEFENCE)	memcpy(chLine, "방어", strlen("방어"));
	else if (_charactor->_mercenartOrder == MERCENARY_ANAUTO)	memcpy(chLine, "수동", strlen("수동"));
	TextOut(MemDC, charactorWinRc.left + 300, charactorWinRc.top + 240, chLine, strlen(chLine));
	
	wsprintf(chLine, "         %d               A+%d  D+%d", _charactor->_commandRange, _charactor->_commandAtk, _charactor->_commandDef);
	TextOut(MemDC, charactorWinRc.left + 40, charactorWinRc.top + 270, chLine, strlen(chLine));
}
