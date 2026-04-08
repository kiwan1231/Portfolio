#include "../stdafx.h"
#include "gameNode.h"


gameNode::gameNode()
{
}


gameNode::~gameNode()
{
}

HRESULT gameNode::init(void)
{
	_hdc = GetDC(_hWnd);
	_managerInit = false;

	return S_OK;
}

HRESULT gameNode::init(bool managerInit)
{
	_hdc = GetDC(_hWnd);
	_managerInit = managerInit;

	if (managerInit)
	{
		SetTimer(_hWnd, 1, 10, NULL);

		KEYMANAGER->init();
		IMAGEMANAGER->init();
		TIMEMANAGER->init();
		DATAMANAGER->init();
		SOUNDMANAGER->init();
	}

	return S_OK;
}
HRESULT gameNode::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	 return S_OK;
}
HRESULT gameNode::init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj)
{
	return S_OK;
}
HRESULT gameNode::init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, CALLBACK_FUNCTION_PARAMETER cbFunction3, void* obj)
{
	return S_OK;
}
void gameNode::release(void)
{
	if (_managerInit)
	{
		KillTimer(_hWnd, 1);

		KEYMANAGER->releaseSingleton();
		IMAGEMANAGER->releaseSingleton();
		TIMEMANAGER->releaseSingleton();
		DATAMANAGER->releaseSingleton();
		SOUNDMANAGER->releaseSingleton();
	}
	//DC 해제해준다
	ReleaseDC(_hWnd, _hdc);
}

void gameNode::update(void)
{
	//더블 버퍼 사용하니 이제 꺼준다.
	InvalidateRect(_hWnd, NULL, false);
}


void gameNode::render(void)
{

}


LRESULT gameNode::MainProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
	/*case WM_TIMER:
		this->update();
		
		break;
	case WM_PAINT:
		this->render();
		break;*/
	case WM_MOUSEMOVE:
		_ptMouse.x = static_cast<float>(LOWORD(lParam));
		_ptMouse.y = static_cast<float>(HIWORD(lParam));
		break;
	/*case WM_KEYDOWN:
		switch (wParam){
		}
		break;*/
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}