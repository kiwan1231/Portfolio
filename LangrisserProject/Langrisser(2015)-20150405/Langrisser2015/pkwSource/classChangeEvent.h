#pragma once

#include "battleEvent.h"

enum CLASSCHANGE_MODE{
	CHOICE_CLASS,
	VIEW_CLASS1,
	VIEW_CLASS2,
	CLASS1_YESNO,
	CLASS2_YESNO
};
class classChangeEvent : public battleEvent
{
private:
	PkwCharactorPlay*		_pCharactorPlay;
	PkwCharactor*			_classChangeCommander;
	CLASSCHANGE_MODE		_mode;
	bool					_isClassTwo;

	PkwClassData			_nextClass1;
	PkwClassData			_nextClass2;
	int						_nChoice;
public:
	virtual void init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight);
	virtual void release(void);
	virtual void update(float elapsedTime);
	virtual void render(HDC memDC);
	virtual void checkEvent();

	void classChange();
	NumDigit GetNumDigit(int num);
public:
	classChangeEvent();
	~classChangeEvent();
};

