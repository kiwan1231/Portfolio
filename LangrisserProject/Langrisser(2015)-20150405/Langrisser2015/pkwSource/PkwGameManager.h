#pragma once
#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwCharactorPlay.h"
#include "PkwMap.h"
#include "PkwDataBase.h"
#include "Pkw2DUserInterface.h"
#include "PkwActionProcess.h"
#include "PkwEffectPlay.h"

#include "PkwLoadingScene.h"
#include "PkwOpeningScene.h"
#include "PkwScenarioScene.h"
#include "PkwBattleSetScene.h"
#include "PkwBattleScene.h"

class PkwGameManager : public gameNode
{
public:
	PkwGameManager();
	~PkwGameManager();

	virtual HRESULT init(void);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public:
	SCENE_MODE _curSceneMode;

	gameNode* _loadingScene;
	gameNode* _openingScene;
	gameNode* _scenarioScene;
	gameNode* _battleSetScene;
	gameNode* _battleScene;
	gameNode* _currentScene;
public:
	static void loadingToOpening(void* obj);
	static void openingToScenario(void* obj);
	static void scenarioToBattleSet(void* obj);
	static void battleSetToBattle(void* obj);
	static void battleToScenario(void* obj);
	static void battleSetOpening(void* obj);
};

