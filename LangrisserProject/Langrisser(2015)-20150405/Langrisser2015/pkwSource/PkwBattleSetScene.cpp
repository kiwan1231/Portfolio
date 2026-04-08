#include "../stdafx.h"
#include "PkwBattleSetScene.h"


PkwBattleSetScene::PkwBattleSetScene(){}

PkwBattleSetScene::~PkwBattleSetScene(){}

HRESULT PkwBattleSetScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	_normalScene = NULL;
	_normalScene = new PkwBsNormalScene;
	_normalScene->init(normalToShop, battleSetSceneEnd,this);

	_shopScene = NULL;
	_shopScene = new PkwBsShopScene;
	_shopScene->init(ShopToNormal,this);

	_currentScene = _normalScene;
	
	SOUNDMANAGER->stop(SOUNDMANAGER->_curBGSound);

	memcpy(SOUNDMANAGER->_curBGSound, "3_battleSet", strlen("3_battleSet"));
	SOUNDMANAGER->play(SOUNDMANAGER->_curBGSound, 1.0);

	return S_OK;
}
void PkwBattleSetScene::release(void)
{
	if (_normalScene != NULL){
		_normalScene->release();
		SAFE_DELETE(_normalScene);
	}
	if (_shopScene != NULL){
		_shopScene->release();
		SAFE_DELETE(_shopScene);
	}
}
void PkwBattleSetScene::update(void)
{
	_currentScene->update();
}
void PkwBattleSetScene::render(void)
{
	_currentScene->render();
}

void PkwBattleSetScene::normalToShop(void* obj)
{
	PkwBattleSetScene* bsScene = (PkwBattleSetScene*)obj;
	PkwBsShopScene* pShopScene = (PkwBsShopScene*)bsScene->_shopScene;
	PkwBsNormalScene* pNormalScene = (PkwBsNormalScene*)bsScene->_normalScene;

	pShopScene->_pChoiceCommander = pNormalScene->_pChoiceCommander;

	bsScene->_currentScene = bsScene->_shopScene;
}

void PkwBattleSetScene::ShopToNormal(void* obj)
{
	PkwBattleSetScene* bsScene = (PkwBattleSetScene*)obj;
	bsScene->_currentScene = bsScene->_normalScene;
}

void PkwBattleSetScene::battleSetSceneEnd(void* obj)
{
	//여기서는 gm에게 battleSetScene 를 종료를 알린다
	PkwBattleSetScene* bsScene = (PkwBattleSetScene*)obj;
	bsScene->_callbackFunctionParameter(bsScene->_obj);
}
