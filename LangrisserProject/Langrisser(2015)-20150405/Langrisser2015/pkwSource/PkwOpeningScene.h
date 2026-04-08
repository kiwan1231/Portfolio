#pragma once
#include "../2DGameFramework/gameNode.h"
#include"../common.h"

enum TITLE_CHOICE{
	TITLE_CHOICE_OFF,
	TITLE_START,
	TITLE_LOAD,
	TITLE_EXIT
};

class PkwOpeningScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;
public:
	PkwOpeningScene();
	~PkwOpeningScene();
	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public:
	image*				_backBuffer;
	HDC					_MemDC;
	float				_elapsed;
	float				_choiceDelay;
	float				_mouseDelay;
public:
	TITLE_CHOICE		_titleChoice;
	int					_nWindowWidth;
	int					_nWindowHeight;
	RECT				_choiceRc[3];
public:
	void				frameProcess();
	void				checkChoice();
	void				renderChoice();
};

