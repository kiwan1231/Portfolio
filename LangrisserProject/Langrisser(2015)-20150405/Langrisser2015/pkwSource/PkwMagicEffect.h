#pragma once

#include"../common.h"

enum EXPLOSION_STATUS{
	EXPLOSION_WAIT,
	EXPLOSION_START,
	EXPLOSION_END
};

struct explosionObj{
	EXPLOSION_STATUS	status;
	POINTF				pos;
	float				waitTime;
	int					curFrameY;
	float				elaspedTime;
};

class PkwMagicEffect
{
protected:
	PkwCharactorPlay*				_pCharactorPlay;
	//넘겨 받는 값
	vector<PkwMagicEffect*>*		_vpEffect;
	PkwCharactor*					_attackUnit;
	PkwCharactor*					_targetUnit;
	POINT							_targetPos;
	SkillData						_skill;
	char							_imageKey[128];//이미지 키
	int								_frameWidth;//읽어들이는 한프레임 넓이
	int								_frameHeight;//읽어들이는 한프레임 높이
	int								_frameMax;// 총 플레이 하는 프레임 갯수
	float							_frameDelay;//프레임 딜레이(1 = 1초에 한장 출력)
	bool							_isAlpa;// 알파 할지 여부
	
	//넘겨받지 않고 자기가 진행하면서 저장하는 값
	bool							_isPlay;
	int								_curFrame;//현재 프레임
	float							_freameTime;
	float							_elapsedTime;

	vector<POINT>		_vMigScalePos;//마법의 범위 저장
	float				_alpaValue;
	explosionObj		_explisionObj[9];
	int					_explosionCnt;
public:
	virtual void init(vector<PkwMagicEffect*>* vpEffect, PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill, char* imageKey, int frameWidth, int frameHeight, int frameMax, float frameDelay, bool isAlpa);
	virtual void release();
	virtual void update();
	virtual void render(HDC memDC);
	virtual void magicEffect();
public:
	void magicScaleInit(POINT pos, int range);
	void magicScaleUpdate(float elaspedTime);
	void magicScaleRender(HDC memDC);

	void explosionInit();
	void explosionUpdate(float elaspedTime);
	void explisionRender(HDC memDC);

	bool getIsPlay();
	bool isMagicAction(POINT pos);
	
public:
	PkwMagicEffect();
	~PkwMagicEffect();
};

