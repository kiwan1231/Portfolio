#pragma once
#include"../common.h"

enum AI_UNIT_ACTION;

enum CHARACTOR_ACTION{
	CHARACTOR_ACTION_ON,
	CHARACTOR_ACTION_OFF,
	CHARACTOR_DEATH
};

class unitAI_Algorism;

class PkwActionProcess;

class PkwEffectPlay;

class PkwBattleMapScene;

class PkwCharactor{
public:
	PkwCharactor();
	~PkwCharactor();
public:
	CHARACTOR_ACTION		_action;
	PkwCharactor*			_commander;
	vector<PkwCharactor>	_vArms;
	int						_maxArms;

	int						_number;
	
	POINT					_pos;
	int						_armyType;
	char					_name[128];
	CLASS_TYPE				_class;
	char					_chClass[128];
	int						_type;
	MERCENARY_ORDER			_mercenartOrder;
	int						_level;
	int						_curExp;
	int						_maxExp;
	int						_maxHp;
	int						_maxMp;
	int						_atk;
	int						_def;
	int						_migAtk;
	int						_migDef;	//마법방어가 -1이면 면역?
	int						_move;
	int						_atkRange;
	int						_commandRange;
	int						_commandAtk;
	int						_commandDef;
	int						_properties;
	int						_price;

	ItemData				_atkItem;
	ItemData				_DefItem;
	vector<SkillData>		_vSkill;
	vector<SkillData>		_vSummon;
	vector<MercenaryData>	_vMercenaryData;
	//vector<SummonData>		_vSummonData;
	InGameValue				_curArmyType;
	int						_curHp;
	int						_curMp;
	InGameValue				_curAtk;
	InGameValue				_curDef;
	InGameValue				_curMigAtk;
	InGameValue				_curMigDef;
	InGameValue				_curMove;
	InGameValue				_condition; // 0-정상상태
	InGameValue				_curAtkRange;
	InGameValue				_curCommandRange;
	InGameValue				_curCommandAtk;
	InGameValue				_curCommandDef;
	InGameValue				_curProperties;

	int						_imageNum;
	char					_imageKey[128];
	char					_faceImageKey[128];
	char					_talkImageKey[128];
	ANI_TYPE				_AniStatus;
	float					_AniFrame;
	int						_AniNum;

	unitAI_Algorism*		_AI;
public:
	void CharactorInit();
	void CharactorInit(CommanderData* commander);
	void CharactorRelease();
	void MercenarySet(MercenaryData data);
	void summonSet(PkwCharactor* commander, SummonData data, POINT pos, char* imageKey);

	void aiSet(PkwBattleMapScene* pBttleMapScene, PkwCharactor* pTargetUnit, POINT targetPos, int action, PkwActionProcess* pActionProcess, PkwEffectPlay* pEffectPlay);

	virtual void update(float elapsed);
	virtual void render(HDC MemDC, bool isBattle);
	virtual void CharactorAniDraw(HDC MemDC, int destX, int destY, int width, int height, ANI_TYPE aniStatus, int aniNum, char* imageKey);
	void AniPlay();
	void Move();
};

class PkwCharactorPlay
{
public:
	PkwCharactorPlay();
	~PkwCharactorPlay();
public:
	vector<PkwCharactor>	_vCoCommander;
	vector<PkwCharactor>	_vSupportCommander;
	vector<PkwCharactor>	_vEnemyCommander;
	PkwCharactor			_tempCharac;
public:
	void CommanderRelease();
	void CreateCharactor(CommanderData* commander);
	PkwCharactor* GetCommander(char* name);
	void MercenaryIntoCommander(PkwCharactor mercenary);
};