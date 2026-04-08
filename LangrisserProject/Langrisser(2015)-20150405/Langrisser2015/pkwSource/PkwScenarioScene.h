#pragma once

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwCharactorPlay.h"

enum SCENARIO_MODE{
	SCENARIO_MOVE,
	SCENARIO_COMMANDER_SHOW,
	SCENARIO_SHOW,
	SCENARIO_EXIT
};

enum PAGE_MODE{
	START_PAGE,
	NEXT_PAGE,
	FINAL_PAGE
};

struct scenarioData{
	char data[128];
};
class PkwScenarioScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;
public:
	PkwScenarioScene();
	~PkwScenarioScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public://공용 변수
	image*				_backBuffer;
	HDC					_MemDC;
	float				_elapsed;
	int					_nWindowWidth;
	int					_nWindowHeight;

	int					_backImageWidth;
	int					_backImageHeight;
	POINT				_imageSour;
	SCENARIO_MODE		_scenarioMode;
	float				_modeDelay;
	int					_scenarioNum;
	PkwCharactorPlay	_CharactorPlay;
public://SCENARIO_MOVE
	POINT				_startPos;
	POINT				_destPos;
	float				_curPosX;
	float				_curPosY;
	float				_speed;
public://SCENARIO_SHOW 변수
	PAGE_MODE			_pageMode;
	vector<scenarioData>	_vScenarioTXT;
	char				_txtFilePath[128];
	ifstream			_txtFile;
public:
	void BackImageRender();
	void loadScenarioTXT();
	void scenarioTXTRender();
	void MapMove();
};

