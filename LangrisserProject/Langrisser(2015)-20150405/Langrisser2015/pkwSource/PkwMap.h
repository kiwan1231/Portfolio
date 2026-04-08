#pragma once
#include"../common.h"

#define MAP_NUM						1

class MapData
{
public:
	int *map;
	int width;
	int height;
	char imageKey[128];
};

class PkwMap
{
public:
	PkwMap();
	~PkwMap();
public:
	MapData _map[MAP_NUM];
	int		_curMapStage;
public:
	void Init();
	void release();
	void CreateMap();
	void CreateMap_0();
	void ReleaseMap();
	MapData* GetMap();
	MapData* GetMap(int mapNumber);
};

