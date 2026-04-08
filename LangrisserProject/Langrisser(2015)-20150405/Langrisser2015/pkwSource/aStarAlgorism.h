#pragma once
#include <vector>
#include <string>

class tile{
public:
	POINT _pos;

	int _totlalCost;    //f : g + h 의 값
	int _chstFromStart;	//g : 시작점부터 현재 노드까지의 경로 비용
	int _costToGoal;	//h : 현재 노드부터 끝점까지의 경로 비용

	bool _isOpen;		//열수 있는지 여부?

	tile* _parentNode;

	string _attribute;

	void init(POINT pos);

	tile();
	~tile();
};

class aStarAlgorism
{
private:
	MapData*		_pMapData;
	PkwCharactor*	_choiceUnit;
	int				_unitType;

	vector<tile*> _vTotalList;
	vector<tile*>::iterator _viTotalList;

	vector<tile*> _vOpenList;	 //이동 해야 할 타일을 담아두는 곳
	vector<tile*>::iterator _viOpenList;

	vector<tile*> _vCloseList;	//이동한 타일을 담아두는 곳
	vector<tile*>::iterator _viCloseList;

	POINT _startTilePos;
	POINT _endTilePos;

	tile* _currentTile;

	int		_tileNumX;
	int		_tileNumY;
public:
	vector<POINT>	_vMovePos;	//시작지점부터 목표 지점까지 위치 저장
public:
	void init();
	void release();
	//맵 관련 타일을 셋팅해 둔다
	void setTiles();

	//선택된 유닛의 길을 찾기위한 변수를 저장한다
	void setUnitStartEndPos(PkwCharactor* choiceUnit, POINT startTilePos, POINT endTilePos);
	//길찾기
	void pathFinder(tile* currentTile);

	//넓이 우선 탐색을 활용해 가야할 타일을 찾아서 벡터로 반환해 준다
	vector<tile*> addOpenList(tile* currentTile);
public:
	aStarAlgorism();
	~aStarAlgorism();
};

