#include "../stdafx.h"
#include "PkwOpeningScene.h"


PkwOpeningScene::PkwOpeningScene()
{
}

PkwOpeningScene::~PkwOpeningScene()
{
}

HRESULT PkwOpeningScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	_titleChoice = TITLE_CHOICE_OFF;

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;

	_backBuffer = NULL;
	_backBuffer = new image;
	if (FAILED(_backBuffer->init(_nWindowWidth, _nWindowHeight))){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
		return E_FAIL;
	}
	//dc 얻어오기
	_MemDC = _backBuffer->getMemDC();
	//글자 배경모드(TRANSPARENT : 투명 QPAQUEL : 불투명)
	SetBkMode(_MemDC, TRANSPARENT);

	_choiceRc[0].left = 278;	_choiceRc[0].right = 278 + 65;
	_choiceRc[0].top = 385;		_choiceRc[0].bottom = 385 + 16;
	_choiceRc[1].left = 278;	_choiceRc[1].right = 278 + 65;
	_choiceRc[1].top = 407;		_choiceRc[1].bottom = 407 + 16;
	_choiceRc[2].left = 278;	_choiceRc[2].right = 278 + 65;
	_choiceRc[2].top = 432;		_choiceRc[2].bottom = 432 + 16;

	_choiceDelay = 0.0;

	//시스템 출력으르 위한 폰트 set
	HFONT				font;
	font = CreateFont(20, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "Times New Roman");
	SelectObject(_MemDC, font);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	DeleteObject(font);

	SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);
	memset(SOUNDMANAGER->_curBGSound, 0, 128);

	memcpy(SOUNDMANAGER->_curBGSound, "1_opening", strlen("1_opening"));
	SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);

	return S_OK;
}

void PkwOpeningScene::release(void)
{
	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
	}
}
void PkwOpeningScene::update(void)
{
	_elapsed = TIMEMANAGER->getElapsedTime();
	
	frameProcess();
	
	checkChoice();

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		if (_titleChoice == TITLE_START){
			DATAMANAGER->setStage();
			_callbackFunctionParameter(_obj);
		}
		else if (_titleChoice == TITLE_LOAD){

		}
		else if (_titleChoice == TITLE_EXIT){
			PostQuitMessage(WM_DESTROY);
		}
	}
}
void PkwOpeningScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	//배경 이미지 그리기
	IMAGEMANAGER->findImage("titleBack")->render(_MemDC, 0, 0);
	
	//선택 이미지 그리기
	renderChoice();

	//커서 그리기
	IMAGEMANAGER->findImage("titleCursor")->frameRender(_MemDC, _ptMouse.x, _ptMouse.y);

	//시스템 출력
	sprintf_s(str, "opening Scene - elapse time : %f", _elapsed);
	TextOut(_MemDC, 0, 20, str, strlen(str));

	//백 이미지 그려주기
	_backBuffer->render(DATAMANAGER->_mainDc, 0, 0, 0, 0, _nWindowWidth, _nWindowHeight);
}
void PkwOpeningScene::frameProcess()
{
	_choiceDelay += _elapsed;
	if (_choiceDelay > 0.15){
		_choiceDelay = 0.0;
		int frameX = IMAGEMANAGER->findImage("titleChoice")->getFrameX();
		int max = IMAGEMANAGER->findImage("titleChoice")->getMaxFrameX();
		if (frameX >= IMAGEMANAGER->findImage("titleChoice")->getMaxFrameX())
			IMAGEMANAGER->findImage("titleChoice")->setFrameX(0);
		else
			IMAGEMANAGER->findImage("titleChoice")->setFrameX(++frameX);
	}

	_mouseDelay += _elapsed;
	if (_mouseDelay > 0.1){
		_mouseDelay = 0.0;
		int frameX = IMAGEMANAGER->findImage("titleCursor")->getFrameX();

		if (frameX >= IMAGEMANAGER->findImage("titleCursor")->getMaxFrameX())
			IMAGEMANAGER->findImage("titleCursor")->setFrameX(0);
		else
			IMAGEMANAGER->findImage("titleCursor")->setFrameX(++frameX);
	}
}
void PkwOpeningScene::checkChoice()
{
	if (PtInRect(&_choiceRc[0], _ptMouse))
		_titleChoice = TITLE_START;
	else if (PtInRect(&_choiceRc[1], _ptMouse))
		_titleChoice = TITLE_LOAD;
	else if (PtInRect(&_choiceRc[2], _ptMouse))
		_titleChoice = TITLE_EXIT;
	else
		_titleChoice = TITLE_CHOICE_OFF;
}
void PkwOpeningScene::renderChoice()
{
	if (_titleChoice == TITLE_START)
		IMAGEMANAGER->findImage("titleChoice")->frameRender(_MemDC, _choiceRc[0].left - 55, _choiceRc[0].top-10);
	else if (_titleChoice == TITLE_LOAD)
		IMAGEMANAGER->findImage("titleChoice")->frameRender(_MemDC, _choiceRc[1].left - 55, _choiceRc[1].top-10);
	else if (_titleChoice == TITLE_EXIT)
		IMAGEMANAGER->findImage("titleChoice")->frameRender(_MemDC, _choiceRc[2].left - 55, _choiceRc[2].top-10);
}
