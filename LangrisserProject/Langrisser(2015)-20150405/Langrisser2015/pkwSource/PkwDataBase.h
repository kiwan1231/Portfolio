#pragma once
#include"../common.h"
#include "PkwClassData.h"
class PkwDataBase
{
public:
	PkwDataBase();
	~PkwDataBase();
public:
	vector<CommanderData>	_vCommandData;
	vector<ItemData>		_vItemData;
	vector<SkillData>		_vSkillData;
	vector<MercenaryData>	_vMercenaryData;
	vector<SummonData>		_vSummonData;
	vector<PkwClassData>	_vClassData;
public:
	void DataInit();
	void DataRelease();

	bool LoadTxtFile(char* fileName);

	void CommanderDataAdd(char* commanderData);
	void CommanderValueAdd(CommanderData* commander, char* commanderValue, int cnt);
	void ClassValueAdd(CommanderData* commander, char* commanderValue);

	void ItempDataAdd(char* itemData);
	void ItemValueAdd(ItemData* item, char* itemValue, int cnt);

	void SkillDataAdd(char* skillValue);
	void SkillValueAdd(SkillData* skill, char* skillValue, int cnt);

	void MercenaryDataAdd(char* mercenaryValue);
	void MercenaryValueAdd(MercenaryData* mercenary, char* mercenaryValue, int cnt);

	void SummonDataAdd(char* summonValue);
	void SummonValueAdd(SummonData* summon, char* summonValue, int cnt);

	void ClassDataAdd(char* chClass);
	void ClassValue(PkwClassData* classData, char* classValue, int cnt);
	CLASS_TYPE ClassValueAdd(char* commanderValue);

	MercenaryData getMercenary(char* name);
};
