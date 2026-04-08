#include "PkwDataBase.h"


PkwDataBase::PkwDataBase()
{
}
PkwDataBase::~PkwDataBase()
{
}

void PkwDataBase::DataInit()
{
	_vClassData.clear();

	if (!LoadTxtFile("data\\itemData.txt"))
		return;
	if (!LoadTxtFile("data\\skillData.txt"))
		return;
	if (!LoadTxtFile("data\\mercenaryData.txt"))
		return;
	if (!LoadTxtFile("data\\summonData.txt"))
		return;
	if (!LoadTxtFile("data\\classData.txt"))
		return;
	if (!LoadTxtFile("data\\commanderData.txt"))
		return;

	return;
}
void PkwDataBase::DataRelease()
{
	int size;

	size = _vCommandData.size();
	 for (int i = 0; i < size; i++){
		 _vCommandData[i]._vMercenary.clear();
		 _vCommandData[i]._vSkill.clear();
	 }
	 _vCommandData.clear();

	_vItemData.clear();
	_vSkillData.clear();
	_vMercenaryData.clear();
	_vClassData.clear();

	size = _vSummonData.size();
	for (int i = 0; i < size; i++)
		_vSummonData[i]._vSkill.clear();
	_vSummonData.clear();
}
bool PkwDataBase::LoadTxtFile(char* fileName)
{
	ifstream file;
	char temp[512];
	int type;
	int length;
	file.open(fileName);
	file.getline(temp,512);
	length = strlen(temp);

	if (strcmp(temp, "#itemData") == 0){
		type = 0;
	}
	else if ( strcmp(temp, "#skillData")== 0 ){
		type = 1;
	}
	else if (strcmp(temp, "#mercenaryData")== 0){
		type = 2;
	}
	else if (strcmp(temp, "#summonData") == 0){
		type = 3;
	}
	else if (strcmp(temp, "#commanderData") == 0){
		type = 4;
	}
	else if (strcmp(temp, "#classData") == 0){
		type = 5;
	}
	else{
		file.close();
		return false;
	}

	while (!file.eof()){
		file.getline(temp, 512);
		if (temp[0] == '#') continue;

		if (type == 0)			ItempDataAdd(temp);
		else if (type == 1)		SkillDataAdd(temp);
		else if (type == 2)		MercenaryDataAdd(temp);
		else if (type == 3)		SummonDataAdd(temp);
		else if (type == 4)		CommanderDataAdd(temp);
		else if (type == 5)		ClassDataAdd(temp);
		else{}
	}
	file.close();
	return true;
}

void PkwDataBase::CommanderDataAdd(char* commanderData)
{
	CommanderData temp;
	
	char value[512];
	bool valueCheck = false;
	int length = strlen(commanderData);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (commanderData[i] == ' '){
			if (!valueCheck)	continue;

			//지휘관 속성 추가하기
			CommanderValueAdd(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = commanderData[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//지휘관 추가
	size = _vItemData.size();
	_vCommandData.push_back(temp);
}
void PkwDataBase::CommanderValueAdd(CommanderData* commander, char* commanderValue, int cnt)
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
		ClassValueAdd(commander, commanderValue);
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
		for (int i = 0; i < _vItemData.size(); i++){
			if (_vItemData[i]._number != value) continue;
			item = _vItemData[i];
			commander->_atkItem = item;
		}
	}
	else if (cnt == 27){
		ItemData item;
		for (int i = 0; i < _vItemData.size(); i++){
			if (_vItemData[i]._number != value) continue;
			item = _vItemData[i];
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
			for (int i = 0; i < _vSkillData.size(); i++){
				if (_vSkillData[i]._number != value) continue;
				skill = _vSkillData[i];
				commander->_vSkill.push_back(skill);
			}
		}
		else if (commanderValue[0] == 'm'){
			MercenaryData mercenary;
			for (int i = 0; i < _vMercenaryData.size(); i++){
				if (_vMercenaryData[i]._number != value) continue;
				mercenary = _vMercenaryData[i];
				commander->_vMercenary.push_back(mercenary);
			}
		}
		else if (commanderValue[0] == 'r'){
			SkillData summon;
			for (int i = 0; i < _vSkillData.size(); i++){
				if (_vSkillData[i]._number != value) continue;
				summon = _vSkillData[i];
				commander->_vSummon.push_back(summon);
			}
		}

		delete[] temp;
		temp = NULL;
	}
}
void PkwDataBase::ClassValueAdd(CommanderData* commander, char* commanderValue)
{
	if (strcmp(commanderValue, "파이터") == 0){
		commander->_class = FIGHTER;
	}
	else if (strcmp(commanderValue, "로드") == 0){
		commander->_class = LOAD;
	}
	else if (strcmp(commanderValue, "하이로드") == 0){
		commander->_class = HIGH_LOAD;
	}
	else if (strcmp(commanderValue, "제너럴") == 0){
		commander->_class = GENERAL;
	}
	else if (strcmp(commanderValue, "엠페러") == 0){
		commander->_class = EMPEROR;
	}

	else if (strcmp(commanderValue, "글레디에이터") == 0){
		commander->_class = GLADIATOR;
	}
	else if (strcmp(commanderValue, "어썌신") == 0){
		commander->_class = ASSASSIN;
	}
	else if (strcmp(commanderValue, "소드맨") == 0){
		commander->_class = SWORD_MAN;
	}
	else if (strcmp(commanderValue, "레인져") == 0){
		commander->_class = RANGER;
	}
	else if (strcmp(commanderValue, "하이마스터") == 0){
		commander->_class = HIGH_MASTER;
	}

	else if (strcmp(commanderValue, "나이트") == 0){
		commander->_class = KNIGHT;
	}
	else if (strcmp(commanderValue, "실버나이트") == 0){
		commander->_class = LOAD;
	}
	else if (strcmp(commanderValue, "하이랜서") == 0){
		commander->_class = HIGH_LANDER;
	}
	else if (strcmp(commanderValue, "나이트마스터") == 0){
		commander->_class = KNIGHT_MASTER;
	}
	else if (strcmp(commanderValue, "로얄가드") == 0){
		commander->_class = ROYAL_GUARD;
	}
	else if (strcmp(commanderValue, "워록") == 0){
		commander->_class = WAR_ROCK;
	}
	else if (strcmp(commanderValue, "소서러") == 0){
		commander->_class = SORCERER;
	}
	else if (strcmp(commanderValue, "메이지") == 0){
		commander->_class = MAGE;
	}
	else if (strcmp(commanderValue, "아크메이지") == 0){
		commander->_class = ARCHMAGE;
	}
	else if (strcmp(commanderValue, "위자드") == 0){
		commander->_class = WIZARD;
	}
	
	else if (strcmp(commanderValue, "시스터") == 0){
		commander->_class = SISTER;
	}
	else if (strcmp(commanderValue, "클레릭") == 0){
		commander->_class = CLERIC;
	}
	else if (strcmp(commanderValue, "프리스트") == 0){
		commander->_class = PRIEST;
	}
	else if (strcmp(commanderValue, "하이프리스트") == 0){
		commander->_class = HIGH_PRIEST;
	}
	else if (strcmp(commanderValue, "에이전트") == 0){
		commander->_class = AGENT;
	}
		
	else if (strcmp(commanderValue, "파이러트") == 0){
		commander->_class = PIRATE;
	}
	else if (strcmp(commanderValue, "캡틴") == 0){
		commander->_class = CAPTAIN;
	}
	else if (strcmp(commanderValue, "서펜나이트") == 0){
		commander->_class = SERPEN_KNIGHT;
	}
	else if (strcmp(commanderValue, "서펜로드") == 0){
		commander->_class = SERPEN_LOAD;
	}
	else if (strcmp(commanderValue, "서펜마스터") == 0){
		commander->_class = SERPEN_MASTER;
	}
}

void PkwDataBase::ItempDataAdd(char* itemData)
{
	ItemData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(itemData);
	int cnt = 0,j=0;
	
	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (itemData[i] == ' '){
			if (!valueCheck)	continue;

			//아이템 속성 추가하기
			ItemValueAdd(&temp, value, cnt);
			
			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = itemData[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//아이템 추가
	size = _vItemData.size();
	_vItemData.push_back(temp);
}
void PkwDataBase::ItemValueAdd(ItemData* item, char* itemValue, int cnt)
{
	int length = strlen(itemValue);
	int value;
	char *temp;
	if (cnt == 0){
		value = atoi(itemValue);
		item->_number = value;
	}
	else if (cnt == 1){
		value = atoi(itemValue);
		item->_imageNum = value;
	}
	else if (cnt == 2){
		value = atoi(itemValue);
		item->_type = value;
	}
	else if (cnt == 3){
		memset(item->_name, 0, 128);
		memcpy(item->_name, itemValue, length);
	}
	else if (cnt >= 4 && cnt <= 16){
		temp = new char[length-1];
		for (int i = 1; i < length; i++)
			temp[i - 1] = itemValue[i];
		value = atoi(temp);

		if (itemValue[0] == '-')
			value *= -1;

		if (cnt == 4)			item->_atk = value;
		else if (cnt == 5)		item->_def = value;
		else if (cnt == 6)		item->_mercenaryAtk = value;
		else if (cnt == 7)		item->_mercenaryDef = value;
		else if (cnt == 8)		item->_magicAtk = value;
		else if (cnt == 9)		item->_magicDef = value;
		else if (cnt == 10)		item->_move = value;
		else if (cnt == 11)		item->_mercenaryeMove = value;
		else if (cnt == 12)		item->_atkRange = value;
		else if (cnt == 13)		item->_commandRange = value;
		else if (cnt == 14)		item->_mp = value;
		else if (cnt == 15)		item->_summon = value;
		else if (cnt == 16)		item->_money = value;
		else{}
		
		delete[] temp;
		temp = NULL;
	}
	
}

void PkwDataBase::SkillDataAdd(char* skillValue)
{
	SkillData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(skillValue);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (skillValue[i] == ' '){
			if (!valueCheck)	continue;

			//스킬 속성 추가하기
			SkillValueAdd(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = skillValue[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//스킬 추가
	size = _vItemData.size();
	_vSkillData.push_back(temp);
}
void PkwDataBase::SkillValueAdd(SkillData* skill, char* skillValue, int cnt)
{
	int length = strlen(skillValue);
	int value;
	char *temp;
	value = atoi(skillValue);
	if (cnt == 0){
		skill->_number = value;
	}
	else if (cnt == 1){
		skill->_type = value;
	}
	else if (cnt == 2){
		memset(skill->_name, 0, 128);
		memcpy(skill->_name, skillValue, length);
	}
	else if (cnt == 3){
		skill->_demage = value;
	}
	else if (cnt == 4)
		skill->_heal = value;
	else if (cnt == 5)
		skill->_atkUp = value;
	else if (cnt == 6)		
		skill->_defUp = value;
	else if (cnt == 7)		
		skill->_magicAtkUp = value;
	else if (cnt == 8)		
		skill->_magicDefUp = value;
	else if (cnt == 9)		
		skill->_moveUp = value;
	else if (cnt == 10)		
		skill->_atkRange = value;
	else if (cnt == 11)		
		skill->_atkScale = value;
	else if (cnt == 12)		
		skill->_commandRange = value;
	else if (cnt == 13)		
		skill->_mpSize = value;
	else if (cnt == 14)		
		skill->_etc = value;
}

void PkwDataBase::MercenaryDataAdd(char* mercenaryValue)
{
	MercenaryData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(mercenaryValue);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (mercenaryValue[i] == ' '){
			if (!valueCheck)	continue;

			//용병 속성 추가하기
			MercenaryValueAdd(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = mercenaryValue[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//용병 추가
	size = _vItemData.size();
	_vMercenaryData.push_back(temp);
}
void PkwDataBase::MercenaryValueAdd(MercenaryData* mercenary, char* mercenaryValue, int cnt)
{
	int length = strlen(mercenaryValue);
	int value;
	//char *temp;

	value = atoi(mercenaryValue);

	if (cnt == 0){
		mercenary->_number = value;
	}
	else if (cnt == 1){
		memset(mercenary->_imageKey, 0, 128);
		memcpy(mercenary->_imageKey, mercenaryValue, length);
	}
	else if (cnt == 2){
		memset(mercenary->_name, 0, 128);
		memcpy(mercenary->_name, mercenaryValue, length);
	}
	
	else if (cnt >= 3 && cnt <= 15){
		if (cnt == 3)			mercenary->_type = value;
		else if (cnt == 4)		mercenary->_maxHp = value;
		else if (cnt == 5)		mercenary->_atk = value;
		else if (cnt == 6)		mercenary->_def = value;
		else if (cnt == 7)		mercenary->_magicAtk = value;
		else if (cnt == 8)		mercenary->_magicDef = value;
		else if (cnt == 9)		mercenary->_move = value;
		else if (cnt == 10)		mercenary->_atkRange = value;
		else if (cnt == 11)		mercenary->_price = value;
		else{}
		/*delete[] temp;
		temp = NULL;*/
	}
}

void PkwDataBase::SummonDataAdd(char* summonValue)
{
	SummonData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(summonValue);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (summonValue[i] == ' '){
			if (!valueCheck)	continue;

			//소환수 속성 추가하기
			SummonValueAdd(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = summonValue[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//소환수 추가
	size = _vItemData.size();
	_vSummonData.push_back(temp);
}
void PkwDataBase::SummonValueAdd(SummonData* summon, char* summonValue, int cnt)
{
	int length = strlen(summonValue);
	int value;
	char *temp;

	value = atoi(summonValue);
	if (cnt == 0){	
		summon->_number = value;
	}
	else if (cnt == 1){
		summon->_type = value;
	}
	else if (cnt == 2){
		memset(summon->_name, 0, 128);
		memcpy(summon->_name, summonValue, length);
	}
	else if (cnt == 3)		summon->_atk = value;
	else if (cnt == 4)		summon->_def = value;
	else if (cnt == 5)		summon->_magicAtk = value;
	else if (cnt == 6)		summon->_magicDef = value;
	else if (cnt == 7)		summon->_move = value;
	else if (cnt == 8)		summon->_atkRange = value;
	else if (cnt == 9)		summon->_maxHp = value;
	else if (cnt == 10)		summon->_maxMp = value;
	else if (cnt == 11)		summon->_useMp = value;
	else if (cnt == 12)		summon->_properties = value;
	else{
		SkillData skill;
		for (int i = 0; i < _vSkillData.size(); i++){
			if (_vSkillData[i]._number != value) continue;
			skill = _vSkillData[i];
			summon->_vSkill.push_back(skill);
		}
	}
}
MercenaryData PkwDataBase::getMercenary(char* name)
{
	MercenaryData temp;
	temp._atk = -1;
	for (int i = 0; i < _vMercenaryData.size(); i++){
		if (strcmp(_vMercenaryData[i]._name, name) == 0)
			return _vMercenaryData[i];
		
	}
	return temp;
}

void PkwDataBase::ClassDataAdd(char* chClass)
{
	PkwClassData temp;

	char value[512];
	bool valueCheck = false;
	int length = strlen(chClass);
	int cnt = 0, j = 0;

	memset(value, 0, 512);

	for (int i = 0; i < length; i++){
		if (chClass[i] == ' '){
			if (!valueCheck)	continue;

			//소환수 속성 추가하기
			ClassValue(&temp, value, cnt);

			memset(value, 0, 512);
			cnt++; j = 0;
			valueCheck = false;
		}
		else{
			value[j] = chClass[i];
			j++;
			valueCheck = true;
		}
	}
	int size;
	//소환수 추가
	size = _vItemData.size();
	_vClassData.push_back(temp);
}
void PkwDataBase::ClassValue(PkwClassData* classData, char* classValue, int cnt)
{
	int length = strlen(classValue);
	int value;
	char *temp;

	value = atoi(classValue);

	if (cnt == 0){
		classData->_myClass = ClassValueAdd(classValue);
		memset(classData->_name, 0, 128);
		memcpy(classData->_name, classValue, length);
	}
	else if (cnt == 1){
		classData->_nextClass1 = ClassValueAdd(classValue);
	}
	else if (cnt == 2){
		classData->_nextClass2 = ClassValueAdd(classValue);
	}
	else if (cnt == 3)		classData->_upAtk = value;
	else if (cnt == 4)		classData->_upDef = value;
	else if (cnt == 5)		classData->_upMp = value;
	else if (cnt == 6)		classData->_move = value;
	else if (cnt == 7)		classData->_commanderRange = value;
	else if (cnt == 8)		classData->_upCommanderAtk = value;
	else if (cnt == 9)		classData->_upCommanderDef = value;
	else{
		temp = new char[length - 1];
		for (int i = 1; i < length; i++)
			temp[i - 1] = classValue[i];
		value = atoi(temp);

		if (classValue[0] == 's'){
			SkillData skill;
			for (int i = 0; i < _vSkillData.size(); i++){
				if (_vSkillData[i]._number != value) continue;
				skill = _vSkillData[i];
				classData->_vSkill.push_back(skill);
			}
		}
		else if (classValue[0] == 'm'){
			MercenaryData mercenary;
			for (int i = 0; i < _vMercenaryData.size(); i++){
				if (_vMercenaryData[i]._number != value) continue;
				mercenary = _vMercenaryData[i];
				classData->_vMercenary.push_back(mercenary);
			}
		}
		else if (classValue[0] == 'r'){
			SkillData summon;
			for (int i = 0; i < _vSkillData.size(); i++){
				if (_vSkillData[i]._number != value) continue;
				summon = _vSkillData[i];
				classData->_vSummon.push_back(summon);
			}
		}

		delete[] temp;
		temp = NULL;
	}
}
CLASS_TYPE PkwDataBase::ClassValueAdd(char* commanderValue)
{
	if (strcmp(commanderValue, "파이터") == 0){
		return FIGHTER;
	}
	else if (strcmp(commanderValue, "로드") == 0){
		return LOAD;
	}
	else if (strcmp(commanderValue, "하이로드") == 0){
		return HIGH_LOAD;
	}
	else if (strcmp(commanderValue, "제너럴") == 0){
		return GENERAL;
	}
	else if (strcmp(commanderValue, "엠페러") == 0){
		return EMPEROR;
	}

	else if (strcmp(commanderValue, "글레디에이터") == 0){
		return GLADIATOR;
	}
	else if (strcmp(commanderValue, "어썌신") == 0){
		return ASSASSIN;
	}
	else if (strcmp(commanderValue, "소드맨") == 0){
		return SWORD_MAN;
	}
	else if (strcmp(commanderValue, "레인져") == 0){
		return RANGER;
	}
	else if (strcmp(commanderValue, "하이마스터") == 0){
		return HIGH_MASTER;
	}

	else if (strcmp(commanderValue, "나이트") == 0){
		return KNIGHT;
	}
	else if (strcmp(commanderValue, "실버나이트") == 0){
		return LOAD;
	}
	else if (strcmp(commanderValue, "하이랜서") == 0){
		return HIGH_LANDER;
	}
	else if (strcmp(commanderValue, "나이트마스터") == 0){
		return KNIGHT_MASTER;
	}
	else if (strcmp(commanderValue, "로얄가드") == 0){
		return ROYAL_GUARD;
	}
	else if (strcmp(commanderValue, "워록") == 0){
		return WAR_ROCK;
	}
	else if (strcmp(commanderValue, "소서러") == 0){
		return SORCERER;
	}
	else if (strcmp(commanderValue, "메이지") == 0){
		return MAGE;
	}
	else if (strcmp(commanderValue, "아크메이지") == 0){
		return ARCHMAGE;
	}
	else if (strcmp(commanderValue, "위자드") == 0){
		return WIZARD;
	}

	else if (strcmp(commanderValue, "시스터") == 0){
		return SISTER;
	}
	else if (strcmp(commanderValue, "클레릭") == 0){
		return CLERIC;
	}
	else if (strcmp(commanderValue, "프리스트") == 0){
		return PRIEST;
	}
	else if (strcmp(commanderValue, "하이프리스트") == 0){
		return HIGH_PRIEST;
	}
	else if (strcmp(commanderValue, "에이전트") == 0){
		return AGENT;
	}

	else if (strcmp(commanderValue, "파이러트") == 0){
		return PIRATE;
	}
	else if (strcmp(commanderValue, "캡틴") == 0){
		return CAPTAIN;
	}
	else if (strcmp(commanderValue, "서펜나이트") == 0){
		return SERPEN_KNIGHT;
	}
	else if (strcmp(commanderValue, "서펜로드") == 0){
		return SERPEN_LOAD;
	}
	else if (strcmp(commanderValue, "서펜마스터") == 0){
		return SERPEN_MASTER;
	}

	else if (strcmp(commanderValue, "end") == 0){
		return CLASS_END;
	}
}