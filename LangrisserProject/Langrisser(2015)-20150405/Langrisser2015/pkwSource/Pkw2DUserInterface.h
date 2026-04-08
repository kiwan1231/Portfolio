#pragma once
#include"../common.h"
#include "../stdafx.h"

class PkwCharactor;

class PkwActionProcess;

class Pkw2DUserInterface
{
public:// 게임 매니져에게 받는 변수
	SCENE_MODE			_gameMode;
	CHARACTOR_CONTROL	_control;
	COMMAND_CHOICE		_commandChoice;
	MERCENARY_ORDER_CHOICE	_orderChoice;
	PkwCharactor*		_charactor;
	POINT				_mapStartPos;
	RECT				_gameView;
public:// 인터페이스 창,버튼 렉트
	RECT				_commandWinRc;
	RECT				_commandRc[6];
	RECT				_orderWinRc;
	RECT				_orderRc[4];
	RECT				_charactorWinRc;
public:// 인터페이스 폰트 브러쉬
	HFONT				_hFont[3];
	COLORREF			_color[3];
public://상태 출력 변수
	int	_updateSpeed;
public:
	void Init();
	void release();
	void update();
	void render(HDC MemDC);
	void SetValue(SCENE_MODE gameMode, CHARACTOR_CONTROL control, COMMAND_CHOICE choice, MERCENARY_ORDER_CHOICE orderChoice, PkwCharactor* charactor, POINT mapStartPos, RECT gameView);
	void CommandWinDraw(HDC MemDC);
	void OrderWinDraw(HDC MemDC);
	void CharactorWinDraw(HDC MemDC);
public:
	Pkw2DUserInterface();
	~Pkw2DUserInterface();
};

