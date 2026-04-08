#pragma once

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwCharactorPlay.h"
#include "PkwBsNormalScene.h"
#include "PkwBsShopScene.h"

class PkwBattleSetScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;

	gameNode* _normalScene;
	gameNode* _shopScene;
	gameNode* _currentScene;
public:
	PkwBattleSetScene();
	~PkwBattleSetScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public:
	static void normalToShop(void* obj);
	static void ShopToNormal(void* obj);
	static void battleSetSceneEnd(void* obj);
};

