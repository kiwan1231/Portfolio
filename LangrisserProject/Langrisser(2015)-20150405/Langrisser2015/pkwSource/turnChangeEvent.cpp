#include "../stdafx.h"
#include "turnChangeEvent.h"

turnChangeEvent::turnChangeEvent()
{
}
turnChangeEvent::~turnChangeEvent()
{
}

void turnChangeEvent::init(BATTLE_EVENT_TYPE eventType, int turn, BATTLE_STATUS status, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_turn = turn;
	_status = status;
	
	_eventStatus = EVENT_ACTIVE;

}
void turnChangeEvent::release(void)
{
	battleEvent::release();
}
void turnChangeEvent::update(float elapsedTime)
{
	_delayTime += elapsedTime;
	if (_delayTime > 1.0){
		_eventStatus = EVENT_END;
	}
}
void turnChangeEvent::render(HDC memDC)
{
	char str[128];

	SelectObject(memDC, _font[0]);
	SetTextColor(memDC, RGB(255, 255, 255));

	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 208, _pMapStartPos->y + 144, 0, 0, 240, 64, 150.0);
	IMAGEMANAGER->findImage("UI(240X64)")->render(memDC, _pMapStartPos->x + 208, _pMapStartPos->y + 144);
	
	if (_status == CO_TURN)
		sprintf_s(str, " 아군 %d턴", _turn);
	else if (_status == SUPPORT_TURN)
		sprintf_s(str, "지원군 %d턴", _turn);
	else if (_status == ENEMY_TURN)
		sprintf_s(str, " 적군 %d턴", _turn);
	TextOut(memDC, _pMapStartPos->x + 208 + 32, _pMapStartPos->y + 160 + 2, str, strlen(str));
}
