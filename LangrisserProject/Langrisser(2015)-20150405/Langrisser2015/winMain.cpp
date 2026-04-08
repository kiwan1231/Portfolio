#pragma once

//============================================
// 해더 선언 ##
//============================================
#include"common.h"
#include "stdafx.h"
#include "./pkwSource/PkwGameManager.h"


//============================================
// 전역 변수 선언 ##
//============================================


HINSTANCE				_hInstance;//프로그램 인스턴스(메모리상에 할당되어 실행중인 프로그램)
HWND					_hWnd;	//윈도우 핸들
POINT					_ptMouse = { 0, 0 };//마우스 위치 저장 변수
RECT					_windowRc;
RECT					_winResolutionRc;
PkwGameManager			_GameManager;
LPTSTR _lpszClass = TEXT("Langrisser2015");//윈도우 타이틀

//============================================
// 함수의 프로토 타입 선언 ##
//============================================
void setWindowSize(int x, int y, int width, int height);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	//인스턴스를 전역변수에 담아준다.
	_hInstance = hInstance;

	//MSG : 운영체제에서 발행하는 메시지 정보를 저장하기 위한 구조체
	MSG message;

	//WNDCLASS : 윈도우의 정보를 저장하기 위한 구조체
	WNDCLASS wndClass;

	wndClass.cbClsExtra = 0;           //클래스 여분 메모리
	wndClass.cbWndExtra = 0;           //윈도우 여분 메모리
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);  //백그라운드
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);      //커서
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);     //아이콘
	wndClass.hInstance = hInstance;          //인스턴스
	wndClass.lpfnWndProc = (WNDPROC)WndProc;       //윈도우 프로시져
	wndClass.lpszClassName = _lpszClass;        //클래스 이름
	wndClass.lpszMenuName = NULL;          //메뉴 이름
	wndClass.style = CS_HREDRAW | CS_VREDRAW;       //윈도우 쓰따일~

	RegisterClass(&wndClass);
	
	_winResolutionRc.top = 0; _winResolutionRc.bottom = 480;
	_winResolutionRc.left = 0; _winResolutionRc.right = 640;
	_windowRc.top = 0;	_windowRc.bottom = WINSIZEY;//GetSystemMetrics(SM_CYSCREEN);
	_windowRc.left = 0;	_windowRc.right = WINSIZEX;// GetSystemMetrics(SM_CXSCREEN);
	//------------------------------------------------

#ifdef FULLSCREEN
	DEVMODE dm;

	ZeroMemory(&dm, sizeof(DEVMODE));

	dm.dmSize = sizeof(DEVMODE);
	dm.dmBitsPerPel = 32;
	dm.dmPelsWidth = _winResolutionRc.right;
	dm.dmPelsHeight = _winResolutionRc.bottom;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	//원래 화면 해상도로 되돌리는 코드
	if ((ChangeDisplaySettings(&dm, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL))
	{
		ChangeDisplaySettings(&dm, 0);
	}


	_hWnd = CreateWindow(
		_lpszClass,				//윈도우 클래스의 이름
		_lpszClass,				//윈도우 타이틀바 이름
		WS_POPUP,
		//WS_POPUPWINDOW | WS_MAXIMIZE,				//윈도우 스따일
		_windowRc.left,				//윈도우 왼쪽상단 origin x 좌표
		_windowRc.top,				//윈도우 왼쪽상단 origin y 좌표
		_windowRc.right,				//윈도우 가로크기
		_windowRc.bottom,				//윈도우 세로크기
		NULL,					//부모 윈도우
		(HMENU)NULL,			//메뉴핸들
		hInstance,				//인스턴스 지정
		NULL);					//윈도우 및 자식 윈도우 생성되면

	//화면 작업 사이즈 영역 계산
	//setWindowSize(WINSTARTX, WINSTARTY,GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

#else
	_hWnd = CreateWindow(
		_lpszClass,				//윈도우 클래스의 이름
		_lpszClass,				//윈도우 타이틀바 이름
		WS_POPUP,				//윈도우 스따일
		_windowRc.left,				//윈도우 왼쪽상단 origin x 좌표
		_windowRc.top,				//윈도우 왼쪽상단 origin y 좌표
		_windowRc.right,				//윈도우 가로크기
		_windowRc.bottom,				//윈도우 세로크기
		NULL,					//부모 윈도우
		(HMENU)NULL,			//메뉴핸들
		hInstance,				//인스턴스 지정
		NULL);					//윈도우 및 자식 윈도우 생성되면
	_winResolutionRc = _windowRc;
#endif
	
	//------------------------------------------------
	ShowWindow(_hWnd, nCmdShow);
	
	//ClipCursor(&_windowRc); //마우스 가두기
	
	if (FAILED(_GameManager.init())){
		//게임매니저 로드 실패시 바로 종료
		MessageBox(NULL, TEXT("Can not _GameManager Load."), TEXT("ERROR"), MB_OK);
		return 0;
	}

	int num = 0;
	char str[128];
	while (true)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT) break;
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			TIMEMANAGER->update(60.0F);
			_GameManager.update();
			_GameManager.render();
		}
	}

	//게임데이터 해제
	_GameManager.release();

	UnregisterClass(WINNAME, hInstance);

	return message.wParam;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	return _GameManager.MainProc(hWnd, iMessage, wParam, lParam);
}
void setWindowSize(int x, int y, int width, int height)
{
	//윈도우 작업영역 지정
	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = width;
	rc.bottom = height;

	//실제 윈도우 크기 조정
	AdjustWindowRect(&rc, WINSTYLE, false);

	//위의 RECT 정보로 윈도우 사이즈 셋팅
	SetWindowPos(_hWnd, NULL, x, y, (rc.right - rc.left), (rc.bottom - rc.top), SWP_NOZORDER | SWP_NOMOVE);
}