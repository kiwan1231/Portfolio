#include "../stdafx.h"
#include "PkwDataManager.h"

PkwDataManager::PkwDataManager()
{
}


PkwDataManager::~PkwDataManager()
{
}

HRESULT PkwDataManager::init(void)
{
	_stageNumber = STAGE_01;
	_stageNum = 0;
	_money = 128920;

	_mainDc = GetDC(_hWnd);
	_MapData.Init();
	_MapData.CreateMap();
	_DataBase.DataInit();
	
	/*for (int i = 0; i < _DataBase._vCommandData.size(); i++){
		_CharactorPlay.CreateCharactor(&_DataBase._vCommandData[i]);
	}*/

	for (int i = 0; i < _DataBase._vCommandData.size(); i++){
		PkwCharactor temp;
		temp.CharactorInit(&_DataBase._vCommandData[i]);
		_vCoCommander.push_back(temp);
	}

	_vPlayerItem.clear();
	_vCommanderPos.clear();

	setStageFileRoute();
	_vShopData.clear();

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;
	_gameView.left = 0; _gameView.right = _nWindowWidth;
	_gameView.top = 0; _gameView.bottom = _nWindowHeight;

	return S_OK;
}
void PkwDataManager::release(void)
{
	_MapData.ReleaseMap();
	_DataBase.DataRelease();
	_CharactorPlay.CommanderRelease();
	_vPlayerItem.clear();
	_vCommanderPos.clear();
	_vShopData.clear();
}

void PkwDataManager::setStage()
{
	ifstream file;
	char temp[512];
	int type;
	_stageNumber = STAGE_01;
	_stageNum = STAGE_01;

	_CharactorPlay.CommanderRelease();
	_vShopData.clear();
	_vCommanderPos.clear();

	_CharactorPlay._vCoCommander = _vCoCommander;

	file.open(_stageDataFile[_stageNumber].stageFileRoute);

	while (!file.eof()){
		file.getline(temp, 512);
		if (strcmp(temp, "#commanderData") == 0){
			type = 0;
		}
		else if (strcmp(temp, "#mercenaryData") == 0){
			type = 1;
		}
		else if (strcmp(temp, "#commanderPos") == 0){
			type = 2;
		}
		else if (strcmp(temp, "#shopItem") == 0){
			type = 3;
		}
		else if (temp[0] == '#') continue;
		else{
			if (type == 0)	setStageCommand(temp);
			else if (type == 1) setStageMercenary(temp);
			else if (type == 2) setStageCommanderPos(temp);
			else if (type == 3) setStageShopItem(temp);
		}
	}

	file.close();

	return;
}
void PkwDataManager::setStageCommand(char* chCommand)
{
	CommanderData temp;
	
	char value[512];
	bool valueCheck = false;
	int length = strlen(chCommand);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (chCommand[i] == ' '){
			if (!valueCheck)	continue;

			//지휘관 속성 추가하기
			setStageCommanderValueAdd(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = chCommand[i];
			j++;
			valueCheck = true;
		}
	}
	_CharactorPlay.CreateCharactor(&temp);
}
void PkwDataManager::setStageCommanderValueAdd(CommanderData* commander, char* commanderValue, int cnt)
{
	int length = strlen(commanderValue);
	int value;
	char *temp;

	value = atoi(commanderValue);
	if (cnt == 0){
		commander->_number = value;
	}
	else if (cnt == 1){
		memset(commander->_imageKey, 0, 128);
		memcpy(commander->_imageKey, commanderValue, length);
		commander->_imageNum = 0;
	}
	else if (cnt == 2){
		memset(commander->_faceImageKey, 0, 128);
		memcpy(commander->_faceImageKey, commanderValue, length);
	}
	else if (cnt == 3){
		memset(commander->_talkImageKey, 0, 128);
		memcpy(commander->_talkImageKey, commanderValue, length);
	}
	else if (cnt == 4){
		commander->_pos.x = value;
	}
	else if (cnt == 5){
		commander->_pos.y = value;
	}
	else if (cnt == 6){
		commander->_armyType = value;
	}
	else if (cnt == 7){
		memset(commander->_name, 0, 128);
		memcpy(commander->_name, commanderValue, length);
	}
	else if (cnt == 8){
		memset(commander->_chClass, 0, 128);
		memcpy(commander->_chClass, commanderValue, length);
		_DataBase.ClassValueAdd(commander, commanderValue);
	}
	else if (cnt == 9){
		commander->_type = value;
	}
	else if (cnt == 10){
		if (value == 0)			commander->_mercenartOrder = MERCENARY_BATTLE;
		else if (value == 1)	commander->_mercenartOrder = MERCENARY_ATTACK;
		else if (value == 2)	commander->_mercenartOrder = MERCENARY_DEFENCE;
		else					commander->_mercenartOrder = MERCENARY_ANAUTO;
	}
	else if (cnt == 11){
		commander->_level = value;
	}
	else if (cnt == 12){
		commander->_curExp = value;
	}
	else if (cnt == 13){
		commander->_maxExp = value;
	}
	else if (cnt == 14){
		commander->_maxHp = value;
	}
	else if (cnt == 15){
		commander->_maxMp = value;
	}
	else if (cnt == 16){
		commander->_atk = value;
	}
	else if (cnt == 17){
		commander->_def = value;
	}
	else if (cnt == 18){
		commander->_magicAtk = value;
	}
	else if (cnt == 19){
		commander->_magicDef = value;
	}
	else if (cnt == 20){
		commander->_move = value;
	}
	else if (cnt == 21){
		commander->_atkRange = value;
	}
	else if (cnt == 22){
		commander->_commandRange = value;
	}
	else if (cnt == 23){
		commander->_commandAtk = value;
	}
	else if (cnt == 24){
		commander->_commandDef = value;
	}
	else if (cnt == 25){
		commander->_properties = value;
	}
	else if (cnt == 26){
		ItemData item;
		for (int i = 0; i < _DataBase._vItemData.size(); i++){
			if (_DataBase._vItemData[i]._number != value) continue;
			item = _DataBase._vItemData[i];
			commander->_atkItem = item;
		}
	}
	else if (cnt == 27){
		ItemData item;
		for (int i = 0; i < _DataBase._vItemData.size(); i++){
			if (_DataBase._vItemData[i]._number != value) continue;
			item = _DataBase._vItemData[i];
			commander->_DefItem = item;
		}
	}
	else if (cnt > 27){
		temp = new char[length - 1];
		for (int i = 1; i < length; i++)
			temp[i - 1] = commanderValue[i];
		value = atoi(temp);

		if (commanderValue[0] == 's'){
			SkillData skill;
			for (int i = 0; i < _DataBase._vSkillData.size(); i++){
				if (_DataBase._vSkillData[i]._number != value) continue;
				skill = _DataBase._vSkillData[i];
				commander->_vSkill.push_back(skill);
			}
		}
		else if (commanderValue[0] == 'm'){
			MercenaryData mercenary;
			for (int i = 0; i < _DataBase._vMercenaryData.size(); i++){
				if (_DataBase._vMercenaryData[i]._number != value) continue;
				mercenary = _DataBase._vMercenaryData[i];
				commander->_vMercenary.push_back(mercenary);
			}
		}
		else if (commanderValue[0] == 'r'){
			SkillData summon;
			for (int i = 0; i < _DataBase._vSkillData.size(); i++){
				if (_DataBase._vSkillData[i]._number != value) continue;
				summon = _DataBase._vSkillData[i];
				commander->_vSummon.push_back(summon);
			}
		}
		delete[] temp;
		temp = NULL;
	}
}

void PkwDataManager::setStageMercenary(char* chMercenary)
{
	PkwCharactor charactor;
	MercenaryData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(chMercenary);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){

		if (chMercenary[i] == ' '){
			if (!valueCheck)	continue;

			//지휘관 속성 추가하기
			setStageMercenaryValueAdd(&charactor, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = chMercenary[i];
			j++;
			valueCheck = true;
		}
	}

	MercenaryData buf = _DataBase.getMercenary(charactor._name);
	charactor.MercenarySet(buf);

	charactor._commander->_vArms.push_back(charactor);
}
void PkwDataManager::setStageMercenaryValueAdd(PkwCharactor* mercenary, char* mercenaryValue, int cnt)
{
	int length = strlen(mercenaryValue);
	int value;
	char temp[128];

	value = atoi(mercenaryValue);
	if (cnt == 0){
		memset(temp, 0, 128);
		memcpy(temp, mercenaryValue, length);
		mercenary->_commander = _CharactorPlay.GetCommander(temp);
	}
	else if (cnt == 1){
		memset(mercenary->_name, 0, 128);
		memcpy(mercenary->_name, mercenaryValue, length);
	}
	else if (cnt == 2){
		mercenary->_pos.x = value;
	}
	else if (cnt == 3){
		mercenary->_pos.y = value;
	}
	else if (cnt == 4){
		mercenary->_armyType = value;
	}
	else if (cnt == 5){
		if (value == 0)
			mercenary->_mercenartOrder = MERCENARY_BATTLE;
		else if (value == 1)
			mercenary->_mercenartOrder = MERCENARY_ATTACK;
		else if (value == 2)
			mercenary->_mercenartOrder = MERCENARY_DEFENCE;
		else if (value == 3)
			mercenary->_mercenartOrder = MERCENARY_ANAUTO;
	}
}

void PkwDataManager::setStageCommanderPos(char* chCommanderPos)
{
	POINT pos;

	char value[512];
	bool valueCheck = false;
	int length = strlen(chCommanderPos);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (chCommanderPos[i] == ' '){
			if (!valueCheck)	continue;

			//지휘관 속성 추가하기
			setStageMercenaryValueAdd(&pos, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = chCommanderPos[i];
			j++;
			valueCheck = true;
		}
	}
	_vCommanderPos.push_back(pos);
}
void PkwDataManager::setStageMercenaryValueAdd(POINT* pos, char* posValue, int cnt)
{
	int length = strlen(posValue);
	int value;
	char *temp;

	temp = new char[length - 1];
	for (int i = 1; i < length; i++)
		temp[i - 1] = posValue[i];
	value = atoi(temp);

	if (posValue[0] == 'x')
		pos->x = value;
	else if (posValue[0] == 'y')
		pos->y = value;

	delete[] temp;
	temp = NULL;
}

void PkwDataManager::setStageShopItem(char* chShopItem)
{
	ItemData addItem;

	char value[512];
	bool valueCheck = false;
	int length = strlen(chShopItem);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (chShopItem[i] == ' '){
			if (!valueCheck)	continue;

			//지휘관 속성 추가하기
			setStageShopItemValueAdd(&addItem, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = chShopItem[i];
			j++;
			valueCheck = true;
		}
	}
	_vShopData.push_back(addItem);
}
void PkwDataManager::setStageShopItemValueAdd(ItemData* item, char* itemValue, int cnt)
{
	int length = strlen(itemValue);
	int value;
	char temp[128];

	value = atoi(itemValue);
	if (cnt == 0){
		for (int i = 0; i < _DataBase._vItemData.size(); i++){
			if (value == _DataBase._vItemData[i]._number)
				*item = _DataBase._vItemData[i];
		}

	}
}

void PkwDataManager::setStageFileRoute()
{
	for (int i = 0; i < STAGE_MAX; i++){
		memset(_stageDataFile[i].stageFileRoute, 0, 128);
		memset(_stageDataFile[i].stageMapImageKey, 0, 128);
		memset(_stageDataFile[i].stageTalkFile, 0, 128);
	}

	memcpy(_stageDataFile[0].stageFileRoute, "scenario\\stageSetData-01.txt", strlen("scenario\\stageSetData-01.txt"));
	memcpy(_stageDataFile[1].stageFileRoute, "scenario\\stageSetData-02.txt", strlen("scenario\\stageSetData-02.txt"));

	memcpy(_stageDataFile[0].stageMapImageKey, "map1", strlen("map1"));
	memcpy(_stageDataFile[1].stageMapImageKey, "map1", strlen("map1"));

	memcpy(_stageDataFile[0].stageTalkFile, "scenario\\stageTalkData-01.txt", strlen("scenario\\stageTalkData-01.txt"));
	memcpy(_stageDataFile[1].stageTalkFile, "scenario\\stageTalkData-01.txt", strlen("scenario\\stageTalkData-01.txt"));
}

void PkwDataManager::setBmSceneData(BattleInfo battleInfo, PkwCharactor* choiceUnit, SkillData* choiceSkill, SkillData* choiceSummon, PkwCharactor* targetUnit)
{
	_battleInfo = battleInfo;
	_choiceUnit = choiceUnit;
	_choiceSkill = choiceSkill;
	_choiceSummon = choiceSummon;
	_targetUnit = targetUnit;
}
