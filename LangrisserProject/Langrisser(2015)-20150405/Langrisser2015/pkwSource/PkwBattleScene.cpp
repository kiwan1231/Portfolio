#include "../stdafx.h"
#include "PkwBattleScene.h"

PkwBattleScene::PkwBattleScene()
{
}


PkwBattleScene::~PkwBattleScene()
{
}

HRESULT PkwBattleScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter1 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction1);
	_callbackFunctionParameter2 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction2);

	
	_battleMapScene = new PkwBattleMapScene();
	_battleMapScene->init(battleMapToShow, battleToScenario, battleToOpening, this);
	
	_battleShowScene = NULL;

	_currentScene = _battleMapScene;

	return S_OK;
}
void PkwBattleScene::release(void)
{
	if (_battleMapScene != NULL){
		_battleMapScene->release();
		SAFE_DELETE(_battleMapScene);
	}

	if (_battleShowScene != NULL){
		_battleShowScene->release();
		SAFE_DELETE(_battleShowScene);
	}
}
void PkwBattleScene::update(void)
{
	_currentScene->update();
}
void PkwBattleScene::render(void)
{
	_currentScene->render();
}

void PkwBattleScene::battleMapToShow(void* obj)
{
	PkwBattleScene* battleScene = (PkwBattleScene*)obj;

	battleScene->_battleShowScene = new PkwBattleShowScene();

	battleScene->_battleShowScene->init(battleShowToMap, obj);

	battleScene->_currentScene = battleScene->_battleShowScene;
}

void PkwBattleScene::battleShowToMap(void* obj)
{
	PkwBattleScene* battleScene = (PkwBattleScene*)obj;

	battleScene->_battleShowScene->release();

	SAFE_DELETE(battleScene->_battleShowScene);

	battleScene->_currentScene = battleScene->_battleMapScene;
}

void PkwBattleScene::battleToScenario(void* obj)
{
	PkwBattleScene* battleScene = (PkwBattleScene*)obj;
	battleScene->_callbackFunctionParameter1(battleScene->_obj);
}

void PkwBattleScene::battleToOpening(void* obj)
{
	PkwBattleScene* battleScene = (PkwBattleScene*)obj;
	battleScene->_callbackFunctionParameter2(battleScene->_obj);
}
