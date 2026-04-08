
#pragma once
#include "../2DGameFramework/gameNode.h"
#include"../common.h"

enum BS_SHOW_MODE{
	BS_SHOW_BATTLE_CC,
	BS_SHOW_BATTLE_CA,
	BS_SHOW_BATTLE_AC,
	BS_SHOW_BATTLE_AA,
	BS_SHOW_MAGIC_TARGET,
	BS_SHOW_MAGIC_UNTARGET
};

enum BS_COMMANDER_STATUS{
	BS_COMMANDER_DEATH,//죽은 상태 출력도 안한다
	BS_COMMANDER_DYING,//HP0이 되서 죽는 모션을 출력한다
	BS_COMMANDER_DYING_LIE,
	BS_COMMANDER_WAIT,//시작전에 지휘관이 잠시 기달린다
	BS_COMMANDER_MAGIC,//마법을 쓸때
	BS_COMMANDER_ATK,// 공격하러 갈떄
	BS_COMMANDER_RETURN,//공격하고 돌아올떄
	BS_COMMANDER_ACTION_END//공격이 끝나고 제자리에서 멈춰있을때
};

enum BS_ARMY_STATUS{
	BS_ARMY_DEATH,
	BS_ARMY_DYING,
	BS_ARMY_DYING_LIE,
	BS_ARMY_WAIT,
	BS_ARMY_ATK_ON,
	BS_ARMY_ATK_OFF,
	BS_ARMY_RETURN,
	BS_ARMY_ACTION_END
};

enum MAGIC_MODE{
	MAGIC_CASTING,
	MAGIC_START
};

struct bsShowCommander{
	BS_COMMANDER_STATUS		status;
	image*					image;
	int						frameX;
	int						imageFrameY;
	int						type;
	int						atk;
	int						def;
	int						curHp;
	int						startHp;
	POINTF					pos;
	float					angle;
	POINTF					startPos;
	POINTF					destPos;
	float					delayTime;
	float					alpaTime;
	bool					isAlpa;
	int						atkNumber;
	float					upSpeed;
};

struct bsShowArmy{
	BS_ARMY_STATUS			status;
	image*					image;
	int						frameX;
	bool					isAttacked;//공격 당했으면 true로 만들어 준다
	int						type;
	int						atk;
	int						commanderAtk;
	int						def;
	int						commanderDef;
	POINTF					pos;
	float					angle;
	POINTF					startPos;
	POINTF					destPos;
	float					delayTime;
	float					alpaTime;
	bool					isAlpa;
	float					upSpeed;
};

struct magic_effect{
	bool isShow;
	bool isAction;
	float alpa;
	POINTF pos;
	POINTF destPos;
};

class PkwBattleShowScene : public gameNode
{
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;//배틀쇼씬에서 배틀맵씬으로 넘어가는 콜백함수
private://전투 부대 변수
	bsShowCommander					_atkCommander;
	bsShowCommander					_defCommander;
	vector<bsShowArmy>				_atkArmy;
	vector<bsShowArmy>				_defArmy;
private://업데이트 관련 변수
	float					_elapsed;
	float					_delayTime;
	float					_destance;
private://렌더 관련 변수
	image*					_backBuffer;
	HDC						_MemDC;
	RECT					_gameView;
	int						_nWindowWidth;
	int						_nWindowHeight;
	HFONT					_font[10];
public://여러 변수들 변수
	BS_SHOW_MODE			_mode;
	image*					_backImage;
	BattleInfo				_battleInfo;
	PkwCharactor*			_choiceUnit;
	SkillData*				_choiceSkill;
	SkillData*				_choiceSummon;
	PkwCharactor*			_targetUnit;
	int						_groundProperty;

	MAGIC_MODE				_magicMode;
	magic_effect			_magicEffect;
public:
	PkwBattleShowScene();
	~PkwBattleShowScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);
public://init함수 들
	//init함수에서 mode에 관련된 초기화를 여기서 해준다
	void setMode();
	//mode에 맞게 유닛 데이터를 초기화 해준다
	void setUnit();
	void SetatkCommander();
	void SetdefCommander();
	void SetatkArmy();
	void SetdefArmy();
	void magicEffectInit();
public://update함수들
	void bsShowBattleCCupdate();
	void bsShowBattleCAupdate();
	void bsShowBattleACupdate();
	void bsShowBattleAAupdate();
	void bsShowMagicUpdate();

	void CC_fight();
	void CA_fight();
	void AC_fight();
	void AA_fight();
public://render함수들
	void atkCommanderRender();
	void defCommanderRender();
	void atkArmyRender();
	void defArmyRender();
	void magicRender();
public://기타 함수
	NumDigit GetNumDigit(int num);
};

