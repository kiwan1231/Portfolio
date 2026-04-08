#pragma once
#include "../2DGameFramework/gameNode.h"
#include"../common.h"

#include "PkwBattleMapScene.h"
#include "PkwBattleShowScene.h"

class PkwBattleScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter1;//scenario씬으로 넘어갈때
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter2;//opening씬 종료할때

	gameNode* _battleMapScene;
	gameNode* _battleShowScene;
	gameNode* _currentScene;
public:
	PkwBattleScene();
	~PkwBattleScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public:
	static void battleMapToShow(void* obj);
	static void battleShowToMap(void* obj);
	static void battleToScenario(void* obj);
	static void battleToOpening(void* obj);
};

