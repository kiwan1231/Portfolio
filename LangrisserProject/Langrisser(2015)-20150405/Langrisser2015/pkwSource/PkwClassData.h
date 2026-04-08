#pragma once
#include"../common.h"

class PkwClassData
{
public:
	CLASS_TYPE				_myClass;
	CLASS_TYPE				_nextClass1;
	CLASS_TYPE				_nextClass2;
public:
	char					_name[128];
	int						_commanderRange;
	int						_upAtk;
	int						_upDef;
	int						_upMp;
	int						_move;
	int						_upCommanderAtk;
	int						_upCommanderDef;
	vector<SkillData>		_vSkill;
	vector<MercenaryData>	_vMercenary;
	vector<SkillData>		_vSummon;
public:
	PkwClassData();
	~PkwClassData();
};

