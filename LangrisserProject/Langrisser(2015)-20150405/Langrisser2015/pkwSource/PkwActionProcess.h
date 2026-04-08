#pragma once
#include"../common.h"
#include "aStarAlgorism.h"
enum ALPA_ANI{
	ALPA_UP,
	ALPA_DOWN,
};

class MapData;

class PkwCharactor;

class PkwCharactorPlay;

class PkwActionProcess
{
public://게임 매니져에게 받아오는 변수
	PkwCharactorPlay*	_pCharactorPlay;
	PkwCharactor*		_charactor;
	MapData*			_mapData;
	CHARACTOR_CONTROL	_control;
public://범위검색에 쓰는 변수
	int _move;
	int _commandRange;
	int _atkRange;
	int _migRange;
	int _migScale;
	int _summonRange;
public://범위 저장 벡터
	vector<POINT>	_vCommandPos;//지휘 범위 저장
	vector<POINT>	_vMoveRange;//이동 범위 저장
	vector<POINT>	_vMovePos;	//시작지점부터 목표 지점까지 위치 저장
	vector<POINT>	_vAtkRangePos;// 공격가능 지점 저장
	vector<POINT>	_vMigRangePos;//마법 공격 지점 저장
	vector<POINT>	_vMigScalePos;//마법의 범위 저장
	vector<POINT>	_vSummonRangePos;// 소환 가능 위치 지정
public://길찾기 알고리즘 변수들
	aStarAlgorism	_aStar;
	int				_curPosNum;
	int				_maxPosNum;
public://기타
	ALPA_ANI	_alpaAni;
	float		_alFrame;
public:
	void Init(PkwCharactorPlay*	pCharactorPlay, MapData* mapData);
	void release();
	void render(HDC MemDC, CHARACTOR_CONTROL control, RECT gameView);

	//선택된 유닛의 지휘범위와 이동범위를 알아낸다
	void SearchPos(PkwCharactor* charactor);
	//선택된 유닛의 지휘범위를 저장한다
	void SearchCommandPos(PkwCharactor* charactor,POINT pos, int range);

	//선택된 유닛이 이동범위를 지점을 저장한다
	void SearchMovePos(PkwCharactor* charactor, POINT pos, int move);
	//선택된 유닛이 유저가 지정한 지점 까지 이동하는 경로의 위치를 저장한다
	void movingPosSave(PkwCharactor* choiceUnit, POINT startPos, POINT destPos);

	void SearchAtkPos(PkwCharactor* charactor,POINT pos, int range);
	void atkPosSave(POINT pos);

	void magicRangePosSave(POINT pos, int range);
	void magicScalePosSave(PkwCharactor* targetUnit);
	void magicScalePosSave(POINT pos, int range);

	void summonPosSave(PkwCharactor* choiceUnit);

	int MapMoveTable(POINT pos);
public:
	PkwActionProcess();
	~PkwActionProcess();
};

