// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
//=============================================
//2014.12.10 ## 헤더 파일 선언해준다.
//=============================================

// SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.

// 이전 Windows 플랫폼에 대해 응용 프로그램을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT 매크로를 지원하려는 플랫폼으로 설정하십시오.
#include "targetver.h"
//#include <SDKDDKVer.h>

//MFC가 아닌 응용 프로그램의 경우에는
//WIN32_LEAN_AND_MEAN을 선언하여 commdlg.h 등등
//필요없는 헤더파일을 인클루드 하지 않게 해줌해줌

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>	//스탠다드 입출력 헤더 인클루드 (printf, scanf)
#include <tchar.h>	//윈도우에서 사용할 문자열 출력 헤더 인클루드
//MBCS(Multi Byte Character Set)
//->기존에 사용하던 멀티바이트 형태의 문자열
//WBCS(Wide Byte Character Set)
//->모든 문자를 2바이트로 처리

#include "commonMacroFunction.h"
#include "randomFunction.h"
#include "./2DGameFramework/manager/keyManager.h"
#include "./2DGameFramework/manager/imageManager.h"
#include "./2DGameFramework/manager/timeManager.h"
#include "./2DGameFramework/manager/soundManager.h"
#include "./2DGameFramework/UTIL/utils.h"
#include "./2DGameFramework/txtData/txtData.h"
#include "./pkwSource/PkwDataManager.h"

//
//=============================================
//2014.12.10 ## 디파인문 선언
//=============================================
#define WINNAME (LPTSTR) (TEXT("WindowsAPI"))

/*
#ifdef _DEBUG
	#ifndef DBG_NEW
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW
	#endif
#endif
*/


#define FULLSCREEN

#ifdef FULLSCREEN
#define WINSTARTX 0
#define WINSTARTY 0
#define WINSIZEX GetSystemMetrics(SM_CXSCREEN)
#define WINSIZEY GetSystemMetrics(SM_CYSCREEN)
#else
#define WINSTARTX 0
#define WINSTARTY 0
#define WINSIZEX 640
#define WINSIZEY 480
#endif

#define UNITSIZEX 48
#define UNITSIZEY 48

#define WINSTYLE WS_CAPTION | WS_SYSMENU

#define SAFE_DELETE(p)			{ if(p) {delete (p);		  (p)= NULL;}}
#define SAFE_DELETE_ARRAY(p)	{ if(p) {delete[] (p);		  (p)= NULL;}}
#define SAFE_RELEASE(p)			{ if(p) { (p)->release();     (p)= NULL;}}

using namespace MY_UTIL;
#define RND randomFunction::getSingleton()
#define KEYMANAGER keyManager::getSingleton()
#define IMAGEMANAGER imageManager::getSingleton()
#define TIMEMANAGER timeManager::getSingleton()
#define TXTDATA txtData::getSingleton()
#define DATAMANAGER PkwDataManager::getSingleton()
#define SOUNDMANAGER soundManager::getSingleton()
//=============================================
//2014.12.10 ## 디파인문 선언
//=============================================
extern HINSTANCE _hInstance;
extern HWND _hWnd;
extern POINT _ptMouse;
extern RECT _windowRc;
extern RECT _winResolutionRc;