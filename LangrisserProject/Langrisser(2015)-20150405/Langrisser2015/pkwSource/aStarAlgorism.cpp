#include "../stdafx.h"
#include "aStarAlgorism.h"
//--------------------------------------------타일 소스
tile::tile()
{
}
tile::~tile()
{
}
void tile::init(POINT pos)
{
	_pos = pos;

	_totlalCost = 0;
	_chstFromStart = 0;
	_costToGoal = 0;

	 _isOpen = true;

	_parentNode = NULL;
	_attribute.clear();
}

//--------------------------------------------a 스타 알고리즘 소스
aStarAlgorism::aStarAlgorism()
{
}
aStarAlgorism::~aStarAlgorism()
{
}

void aStarAlgorism::init()
{
	_pMapData = DATAMANAGER->_MapData.GetMap();

	_vTotalList.clear();
	_vOpenList.clear();	 
	_vCloseList.clear();

	setTiles();
}
void aStarAlgorism::release()
{
	_vOpenList.clear();
	_vCloseList.clear();

	for (int i = 0; i < _vTotalList.size(); i++){
		SAFE_DELETE(_vTotalList[i]);
	}
	_vTotalList.clear();
}
void aStarAlgorism::setTiles()
{
	POINT pos;

	_tileNumX = _pMapData->width;
	_tileNumY = _pMapData->height;

	//타일 초기화
	for (int i = 0; i < _tileNumY; i++)
	{
		for (int j = 0; j < _tileNumX; j++)
		{
			tile* node = new tile;
			pos.x = j;		pos.y = i;
			node->init(pos);

			if (_pMapData->map[j + (i*_tileNumX) ] == 0)
				node->_attribute = "벽";
			else if (_pMapData->map[j + (i*_tileNumX)] == 1)
				node->_attribute = "수병만감";
			else if (_pMapData->map[j + (i*_tileNumX)] == 2)
				node->_attribute = "보병기병만감";
			else if (_pMapData->map[j + (i*_tileNumX)] == 3)
				node->_attribute = "비병만감";
			else{
				node->_attribute = "다감";
			}

			_vTotalList.push_back(node);
		}
	}
	return;
}
void aStarAlgorism::setUnitStartEndPos(PkwCharactor* choiceUnit, POINT startTilePos, POINT endTilePos)
{
	//_unitType = charactorType;
	_choiceUnit = choiceUnit;
	_startTilePos = startTilePos;
	_endTilePos = endTilePos;

	for (int i = 0; i < _vTotalList.size(); i++){
		_vTotalList[i]->_chstFromStart = 0;
		_vTotalList[i]->_costToGoal = 0;
		_vTotalList[i]->_totlalCost = 0;
		_vTotalList[i]->_isOpen = true;
		_vTotalList[i]->_parentNode = NULL;
		if (_vTotalList[i]->_pos.x == _startTilePos.x && _vTotalList[i]->_pos.y == _startTilePos.y)
			_currentTile = _vTotalList[i];
	}
	_vOpenList.clear();
	_vCloseList.clear();

	pathFinder(_currentTile);
}
void aStarAlgorism::pathFinder(tile* currentTile)
{
	int tempTotalCost = 1000;
	tile* tempTile = NULL;

	for (int i = 0; i < addOpenList(currentTile).size(); i++)
	{
		//현재 노드부터 끝점까지의 경로 비용
		_vOpenList[i]->_costToGoal = abs(_endTilePos.x - _vOpenList[i]->_pos.x) + abs(_endTilePos.y - _vOpenList[i]->_pos.y);
	
		//시작점부터 현재 노드까지의 경로 비용
		_vOpenList[i]->_chstFromStart = abs(_startTilePos.x - _vOpenList[i]->_pos.x) + abs(_startTilePos.y - _vOpenList[i]->_pos.y);
		
		//총 비용 계산
		_vOpenList[i]->_totlalCost = _vOpenList[i]->_chstFromStart + _vOpenList[i]->_costToGoal;
	
		//가장 비용이 저렴한 타일 뽑는다
		if (tempTotalCost > _vOpenList[i]->_totlalCost)
		{
			tempTotalCost = _vOpenList[i]->_totlalCost;
			tempTile = _vOpenList[i];
		}

		//갈수 있는 길은 모두 오픈리스트에 넣어둔다
		bool addObj = true;
		for (_viOpenList = _vOpenList.begin(); _viOpenList != _vOpenList.end(); ++_viOpenList)
		{
			if (*_viOpenList == tempTile)
			{
				addObj = false;
				break;
			}
		}

		_vOpenList[i]->_isOpen = false;
		if (!addObj) continue;
		_vOpenList.push_back(tempTile);
	}

	//목적지에 다 왔으면 리떤~~~
	if (tempTile == NULL){
		return;
	}
	else if (tempTile->_pos.x == _endTilePos.x && tempTile->_pos.y == _endTilePos.y)
	{
		_vMovePos.clear();
		while (tempTile->_parentNode != NULL)
		{
			_vMovePos.push_back(tempTile->_pos);
			tempTile = tempTile->_parentNode;
		}
		return;
	}
	

	//현재 타일(지나온 곳)을 클로즈 리스트에 넣어준다
	_vCloseList.push_back(tempTile);

	//다음 노드로 설정된 타일을 오픈리스트에서 제거한다
	for (_viOpenList = _vOpenList.begin(); _viOpenList != _vOpenList.end(); ++_viOpenList)
	{
		if (*_viOpenList == tempTile)
		{
			_viOpenList = _vOpenList.erase(_viOpenList);
			break;
		}
	}

	//현재타일에 템프타일 넣는다 (제일 가까운 길로 선정된 타일임...)
	_currentTile = tempTile;

	//다음 노드 찾는다
	pathFinder(_currentTile);
}
vector<tile*> aStarAlgorism::addOpenList(tile* currentTile)
{
	POINT searchPos[4];
	int unitType, armyType;
	PkwCharactor* tempUnit;

	unitType = _choiceUnit->_type;
	armyType = _choiceUnit->_armyType;

	//4방향 검사
	searchPos[0].x = currentTile->_pos.x - 1;
	searchPos[0].y = currentTile->_pos.y;

	searchPos[1].x = currentTile->_pos.x + 1;
	searchPos[1].y = currentTile->_pos.y;

	searchPos[2].x = currentTile->_pos.x;
	searchPos[2].y = currentTile->_pos.y - 1;

	searchPos[3].x = currentTile->_pos.x;
	searchPos[3].y = currentTile->_pos.y + 1;

	//현재 노드에서 주변을 확장하며 검사....
	for (int i = 0; i < 4; i++)
	{
		//맵을 넘어가면 넣어주지 않는다
		if (searchPos[i].x < 0 || searchPos[i].x >= _tileNumX) continue;
		if (searchPos[i].y < 0 || searchPos[i].y >= _tileNumY) continue;

		//타일 속성을 검사해서 못가는 지역이면 넣어주지 않는다
		tile* node = _vTotalList[searchPos[i].x + (searchPos[i].y * _tileNumX) ];
		if (!node->_isOpen) continue;
		if (node->_pos.x == _startTilePos.x && node->_pos.y == _startTilePos.y) continue;
		if (node->_attribute == "벽") continue;
		if ((unitType != 0 && unitType != 1) && node->_attribute == "보병기병만감") continue;
		if (unitType != 2 && node->_attribute == "수병만감") continue;
		if (unitType != 3 && node->_attribute == "비병만감") continue;

		node->_parentNode = _currentTile;

		bool addObj = true;

		//적군은 지나가지 못한다
		if (armyType == 0 || armyType == 1){
			for (int j = 0; j < DATAMANAGER->_CharactorPlay._vEnemyCommander.size(); j++){
				//적 지휘관일떄
				tempUnit = &DATAMANAGER->_CharactorPlay._vEnemyCommander[j];
				if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
					addObj = false;
				//적 병사 일때
				for (int k = 0; k < DATAMANAGER->_CharactorPlay._vEnemyCommander[j]._vArms.size(); k++){
					tempUnit = &DATAMANAGER->_CharactorPlay._vEnemyCommander[j]._vArms[k];
					if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
						addObj = false;
				}
				//지나가지 못하면 false로 만들고 더이상 알아볼 필요없으므로 for문을 나간다
				if (!addObj) break;
			}
		}
		else if (armyType == 2){
			for (int j = 0; j < DATAMANAGER->_CharactorPlay._vCoCommander.size(); j++){

				tempUnit = &DATAMANAGER->_CharactorPlay._vCoCommander[j];
				if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
					addObj = false;

				for (int k = 0; k < DATAMANAGER->_CharactorPlay._vCoCommander[j]._vArms.size(); k++){
					tempUnit = &DATAMANAGER->_CharactorPlay._vCoCommander[j]._vArms[k];
					if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
						addObj = false;
				}

				if (!addObj) break;
			}
			for (int j = 0; j < DATAMANAGER->_CharactorPlay._vSupportCommander.size(); j++){

				tempUnit = &DATAMANAGER->_CharactorPlay._vSupportCommander[j];
				if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
					addObj = false;

				for (int k = 0; k < DATAMANAGER->_CharactorPlay._vSupportCommander[j]._vArms.size(); k++){
					tempUnit = &DATAMANAGER->_CharactorPlay._vSupportCommander[j]._vArms[k];
					if (tempUnit->_pos.x == node->_pos.x && tempUnit->_pos.y == node->_pos.y)
						addObj = false;
				}

				if (!addObj) break;
			}
		}

		//이미 들어가 있는 노드 이면 넣어주지 않는다
		for (_viOpenList = _vOpenList.begin(); _viOpenList != _vOpenList.end(); ++_viOpenList)
		{
			if (*_viOpenList == node)
			{
				addObj = false;
				break;
			}
		}


		if (!addObj) continue;
		
		_vOpenList.push_back(node);
	}

	return _vOpenList;
}
