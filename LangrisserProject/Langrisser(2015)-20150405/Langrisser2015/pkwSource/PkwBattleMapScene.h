#pragma once
#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "PkwActionProcess.h"
#include "PkwEffectPlay.h"
#include "PkwEventManager.h"


class PkwBattleMapScene : public gameNode
{
public:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter1;//배틀쇼씬으로 넘어갈때
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter2;//배틀씬을 종료하고 시나리오씬으로 넘어간다(gameclear)
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter3;//배틀씬을 종료하고 오프닝씬으로 넘어간다(gameover)

	PkwActionProcess		_actionProcess;
	PkwEffectPlay			_effectPlay;
	PkwEventManager			_eventManager;
private://데이터 매니져에게 받아오는 값을 저장하는 변수
	PkwCharactorPlay*		_pCharactorPlay;
	PkwMap*					_pMapData;
	PkwDataBase*			_pDataBase;
	int						_mapStage;
private://업데이트 관련 함수
	PkwCharactor*			_choiceUnit;
	SkillData*				_choiceSkill;
	SkillData*				_choiceSummon;
	PkwCharactor*			_targetUnit;
	POINT					_targetPos;
	float					_elapsed;
	int						_nChoice;
	int						_curPage;
	int						_maxPage;
public://렌더 관련 변수
	BattleInfo				_battleInfo;

	image*					_mapImage;
	image*					_backBuffer;
	HDC						_MemDC;
	RECT					_gameView;
	int						_nWindowWidth;
	int						_nWindowHeight;
	int						_MapMaxWidth;
	int						_MapManHeight;
	POINT					_MapStartPos;
	POINT					_bufPos;
	HFONT					_font[10];
private://기타 변수
	float					_moveDelay;
	float					_pkwDelay;
	imageFrameData			_mouseImageFrame;
public://get,set변수들
	
public:
	PkwBattleMapScene();
	~PkwBattleMapScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, CALLBACK_FUNCTION_PARAMETER cbFunction3, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);

	//init 관련 함수
	void unitAIset();
	PkwCharactor* commanderNameSearch(char* name);

	//업데이트 관련 함수
	void aiUpdate();
	bool aiFocus(PkwCharactor* playUnit);

	void charactorUpdate();
	void controlOffUpdate();
	void controlChoiceUpdate();
	void controlMoveUpdate();
	void controlMovingUpdate();
	void controlAttackUpdate();
	void controlMagicUpdate();
	void controlMagicChoiceUpdate();
	void controlMagicTargetChoiceUpdate();
	void controlMagicPosChoiceUpdate();
	void controlSummonUpdate();
	void controlSummonChoiceUpdate();
	//void controlSummonTargetChoiceUpdate();
	void controlOrderUpdate();

	//렌더 관련 함수
	void backImageDraw();
	void charactorRender();
	void InterfaceRender();
	void commandWindowRender();
	void unitInfoWindowRender();
	void armyCommandWindowRender();
	void magicWindowRender();
	void summonWindowRender();
	void downBarRender();
	//기타 함수
	void turnStarsetUnit(int type);
	NumDigit GetNumDigit(int num);
	void choiceUnitHeal();
	void CharactorAttack(POINT target);
	void BattleProcess(PkwCharactor* atkPlayer, PkwCharactor* defPlayer);
	void mouseImageFrameUpdate();
	//// 마법 초이스 관련 함수
	void targetAtkMagic(POINT pos);
	void targetDefMagic(POINT pos);
	void teleportPos(POINT pos);

	void soundPlay();
};

