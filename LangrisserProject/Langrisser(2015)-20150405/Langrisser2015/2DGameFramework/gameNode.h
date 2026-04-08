#pragma once
#include "./image/image.h"

static image* _backBuffer = IMAGEMANAGER->addImage("backbuffer", WINSIZEX, WINSIZEY);

typedef void(*CALLBACK_FUNCTION_PARAMETER)(void*);

class gameNode
{
private:
	HDC _hdc;
	bool _managerInit;

public:
	virtual HRESULT init(void);
	virtual HRESULT init(bool managerInit);
	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj);
	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, CALLBACK_FUNCTION_PARAMETER cbFunction3, void* obj);

	virtual void release(void);
	virtual void update(void);
	virtual void render(void);

	image* getBackBuffer(void) { return _backBuffer; }
	HDC getMemDC() { return _backBuffer->getMemDC(); }   //메모리 버퍼
	HDC getHDC() { return _hdc; }
	LRESULT MainProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

	gameNode(void);
	virtual ~gameNode(void);
};

