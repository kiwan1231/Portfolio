#pragma once

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwCharactorPlay.h"

enum SHOP_MODE{
	SHOP_NORMAL,
	SHOP_EQUIP_ATTACKITEM,
	SHOP_EQUIP_DEFENCEITEM,
	SHOP_BUY,
	SHOP_SELL
};

class PkwBsShopScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;

	SHOP_MODE _shopMode;
public:
	PkwCharactorPlay*		_pCharactorPlay;
	PkwCharactor*			_pChoiceCommander;
	int						_itemMaxPage;
	int						_itemCurPage;
	int						_nChoice;
public:
	image*					_backBuffer;
	HDC						_MemDC;
	RECT					_gameView;
	int						_nWindowWidth;
	int						_nWindowHeight;
	float					_elapsed;

	HFONT					_font[10];
	imageFrameData			_mouseImageFrame;
public:
	PkwBsShopScene();
	~PkwBsShopScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);

	//update관련 함수
	void shopNormalUpdate();
	void shopEquipAtkItemUpdate();
	void shopEquipDefenceItemUpdate();
	void shopBuyUpdate();
	void shopSellUpdate();

	//render관련 함수
	void InterfaceRender();
	void InfoRender();
	void shopNormalRender();
	void shopEquipAtkItemRender();
	void shopEquipDefenceItemRender();
	void shopBuyRender();
	void shopSellRender();

	//기타 함수
	NumDigit GetNumDigit(int num);

	void itemSetUp(int num, bool isAtkItem);

	void commanderListRender(int value, int type);

	void mouseImageFrameUpdate();
};

