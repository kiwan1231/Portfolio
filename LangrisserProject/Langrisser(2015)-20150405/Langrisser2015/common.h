#pragma once
#include <windows.h>
#include <wchar.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlobj.h>
#include <initguid.h>
#include <objbase.h>
#include <fstream>
#include <locale.h>
#include <string>
#include <tchar.h>
#include <time.h>
#include<iostream>
#include<vector>

using namespace std;

enum CHARACTOR_TYPE{
	FOOT_SOLDIER,
	CAVALRY,
	MARINE,
	AIR_FORCE
};
enum CLASS_TYPE{
	CLASS_END,

	FIGHTER,
	LOAD,
	HIGH_LOAD,
	GENERAL,
	EMPEROR,

	GLADIATOR,
	ASSASSIN,
	SWORD_MAN,
	RANGER,
	HIGH_MASTER,

	KNIGHT,
	SILVER_KNIGHT,
	HIGH_LANDER,
	KNIGHT_MASTER,
	ROYAL_GUARD,

	HAWK_KNIGHT,
	HAWK_LOAD,
	DRAGON_KNIGHT,
	DRAGON_LOAD,
	DRAGON_MASTER,

	WAR_ROCK,
	SORCERER,
	MAGE,
	ARCHMAGE,
	WIZARD,

	SISTER,
	CLERIC,
	PRIEST,
	HIGH_PRIEST,
	AGENT,

	PIRATE,
	CAPTAIN,
	SERPEN_KNIGHT,
	SERPEN_LOAD,
	SERPEN_MASTER
};
enum IMAGE_TYPE{
	NORMAL,
	ITEM,
	CHAT_FACE,
	BATTLE_FACE,
	BATTLE_ANI,
	MAP
};
enum ANI_TYPE{
	NO,
	STAND,
	RIGHT_WALK,
	LEFT_WALK,
	TOP_WALK,
	BOTTOM_WALK,
	ATTACK,
	DEATH
};
enum SCENE_MODE{
	LOADING_SCENE,
	OPENING_SCENE,
	SCENARIO_SCENE,
	BATTLE_SET_SCENE,
	BATTLE_SCENE,
};
enum CHARACTOR_CONTROL{
	SYSTEM_ON,
	CO_CONTROL_OFF,//캐릭터 선택 안한 상태
	CO_CONTROL_CHOICE,//캐릭터 선택 상태(커맨드 선택 안함)
	CO_CONTROL_MOVE,// 캐릭터 선택 상태(커맨드 이동 선택)
	CO_CONTROL_MOVING,// 캐릭터가 이동중인 상태
	CO_CONTROL_ATTACK,// 캐릭터 선택 상태(커맨드 공격 선택)
	CO_CONTROL_MAGIC,// 캐릭터 선택 상태(커맨드 마법 선택)
	CO_CONTROL_MAGIC_CHOICE,
	CO_CONTROL_MAGIC_TARGET_CHOICE,
	CO_CONTROL_MAGIC_POS_CHOICE,
	CO_CONTROL_SUMMON,// 캐릭터 선택 상태(소환 마법 선택)
	CO_CONTROL_SUMMON_CHOICE,
	//CO_CONTROL_SUMMON_POS_CHOICE,
	CO_CONTROL_ORDER,// 캐릭터 선택 상태(커맨드 명령 선택)
	SUPPORT_CHOICE,//지원군 캐릭터 선택 상태(커맨드 할수 없음)
	ENEMY_CHOICE// 적 캐릭터 선택 상태(커맨드 할수 없음)
};
enum COMMAND_CHOICE{
	COMMAND_MOVE,//커맨드창 이동에 커서가 올라가 있는 상태
	COMMAND_ATTACK,//커맨드창 공격에 커서가 올라가 있는 상태
	COMMAND_MAGIC,//커맨드창 마법에 커서가 올라가 있는 상태
	COMMAND_SUMMON,//커맨드창 소환에 커서가 올라가 있는 상태
	COMMAND_HEAL,//커맨드창 회복에 커서가 올라가 있는 상태
	COMMAND_ORDER//커맨드창 명령에 커서가 올라가 있는 상태
};
enum MERCENARY_ORDER_CHOICE{
	ORDER_BATTLE,//커맨드창 이동에 커서가 올라가 있는 상태
	ORDER_ATTACK,//커맨드창 공격에 커서가 올라가 있는 상태
	ORDER_DEFENCE,//커맨드창 마법에 커서가 올라가 있는 상태
	ORDER_ANAUTO,//커맨드창 회복에 커서가 올라가 있는 상태
};
enum MERCENARY_ORDER{
	MERCENARY_BATTLE,
	MERCENARY_ATTACK,
	MERCENARY_DEFENCE,
	MERCENARY_ANAUTO,
};

enum BATTLE_STATUS{
	CO_TURN,
	SUPPORT_TURN,
	ENEMY_TURN,
	GAME_CLEAR,
	GAME_OVER
};
enum BATTLE_EVENT{
	BATTLE_EVENT_ON,
	BATTLE_EVENT_OFF
};
//enum BATTLE_EFFECT{
//	BATTLE_EFFECT_PLAY,
//	BATTLE_EFFECT_STOP
//};

struct BattleInfo{
	int						nTurn;
	BATTLE_STATUS			battleStatus;
	BATTLE_EVENT			eventPlay;
	//BATTLE_EFFECT			effectPlay;

	CHARACTOR_CONTROL		charactorControl;
	COMMAND_CHOICE			commandChoice;
	MERCENARY_ORDER_CHOICE	orderChoice;
};

/* 
---------ItemData---------------
# type - 1 :공격 아이템, 2: 방어아이템
*/
class ItemData{
public:
	int _number;		//0
	int	_imageNum;		//1
	int _type;			//2
	char _name[128];	//3
	int _atk;			//4
	int _def;			//5
	int _mercenaryAtk;	//6
	int _mercenaryDef;	//7
	int _magicAtk;		//8
	int _magicDef;		//9
	int _move;			//10
	int _mercenaryeMove;//11
	int _atkRange;		//12
	int _commandRange;	//13
	int _mp;			//14
	int _summon;		//15
	int _money;			//16
};

/* ---------SkillData---------------
# _type  0 :단일 공격, 1 : 범위 공격, 2: 회복, 3: 버프,디버프,  4 : 소환,  5 : 특수

# _atkScale		값이 -1이면 그건 선택된 병사의 부대가 지정되는 것이다.
# _etc			특정 마법의 효과 저장
*/
class SkillData{
public:
	int		_number;
	int		_type;
	char	_name[128];
	int		_demage;
	int		_heal;
	int		_atkUp;
	int		_defUp;
	int		_magicAtkUp;
	int		_magicDefUp;
	int		_moveUp;
	int		_atkRange;
	int		_atkScale;
	int		_commandRange;
	int		_mpSize;
	int		_etc;
};

/* ---------MercenaryData---------------
# number - 캐릭터 넘버 각 캐릭터를 구분하고 이미지 넘버로도 쓰인다.
# type - 0 :보병, 1: 기병, 2:수병, 3:비병
# properties - 0 : 기본 속성, 1 - 빛  2- 어둠
*/
class MercenaryData{
public:
	int		_number;
	char	_imageKey[128];
	char	_name[128];
	int		_type;
	int		_maxHp;
	int		_atk;
	int		_def;
	int		_magicAtk;
	int		_magicDef;
	int		_move;
	int		_atkRange;

	int		_price;
};

/* ---------SummonData---------------
# number - 캐릭터 넘버 각 캐릭터를 구분하고 이미지 넘버로도 쓰인다.
# type - 0 :보병, 1: 기병, 2:수병, 3:비병
# properties - 0 : 기본 속성, 1 - 빛  2- 어둠
*/
class SummonData
{
public:
	int		_number;
	int		_type;
	char	_name[128];
	int		_atk;
	int		_def;
	int		_magicAtk;
	int		_magicDef;
	int		_move;
	int		_atkRange;
	int		_maxHp;
	int		_maxMp;
	int		_useMp;
	int		_properties;
	vector<SkillData>		_vSkill;
};
/* ---------commanderData---------------
#number - 캐릭터 넘버 각 캐릭터를 구분하고 이미지 넘버로도 쓰인다.
#type - 0 :보병, 1: 기병, 2:수병, 3:비병
#properties - 0 : 기본 속성, 1 - 빛  2- 어둠
*/
class CommanderData{
public:
	int						_number;
	int						_imageNum;
	char					_imageKey[128];
	char					_faceImageKey[128];
	char					_talkImageKey[128];
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
	int						_magicAtk;
	int						_magicDef;
	int						_move;
	int						_atkRange;
	int						_commandRange;
	int						_commandAtk;
	int						_commandDef;
	int						_properties;

	ItemData				_atkItem;
	ItemData				_DefItem;
	vector<SkillData>		_vSkill;
	vector<SkillData>		_vSummon;
	vector<MercenaryData>	_vMercenary;
	//vector<SummonData>		_vSummonData;
};

/*
변수 구조체.... 아직 정의 하기 어렵네
*/
struct InGameValue{
	int		value;
	int		duration;
	//duration은 해당 값의 지속기간을 저장한다.
	//3~1 해당 버프 혹은 디버프의 마법의 지속기간 턴이 지날떄마다 -1이 된다.
	//0 이 될경우 마법이 해제가 되며 해당되는 밸루값을 다시 초기화 해준다.
};

//기타 구조체
struct NumDigit{
	int value[10];
	int strlen;
};

enum FRAME_DIR{
	NORMAL_DIR,
	REVERSE_DIR
};

//이미지 프레임 돌리떄 쓰는 구조체
struct imageFrameData{
	int			frameX;
	int			maxFrameX;
	float		elaspedTime;
	float		frameDelay;
	FRAME_DIR	frameDir;
};