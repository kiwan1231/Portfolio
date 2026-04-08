#include "../stdafx.h"
#include "battleEvent.h"


battleEvent::battleEvent()
{
}
battleEvent::~battleEvent()
{
}

void battleEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight)
{
	_eventType = eventType = eventType;
	_pMapStartPos = pMapStartPos;
	_MapMaxWidth = MapMaxWidth;
	_MapManHeight = MapManHeight;

	_delayTime = 0.0;

	// ÆùÆ® set
	_font[0] = CreateFont(28, 0, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "±¼¸²");
	_font[1] = CreateFont(22, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "±¼¸²");
	_font[2] = CreateFont(16, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "±¼¸²");
}
void battleEvent::release(void)
{
	for (int i = 0; i < 10; i++){
		if (_font[i] != NULL)
			DeleteObject(_font[i]);
	}
}
void battleEvent::update(float elapsedTime)
{

}
void battleEvent::render(HDC memDC)
{

}
void battleEvent::checkEvent()
{

}