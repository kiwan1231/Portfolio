#include "../stdafx.h"
#include "classChangeEvent.h"


classChangeEvent::classChangeEvent()
{
}


classChangeEvent::~classChangeEvent()
{
}

void classChangeEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();

	_classChangeCommander = NULL;

	_nChoice = 0;

	_mode = CHOICE_CLASS;

	_eventStatus = EVENT_DEACTIVE;
}
void classChangeEvent::release(void)
{
	battleEvent::release();

	_classChangeCommander = NULL;
}
void classChangeEvent::update(float elapsedTime)
{
	RECT but[2];

	_delayTime += elapsedTime;
	
	if (_mode == CHOICE_CLASS){

		but[0].left = 160;	but[0].right = but[0].left + 320;
		but[0].top = 160;	but[0].bottom = but[0].top + 48;
		but[1].left = 160;	but[1].right = but[1].left + 320;
		but[1].top = 224;	but[1].bottom = but[1].top + 48;

		if (PtInRect(&but[0], _ptMouse)){
			_nChoice = 0;
			if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
				_mode = VIEW_CLASS1;
				_nChoice = 0;
			}
		}
		else if (PtInRect(&but[1], _ptMouse) && _isClassTwo){
			_nChoice = 1;
			if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
				_mode = VIEW_CLASS2;
				_nChoice = 0;
			}
		}
	}
	else if (_mode == VIEW_CLASS1 || _mode == VIEW_CLASS2){
		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			if (_mode == VIEW_CLASS1) _mode = CLASS1_YESNO;
			else if (_mode == VIEW_CLASS2) _mode = CLASS2_YESNO;
		}
		else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_mode = CHOICE_CLASS;
		}
	}
	else if (_mode == CLASS1_YESNO || _mode == CLASS2_YESNO){
		but[0].left = 496;	but[0].right = but[0].left + 96;
		but[0].top = 288;	but[0].bottom = but[0].top + 32;
		but[1].left = 496;	but[1].right = but[1].left + 96;
		but[1].top = 320;	but[1].bottom = but[1].top + 32;

		if (PtInRect(&but[0], _ptMouse)){
			_nChoice = 0;
			if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
				classChange();
				_eventStatus = EVENT_DEACTIVE;
			}
		}
		else if (PtInRect(&but[1], _ptMouse)){
			_nChoice = 1;
			if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
				_nChoice = 0;
				_mode = CHOICE_CLASS;
			}
		}
		
		if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_nChoice = 0;
			_mode = CHOICE_CLASS;
		}
	}
}
void classChangeEvent::render(HDC memDC)
{
	PkwClassData temp;
	NumDigit numDigit;

	if (_mode == CHOICE_CLASS){
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 96, 0, 0, 352, 192, 150.0);
		IMAGEMANAGER->findImage("UI(352X192)")->render(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 96);
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 48, 0, 0, 208, 64, 150.0);
		IMAGEMANAGER->findImage("UI(208X64)")->render(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 48);

		SelectObject(memDC, _font[1]);
		SetTextColor(memDC, RGB(255, 255, 255));

		TextOut(memDC, _pMapStartPos->x + 160 + 10, _pMapStartPos->y + 64 + 5, "클래스 체인지", strlen("클래스 체인지"));

		TextOut(memDC, _pMapStartPos->x + 192 + 12, _pMapStartPos->y + 112 + 5, _classChangeCommander->_name, strlen(_classChangeCommander->_name));
		
		TextOut(memDC, _pMapStartPos->x + 240 + 2, _pMapStartPos->y + 176 + 5, _nextClass1._name, strlen(_nextClass1._name));
		IMAGEMANAGER->findImage(_classChangeCommander->_imageKey)->render(memDC, _pMapStartPos->x + 176, _pMapStartPos->y + 160, 180,0,60,64);

		if (_isClassTwo){
			TextOut(memDC, _pMapStartPos->x + 240 + 2, _pMapStartPos->y + 240 + 5, _nextClass2._name, strlen(_nextClass2._name));
			IMAGEMANAGER->findImage(_classChangeCommander->_imageKey)->render(memDC, _pMapStartPos->x + 176, _pMapStartPos->y + 224, 180, 0, 60, 64);
		}
		//화살표
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 160, _pMapStartPos->y + 176 - 3 + _nChoice * 64, 160, 224, 15, 26);
		
	}
	else if (_mode == VIEW_CLASS1 || _mode == VIEW_CLASS2){
		//선택된 클래스데이터 저장
		if (_mode == VIEW_CLASS1) temp = _nextClass1;
		else if (_mode == VIEW_CLASS2) temp = _nextClass2;

		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 80, _pMapStartPos->y + 48, 0, 0, 480, 304, 150.0);
		IMAGEMANAGER->findImage("UI(480X304)")->render(memDC, _pMapStartPos->x + 80, _pMapStartPos->y + 48);

		SelectObject(memDC, _font[1]);
		SetTextColor(memDC, RGB(255, 255, 255));

		//지휘관 이미지 출력
		IMAGEMANAGER->findImage(_classChangeCommander->_imageKey)->render(memDC, _pMapStartPos->x + 192, _pMapStartPos->y + 64, 180, 0, 60, 64);
		//지휘관 이름
		TextOut(memDC, _pMapStartPos->x + 272, _pMapStartPos->y + 64 + 5, _classChangeCommander->_name, strlen(_classChangeCommander->_name));
		//선택된 클래스 이름
		TextOut(memDC, _pMapStartPos->x + 272, _pMapStartPos->y + 96 + 5, temp._name, strlen(temp._name));
		//'지휘범위'출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 448, _pMapStartPos->y + 80, 0, 48, 64, 16);
		//지휘범위 출력
		numDigit = GetNumDigit(temp._commanderRange);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 496 - (i * 16), _pMapStartPos->y + 96, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		}

		//'AT' 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 208, _pMapStartPos->y + 128, 16, 0, 16, 16);
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 224, _pMapStartPos->y + 128, 32, 0, 16, 16);
		//'+'출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 208, _pMapStartPos->y + 144, 0, 16, 16, 16);
		//숫자 출력
		numDigit = GetNumDigit(temp._upAtk);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 240 - (i * 16), _pMapStartPos->y + 144, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		//'DT' 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 272, _pMapStartPos->y + 128, 48, 0, 16, 16);
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 288, _pMapStartPos->y + 128, 32, 0, 16, 16);
		//'+'출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 272, _pMapStartPos->y + 144, 0, 16, 16, 16);
		//숫자 출력
		numDigit = GetNumDigit(temp._upDef);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 304 - (i * 16), _pMapStartPos->y + 144, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		//'MP' 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 336, _pMapStartPos->y + 128, 80, 0, 16, 16);
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 352, _pMapStartPos->y + 128, 96, 0, 16, 16);
		//'+'출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 336, _pMapStartPos->y + 144, 0, 16, 16, 16);
		//숫자 출력
		numDigit = GetNumDigit(temp._upMp);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 368 - (i * 16), _pMapStartPos->y + 144, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		//'MV' 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 400, _pMapStartPos->y + 128, 80, 0, 16, 16);
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 416, _pMapStartPos->y + 128, 112, 0, 16, 16);
		//숫자 출력
		numDigit = GetNumDigit(temp._move);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 416 - (i * 16), _pMapStartPos->y + 144, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		//공격 수정치 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 448, _pMapStartPos->y + 128, 16, 0, 16, 16);//'a'
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 460, _pMapStartPos->y + 128, 0, 16, 16, 16);//'+'
		//숫자 출력
		numDigit = GetNumDigit(temp._upCommanderAtk);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 492 - (i * 16), _pMapStartPos->y + 128, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		//방어 수정치 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 448, _pMapStartPos->y + 144, 48, 0, 16, 16);//'d'
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 460, _pMapStartPos->y + 144, 0, 16, 16, 16);//'+'
		//숫자 출력
		numDigit = GetNumDigit(temp._upCommanderDef);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 492 - (i * 16), _pMapStartPos->y + 144, 32 + 16 * numDigit.value[i], 16, 16, 16);
		}

		TextOut(memDC, _pMapStartPos->x + 96 + 14, _pMapStartPos->y + 160 + 5, "추가용병", strlen("추가용병"));
		for (int i = 0; i < temp._vMercenary.size(); i++){
			IMAGEMANAGER->findImage(temp._vMercenary[i]._imageKey)->render(memDC, _pMapStartPos->x + 96, _pMapStartPos->y + 192 + (64 * i), 180, 0, 60, 64);
			TextOut(memDC, _pMapStartPos->x + 156, _pMapStartPos->y + 208 + 5 + (64 * i), temp._vMercenary[i]._name, strlen(temp._vMercenary[i]._name));
		}

		TextOut(memDC, _pMapStartPos->x + 288 + 14, _pMapStartPos->y + 160 + 5, "추가 스킬, 소환", strlen("추가 스킬, 소환"));
		for (int i = 0; i < temp._vSkill.size(); i++){
			TextOut(memDC, _pMapStartPos->x + 320, _pMapStartPos->y + 192 + 5 + (32 * i), temp._vSkill[i]._name, strlen(temp._vSkill[i]._name));
		}

		//캐릭터 얼굴창 출력
		IMAGEMANAGER->findImage("charactorfaceWin")->render(memDC, _pMapStartPos->x + 48, _pMapStartPos->y + 32);
		//캐릭터 얼굴 출력
		IMAGEMANAGER->findImage(_classChangeCommander->_faceImageKey)->render(memDC, _pMapStartPos->x + 64, _pMapStartPos->y + 48);
	}
	else if (_mode == CLASS1_YESNO || _mode == CLASS2_YESNO){
		//선택된 클래스데이터 저장
		if (_mode == CLASS1_YESNO) temp = _nextClass1;
		else if (_mode == CLASS2_YESNO) temp = _nextClass2;

		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128, 0, 0, 576, 160, 150.0);
		IMAGEMANAGER->findImage("UI(576X160)")->render(memDC, _pMapStartPos->x + 32, _pMapStartPos->y + 128);

		SelectObject(memDC, _font[1]);
		SetTextColor(memDC, RGB(255, 255, 255));

		TextOut(memDC, _pMapStartPos->x + 128 + 14, _pMapStartPos->y + 144 + 5, _classChangeCommander->_name, strlen(_classChangeCommander->_name));
		TextOut(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 176 + 5, temp._name, strlen(temp._name));
		TextOut(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 208 + 5, "클래스 체인지 합니다.", strlen("클래스 체인지 합니다."));
		TextOut(memDC, _pMapStartPos->x + 144, _pMapStartPos->y + 240 + 5, "결졍하셨습니까?", strlen("결졍하셨습니까?"));

		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(memDC, _pMapStartPos->x + 480, _pMapStartPos->y + 272, 0, 0, 128, 96, 150.0);
		IMAGEMANAGER->findImage("UI(128X96)")->render(memDC, _pMapStartPos->x + 480, _pMapStartPos->y + 272);
		TextOut(memDC, _pMapStartPos->x + 512, _pMapStartPos->y + 288 + 5, "예", strlen("예"));
		TextOut(memDC, _pMapStartPos->x + 512, _pMapStartPos->y + 320 + 5, "아니오", strlen("아니오"));

		//화살표 출력
		IMAGEMANAGER->findImage("charactorUI")->render(memDC, _pMapStartPos->x + 496, _pMapStartPos->y + 288 - 3 + _nChoice * 32, 160, 224, 15, 26);

		IMAGEMANAGER->findImage(_classChangeCommander->_talkImageKey)->render(memDC, _pMapStartPos->x + 16, _pMapStartPos->y + 64);
	}
}
void classChangeEvent::checkEvent()
{
	PkwClassData temp;

	//아군 체크
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		
		if (_pCharactorPlay->_vCoCommander[i]._level >= 10){
			
			_classChangeCommander = &_pCharactorPlay->_vCoCommander[i];
			//선택된 지휘관의 클래스 데이터를 가져온다
			for (int i = 0; i < DATAMANAGER->_DataBase._vClassData.size(); i++){
				if (_classChangeCommander->_class == DATAMANAGER->_DataBase._vClassData[i]._myClass){
					temp = DATAMANAGER->_DataBase._vClassData[i];
					break;
				}
			}
			//두번째 클래스 체인지 목록이 있은지 확인
			if (temp._nextClass2 == CLASS_END)  _isClassTwo = false;
			else                                _isClassTwo = true;
			//클래스 체인지 데이터 저장
			for (int i = 0; i < DATAMANAGER->_DataBase._vClassData.size(); i++){
				if (temp._nextClass1 == DATAMANAGER->_DataBase._vClassData[i]._myClass){
					_nextClass1 = DATAMANAGER->_DataBase._vClassData[i];

				}
				else if (_isClassTwo && temp._nextClass2 == DATAMANAGER->_DataBase._vClassData[i]._myClass){
					_nextClass2 = DATAMANAGER->_DataBase._vClassData[i];
				}
			}
			_nChoice = 0;
			_mode = CHOICE_CLASS;
			_eventStatus = EVENT_ACTIVE;
			return;
		}
	}
}

void classChangeEvent::classChange()
{
	PkwClassData temp;

	if (_mode == CLASS1_YESNO)
		temp = _nextClass1;
	else if (_mode == CLASS2_YESNO)
		temp = _nextClass2;

	memset(_classChangeCommander->_chClass, 0, 128);
	memcpy(_classChangeCommander->_chClass, temp._name, strlen(temp._name));

	_classChangeCommander->_class = temp._myClass;
	_classChangeCommander ->_commandRange = temp._commanderRange;
	_classChangeCommander->_move = temp._move;
	_classChangeCommander->_commandAtk += temp._upCommanderAtk;
	_classChangeCommander->_commandDef += temp._upCommanderDef;
	
	for (int i = 0; i < temp._vMercenary.size(); i++)
		_classChangeCommander->_vMercenaryData.push_back(temp._vMercenary[i]);
	for (int i = 0; i < temp._vSkill.size(); i++)
		_classChangeCommander->_vSkill.push_back(temp._vSkill[i]);
	for (int i = 0; i < temp._vSummon.size(); i++)
		_classChangeCommander->_vSummon.push_back(temp._vSummon[i]);

	_classChangeCommander->_level = 1;
}

//기능 함수
NumDigit classChangeEvent::GetNumDigit(int num)
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