#pragma once
#include "../2DGameFramework/manager/singletonBase.h"
#include "PkwCharactorPlay.h"
#include "PkwMap.h"
#include "PkwDataBase.h"
#include "PkwCharactorPlay.h"


#define STAGE_MAX 2

struct stageDataFile{
	char stageMapImageKey[128];
	char stageFileRoute[128];
	char stageTalkFile[128];
};
enum STAGE_NUMBER{
	STAGE_01,
	STAGE_02
};

class PkwDataManager : public singletonBase<PkwDataManager>
{
public:
	PkwDataManager();
	~PkwDataManager();

	HRESULT init(void);
	void release(void);
public://기본 정보들
	PkwDataBase				_DataBase;
	PkwMap					_MapData;
public://스테이지 관련 데이터들 
	stageDataFile		_stageDataFile[STAGE_MAX];
	STAGE_NUMBER		_stageNumber;
	int					_stageNum;
	
	vector<POINT>		_vCommanderPos;
	vector<ItemData>	_vShopData;
public://pkwBattleMapScene에게 받는 변수
	BattleInfo			_battleInfo;
	PkwCharactor*		_choiceUnit;
	SkillData*			_choiceSkill;
	SkillData*			_choiceSummon;
	PkwCharactor*		_targetUnit;
public:// 플레이어가 가지고 있는 데이터
	vector<PkwCharactor> _vCoCommander;// 아군 지휘관들 정보 벡터
	PkwCharactorPlay	_CharactorPlay;//이건 아군은 기본정보 적군 지원군은 스테이지 데이터가 들어감
	vector<ItemData>	_vPlayerItem;
	int					_money;
public://화면에 관련된 변수
	HDC					_mainDc;
	RECT				_gameView;
	int					_nWindowWidth;
	int					_nWindowHeight;
public:
	void setStageFileRoute();
	void setStage();

	void setStageCommand(char* chCommand);
	void setStageCommanderValueAdd(CommanderData* commander, char* commanderValue, int cnt);

	void setStageMercenary(char* chMercenary);
	void setStageMercenaryValueAdd(PkwCharactor* mercenary, char* mercenaryValue, int cnt);

	void setStageCommanderPos(char* chCommanderPos);
	void setStageMercenaryValueAdd(POINT* pos, char* posValue, int cnt);

	void setStageShopItem(char* chShopItem);
	void setStageShopItemValueAdd(ItemData* item, char* itemValue, int cnt);

	PkwCharactorPlay* getCharactorPlay(){ return &_CharactorPlay; }

	void setBmSceneData(BattleInfo battleInfo, PkwCharactor* choiceUnit, SkillData* choiceSkill, SkillData* choiceSummon, PkwCharactor* targetUnit);
};

