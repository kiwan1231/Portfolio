#include "../stdafx.h"
#include "PkwBattleShowScene.h"

PkwBattleShowScene::PkwBattleShowScene()
{
}
PkwBattleShowScene::~PkwBattleShowScene()
{
}

HRESULT PkwBattleShowScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	setMode();
	setUnit();

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;
	_gameView.left = 0;
	_gameView.top = 0;
	_gameView.right = _nWindowWidth;
	_gameView.bottom = _nWindowHeight;

	//업데이트
	_elapsed = 0.0;
	_delayTime = 0.0;

	//백dc 생성
	_backBuffer = new image;
	_backBuffer->init(_nWindowWidth, _nWindowHeight);
	_MemDC = _backBuffer->getMemDC();

	//글자 배경모드(TRANSPARENT : 투명 QPAQUEL : 불투명)
	SetBkMode(_MemDC, TRANSPARENT);

	//글자 간격
	SetTextCharacterExtra(_MemDC, 5);

	// 폰트 set
	_font[0] = CreateFont(28, 0, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[1] = CreateFont(22, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[2] = CreateFont(16, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	
	return S_OK;
}
void PkwBattleShowScene::release(void)
{
	for (int i = 0; i < 10; i++){
		if (_font[i] != NULL)
			DeleteObject(_font[i]);
	}

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
	}
}
void PkwBattleShowScene::update(void)
{
	int cnt;
	int cnt1 = 0, cnt2 = 0;
	_elapsed = TIMEMANAGER->getElapsedTime();
	_delayTime += _elapsed;

	//bool isAtkCommanderEnd, isDefCommanderEnd;
	if (_mode == BS_SHOW_BATTLE_CC){
		bsShowBattleCCupdate();

		if ((_atkCommander.status == BS_COMMANDER_DEATH || _atkCommander.status == BS_COMMANDER_ACTION_END)
			&& (_defCommander.status == BS_COMMANDER_DEATH || _defCommander.status == BS_COMMANDER_ACTION_END)){
			
			_choiceUnit->_curHp = _atkCommander.curHp;
			_targetUnit->_curHp = _defCommander.curHp;

			_callbackFunctionParameter(_obj);
		}

	}
	else if (_mode == BS_SHOW_BATTLE_CA){
		bsShowBattleCAupdate();
		
		cnt = 0;
		for (int i = 0; i < _defArmy.size(); i++){
			if (_defArmy[i].status == BS_ARMY_DEATH || _defArmy[i].status == BS_ARMY_ACTION_END)
				cnt++;
		}
		if ((_atkCommander.status == BS_COMMANDER_DEATH || _atkCommander.status == BS_COMMANDER_ACTION_END) && cnt == _defArmy.size()){

			_choiceUnit->_curHp = _atkCommander.curHp;
			
			cnt = 0;
			for (int i = 0; i < _defArmy.size(); i++){
				if (_defArmy[i].status == BS_ARMY_ACTION_END)
					cnt++;
			}
			_targetUnit->_curHp = cnt;

			_callbackFunctionParameter(_obj);
		}
	}
	else if (_mode == BS_SHOW_BATTLE_AC){
		bsShowBattleACupdate();

		cnt = 0;
		for (int i = 0; i < _atkArmy.size(); i++){
			if (_atkArmy[i].status == BS_ARMY_DEATH || _atkArmy[i].status == BS_ARMY_ACTION_END)
				cnt++;
		}
		if ((_defCommander.status == BS_COMMANDER_DEATH || _defCommander.status == BS_COMMANDER_ACTION_END) && cnt == _atkArmy.size()){

			_targetUnit->_curHp = _defCommander.curHp;

			cnt = 0;
			for (int i = 0; i < _atkArmy.size(); i++){
				if (_atkArmy[i].status == BS_ARMY_ACTION_END)
					cnt++;
			}
			_choiceUnit->_curHp = cnt;

			_callbackFunctionParameter(_obj);
		}
	}
	else if (_mode == BS_SHOW_BATTLE_AA){
		bsShowBattleAAupdate();

		for (int i = 0; i < _atkArmy.size(); i++){
			if (_atkArmy[i].status == BS_ARMY_DEATH || _atkArmy[i].status == BS_ARMY_ACTION_END)
				cnt1++;
		}
		for (int i = 0; i < _defArmy.size(); i++){
			if (_defArmy[i].status == BS_ARMY_DEATH || _defArmy[i].status == BS_ARMY_ACTION_END)
				cnt2++;
		}
		if (cnt1 == _atkArmy.size() && cnt2 == _defArmy.size()){

			cnt1 = 0; cnt2 = 0;

			for (int i = 0; i < _atkArmy.size(); i++){
				if (_atkArmy[i].status == BS_ARMY_ACTION_END)
					cnt1++;
			}
			_choiceUnit->_curHp = cnt1;

			for (int i = 0; i < _defArmy.size(); i++){
				if (_defArmy[i].status == BS_ARMY_ACTION_END)
					cnt2++;
			}
			_targetUnit->_curHp = cnt2;

			_callbackFunctionParameter(_obj);
		}
	}
	else if (_mode == BS_SHOW_MAGIC_TARGET || _mode == BS_SHOW_MAGIC_UNTARGET){
		bsShowMagicUpdate();

		if (_atkCommander.status == BS_COMMANDER_ACTION_END){
			_callbackFunctionParameter(_obj);
		}
	}

	/*if (_delayTime >= 5.0){
		_callbackFunctionParameter(_obj);
	}*/
}
void PkwBattleShowScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	_backImage->render(_MemDC,0,0);

	if (_mode == BS_SHOW_BATTLE_CC){
		IMAGEMANAGER->findImage("bsUnitBar2")->render(_MemDC, 0, 272);

		atkCommanderRender();
		defCommanderRender();
	}
	else if (_mode == BS_SHOW_BATTLE_CA){
		IMAGEMANAGER->findImage("bsUnitBar2")->render(_MemDC, 0, 272);

		atkCommanderRender();
		defArmyRender();
	}
	else if (_mode == BS_SHOW_BATTLE_AC){
		IMAGEMANAGER->findImage("bsUnitBar2")->render(_MemDC, 0, 272);

		atkArmyRender();
		defCommanderRender();
	}
	else if (_mode == BS_SHOW_BATTLE_AA){
		IMAGEMANAGER->findImage("bsUnitBar2")->render(_MemDC, 0, 272);

		atkArmyRender();
		defArmyRender();
	}
	else if (_mode == BS_SHOW_MAGIC_TARGET){
		IMAGEMANAGER->findImage("bsUnitBar2")->render(_MemDC, 0, 272);

		atkCommanderRender();
		defCommanderRender();
		magicRender();
	}
	else if (_mode == BS_SHOW_MAGIC_UNTARGET){
		IMAGEMANAGER->findImage("bsUnitBar1")->render(_MemDC, 0, 272);

		atkCommanderRender();
		magicRender();
	}

	// 경과 시간 그리기 getHDC()
	sprintf_s(str, "아군 지휘관 거리: %f", _destance);
	TextOut(_MemDC, 0, 0, str, strlen(str));

	_backBuffer->render(DATAMANAGER->_mainDc, _gameView.left, _gameView.top, 0, 0, _gameView.right - _gameView.left, _gameView.bottom - _gameView.top);
}

//init함수 들
void PkwBattleShowScene::setMode()
{
	//데이타매니져로부터 값을 받아온다
	_battleInfo = DATAMANAGER->_battleInfo;
	_choiceUnit = DATAMANAGER->_choiceUnit;
	_choiceSkill = DATAMANAGER->_choiceSkill;
	_choiceSummon = DATAMANAGER->_choiceSummon;
	_targetUnit = DATAMANAGER->_targetUnit;
	
	//선택된 유닛의 맵 속성에 따라 배경 이미지 바뀜
	MapData* mapData = DATAMANAGER->_MapData.GetMap();
	int value = mapData->map[_choiceUnit->_pos.x + (_choiceUnit->_pos.y * mapData->width)];

	if (value == 10){
		_backImage = IMAGEMANAGER->findImage("bsBack10");
		_groundProperty = 10;
	}
	else if (value == 11){
		_backImage = IMAGEMANAGER->findImage("bsBack11");
		_groundProperty = 10;
	}
	else if (value == 12){
		_backImage = IMAGEMANAGER->findImage("bsBack12");
		_groundProperty = 10;
	}
	else if (value == 13){
		_backImage = IMAGEMANAGER->findImage("bsBack13");
		_groundProperty = 20;
	}
	else if (value == 15){
		_backImage = IMAGEMANAGER->findImage("bsBack15");
		_groundProperty = 15;
	}
	else if (value == 16){
		_backImage = IMAGEMANAGER->findImage("bsBack16");
		_groundProperty = 20;
	}
	else if (value == 17){
		_backImage = IMAGEMANAGER->findImage("bsBack17");
		_groundProperty = 40;
	}
	else if (value == 18){
		_backImage = IMAGEMANAGER->findImage("bsBack18");
		_groundProperty = 10;
	}
	else{
		_backImage = IMAGEMANAGER->findImage("bsBack10");
		_groundProperty = 10;
	}


	//모드를 초기화 해준다
	bool isAtkCommander, isDefCommander;

	if (_choiceUnit->_number < 1000)	isAtkCommander = true;
	else								isAtkCommander = false;

	if (_battleInfo.charactorControl == CO_CONTROL_ATTACK){

		if (_targetUnit->_number < 1000)	isDefCommander = true;
		else								isDefCommander = false;

		if ( isAtkCommander && isDefCommander )
			_mode = BS_SHOW_BATTLE_CC;
		else if ( !(isAtkCommander) && isDefCommander )
			_mode = BS_SHOW_BATTLE_AC;
		else if ( isAtkCommander && !(isDefCommander) )
			_mode = BS_SHOW_BATTLE_CA;
		else if ( !(isAtkCommander) && !(isDefCommander) )
			_mode = BS_SHOW_BATTLE_AA;
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_TARGET_CHOICE){

		if (_targetUnit->_number < 1000)	isDefCommander = true;
		else								isDefCommander = false;

		//마법타겟은 지휘관이 대표로 나오기 때문에 선택된 유닛이 지휘관이 아니면 지휘관으로 바꾸어 준다 
		if (!isDefCommander){
			_targetUnit = _targetUnit->_commander;
		}
		magicEffectInit();
		_mode = BS_SHOW_MAGIC_TARGET;
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_MAGIC_POS_CHOICE){
		magicEffectInit();
		_mode = BS_SHOW_MAGIC_UNTARGET;
	}
	else if (_battleInfo.charactorControl == CO_CONTROL_SUMMON_CHOICE){
		magicEffectInit();
		_mode = BS_SHOW_MAGIC_UNTARGET;
	}
	else{
		//뭔가 잘못된것 바로 콜백함수를 호출하자
		_callbackFunctionParameter(_obj);
	}
}
void PkwBattleShowScene::setUnit()
{
	if (_mode == BS_SHOW_BATTLE_CC){
		SetatkCommander();
		SetdefCommander();
	}
	else if (_mode == BS_SHOW_BATTLE_AC){
		SetatkArmy();
		SetdefCommander();
	}
	else if (_mode == BS_SHOW_BATTLE_CA){
		SetatkCommander();
		SetdefArmy();
	}
	else if (_mode == BS_SHOW_BATTLE_AA){
		SetatkArmy();
		SetdefArmy();
	}
	else if (_mode == BS_SHOW_MAGIC_TARGET){
		SetatkCommander();
		SetdefCommander();
	}
	else if (_mode == BS_SHOW_MAGIC_UNTARGET){
		SetatkCommander();
	}
}
void PkwBattleShowScene::SetatkCommander()
{
	_atkCommander.status = BS_COMMANDER_WAIT;
	_atkCommander.image = IMAGEMANAGER->findImage(_choiceUnit->_imageKey);
	_atkCommander.frameX = 0;
	_atkCommander.imageFrameY = 0;
	_atkCommander.type = _choiceUnit->_type;
	_atkCommander.atk = _choiceUnit->_atk + _choiceUnit->_atkItem._atk + _choiceUnit->_DefItem._atk;
	_atkCommander.def = _choiceUnit->_def + _choiceUnit->_atkItem._def + _choiceUnit->_DefItem._def;
	_atkCommander.curHp = _choiceUnit->_curHp;
	_atkCommander.startHp = _choiceUnit->_curHp;
	_atkCommander.angle = 0.0;
	_atkCommander.pos.x = 80.0;
	_atkCommander.pos.y = 176.0;
	_atkCommander.startPos.x = 80.0;
	_atkCommander.startPos.y = 176.0;
	_atkCommander.destPos.x = 624.0;
	_atkCommander.destPos.y = 176.0;
	_atkCommander.delayTime = 0.0;
	_atkCommander.isAlpa = false;
	_atkCommander.alpaTime = 0.0;
	_atkCommander.upSpeed = 5.0;

	if (_atkCommander.startHp == 10)
		_atkCommander.atkNumber = 10;
	else if (_atkCommander.startHp == 9 || _atkCommander.startHp == 8)
		_atkCommander.atkNumber = 9;
	else 
		_atkCommander.atkNumber = 8;
}
void PkwBattleShowScene::SetdefCommander()
{
	_defCommander.status = BS_COMMANDER_WAIT;
	_defCommander.image = IMAGEMANAGER->findImage(_targetUnit->_imageKey);
	_defCommander.frameX = 0;
	_defCommander.imageFrameY = 0;
	_defCommander.type = _targetUnit->_type;
	_defCommander.atk = _targetUnit->_atk + _targetUnit->_atkItem._atk + _targetUnit->_DefItem._atk;
	_defCommander.def = _targetUnit->_def + _targetUnit->_DefItem._def + _targetUnit->_DefItem._def;
	_defCommander.curHp = _targetUnit->_curHp;
	_defCommander.startHp = _targetUnit->_curHp;
	_defCommander.angle = 0.0;
	_defCommander.pos.x = 576.0;
	_defCommander.pos.y = 176.0;
	_defCommander.startPos.x = 576.0;
	_defCommander.startPos.y = 176.0;
	_defCommander.destPos.x = 16.0;
	_defCommander.destPos.y = 176.0;
	_defCommander.delayTime = 0.0;
	_defCommander.isAlpa = false;
	_defCommander.alpaTime = 0.0;
	_defCommander.upSpeed = 5.0;

	if (_defCommander.startHp == 10)
		_defCommander.atkNumber = 10;
	else if (_defCommander.startHp == 9 || _defCommander.startHp == 8)
		_defCommander.atkNumber = 9;
	else
		_defCommander.atkNumber = 8;
}
void PkwBattleShowScene::SetatkArmy()
{
	bsShowArmy temp;
	int distance;
	_atkArmy.clear();

	temp.status = BS_ARMY_WAIT;
	temp.image = IMAGEMANAGER->findImage(_choiceUnit->_imageKey);
	temp.frameX = 0;
	temp.isAttacked = false;
	temp.type = _choiceUnit->_type;
	temp.atk = _choiceUnit->_atk;
	temp.def = _choiceUnit->_def;

	distance = abs(_choiceUnit->_commander->_pos.x - _choiceUnit->_pos.x) + abs(_choiceUnit->_commander->_pos.y - _choiceUnit->_pos.y);
	if (_choiceUnit->_commander->_commandRange >= distance){
		temp.commanderAtk = _choiceUnit->_commander->_commandAtk;
		temp.commanderDef = _choiceUnit->_commander->_commandDef;
	}
	else{
		temp.commanderAtk = 0;
		temp.commanderDef = 0;
	}

	temp.angle = 0.0;
	temp.delayTime = 0.0;
	temp.alpaTime = 0.0;
	temp.isAlpa = false;
	temp.upSpeed = 5.0;

	for (int i = 0; i < _choiceUnit->_curHp; i++){
		if (i < 3){
			temp.startPos.x = 16.0 + (i * 16);
			temp.startPos.y = 144.0 + (i * 32);
			temp.destPos.x = 608.0;
			temp.destPos.y = 144.0 + (i * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;
		}
		else if(i < 7){
			temp.startPos.x = 64.0 + ( (i-3) * 16);
			temp.startPos.y = 128.0 + ( (i-3) * 32);
			temp.destPos.x = 608.0;
			temp.destPos.y = 128.0 + ( (i-3) * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;
		}
		else{
			temp.startPos.x = 128.0 + ( (i-7) * 16);
			temp.startPos.y = 144.0 + ( (i-7) * 32);
			temp.destPos.x = 608.0;
			temp.destPos.y = 144.0 + ( (i-7) * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;

		}
		_atkArmy.push_back(temp);
	}
}
void PkwBattleShowScene::SetdefArmy()
{
	bsShowArmy temp;
	int distance;
	
	_defArmy.clear();

	temp.status = BS_ARMY_WAIT;
	temp.image = IMAGEMANAGER->findImage(_targetUnit->_imageKey);
	temp.frameX = 0;
	temp.isAttacked = false;
	temp.type = _targetUnit->_type;
	temp.atk = _targetUnit->_atk;
	temp.def = _targetUnit->_def;

	distance = abs(_targetUnit->_commander->_pos.x - _targetUnit->_pos.x) + abs(_targetUnit->_commander->_pos.y - _targetUnit->_pos.y);
	if (_targetUnit->_commander->_commandRange >= distance){
		temp.commanderAtk = _targetUnit->_commander->_commandAtk;
		temp.commanderDef = _targetUnit->_commander->_commandDef;
	}
	else{
		temp.commanderAtk = 0;
		temp.commanderDef = 0;
	}

	temp.angle = 0.0;
	temp.delayTime = 0.0;
	temp.alpaTime = 0.0;
	temp.isAlpa = false;
	temp.upSpeed = 5.0;

	for (int i = 0; i < _targetUnit->_curHp; i++){

		if (i < 3){
			temp.startPos.x = 576.0 + (i * 16);
			temp.startPos.y = 144.0 + (i * 32);
			temp.destPos.x = 32;
			temp.destPos.y = 144.0 + (i * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;
		}
		else if (i < 7){
			temp.startPos.x = 496 + ( (i-3) * 16);
			temp.startPos.y = 128.0 + ( (i-3) * 32);
			temp.destPos.x = 32.0;
			temp.destPos.y = 128.0 + ( (i-3) * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;
		}
		else{
			temp.startPos.x = 448.0 + ( (i-7) * 16);
			temp.startPos.y = 144.0 + ( (i-7) * 32);
			temp.destPos.x = 32.0;
			temp.destPos.y = 144.0 + ( (i-7) * 32);
			temp.pos.x = temp.startPos.x;
			temp.pos.y = temp.startPos.y;

		}

		_defArmy.push_back(temp);
	}
}
void PkwBattleShowScene::magicEffectInit()
{
	_magicMode = MAGIC_CASTING;

	_magicEffect.alpa = 100.0;
	_magicEffect.isAction = true;
	_magicEffect.isShow = true;
	
	_magicEffect.pos.x = 96;
	_magicEffect.pos.y = 160;
	_magicEffect.destPos.x = 96;
	_magicEffect.destPos.y = 160;
}
//update 함수 들
void PkwBattleShowScene::bsShowBattleCCupdate()
{
	float distance;

	//----------------------공격 지휘관 업데이트
	if (_atkCommander.status == BS_COMMANDER_DEATH){
		//아무것도 안한다
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING){
		//캐릭터 이동 업데이트
		_atkCommander.pos.x -= _elapsed * 100;
		_atkCommander.pos.y -= _atkCommander.upSpeed;
		_atkCommander.upSpeed -= _elapsed*20;

		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 0.6){
			_atkCommander.status = BS_COMMANDER_DYING_LIE;
			_atkCommander.delayTime = 0.0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING_LIE){
		_atkCommander.alpaTime += _elapsed;
		if (_atkCommander.alpaTime > 0.05){
			_atkCommander.alpaTime = 0.0;

			if (_atkCommander.isAlpa == false)	_atkCommander.isAlpa = true;
			else								_atkCommander.isAlpa = false;
		}

		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 1.0){
			_atkCommander.status = BS_COMMANDER_DEATH;
			_atkCommander.delayTime = 0.0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_WAIT){
		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 0.9){
			_atkCommander.status = BS_COMMANDER_ATK;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_ATK){
		//애니메이션 업데이트
		_atkCommander.delayTime += _elapsed;
		if (_atkCommander.delayTime > 0.05){
			_atkCommander.delayTime = 0.0;
			_atkCommander.frameX++;
			if (_atkCommander.frameX > 1) _atkCommander.frameX = 0;
		}
		
		//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
		_atkCommander.pos.x += _elapsed * 200;
		distance = MY_UTIL::getDistance(_atkCommander.pos.x, _atkCommander.pos.y, _atkCommander.destPos.x, _atkCommander.destPos.y);
		if (distance < 10.0){
			_atkCommander.status = BS_COMMANDER_RETURN;
			_atkCommander.frameX = 0;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_RETURN){
		//애니메이션 업데이트
		_atkCommander.delayTime += _elapsed;
		if (_atkCommander.delayTime > 0.05){
			_atkCommander.delayTime = 0.0;
			_atkCommander.frameX++;
			if (_atkCommander.frameX > 1) _atkCommander.frameX = 0;
		}

		_atkCommander.pos.x -= _elapsed * 200;
		distance = MY_UTIL::getDistance(_atkCommander.pos.x, _atkCommander.pos.y, _atkCommander.startPos.x, _atkCommander.startPos.y);
		if (distance < 10.0){
			_atkCommander.status = BS_COMMANDER_ACTION_END;
			_atkCommander.frameX = 0;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_ACTION_END){

	}
	else{
		//여기 들어오면 안됨
	}

	//----------------------방어 지휘관 업데이트
	if (_defCommander.status == BS_COMMANDER_DEATH){
		//아무것도 안한다
	}
	else if (_defCommander.status == BS_COMMANDER_DYING){
		//캐릭터 이동 업데이트
		_defCommander.pos.x += _elapsed * 100;
		_defCommander.pos.y -= _defCommander.upSpeed;
		_defCommander.upSpeed -= _elapsed * 20;

		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 0.6){
			_defCommander.status = BS_COMMANDER_DYING_LIE;
			_defCommander.delayTime = 0.0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_DYING_LIE){

		_defCommander.alpaTime += _elapsed;
		if (_defCommander.alpaTime > 0.05){
			_defCommander.alpaTime = 0.0;

			if (_defCommander.isAlpa == false)	_defCommander.isAlpa = true;
			else								_defCommander.isAlpa = false;
		}

		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 1.0){
			_defCommander.status = BS_COMMANDER_DEATH;
			_defCommander.delayTime = 0.0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_WAIT){
		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 0.9){
			_defCommander.status = BS_COMMANDER_ATK;
			_defCommander.delayTime = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_ATK){
		
		//애니메이션 업데이트
		_defCommander.delayTime += _elapsed;
		if (_defCommander.delayTime > 0.05){
			_defCommander.frameX++;
			_defCommander.delayTime = 0.0;
			if (_defCommander.frameX > 1) _defCommander.frameX = 0;
		}

		//끝까지 갔으면 다시 돌아오는 상태로
		_defCommander.pos.x -= _elapsed * 200;
		distance = MY_UTIL::getDistance(_defCommander.pos.x, _defCommander.pos.y, _defCommander.destPos.x, _defCommander.destPos.y);
		if (distance < 10.0){
			_defCommander.status = BS_COMMANDER_RETURN;
			_defCommander.delayTime = 0;
			_defCommander.frameX = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_RETURN){
		//애니메이션 업데이트
		_defCommander.delayTime += _elapsed;
		if (_defCommander.delayTime > 0.05){
			_defCommander.frameX++;
			_defCommander.delayTime = 0.0;
			if (_defCommander.frameX > 1) _defCommander.frameX = 0;
		}

		_defCommander.pos.x += _elapsed * 200;

		distance = MY_UTIL::getDistance(_defCommander.pos.x, _defCommander.pos.y, _defCommander.startPos.x, _defCommander.startPos.y);
		if (distance < 10.0){
			_defCommander.status = BS_COMMANDER_ACTION_END;
			_defCommander.delayTime = 0;
			_defCommander.frameX = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_ACTION_END){

	}
	else{
		//여기 들어오면 안됨
	}

	//서로 만나면 싸우는 걸로
	if (_atkCommander.status == BS_COMMANDER_ATK || _defCommander.status == BS_COMMANDER_ATK)
		CC_fight();
}
void PkwBattleShowScene::bsShowBattleCAupdate()
{
	float distance;

	CA_fight();

	//----------------------공격 지휘관 업데이트
	if (_atkCommander.status == BS_COMMANDER_DEATH){
		//아무것도 안한다
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING){
		//캐릭터 이동 업데이트
		_atkCommander.pos.x -= _elapsed * 100;
		_atkCommander.pos.y -= _atkCommander.upSpeed;
		_atkCommander.upSpeed -= _elapsed * 20;

		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 0.6){
			_atkCommander.status = BS_COMMANDER_DYING_LIE;
			_atkCommander.delayTime = 0.0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING_LIE){
		_atkCommander.alpaTime += _elapsed;
		if (_atkCommander.alpaTime > 0.05){
			_atkCommander.alpaTime = 0.0;

			if (_atkCommander.isAlpa == false)	_atkCommander.isAlpa = true;
			else								_atkCommander.isAlpa = false;
		}

		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 1.0){
			_atkCommander.status = BS_COMMANDER_DEATH;
			_atkCommander.delayTime = 0.0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_WAIT){
		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 0.9){
			_atkCommander.status = BS_COMMANDER_ATK;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_ATK){
		//애니메이션 업데이트
		_atkCommander.delayTime += _elapsed;
		if (_atkCommander.delayTime > 0.05){
			_atkCommander.delayTime = 0.0;
			_atkCommander.frameX++;
			if (_atkCommander.frameX > 1) _atkCommander.frameX = 0;
		}

		//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
		_atkCommander.pos.x += _elapsed * 200;
		distance = MY_UTIL::getDistance(_atkCommander.pos.x, _atkCommander.pos.y, _atkCommander.destPos.x, _atkCommander.destPos.y);
		if (distance < 10.0){
			_atkCommander.status = BS_COMMANDER_RETURN;
			_atkCommander.frameX = 0;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_RETURN){
		//애니메이션 업데이트
		_atkCommander.delayTime += _elapsed;
		if (_atkCommander.delayTime > 0.05){
			_atkCommander.delayTime = 0.0;
			_atkCommander.frameX++;
			if (_atkCommander.frameX > 1) _atkCommander.frameX = 0;
		}

		_atkCommander.pos.x -= _elapsed * 200;
		distance = MY_UTIL::getDistance(_atkCommander.pos.x, _atkCommander.pos.y, _atkCommander.startPos.x, _atkCommander.startPos.y);
		if (distance < 10.0){
			_atkCommander.status = BS_COMMANDER_ACTION_END;
			_atkCommander.frameX = 0;
			_atkCommander.delayTime = 0;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_ACTION_END){

	}
	else{
		//여기 들어오면 안됨
	}

	//-----------------------------------------------------------방어 병력들 업데이트

	for (int i = 0; i < _defArmy.size(); i++){
		if (_defArmy[i].status == BS_ARMY_DEATH){
			//아무것도 안한다
		}
		else if (_defArmy[i].status == BS_ARMY_DYING){
			//캐릭터 이동 업데이트
			_defArmy[i].pos.x += _elapsed * 100;
			_defArmy[i].pos.y -= _defArmy[i].upSpeed;
			_defArmy[i].upSpeed -= _elapsed * 20;
			
			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 0.6){
				_defArmy[i].status = BS_ARMY_DYING_LIE;
				_defArmy[i].delayTime = 0.0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_DYING_LIE){
			_defArmy[i].alpaTime += _elapsed;
			if (_defArmy[i].alpaTime > 0.05){
				_defArmy[i].alpaTime = 0.0;

				if (_defArmy[i].isAlpa == false)	_defArmy[i].isAlpa = true;
				else								_defArmy[i].isAlpa = false;
			}

			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 1.0){
				_defArmy[i].status = BS_ARMY_DEATH;
				_defArmy[i].delayTime = 0.0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_WAIT){
			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 0.9){
				_defArmy[i].status = BS_ARMY_ATK_ON;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_ATK_ON || _defArmy[i].status == BS_ARMY_ATK_OFF){
			//애니메이션 업데이트
			_defArmy[i].delayTime += _elapsed;
			if (_defArmy[i].delayTime > 0.05){
				_defArmy[i].delayTime = 0.0;
				_defArmy[i].frameX++;
				if (_defArmy[i].frameX > 1) _defArmy[i].frameX = 0;
			}

			//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
			_defArmy[i].pos.x -= _elapsed * 200;
			distance = MY_UTIL::getDistance(_defArmy[i].pos.x, _defArmy[i].pos.y, _defArmy[i].destPos.x, _defArmy[i].destPos.y);
			if (distance < 10.0){
				_defArmy[i].status = BS_ARMY_RETURN;
				_defArmy[i].frameX = 0;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_RETURN){
			//애니메이션 업데이트
			_defArmy[i].delayTime += _elapsed;
			if (_defArmy[i].delayTime > 0.05){
				_defArmy[i].delayTime = 0.0;
				_defArmy[i].frameX++;
				if (_defArmy[i].frameX > 1) _defArmy[i].frameX = 0;
			}

			_defArmy[i].pos.x += _elapsed * 200;
			distance = MY_UTIL::getDistance(_defArmy[i].pos.x, _defArmy[i].pos.y, _defArmy[i].startPos.x, _defArmy[i].startPos.y);
			if (distance < 10.0){
				_defArmy[i].status = BS_ARMY_ACTION_END;
				_defArmy[i].frameX = 0;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_ACTION_END){

		}
	}
}
void PkwBattleShowScene::bsShowBattleACupdate()
{
	float distance;

	AC_fight();

	//----------------------방어 지휘관 업데이트
	if (_defCommander.status == BS_COMMANDER_DEATH){
		//아무것도 안한다
	}
	else if (_defCommander.status == BS_COMMANDER_DYING){
		//캐릭터 이동 업데이트
		_defCommander.pos.x += _elapsed * 100;
		_defCommander.pos.y -= _defCommander.upSpeed;
		_defCommander.upSpeed -= _elapsed * 20;

		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 0.6){
			_defCommander.status = BS_COMMANDER_DYING_LIE;
			_defCommander.delayTime = 0.0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_DYING_LIE){
		_defCommander.alpaTime += _elapsed;
		if (_defCommander.alpaTime > 0.05){
			_defCommander.alpaTime = 0.0;

			if (_defCommander.isAlpa == false)	_defCommander.isAlpa = true;
			else								_defCommander.isAlpa = false;
		}

		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 1.0){
			_defCommander.status = BS_COMMANDER_DEATH;
			_defCommander.delayTime = 0.0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_WAIT){
		_defCommander.delayTime += _elapsed;

		if (_defCommander.delayTime > 0.9){
			_defCommander.status = BS_COMMANDER_ATK;
			_defCommander.delayTime = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_ATK){
		//애니메이션 업데이트
		_defCommander.delayTime += _elapsed;
		if (_defCommander.delayTime > 0.05){
			_defCommander.delayTime = 0.0;
			_defCommander.frameX++;
			if (_defCommander.frameX > 1) _defCommander.frameX = 0;
		}

		//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
		_defCommander.pos.x -= _elapsed * 200;
		distance = MY_UTIL::getDistance(_defCommander.pos.x, _defCommander.pos.y, _defCommander.destPos.x, _defCommander.destPos.y);
		if (distance < 10.0){
			_defCommander.status = BS_COMMANDER_RETURN;
			_defCommander.frameX = 0;
			_defCommander.delayTime = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_RETURN){
		//애니메이션 업데이트
		_defCommander.delayTime += _elapsed;
		if (_defCommander.delayTime > 0.05){
			_defCommander.delayTime = 0.0;
			_defCommander.frameX++;
			if (_defCommander.frameX > 1) _defCommander.frameX = 0;
		}

		_defCommander.pos.x += _elapsed * 200;
		distance = MY_UTIL::getDistance(_defCommander.pos.x, _defCommander.pos.y, _defCommander.startPos.x, _defCommander.startPos.y);
		if (distance < 10.0){
			_defCommander.status = BS_COMMANDER_ACTION_END;
			_defCommander.frameX = 0;
			_defCommander.delayTime = 0;
		}
	}
	else if (_defCommander.status == BS_COMMANDER_ACTION_END){

	}
	else{
		//여기 들어오면 안됨
	}

	//-----------------------------------------------------------공격 병력들 업데이트

	for (int i = 0; i < _atkArmy.size(); i++){
		if (_atkArmy[i].status == BS_ARMY_DEATH){
			//아무것도 안한다
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING){
			//캐릭터 이동 업데이트
			_atkArmy[i].pos.x -= _elapsed * 100;
			_atkArmy[i].pos.y -= _atkArmy[i].upSpeed;
			_atkArmy[i].upSpeed -= _elapsed * 20;

			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 0.6){
				_atkArmy[i].status = BS_ARMY_DYING_LIE;
				_atkArmy[i].delayTime = 0.0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING_LIE){
			_atkArmy[i].alpaTime += _elapsed;
			if (_atkArmy[i].alpaTime > 0.05){
				_atkArmy[i].alpaTime = 0.0;

				if (_atkArmy[i].isAlpa == false)	_atkArmy[i].isAlpa = true;
				else								_atkArmy[i].isAlpa = false;
			}

			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 1.0){
				_atkArmy[i].status = BS_ARMY_DEATH;
				_atkArmy[i].delayTime = 0.0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_WAIT){
			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 0.9){
				_atkArmy[i].status = BS_ARMY_ATK_ON;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_ATK_ON || _atkArmy[i].status == BS_ARMY_ATK_OFF){
			//애니메이션 업데이트
			_atkArmy[i].delayTime += _elapsed;
			if (_atkArmy[i].delayTime > 0.05){
				_atkArmy[i].delayTime = 0.0;
				_atkArmy[i].frameX++;
				if (_atkArmy[i].frameX > 1) _atkArmy[i].frameX = 0;
			}

			//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
			_atkArmy[i].pos.x += _elapsed * 200;
			distance = MY_UTIL::getDistance(_atkArmy[i].pos.x, _atkArmy[i].pos.y, _atkArmy[i].destPos.x, _atkArmy[i].destPos.y);
			if (distance < 10.0){
				_atkArmy[i].status = BS_ARMY_RETURN;
				_atkArmy[i].frameX = 0;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_RETURN){
			//애니메이션 업데이트
			_atkArmy[i].delayTime += _elapsed;
			if (_atkArmy[i].delayTime > 0.05){
				_atkArmy[i].delayTime = 0.0;
				_atkArmy[i].frameX++;
				if (_atkArmy[i].frameX > 1) _atkArmy[i].frameX = 0;
			}

			_atkArmy[i].pos.x -= _elapsed * 200;
			distance = MY_UTIL::getDistance(_atkArmy[i].pos.x, _atkArmy[i].pos.y, _atkArmy[i].startPos.x, _atkArmy[i].startPos.y);
			if (distance < 10.0){
				_atkArmy[i].status = BS_ARMY_ACTION_END;
				_atkArmy[i].frameX = 0;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_ACTION_END){

		}
	}
}
void PkwBattleShowScene::bsShowBattleAAupdate()
{
	float distance;
	
	AA_fight();
	//-----------------------------------------------------------공격 병력들 업데이트

	for (int i = 0; i < _atkArmy.size(); i++){
		if (_atkArmy[i].status == BS_ARMY_DEATH){
			//아무것도 안한다
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING){
			//캐릭터 이동 업데이트
			_atkArmy[i].pos.x -= _elapsed * 100;
			_atkArmy[i].pos.y -= _atkArmy[i].upSpeed;
			_atkArmy[i].upSpeed -= _elapsed * 20;

			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 0.6){
				_atkArmy[i].status = BS_ARMY_DYING_LIE;
				_atkArmy[i].delayTime = 0.0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING_LIE){
			_atkArmy[i].alpaTime += _elapsed;
			if (_atkArmy[i].alpaTime > 0.05){
				_atkArmy[i].alpaTime = 0.0;

				if (_atkArmy[i].isAlpa == false)	_atkArmy[i].isAlpa = true;
				else								_atkArmy[i].isAlpa = false;
			}

			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 1.0){
				_atkArmy[i].status = BS_ARMY_DEATH;
				_atkArmy[i].delayTime = 0.0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_WAIT){
			_atkArmy[i].delayTime += _elapsed;

			if (_atkArmy[i].delayTime > 0.9){
				_atkArmy[i].status = BS_ARMY_ATK_ON;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_ATK_ON || _atkArmy[i].status == BS_ARMY_ATK_OFF){
			//애니메이션 업데이트
			_atkArmy[i].delayTime += _elapsed;
			if (_atkArmy[i].delayTime > 0.05){
				_atkArmy[i].delayTime = 0.0;
				_atkArmy[i].frameX++;
				if (_atkArmy[i].frameX > 1) _atkArmy[i].frameX = 0;
			}

			//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
			_atkArmy[i].pos.x += _elapsed * 200;
			distance = MY_UTIL::getDistance(_atkArmy[i].pos.x, _atkArmy[i].pos.y, _atkArmy[i].destPos.x, _atkArmy[i].destPos.y);
			if (distance < 10.0){
				_atkArmy[i].status = BS_ARMY_RETURN;
				_atkArmy[i].frameX = 0;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_RETURN){
			//애니메이션 업데이트
			_atkArmy[i].delayTime += _elapsed;
			if (_atkArmy[i].delayTime > 0.05){
				_atkArmy[i].delayTime = 0.0;
				_atkArmy[i].frameX++;
				if (_atkArmy[i].frameX > 1) _atkArmy[i].frameX = 0;
			}

			_atkArmy[i].pos.x -= _elapsed * 200;
			distance = MY_UTIL::getDistance(_atkArmy[i].pos.x, _atkArmy[i].pos.y, _atkArmy[i].startPos.x, _atkArmy[i].startPos.y);
			if (distance < 10.0){
				_atkArmy[i].status = BS_ARMY_ACTION_END;
				_atkArmy[i].frameX = 0;
				_atkArmy[i].delayTime = 0;
			}
		}
		else if (_atkArmy[i].status == BS_ARMY_ACTION_END){

		}
	}

	//-----------------------------------------------------------방어 병력들 업데이트

	for (int i = 0; i < _defArmy.size(); i++){
		if (_defArmy[i].status == BS_ARMY_DEATH){
			//아무것도 안한다
		}
		else if (_defArmy[i].status == BS_ARMY_DYING){
			//캐릭터 이동 업데이트
			_defArmy[i].pos.x += _elapsed * 100;
			_defArmy[i].pos.y -= _defArmy[i].upSpeed;
			_defArmy[i].upSpeed -= _elapsed * 20;

			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 0.6){
				_defArmy[i].status = BS_ARMY_DYING_LIE;
				_defArmy[i].delayTime = 0.0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_DYING_LIE){
			_defArmy[i].alpaTime += _elapsed;
			if (_defArmy[i].alpaTime > 0.05){
				_defArmy[i].alpaTime = 0.0;

				if (_defArmy[i].isAlpa == false)	_defArmy[i].isAlpa = true;
				else								_defArmy[i].isAlpa = false;
			}

			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 1.0){
				_defArmy[i].status = BS_ARMY_DEATH;
				_defArmy[i].delayTime = 0.0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_WAIT){
			_defArmy[i].delayTime += _elapsed;

			if (_defArmy[i].delayTime > 0.9){
				_defArmy[i].status = BS_ARMY_ATK_ON;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_ATK_ON || _defArmy[i].status == BS_ARMY_ATK_OFF){
			//애니메이션 업데이트
			_defArmy[i].delayTime += _elapsed;
			if (_defArmy[i].delayTime > 0.05){
				_defArmy[i].delayTime = 0.0;
				_defArmy[i].frameX++;
				if (_defArmy[i].frameX > 1) _defArmy[i].frameX = 0;
			}

			//끝까지 같으면 다시 돌아오는 상태로 바꿔준다
			_defArmy[i].pos.x -= _elapsed * 200;
			distance = MY_UTIL::getDistance(_defArmy[i].pos.x, _defArmy[i].pos.y, _defArmy[i].destPos.x, _defArmy[i].destPos.y);
			if (distance < 10.0){
				_defArmy[i].status = BS_ARMY_RETURN;
				_defArmy[i].frameX = 0;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_RETURN){
			//애니메이션 업데이트
			_defArmy[i].delayTime += _elapsed;
			if (_defArmy[i].delayTime > 0.05){
				_defArmy[i].delayTime = 0.0;
				_defArmy[i].frameX++;
				if (_defArmy[i].frameX > 1) _defArmy[i].frameX = 0;
			}

			_defArmy[i].pos.x += _elapsed * 200;
			distance = MY_UTIL::getDistance(_defArmy[i].pos.x, _defArmy[i].pos.y, _defArmy[i].startPos.x, _defArmy[i].startPos.y);
			if (distance < 10.0){
				_defArmy[i].status = BS_ARMY_ACTION_END;
				_defArmy[i].frameX = 0;
				_defArmy[i].delayTime = 0;
			}
		}
		else if (_defArmy[i].status == BS_ARMY_ACTION_END){

		}
	}
}
void PkwBattleShowScene::bsShowMagicUpdate()
{
	if (_atkCommander.status == BS_COMMANDER_WAIT){
		_atkCommander.delayTime += _elapsed;

		if (_atkCommander.delayTime > 0.5){
			_atkCommander.status = BS_COMMANDER_MAGIC;
			_atkCommander.delayTime = 0;
			_magicEffect.isShow = true;
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_MAGIC){
		
		_atkCommander.delayTime += _elapsed;

		if (_magicMode == MAGIC_CASTING){
			_magicEffect.alpa += _elapsed*10;

			if (_atkCommander.delayTime > 0.8){
				_atkCommander.delayTime = 0;
				_atkCommander.frameX += 1;
				_magicMode = MAGIC_START;
			}
		}
		else if (_magicMode == MAGIC_START){
			if (_atkCommander.delayTime > 0.8){
				_atkCommander.delayTime = 0;
				_atkCommander.status = BS_COMMANDER_ACTION_END;
			}
		}
	}
	else if (_atkCommander.status == BS_COMMANDER_ACTION_END){

	}

}

void PkwBattleShowScene::CC_fight()
{
	int probability,random;//공격 확률 계산
	float distance;

	//거리가 가까우면 공격하고 아니면 공격 하지 않기 때문에 함수 종료한다
	distance = abs(_atkCommander.pos.x - _defCommander.pos.x) + abs(_atkCommander.pos.y - _defCommander.pos.y);
	if (distance > 64) return;

	//공격 지휘관 공격
	if (_atkCommander.atkNumber > 0 || _atkCommander.status == BS_COMMANDER_ATK){
		int addValue = 0;
		if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
			if (strcmp(_choiceUnit->_name, "파이크") == 0){
				addValue = 5;
			}
			else
				addValue = -5;
		}
		else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
			if (strcmp(_targetUnit->_name, "파이크") == 0){
				addValue = -5;
			}
			else
				addValue = 5;
		}
		//공격 성공 확률을 정한다
		probability = (_atkCommander.atk - _defCommander.def + addValue);

		probability *= 10;

		if (probability <= 0)			probability = 5;
		else if (probability >= 100)	probability = 95;

		//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
		random = RND->getFromIntTo(1,100); // 1~100안에 숫자를 랜덤하게 뽑는다
		
		if (probability > random){//공격 성공
			_defCommander.curHp--;
			if (_defCommander.curHp < 0) _defCommander.curHp = 0;
		}

		_atkCommander.atkNumber--;
	}

	//방어 지휘관 공격
	if (_defCommander.atkNumber > 0 || _defCommander.status == BS_COMMANDER_ATK){
		int addValue = 0;
		if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
			if (strcmp(_choiceUnit->_name, "파이크") == 0){
				addValue = -5;
			}
			else
				addValue = 5;
		}
		else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
			if (strcmp(_targetUnit->_name, "파이크") == 0){
				addValue = 5;
			}
			else
				addValue = -5;
		}
		//공격 성공 확률을 정한다
		probability = (_defCommander.atk - _atkCommander.def + addValue);

		probability *= 10;

		if (probability <= 0)			probability = 5;
		else if (probability >= 100)	probability = 95;

		//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
		random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

		if (probability > random){//공격 성공
			_atkCommander.curHp--;
			if (_atkCommander.curHp < 0) _atkCommander.curHp = 0;
		}

		_defCommander.atkNumber--;
	}

	if (_atkCommander.status == BS_COMMANDER_ATK && _atkCommander.curHp <= 0)
		_atkCommander.status = BS_COMMANDER_DYING;

	if (_defCommander.status == BS_COMMANDER_ATK && _defCommander.curHp <= 0)
		_defCommander.status = BS_COMMANDER_DYING;
}
void PkwBattleShowScene::CA_fight()
{
	int addValue = 0;
	int probability, random;//공격 확률 계산
	float distance;

	//공격 지휘관 처리
	if (_atkCommander.status == BS_COMMANDER_ATK){
		for (int i = 0; i < _defArmy.size(); i++){
			if (_defArmy[i].status != BS_ARMY_ATK_ON && _defArmy[i].status != BS_ARMY_ATK_OFF) continue;

			//거리가 가까우면 서로 공격 주고 받지 않는다
			distance = abs(_atkCommander.pos.x - _defArmy[i].pos.x) + abs(_atkCommander.pos.y - _defArmy[i].pos.y);
			if (distance > 64) continue;

			if (!_defArmy[i].isAttacked){
				_defArmy[i].isAttacked = true;

				if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
					if (strcmp(_choiceUnit->_name, "파이크") == 0){
						addValue = -5;
					}
					else
						addValue = 5;
				}
				else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
					if (strcmp(_targetUnit->_name, "파이크") == 0){
						addValue = 5;
					}
					else
						addValue = -5;
				}

				// ->방어 유닛이 공격 지휘관을 공격
				probability = (_defArmy[i].atk + _defArmy[i].commanderAtk - _atkCommander.def + addValue);
				addValue = 0;
				probability *= 10;
				if (probability <= 0)			probability = 5;
				else if (probability >= 100)	probability = 95;

				//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
				random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

				if (probability > random){//공격 성공
					_atkCommander.curHp--;
				}

				if (_atkCommander.atkNumber > 0){
					_atkCommander.atkNumber--;

					
					if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
						if (strcmp(_choiceUnit->_name, "파이크") == 0){
							addValue = 5;
						}
						else
							addValue = -5;
					}
					else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
						if (strcmp(_targetUnit->_name, "파이크") == 0){
							addValue = -5;
						}
						else
							addValue = 5;
					}

					// ->공격 지휘관이 방어 유닛을 공격
					probability = (_atkCommander.atk - _defArmy[i].def - _defArmy[i].commanderDef + addValue);
					addValue = 0;
					probability *= 10;
					if (probability <= 0)			probability = 5;
					else if (probability >= 100)	probability = 95;

					//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
					random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

					if (probability > random){//공격 성공
						_defArmy[i].status = BS_ARMY_DYING;
					}
				}
			}
			//공격 지휘관 hp상태 확인
			if (_atkCommander.curHp <= 0){
				_atkCommander.curHp = 0;
				_atkCommander.status = BS_COMMANDER_DYING;
				break;
			}
		}
	}
}
void PkwBattleShowScene::AC_fight()
{
	int addValue = 0;
	int probability, random;//공격 확률 계산
	float distance;

	//공격 지휘관 처리
	if (_defCommander.status == BS_COMMANDER_ATK && _defCommander.atkNumber > 0){
		for (int i = 0; i < _atkArmy.size(); i++){
			if (_atkArmy[i].status != BS_ARMY_ATK_ON && _atkArmy[i].status != BS_ARMY_ATK_OFF) continue;

			//거리가 가까우면 서로 공격 주고 받지 않는다
			distance = abs(_defCommander.pos.x - _atkArmy[i].pos.x) + abs(_defCommander.pos.y - _atkArmy[i].pos.y);
			if (distance > 64) continue;

			if (!_atkArmy[i].isAttacked){
				_atkArmy[i].isAttacked = true;

				if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
					if (strcmp(_choiceUnit->_name, "파이크") == 0){
						addValue = 5;
					}
					else
						addValue = -5;
				}
				else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
					if (strcmp(_targetUnit->_name, "파이크") == 0){
						addValue = -5;
					}
					else
						addValue = 5;
				}

				// ->방어 유닛이 공격 지휘관을 공격
				probability = (_atkArmy[i].atk + _atkArmy[i].commanderAtk - _defCommander.def + addValue);
				addValue = 0;
				probability *= 10;
				if (probability <= 0)			probability = 5;
				else if (probability >= 100)	probability = 95;

				//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
				random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

				if (probability > random){//공격 성공
					_defCommander.curHp--;
				}

				if (_defCommander.atkNumber > 0){
					_defCommander.atkNumber--;
					// ->공격 지휘관이 방어 유닛을 공격
					if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
						if (strcmp(_choiceUnit->_name, "파이크") == 0){
							addValue = -5;
						}
						else
							addValue = 5;
					}
					else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
						if (strcmp(_targetUnit->_name, "파이크") == 0){
							addValue = 5;
						}
						else
							addValue = -5;
					}

					probability = (_defCommander.atk - _atkArmy[i].def - _atkArmy[i].commanderDef + addValue);
					addValue = 0;
					probability *= 10;
					if (probability <= 0)			probability = 5;
					else if (probability >= 100)	probability = 95;

					//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
					random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

					if (probability > random){//공격 성공
						_atkArmy[i].status = BS_ARMY_DYING;
					}
				}
			}
			//공격 지휘관 hp상태 확인
			if (_defCommander.curHp <= 0){
				_defCommander.curHp = 0;
				_defCommander.status = BS_COMMANDER_DYING;
				break;
			}
		}
	}
}
void PkwBattleShowScene::AA_fight()
{
	int addValue = 0;
	int probability, random;//공격 확률 계산
	float distance;

	//공격 병력  처리
	
	for (int i = 0; i < _atkArmy.size(); i++){
		if (_atkArmy[i].status != BS_ARMY_ATK_ON) continue;

		for (int j = 0; j< _defArmy.size(); j++){
			//공격 당한 유닛이면 공격 안한다
			if (_defArmy[j].isAttacked) continue;

			//거리가 멀면 서로 공격 주고 받지 않는다
			distance = abs(_atkArmy[i].pos.x - _defArmy[j].pos.x) + abs(_atkArmy[i].pos.y - _defArmy[j].pos.y);
			if (distance > 64) continue;

			_atkArmy[i].status = BS_ARMY_ATK_OFF;
			_atkArmy[i].isAttacked = true;
			_defArmy[j].status = BS_ARMY_ATK_OFF;
			_defArmy[j].isAttacked = true;
			
			if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
				if (strcmp(_choiceUnit->_name, "파이크") == 0){
					addValue = 5;
				}
				else
					addValue = -5;
			}
			else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
				if (strcmp(_targetUnit->_name, "파이크") == 0){
					addValue = -5;
				}
				else
					addValue = 5;
			}

			// ->공격 유닛이 방어 유닛을 공격
			probability = (_atkArmy[i].atk + _atkArmy[i].commanderAtk - _defArmy[j].def - _defArmy[j].commanderDef + addValue);
			addValue = 0;
			probability *= 10;
			if (probability <= 0)			probability = 5;
			else if (probability >= 100)	probability = 95;

			//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
			random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

			if (probability > random){//공격 성공
				_defArmy[j].status = BS_ARMY_DYING;
			}

			if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
				if (strcmp(_choiceUnit->_name, "파이크") == 0){
					addValue = -5;
				}
				else
					addValue = 5;
			}
			else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
				if (strcmp(_targetUnit->_name, "파이크") == 0){
					addValue = 5;
				}
				else
					addValue = -5;
			}

			// ->방어 유닛이 공격 유닛을 공격
			probability = (_defArmy[j].atk + _defArmy[j].commanderAtk - _atkArmy[i].def - _atkArmy[i].commanderDef + addValue);
			addValue = 0;
			probability *= 10;
			if (probability <= 0)			probability = 5;
			else if (probability >= 100)	probability = 95;

			//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
			random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

			if (probability > random){//공격 성공
				_atkArmy[i].status = BS_ARMY_DYING;
			}
			break;
		}
	}
	
	//방어 벙력 처리
	for (int i = 0; i < _defArmy.size(); i++){
		if (_defArmy[i].status != BS_ARMY_ATK_ON) continue;

		for (int j = 0; j< _atkArmy.size(); j++){
			//공격 당한 유닛이면 공격 안한다
			if (_atkArmy[j].isAttacked) continue;

			//거리가 멀면 서로 공격 주고 받지 않는다
			distance = abs(_defArmy[i].pos.x - _atkArmy[j].pos.x) + abs(_defArmy[i].pos.y - _atkArmy[j].pos.y);
			if (distance > 64) continue;

			_defArmy[i].status = BS_ARMY_ATK_OFF;
			_defArmy[i].isAttacked = true;
			_atkArmy[j].status = BS_ARMY_ATK_OFF;
			_atkArmy[j].isAttacked = true;

			// ->공격 유닛이 방어 유닛을 공격
			if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
				if (strcmp(_choiceUnit->_name, "파이크") == 0){
					addValue = -5;
				}
				else
					addValue = 5;
			}
			else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
				if (strcmp(_targetUnit->_name, "파이크") == 0){
					addValue = 5;
				}
				else
					addValue = -5;
			}

			probability = (_defArmy[i].atk + _defArmy[i].commanderAtk - _atkArmy[j].def - _atkArmy[j].commanderDef + addValue);
			addValue = 0;
			probability *= 10;
			if (probability <= 0)			probability = 5;
			else if (probability >= 100)	probability = 95;

			//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
			random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

			if (probability > random){//공격 성공
				_atkArmy[j].status = BS_ARMY_DYING;
			}

			// ->방어 유닛이 공격 유닛을 공격
			if (_choiceUnit->_type == 0 && _targetUnit->_type == 1){
				if (strcmp(_choiceUnit->_name, "파이크") == 0){
					addValue = 5;
				}
				else
					addValue = -5;
			}
			else if (_choiceUnit->_type == 1 && _targetUnit->_type == 0){
				if (strcmp(_targetUnit->_name, "파이크") == 0){
					addValue = -5;
				}
				else
					addValue = 5;
			}

			probability = (_atkArmy[j].atk + _atkArmy[j].commanderAtk - _defArmy[i].def - _defArmy[i].commanderDef + addValue);
			addValue = 0;
			probability *= 10;
			if (probability <= 0)			probability = 5;
			else if (probability >= 100)	probability = 95;

			//랜덤으로 뽑은 값이 공격 성공 확률 값보다 작으면 공격 성공
			random = RND->getFromIntTo(1, 100); // 1~100안에 숫자를 랜덤하게 뽑는다

			if (probability > random){//공격 성공
				_defArmy[i].status = BS_ARMY_DYING;
			}
			break;
		}
	}
}

//render 함수 들
void PkwBattleShowScene::atkCommanderRender()
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;

	//퍼센티지 저장(경험치바 그릴때 씀)
	float per;
	float width;

	//캐릭터 얼굴 출력
	if (_choiceUnit->_number<1000)
		IMAGEMANAGER->findImage(_choiceUnit->_faceImageKey)->render(_MemDC, 32, 320);
	else
		IMAGEMANAGER->findImage(_choiceUnit->_commander->_faceImageKey)->render(_MemDC, 32, 320);

	//경험치 바 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 144, 288, 0, 144, 144, 16);

	if (_choiceUnit->_number < 1000)
		per = (float)_choiceUnit->_curExp / (float)_choiceUnit->_maxExp;
	else
		per = (float)_choiceUnit->_commander->_curExp / (float)_choiceUnit->_commander->_maxExp;

	//per = (float)_choiceUnit->_curExp / (float)_choiceUnit->_maxExp;
	width = 112.0 * per;
	if (width >= 112.0) width = 112.0;
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 160, 288, 0, 160, (int)width, 16);

	//이름 출력
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	TextOut(_MemDC, 144, 320, _choiceUnit->_name, strlen(_choiceUnit->_name));
	
	//hp 출력
	IMAGEMANAGER->findImage("bsNumImage")->render(_MemDC, 160, 352, _atkCommander.curHp * 48, 0, 48, 48);

	//공격력 출력
	numDigit = GetNumDigit(_atkCommander.atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 384, 0, 16, 16, 16);
	//지휘관 공격 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//방어력 출력
	numDigit = GetNumDigit(_atkCommander.def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 400, 0, 16, 16, 16);
	//지휘관 방어 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//지형효과 출력
	numDigit = GetNumDigit(_groundProperty);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 416, 0, 16, 16, 16);
	//유닛 속성에 대한 지형효과 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//--------------캐릭터를 그려준다
	int frameWidth,frameHeight;

	frameWidth = _atkCommander.image->getFrameWidth();
	frameHeight = _atkCommander.image->getFrameHeight();

	if (_atkCommander.status == BS_COMMANDER_DEATH){
		//안그린다
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, 4 * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_atkCommander.status == BS_COMMANDER_DYING_LIE){
		if (_atkCommander.isAlpa)
			_atkCommander.image->alphaRender(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, 4 * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight, 150.0);
	}
	else if (_atkCommander.status == BS_COMMANDER_WAIT){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, _atkCommander.frameX * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_atkCommander.status == BS_COMMANDER_MAGIC){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, _atkCommander.frameX * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_atkCommander.status == BS_COMMANDER_ATK){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, _atkCommander.frameX * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_atkCommander.status == BS_COMMANDER_RETURN){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, (_atkCommander.frameX + 2) * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_atkCommander.status == BS_COMMANDER_ACTION_END){
		_atkCommander.image->render(_MemDC, _atkCommander.pos.x - frameWidth / 2, _atkCommander.pos.y - frameHeight / 2
			, _atkCommander.frameX * frameWidth, _atkCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
}
void PkwBattleShowScene::defCommanderRender()
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;

	//퍼센티지 저장(경험치바 그릴때 씀)
	float per;
	float width;

	//캐릭터 얼굴 출력
	IMAGEMANAGER->findImage(_targetUnit->_faceImageKey)->render(_MemDC, 528, 320);

	//경험치 바 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 352, 288, 0, 144, 144, 16);
	per = (float)_targetUnit->_curExp / (float)_targetUnit->_maxExp;
	width = 112.0 * per;
	if (width >= 112.0) width = 112.0;
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368, 288, 0, 160, (int)width, 16);

	//이름 출력
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	TextOut(_MemDC, 380, 320, _targetUnit->_name, strlen(_targetUnit->_name));

	//hp 출력
	IMAGEMANAGER->findImage("bsNumImage")->render(_MemDC, 432, 352, _defCommander.curHp * 48, 0, 48, 48);

	//공격력 출력
	numDigit = GetNumDigit(_defCommander.atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 384, 0, 16, 16, 16);
	//지휘관 공격 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//방어력 출력
	numDigit = GetNumDigit(_defCommander.def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 400, 0, 16, 16, 16);
	//지휘관 방어 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//지형효과 출력
	numDigit = GetNumDigit(_groundProperty);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 416, 0, 16, 16, 16);
	//유닛 속성에 대한 지형효과 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//이 모드 일때는 캐릭터는 안 그린다
	if (_mode == BS_SHOW_MAGIC_TARGET) return;

	//--------------캐릭터를 그려준다
	int frameWidth, frameHeight;

	frameWidth = _defCommander.image->getFrameWidth();
	frameHeight = _defCommander.image->getFrameHeight();

	if (_defCommander.status == BS_COMMANDER_DEATH){
		//안그린다
	}
	else if (_defCommander.status == BS_COMMANDER_DYING){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, 5 * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_defCommander.status == BS_COMMANDER_DYING_LIE){
		if (_defCommander.isAlpa)
			_defCommander.image->alphaRender(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, 5 * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight, 150.0);
	}
	else if (_defCommander.status == BS_COMMANDER_WAIT){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, 3 * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_defCommander.status == BS_COMMANDER_MAGIC){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, 3 * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_defCommander.status == BS_COMMANDER_ATK){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, (_defCommander.frameX+2) * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_defCommander.status == BS_COMMANDER_RETURN){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, _defCommander.frameX * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
	else if (_defCommander.status == BS_COMMANDER_ACTION_END){
		_defCommander.image->render(_MemDC, _defCommander.pos.x - frameWidth / 2, _defCommander.pos.y - frameHeight / 2
			, 3 * frameWidth, _defCommander.imageFrameY * frameHeight * 2, frameWidth, frameHeight);
	}
}
void PkwBattleShowScene::atkArmyRender()
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	int num;
	//퍼센티지 저장(경험치바 그릴때 씀)
	float per;
	float width;

	//캐릭터 얼굴 출력
	IMAGEMANAGER->findImage(_choiceUnit->_commander->_faceImageKey)->render(_MemDC, 32, 320);

	//경험치 바 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 144, 288, 0, 144, 144, 16);
	per = (float)_choiceUnit->_commander->_curExp / (float)_choiceUnit->_commander->_maxExp;
	width = 112.0 * per;
	if (width >= 112.0) width = 112.0;
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 160, 288, 0, 160, (int)width, 16);

	//이름 출력
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	TextOut(_MemDC, 144, 320, _choiceUnit->_name, strlen(_choiceUnit->_name));

	//hp 출력
	num = 0;
	for (int i = 0; i < _atkArmy.size(); i++){
		if (_atkArmy[i].status != BS_ARMY_DEATH && _atkArmy[i].status != BS_COMMANDER_DYING && _atkArmy[i].status != BS_COMMANDER_DYING_LIE) num++;
	}
	IMAGEMANAGER->findImage("bsNumImage")->render(_MemDC, 160, 352, num * 48, 0, 48, 48);

	//공격력 출력
	numDigit = GetNumDigit(_atkArmy[0].atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 384, 0, 16, 16, 16);
	//지휘관 공격 수정치 출력
	numDigit = GetNumDigit(_atkArmy[0].commanderAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//방어력 출력
	numDigit = GetNumDigit(_atkArmy[0].def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 400, 0, 16, 16, 16);
	//지휘관 방어 수정치 출력
	numDigit = GetNumDigit(_atkArmy[0].commanderDef);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//지형효과 출력
	numDigit = GetNumDigit(_groundProperty);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 224 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 240, 416, 0, 16, 16, 16);
	//유닛 속성에 대한 지형효과 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 272 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//---------------------------------공격 병력 출력
	int frameWidth, frameHeight;

	frameWidth = _atkArmy[0].image->getFrameWidth();
	frameHeight = _atkArmy[0].image->getFrameHeight();

	for (int i = 0; i < _atkArmy.size(); i++){

		if (_atkArmy[i].status == BS_ARMY_DEATH){
			//출력 안한다.. 일단
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, 4 * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_atkArmy[i].status == BS_ARMY_DYING_LIE){
			if (_atkArmy[i].isAlpa)
				_atkArmy[i].image->alphaRender(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, 4 * frameWidth, 0, frameWidth, frameHeight, 150.0);
		}
		else if (_atkArmy[i].status == BS_ARMY_WAIT){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, 0, 0, frameWidth, frameHeight);
		}
		else if (_atkArmy[i].status == BS_ARMY_ATK_ON){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, _atkArmy[i].frameX * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_atkArmy[i].status == BS_ARMY_ATK_OFF){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, _atkArmy[i].frameX * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_atkArmy[i].status == BS_ARMY_RETURN){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, (_atkArmy[i].frameX+2) * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_atkArmy[i].status == BS_ARMY_ACTION_END){
			_atkArmy[i].image->render(_MemDC, _atkArmy[i].pos.x - frameWidth / 2, _atkArmy[i].pos.y - frameHeight / 2
				, 0, 0, frameWidth, frameHeight);
		}

	}

}
void PkwBattleShowScene::defArmyRender()
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	int num;
	//퍼센티지 저장(경험치바 그릴때 씀)
	float per;
	float width;
	
	//캐릭터 얼굴 출력
	IMAGEMANAGER->findImage(_targetUnit->_commander->_faceImageKey)->render(_MemDC, 528, 320);

	//경험치 바 그리기
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 352, 288, 0, 144, 144, 16);
	per = (float)_targetUnit->_commander->_curExp / (float)_targetUnit->_commander->_maxExp;
	width = 112.0 * per;
	if (width >= 112.0) width = 112.0;
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368, 288, 0, 160, (int)width, 16);

	//이름 출력
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	TextOut(_MemDC, 380, 320, _targetUnit->_name, strlen(_targetUnit->_name));

	//hp 출력
	num = 0;
	for (int i = 0; i < _defArmy.size(); i++){
		if (_defArmy[i].status != BS_ARMY_DEATH && _defArmy[i].status != BS_COMMANDER_DYING && _defArmy[i].status != BS_COMMANDER_DYING_LIE) num++;
	}
	IMAGEMANAGER->findImage("bsNumImage")->render(_MemDC, 432, 352, num * 48, 0, 48, 48);

	//공격력 출력
	numDigit = GetNumDigit(_defArmy[0].atk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 384, 0, 16, 16, 16);
	//지휘관 공격 수정치 출력
	numDigit = GetNumDigit(_defArmy[0].commanderAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 384, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//방어력 출력
	numDigit = GetNumDigit(_defArmy[0].def);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 400, 0, 16, 16, 16);
	//지휘관 방어 수정치 출력
	numDigit = GetNumDigit(_defArmy[0].commanderDef);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 400, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//지형효과 출력
	numDigit = GetNumDigit(_groundProperty);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 368 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}
	//'+'출력
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 384, 416, 0, 16, 16, 16);
	//유닛 속성에 대한 지형효과 수정치 출력
	numDigit = GetNumDigit(0);
	for (int i = 0; i < numDigit.strlen; i++){
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, 416 - (i * 16), 416, 32 + 16 * numDigit.value[i], 16, 16, 16);
	}

	//---------------------------------방어 병력 출력
	int frameWidth, frameHeight;

	frameWidth = _defArmy[0].image->getFrameWidth();
	frameHeight = _defArmy[0].image->getFrameHeight();

	for (int i = 0; i < _defArmy.size(); i++){

		if (_defArmy[i].status == BS_ARMY_DEATH){
			//출력 안한다.. 일단
		}
		else if (_defArmy[i].status == BS_ARMY_DYING){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, 5 * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_defArmy[i].status == BS_ARMY_DYING_LIE){
			if (_defArmy[i].isAlpa)
				_defArmy[i].image->alphaRender(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, 5 * frameWidth, 0, frameWidth, frameHeight, 150.0);
		}
		else if (_defArmy[i].status == BS_ARMY_WAIT){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, 3 * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_defArmy[i].status == BS_ARMY_ATK_ON){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, (_defArmy[i].frameX + 2) * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_defArmy[i].status == BS_ARMY_ATK_OFF){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, (_defArmy[i].frameX + 2) * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_defArmy[i].status == BS_ARMY_RETURN){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, _defArmy[i].frameX * frameWidth, 0, frameWidth, frameHeight);
		}
		else if (_defArmy[i].status == BS_ARMY_ACTION_END){
			_defArmy[i].image->render(_MemDC, _defArmy[i].pos.x - frameWidth / 2, _defArmy[i].pos.y - frameHeight / 2
				, 3 * frameWidth, 0, frameWidth, frameHeight);
		}

	}
}
void PkwBattleShowScene::magicRender()
{
	if (_atkCommander.status != BS_COMMANDER_MAGIC) return;

	if (_magicMode == MAGIC_CASTING){
		if (_magicEffect.isShow){
			IMAGEMANAGER->findImage("summonEffect1")->alphaRender(_MemDC, _magicEffect.pos.x - 64, _magicEffect.pos.y - 64, _magicEffect.alpa);
			IMAGEMANAGER->findImage("charactorAction")->render(_MemDC, _atkCommander.pos.x -30, _atkCommander.pos.y -32, 128, 32, 32, 32);
		}
	}
	else if (_magicMode == MAGIC_START){
		if (_magicEffect.isShow){
			IMAGEMANAGER->findImage("summonEffect1")->alphaRender(_MemDC, _magicEffect.pos.x - 64, _magicEffect.pos.y - 64, _magicEffect.alpa);

			IMAGEMANAGER->findImage("charactorAction")->render(_MemDC, _atkCommander.pos.x-5, _atkCommander.pos.y - 30, 160, 32, 32, 32);
		}
	}
}

//기능 함수
NumDigit PkwBattleShowScene::GetNumDigit(int num)
{
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	//카운트 변수
	int cnt;
	//여러 값들 임시 저장
	int temp1, temp2, temp3;

	int value = num;

	cnt = 0;
	temp1 = 10;

	while (1){
		if (cnt == 0)	numDigit.value[cnt++] = value % 10;

		temp2 = value / temp1;
		if (temp2 == 0){
			break;
		}
		else{
			numDigit.value[cnt++] = temp2 % 10;
			temp1 *= 10;
		}
	}
	numDigit.strlen = cnt;

	return numDigit;
}