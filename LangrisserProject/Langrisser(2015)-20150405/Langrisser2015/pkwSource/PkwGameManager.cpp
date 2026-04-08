#include "../stdafx.h"
#include "PkwGameManager.h"

PkwGameManager::PkwGameManager()
{
}
PkwGameManager::~PkwGameManager()
{
}
//초기화는 무조건 여기서 해준다
HRESULT PkwGameManager::init(void)
{
	gameNode::init(true);
	
	ShowCursor(FALSE);

	_curSceneMode = LOADING_SCENE;
	
	_loadingScene = NULL;
	_openingScene	= NULL;
	_scenarioScene	= NULL;
	_battleSetScene = NULL;
	_battleScene	= NULL;
	_currentScene = NULL; 
	
	_loadingScene = new PkwLoadingScene;
	_loadingScene->init(loadingToOpening, this);
	_currentScene = _loadingScene;
	
	/*
	//메모리 누수(Leak)을 위한 플래그 선언
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);

	//찾으면 알려준다
	_crtBreakAlloc;
	_CrtSetBreakAlloc(1186);
	//덤프 메모리 새는걸 감지해줌
	_CrtDumpMemoryLeaks();
	*/
	
	return S_OK;
}
//동적할당등의 메모리 해제는 여기서
void PkwGameManager::release(void)
{
	if (_loadingScene != NULL){
		_loadingScene->release();
		SAFE_DELETE(_loadingScene);
	}
	if (_openingScene != NULL){
		_openingScene->release();
		SAFE_DELETE(_openingScene);
	}
	if (_scenarioScene != NULL){
		_scenarioScene->release();
		SAFE_DELETE(_scenarioScene);
	}
	if (_battleSetScene != NULL){
		_battleSetScene->release();
		SAFE_DELETE(_battleSetScene);
	}
	if (_battleScene != NULL){
		_battleScene->release();
		SAFE_DELETE(_battleScene);
	}

	gameNode::release();
}
//모든 움직임을 여기서 담당한다
void PkwGameManager::update(void)
{
	gameNode::update();
	
	_currentScene->update();

	if (KEYMANAGER->isOnceKeyDown(VK_ESCAPE))
	{
		PostQuitMessage(WM_DESTROY);
	}
}

//여기서는 무조건 그려준다
void PkwGameManager::render(void)
{
	_currentScene->render();
}

//씬 콜백 함수들
void PkwGameManager::loadingToOpening(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_loadingScene->release();
	SAFE_DELETE(gm->_loadingScene);
	
	gm->_openingScene = new PkwOpeningScene;
	gm->_openingScene->init(openingToScenario, obj);
	gm->_currentScene = gm->_openingScene;

	gm->_curSceneMode = OPENING_SCENE;
}
void PkwGameManager::openingToScenario(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_openingScene->release();
	SAFE_DELETE(gm->_openingScene);

	gm->_scenarioScene = new PkwScenarioScene;
	gm->_scenarioScene->init(scenarioToBattleSet, obj);
	gm->_currentScene = gm->_scenarioScene;

	gm->_curSceneMode = SCENARIO_SCENE;

	/*gm->_battleSetScene = new PkwBattleSetScene;
	gm->_battleSetScene->init(battleSetToBattle, obj);
	gm->_currentScene = gm->_battleSetScene;

	gm->_curSceneMode = BATTLE_SET_SCENE;*/
}
void PkwGameManager::scenarioToBattleSet(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_scenarioScene->release();
	SAFE_DELETE(gm->_scenarioScene);

	gm->_battleSetScene = new PkwBattleSetScene;
	gm->_battleSetScene->init(battleSetToBattle, obj);
	gm->_currentScene = gm->_battleSetScene;

	gm->_curSceneMode = BATTLE_SET_SCENE;
}
void PkwGameManager::battleSetToBattle(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_battleSetScene->release();
	SAFE_DELETE(gm->_battleSetScene);

	gm->_battleScene = new PkwBattleScene;
	gm->_battleScene->init(battleToScenario, battleSetOpening, obj);
	gm->_currentScene = gm->_battleScene;
	gm->_curSceneMode = BATTLE_SCENE;
}
void PkwGameManager::battleToScenario(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_battleScene->release();
	SAFE_DELETE(gm->_battleScene);

	gm->_scenarioScene = new PkwScenarioScene;
	gm->_scenarioScene->init(scenarioToBattleSet, obj);
	gm->_currentScene = gm->_scenarioScene;

	gm->_curSceneMode = SCENARIO_SCENE;
}
void PkwGameManager::battleSetOpening(void* obj)
{
	PkwGameManager* gm = (PkwGameManager*)obj;

	gm->_battleScene->release();
	SAFE_DELETE(gm->_battleScene);

	gm->_openingScene = new PkwOpeningScene;
	gm->_openingScene->init(openingToScenario, obj);
	gm->_currentScene = gm->_openingScene;

	gm->_curSceneMode = OPENING_SCENE;
}