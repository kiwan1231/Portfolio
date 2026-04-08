#pragma once

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwCharactorPlay.h"

enum BATTLESET_MODE{
	BATTLESET_NORMAL,
	BATTLESET_MERCENARY,
	BATTLESET_ITEM,
	BATTLESET_COMMANDERPOS,
	BATTLESET_BATTLESTART
};

enum BS_MERCENARY_MODE{
	BS_MERCENARY_NORMAL,
	BS_MERCENARY_COMMANDER_CHOICE,
	BS_MERCENARY_MERCENARY_CHOICE
};

enum BS_COMMANDERPOS_MODE{
	BS_COMMANDERPOS_NORMAL,
	BS_COMMANDERPOS_COMMANDER_CHOICE
};


class PkwBsNormalScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter1;//shop씬으로 넘어갈때
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter2;//set씬 종료할때

	image*					_mapImage;
	PkwCharactorPlay*		_pCharactorPlay;

	BATTLESET_MODE			_battleSetMode;
	BS_MERCENARY_MODE		_bsMercenaryMode;
	BS_COMMANDERPOS_MODE	_bsCommanderPosMode;
public://선택된 지휘관 관련 변수
	PkwCharactor*			_pChoiceCommander;
	MercenaryData*			_pChoiceMercenary;
	int						_mercenaryMaxList;
	int						_mercenaryCurList;
	int						_nCnoice;
	int						_nCnoice2;
	int						_nCnoice3;
private:
	image*					_backBuffer;
	HDC						_MemDC;
	RECT					_gameView;
	int						_nWindowWidth;
	int						_nWindowHeight;
	int						_MapMaxWidth;
	int						_MapManHeight;
	POINT					_MapStartPos;
	float					_elapsed;

	HFONT					_font[10];
	imageFrameData			_mouseImageFrame;
public:
	PkwBsNormalScene();
	~PkwBsNormalScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public:
	//update관련 함수
	void charactorUpdate();
	void battleSetNormalUpdate();
	void battleSetMercenaryUpdate();
	void battleSetShopUpdate();
	void battleSetCommanderPosUpdate();

	//render관련 함수
	void BackImageDraw();
	void CharactorRender();
	void InterfaceRender();
	void InfoRender();
	void MercenaryListRender();
	void MercenaryChoiceRender();
	
	//기타 함수
	NumDigit GetNumDigit(int num);
	void createMercenary(int num);
	void mercenaryRepos();
	void mouseImageFrameUpdate();
};

