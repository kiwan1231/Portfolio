#include "../stdafx.h"
#include "PkwCharactorPlay.h"

#include "PkwActionProcess.h"
#include "PkwEffectPlay.h"

#include "PkwBattleMapScene.h"
#include "unitAI_Algorism.h"
#include "commanderAI.h"
#include "mercenaryAI.h"
PkwCharactor::PkwCharactor()
{

}
PkwCharactor::~PkwCharactor()
{

}

void PkwCharactor::CharactorInit()
{
	_AniStatus = STAND;
	_pos.x = 0;
	_pos.y = 0;
	//_name[0] = '\n';
	//_MaxHp = 10;
	//
	//_isEnemy = false;

	_imageNum = 0;
	
	_AniFrame = 0.0;
	_AniNum = 0;
}
void PkwCharactor::CharactorInit(CommanderData* commander)
{
	_commander		= NULL;
	_action = CHARACTOR_ACTION_ON;
	_vArms.clear();
	_maxArms = 6;

	_number			= commander->_number;

	_pos			= commander->_pos;
	_armyType		= commander->_armyType;

	int length = strlen(commander->_name);
	memset(_name, 0, 128);
	memcpy(_name, commander->_name, length);

	_class			= commander->_class;
	length = strlen(commander->_chClass);
	memset(_chClass, 0, 128);
	memcpy(_chClass, commander->_chClass, length);

	_type			= commander->_type;
	_mercenartOrder = commander->_mercenartOrder;
	_level = commander->_level;
	_curExp = commander->_curExp;
	_maxExp = commander->_maxExp;
	_maxHp = commander->_maxHp;
	_maxMp = commander->_maxMp;
	_atk = commander->_atk;
	_def = commander->_def;
	_migAtk = commander->_magicAtk;
	_migDef = commander->_magicDef;
	_move = commander->_move;
	_atkRange = commander->_atkRange;
	_commandRange = commander->_commandRange;
	_commandAtk = commander->_commandAtk;
	_commandDef = commander->_commandDef;
	_properties = commander->_properties;

	_atkItem = commander->_atkItem;
	_DefItem = commander->_DefItem;


	int size = commander->_vSkill.size();
	_vSkill.clear();
	for (int i = 0; i < size; i++)
		_vSkill.push_back(commander->_vSkill[i]);

	size = commander->_vMercenary.size();
	_vMercenaryData.clear();
	for (int i = 0; i < size; i++)
		_vMercenaryData.push_back(commander->_vMercenary[i]);

	size = commander->_vSummon.size();
	_vSummon.clear();
	for (int i = 0; i < size; i++)
		_vSummon.push_back(commander->_vSummon[i]);


	_curArmyType.value = _armyType;
	_curArmyType.duration = 0;

	_curHp = _maxHp;
	_curMp = _maxMp;

	_curAtk.value = _atk;
	_curAtk.duration = 0;

	_curDef.value = _def;
	_curDef.duration = 0;

	_curMigAtk.value = _migAtk;
	_curMigAtk.duration = 0;

	_curMigDef.value = _migDef;
	_curMigDef.duration = 0;

	_curMove.value = _move;
	_curMove.duration = 0;

	_condition.value = 0;
	_condition.duration = 0;

	_curAtkRange.value = _atkRange;
	_curAtkRange.duration = 0;

	_curCommandRange.value = _commandRange;
	_curCommandRange.duration = 0;

	_curCommandAtk.value = _commandAtk;
	_curCommandAtk.duration = 0;

	_curCommandDef.value = _commandDef;
	_curCommandDef.duration = 0;

	_curProperties.value = _properties;
	_curProperties.duration = 0;

	_imageNum = commander->_imageNum;

	length = strlen(commander->_imageKey);
	memset(_imageKey, 0, 128);
	memcpy(_imageKey, commander->_imageKey, length);

	length = strlen(commander->_faceImageKey);
	memset(_faceImageKey, 0, 128);
	memcpy(_faceImageKey, commander->_faceImageKey, length);

	length = strlen(commander->_talkImageKey);
	memset(_talkImageKey, 0, 128);
	memcpy(_talkImageKey, commander->_talkImageKey, length);

	_AniStatus = BOTTOM_WALK;
	_AniFrame = 0.0;
	_AniNum = 0;
}
void PkwCharactor::CharactorRelease()
{
	_vArms.clear();
	_vSkill.clear();
	_vMercenaryData.clear();

	delete _AI;
}
void PkwCharactor::update(float elapsed)
{
	_AniFrame += elapsed;
	AniPlay();
}
void PkwCharactor::render(HDC MemDC, bool isBattle)
{
	int width = IMAGEMANAGER->findImage(_imageKey)->getFrameWidth();
	int height = IMAGEMANAGER->findImage(_imageKey)->getFrameHeight();

	//캐릭터 애니 출력
	if (isBattle == false){
		CharactorAniDraw(MemDC, _pos.x, _pos.y, width, height, _AniStatus, _AniNum, _imageKey);
	}
	else{
		//캐릭터 출력
		CharactorAniDraw(MemDC, _pos.x * UNITSIZEX, _pos.y * UNITSIZEY, width, height, _AniStatus, _AniNum, _imageKey);
		//hp출력
		int hpImageXPos;
		if (_armyType == 0)			hpImageXPos = 64;
		else if(_armyType == 1)		hpImageXPos = 96;
		else if(_armyType == 2)		hpImageXPos = 128;
		IMAGEMANAGER->findImage("charactorUI")->render(MemDC, (_pos.x * UNITSIZEX) + 32, (_pos.y * UNITSIZEY) + 32, _curHp * 16, hpImageXPos, 16, 16);
	}
}
void PkwCharactor::CharactorAniDraw(HDC MemDC, int destX, int destY, int width, int height, ANI_TYPE aniStatus, int aniNum, char* imageKey)
{
	int sourX = 0, sourY = 0;
	
	if (aniStatus == STAND){
		sourX = 0;
		sourY = height;
	}
	else if (aniStatus == RIGHT_WALK){
		sourX = width*(aniNum);
		sourY = 0;
	}
	else if (aniStatus == LEFT_WALK){
		sourX = width*(aniNum + 2);
		sourY = 0;
	}
	else if (aniStatus == TOP_WALK){
		sourX = width*(aniNum + 2);
		sourY = height;
	}
	else if (aniStatus == BOTTOM_WALK){
		sourX = width*(aniNum);
		sourY = height;
	}
	else if (aniStatus == ATTACK){
	}
	else if (aniStatus == DEATH){
		sourX = width * 4;
		sourY = 0;
	}
	
	destX -= (width - UNITSIZEX) / 2;
	destY -= (height - UNITSIZEY) / 2;

	IMAGEMANAGER->findImage(imageKey)->render(MemDC, destX, destY, sourX, sourY, width, height);
}
void PkwCharactor::AniPlay()
{
	//1초 지나면 애니 프레임 바꿈
	if (_AniFrame >= 0.5){
		_AniNum += 1;
		_AniFrame = 0.0;
	}
	//애니 프레임이 2개라서 2가 들어감.
	_AniNum %= 2;
}
void PkwCharactor::Move()
{

}
void PkwCharactor::MercenarySet(MercenaryData data)
{
	_vArms.clear();
	_action = CHARACTOR_ACTION_ON;
	_number = data._number;

	int length = strlen(data._name);
	memset(_name, 0, 128);
	memcpy(_name, data._name, length);

	_type = data._type;
	_maxHp = data._maxHp;
	_atk = data._atk;
	_def = data._def;
	_migAtk = data._magicAtk;
	_migDef = data._magicDef;
	_move = data._move;
	_atkRange = data._atkRange;
	_pos.x += _commander->_pos.x;
	_pos.y += _commander->_pos.y;
	_price = data._price;

	_vSkill.clear();
	_vMercenaryData.clear();
	_vSummon.clear();
	//----------------------------------------------
	_curArmyType.value = _armyType;
	_curArmyType.duration = 0;

	_curHp = _maxHp;

	_curAtk.value = _atk;
	_curAtk.duration = 0;

	_curDef.value = _def;
	_curDef.duration = 0;

	_curMigAtk.value = _migAtk;
	_curMigAtk.duration = 0;

	_curMigDef.value = _migDef;
	_curMigDef.duration = 0;

	_curMove.value = _move;
	_curMove.duration = 0;

	_condition.value = 0;
	_condition.duration = 0;

	_curAtkRange.value = _atkRange;
	_curAtkRange.duration = 0;


	length = strlen(data._imageKey);
	memset(_imageKey, 0, 128);
	memcpy(_imageKey, data._imageKey, length);

	_AniStatus = BOTTOM_WALK;
	_AniFrame = 0.0;
	_AniNum = 0;
}
void PkwCharactor::summonSet(PkwCharactor* commander, SummonData data, POINT pos, char* imageKey)
{
	_action = CHARACTOR_ACTION_ON;
	_commander = commander;
	_number = data._number;
	_pos.x = pos.x;
	_pos.y = pos.y;
	_armyType = _commander->_armyType;

	int length = strlen(data._name);
	memset(_name, 0, 128);
	memcpy(_name, data._name, length);

	_type = data._type;
	_mercenartOrder = _commander->_mercenartOrder;
	_maxHp = data._maxHp;
	_maxMp = data._maxMp;
	_atk = data._atk;
	_def = data._def;
	_migAtk = data._magicAtk;
	_migDef = data._magicDef;
	_move = data._move;
	_atkRange = data._atkRange;
	_properties = data._properties;
	
	_vSkill.clear();
	_vMercenaryData.clear();
	_vSummon.clear();
	_vSkill = data._vSkill;
	//----------------------------------------------
	_curArmyType.value = _armyType;
	_curArmyType.duration = 0;

	_curHp = _maxHp;
	_curMp = _maxMp;

	_curAtk.value = _atk;
	_curAtk.duration = 0;

	_curDef.value = _def;
	_curDef.duration = 0;

	_curMigAtk.value = _migAtk;
	_curMigAtk.duration = 0;

	_curMigDef.value = _migDef;
	_curMigDef.duration = 0;

	_curMove.value = _move;
	_curMove.duration = 0;

	_condition.value = 0;
	_condition.duration = 0;

	_curAtkRange.value = _atkRange;
	_curAtkRange.duration = 0;


	length = strlen(imageKey);
	memset(_imageKey, 0, 128);
	memcpy(_imageKey, imageKey, length);

	_AniStatus = BOTTOM_WALK;
	_AniFrame = _commander->_AniFrame;
	_AniNum = 0;
}
void PkwCharactor::aiSet(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pTargetUnit, POINT targetPos, int action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay)
{
	/*
	//일반 적인 AI
	AI_STOP,//아무것도 안하고 가만히 있는다(수동)
	AI_MOVE,//이동하고 멈춘다(전투)
	AI_ATK,//이동안하고 공격한다(돌격)
	AI_DEF,//방어만 한다
	//지휘관만 쓰는 AI
	AI_MAGIC,//가만히 있으면서 마법만 쓰고 마나가 다 달으면 치료 한다
	AI_POS_MOVE,//특정 지점으로 이동을 한다
	AI_TARGET_MOVE,//특정 지휘관을 향해 이동한다(char형으로 이름을 저장해 검색한다)
	AI_TARGET_ATK//특정 지휘관을 공격한다(char형으로 이름을 저장해 검색한다)
	*/
	AI_UNIT_ACTION aiAction;

	if (action == 0) aiAction = AI_STOP;
	else if (action == 1) aiAction = AI_MOVE;
	else if (action == 2) aiAction = AI_ATK;
	else if (action == 3) aiAction = AI_DEF;
	else if (action == 4) aiAction = AI_MAGIC;
	else if (action == 5) aiAction = AI_POS_MOVE;
	else if (action == 6) aiAction = AI_TARGET_MOVE;
	else  aiAction = AI_TARGET_ATK;



	if (_number < 1000){

		_AI = new commanderAI();
		if (aiAction == AI_POS_MOVE)
			_AI->init(pBttleMapScene, this, targetPos, aiAction, pActionProcess, pEffectPlay);
		else if (aiAction == AI_TARGET_MOVE)
			_AI->init(pBttleMapScene, this, pTargetUnit, aiAction, pActionProcess, pEffectPlay);
		else if (aiAction == AI_TARGET_ATK)
			_AI->init(pBttleMapScene, this, pTargetUnit, aiAction, pActionProcess, pEffectPlay);
		else if (aiAction == AI_MAGIC)
			_AI->init(pBttleMapScene, this, targetPos, aiAction, pActionProcess, pEffectPlay);
		else
			_AI->init(pBttleMapScene, this, aiAction, pActionProcess, pEffectPlay);
	}
	else{
		_AI = new mercenaryAI();

		_AI->init(pBttleMapScene, this, aiAction, pActionProcess, pEffectPlay);
	}
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

PkwCharactorPlay::PkwCharactorPlay()
{
	_tempCharac.CharactorInit();
}
PkwCharactorPlay::~PkwCharactorPlay()
{
}

void PkwCharactorPlay::CommanderRelease()
{
	int size = _vCoCommander.size();
	for (int i = 0; i < size; i++)
		_vCoCommander[i].CharactorRelease();
	_vCoCommander.clear();

	size = _vSupportCommander.size();
	for (int i = 0; i < size; i++)
		_vSupportCommander[i].CharactorRelease();
	_vSupportCommander.clear();

	size = _vEnemyCommander.size();
	for (int i = 0; i < size; i++)
		_vEnemyCommander[i].CharactorRelease();
	_vEnemyCommander.clear();
}
void PkwCharactorPlay::CreateCharactor(CommanderData* commander)
{
	PkwCharactor temp;

	temp.CharactorInit(commander);
	if (commander->_armyType == 0){
		_vCoCommander.push_back(temp);
	}
	else if (commander->_armyType == 1){
		_vSupportCommander.push_back(temp);
	}
	else if (commander->_armyType == 2){
		_vEnemyCommander.push_back(temp);
	}
}
PkwCharactor* PkwCharactorPlay::GetCommander(char* name)
{
	for (int i = 0; i < _vCoCommander.size(); i++){
		if (strcmp(_vCoCommander[i]._name, name) == 0)
			return &_vCoCommander[i];
	}
	
	for (int i = 0; i < _vSupportCommander.size(); i++){
		if (strcmp(_vSupportCommander[i]._name, name) == 0)
			return &_vSupportCommander[i];
	}

	for (int i = 0; i < _vEnemyCommander.size(); i++){
		if (strcmp(_vEnemyCommander[i]._name, name) == 0)
			return &_vEnemyCommander[i];
	}
	return NULL;
}
void PkwCharactorPlay::MercenaryIntoCommander(PkwCharactor mercenary)
{
	for (int i = 0; i < _vCoCommander.size(); i++){
		if (strcmp(_vCoCommander[i]._name, mercenary._name) == 0)
			_vCoCommander[i]._vArms.push_back(mercenary);
	}

	for (int i = 0; i < _vSupportCommander.size(); i++){
		if (strcmp(_vSupportCommander[i]._name, mercenary._name) == 0)
			_vSupportCommander[i]._vArms.push_back(mercenary);
	}

	for (int i = 0; i < _vEnemyCommander.size(); i++){
		if (strcmp(_vEnemyCommander[i]._name, mercenary._name) == 0)
			_vEnemyCommander[i]._vArms.push_back(mercenary);
	}
}
