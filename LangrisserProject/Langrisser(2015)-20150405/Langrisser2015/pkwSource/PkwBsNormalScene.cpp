#include "../stdafx.h"
#include "PkwBsNormalScene.h"

/*
HFONT CreateFont(
int nHeight,             // 폰트의 높이
int nWidth,              // 폰트의 넓이
int nEscapement,         // 베이스라인에 대한 전체문자열의 기울기
int nOrientation,        // 개별문자열의 기울기
int fnWeight,            // 폰트의 두께
DWORD fdwItalic,         // 이탤릭 (4byte값을 주면 이탤릭체 표현)
DWORD fdwUnderline,      // 밑줄           "
DWORD fdwStrikeOut,      // 취소선         "
DWORD fdwCharSet,        // 문자집합
DWORD fdwOutputPrecision,  // 정밀도(정밀도가 높을수록 출력속도가 느림)
DWORD fdwClipPrecision,  // clipping 영역의 정밀도
DWORD fdwQuality,        // output quality
DWORD fdwPitchAndFamily,  // pitch and family
LPCTSTR lpszFace         // 글꼴
);
이걸 보면 i값에 따라 기울기가 변하는걸 볼수 있죠? 그래서 글씨가 돌아가면서 기울기가바뀌는것.
*/

PkwBsNormalScene::PkwBsNormalScene()
{
}
PkwBsNormalScene::~PkwBsNormalScene()
{
}

//기본 함수
HRESULT PkwBsNormalScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction1, CALLBACK_FUNCTION_PARAMETER cbFunction2, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter1 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction1);
	_callbackFunctionParameter2 = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction2);

	_mapImage = IMAGEMANAGER->findImage(DATAMANAGER->_stageDataFile[DATAMANAGER->_stageNumber].stageMapImageKey);
	_pCharactorPlay = DATAMANAGER->getCharactorPlay();
	
	//_pChoiceCommander = NULL;
	
	_pChoiceCommander = &_pCharactorPlay->_vCoCommander[0];
	_pChoiceMercenary = &_pChoiceCommander->_vMercenaryData[0];
	_mercenaryMaxList = (_pChoiceCommander->_vMercenaryData.size()-1) / 3;
	_mercenaryCurList = 0;
	_nCnoice = 0;
	_nCnoice2 = 0;
	_nCnoice3 = 0;

	_battleSetMode = BATTLESET_NORMAL;//BATTLESET_NORMAL | BATTLESET_MERCENARY
	_bsMercenaryMode = BS_MERCENARY_NORMAL;//BS_MERCENARY_NORMAL | BS_MERCENARY_COMMANDER_CHOICE | BS_MERCENARY_MERCENARY_CHOICE
	_bsCommanderPosMode = BS_COMMANDERPOS_NORMAL;

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;
	_gameView.left = 0; _gameView.right = _nWindowWidth;
	_gameView.top = 0; _gameView.bottom = _nWindowHeight;

	_MapMaxWidth = _mapImage->getWidth();
	_MapManHeight = _mapImage->getHeight();
	_MapStartPos.x = 1300;		_MapStartPos.y = 1300;
	_elapsed = 0.0;

	_backBuffer = NULL;
	_backBuffer = new image;
	if (FAILED(_backBuffer->init(_MapMaxWidth, _MapManHeight))){
		SAFE_DELETE(_backBuffer);
		return E_FAIL;
	}

	_MemDC = _backBuffer->getMemDC();

	SetBkMode(_MemDC, TRANSPARENT);

	// 폰트 set
	_font[0] = CreateFont(28,0, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[1] = CreateFont(22,0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[2] = CreateFont(16, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	
	//글자 간격
	SetTextCharacterExtra(_MemDC, 5);

	_mouseImageFrame.frameDelay = 0.1;
	_mouseImageFrame.elaspedTime = 0.0;
	_mouseImageFrame.frameDir = NORMAL_DIR;
	_mouseImageFrame.frameX = 0;
	_mouseImageFrame.maxFrameX = IMAGEMANAGER->findImage("titleChoice")->getMaxFrameX();

	
	return S_OK;
}
void PkwBsNormalScene::release(void)
{
	for (int i = 0; i < 10; i++){
		if (_font[i] !=NULL)	DeleteObject(_font[i]);
	}

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
	}
}
void PkwBsNormalScene::update(void)
{
	POINT pos;
	_elapsed = TIMEMANAGER->getElapsedTime();
	
	mouseImageFrameUpdate();

	charactorUpdate();

	if (_battleSetMode == BATTLESET_NORMAL){
		battleSetNormalUpdate();
	}
	else if (_battleSetMode == BATTLESET_MERCENARY){
		battleSetMercenaryUpdate();
	}
	else if (_battleSetMode == BATTLESET_ITEM){
		battleSetShopUpdate();
	}
	else if (_battleSetMode == BATTLESET_COMMANDERPOS){
		battleSetCommanderPosUpdate();
	}
	else if (_battleSetMode == BATTLESET_BATTLESTART){
		//게임셋 씬 종료
		_callbackFunctionParameter2(_obj);
	}
}
void PkwBsNormalScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	//맵 그리기
	_mapImage->render(_MemDC, 0, 0);

	//지휘관 포인트 그리기
	for (int i = 0; i < DATAMANAGER->_vCommanderPos.size(); i++){
		IMAGEMANAGER->findImage("commanderPos")->render(_MemDC, DATAMANAGER->_vCommanderPos[i].x*UNITSIZEX, DATAMANAGER->_vCommanderPos[i].y*UNITSIZEY, 0, 0, UNITSIZEX, UNITSIZEY);
	}

	// 캐릭터 그리기
	CharactorRender();

	//인터페이스 그리기
	InterfaceRender();

	// 경과 시간 그리기 getHDC()
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(250, 250, 250));

	sprintf_s(str, "battleSet Scene - elapse time : %f  ", _elapsed);
	TextOut(_MemDC, _MapStartPos.x, _MapStartPos.y, str, strlen(str));

	//커서 그리기
	IMAGEMANAGER->findImage("titleCursor")->render(_MemDC, _MapStartPos.x + _ptMouse.x, _MapStartPos.y + _ptMouse.y, _mouseImageFrame.frameX * 46, 0, 46, 46);

	//백 이미지 그려주기
	BackImageDraw();
}

//update관련 함수
void PkwBsNormalScene::charactorUpdate()
{
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++)
			_pCharactorPlay->_vCoCommander[i]._vArms[j].update(_elapsed);

		_pCharactorPlay->_vCoCommander[i].update(_elapsed);
	}

	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++)
			_pCharactorPlay->_vSupportCommander[i]._vArms[j].update(_elapsed);

		_pCharactorPlay->_vSupportCommander[i].update(_elapsed);
	}

	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++)
			_pCharactorPlay->_vEnemyCommander[i]._vArms[j].update(_elapsed);

		_pCharactorPlay->_vEnemyCommander[i].update(_elapsed);
	}
}
void PkwBsNormalScene::battleSetNormalUpdate()
{
	RECT rc[4];
	for (int i = 0; i < 4; i++){
		rc[i].left = 64;
		rc[i].right = rc[i].left + 130;
		rc[i].top = 112 + (i*32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nCnoice = i;
	}

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < 4; i++){
			if (PtInRect(&rc[i], _ptMouse)){

				if (i == 0){
					_battleSetMode = BATTLESET_MERCENARY;
					_bsMercenaryMode = BS_MERCENARY_NORMAL;
				}
				else if (i == 1){
					_battleSetMode = BATTLESET_ITEM;
					_nCnoice = 0;
				}
				else if (i == 2){
					_battleSetMode = BATTLESET_COMMANDERPOS;
					_bsCommanderPosMode = BS_COMMANDERPOS_NORMAL;
					_nCnoice = 0;
				}
				else if (i == 3){
					_battleSetMode = BATTLESET_BATTLESTART;
				}
			}
		}
	}
}
void PkwBsNormalScene::battleSetMercenaryUpdate()
{
	int commanderNum = _pCharactorPlay->_vCoCommander.size();
	RECT *rc = new RECT[commanderNum];
	RECT rc2[5];
	RECT rc3[2];
	
	if (_bsMercenaryMode == BS_MERCENARY_NORMAL){

		for (int i = 0; i < commanderNum; i++){
			rc[i].left = 64;
			rc[i].right = rc[i].left + 130;
			rc[i].top = 80 + (i * 32);
			rc[i].bottom = rc[i].top + 32;

			if (PtInRect(&rc[i], _ptMouse)) _nCnoice = i;
		}

		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			for (int i = 0; i < commanderNum; i++){
				if (PtInRect(&rc[i], _ptMouse)){

					_pChoiceCommander = &_pCharactorPlay->_vCoCommander[i];
					_mercenaryMaxList = (_pChoiceCommander->_vMercenaryData.size() - 1) / 3;
					_mercenaryCurList = 0;

					if (_pChoiceCommander->_vArms.size() == 0){
						_bsMercenaryMode = BS_MERCENARY_COMMANDER_CHOICE;
						_pChoiceMercenary = NULL;
					}
					else{
						_bsMercenaryMode = BS_MERCENARY_MERCENARY_CHOICE;
						for (int j = 0; j < _pChoiceCommander->_vMercenaryData.size(); j++){
							if (strcmp(_pChoiceCommander->_vMercenaryData[j]._name, _pChoiceCommander->_vArms[0]._name) == 0){
								_pChoiceMercenary = &_pChoiceCommander->_vMercenaryData[j];
								_mercenaryCurList = (j-1) / 3;
								_nCnoice2 = j % 3;
								break;
							}
						}
					}
				}
			}
		}
		if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_battleSetMode = BATTLESET_NORMAL;
			_nCnoice = 0;
			_nCnoice2 = 0;
			_nCnoice3 = 0;
		}

	}
	else if (_bsMercenaryMode == BS_MERCENARY_COMMANDER_CHOICE){

		for (int i = 0; i < 3; i++){
			rc2[i].left = 224;
			rc2[i].right = rc2[i].left + 320;
			rc2[i].top = 179 + 48*i;
			rc2[i].bottom = rc2[i].top +  48;

			if (PtInRect(&rc2[i], _ptMouse)){
				//마지막 장에서 없는 용병칸은 이동하지 않는다
				if (_mercenaryCurList * 3 + i >= _pChoiceCommander->_vMercenaryData.size()) continue;
				_nCnoice2 = i;
			}
		}

		rc2[3].left = 576;	rc2[3].right = rc2[3].left + 16;
		rc2[3].top = 176;	rc2[3].bottom = rc2[3].top + 16;
		rc2[4].left = 576;	rc2[4].right = rc2[4].left + 16;
		rc2[4].top = 288;	rc2[4].bottom = rc2[4].top + 16;


		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			for (int i = 0; i < 5; i++){
				if (PtInRect(&rc2[i], _ptMouse)){
					if (i < 3){
						if (_mercenaryCurList * 3 + i < _pChoiceCommander->_vMercenaryData.size()){
							_bsMercenaryMode = BS_MERCENARY_MERCENARY_CHOICE;
							_pChoiceMercenary = &_pChoiceCommander->_vMercenaryData[i + (_mercenaryCurList * 3)];
						}
						//_nCnoice3 = 0;
					}
					else{
						if (i == 3){
							if (_mercenaryCurList <= 0) break;
							else{
								_mercenaryCurList--;
								_nCnoice2 = 0;
							}
						}
						else if (i == 4){
							if (_mercenaryCurList >= _mercenaryMaxList) break;
							else{
								_mercenaryCurList++;
								_nCnoice2 = 0;
							}
						}
					}
				}
			}
		}
		if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_bsMercenaryMode = BS_MERCENARY_NORMAL;
			_nCnoice = 0;
			_nCnoice2 = 0;
			_nCnoice3 = 0;
		}
	}
	else if (_bsMercenaryMode == BS_MERCENARY_MERCENARY_CHOICE){

		rc3[0].left = 272;	rc3[0].right = rc3[0].left + 192;
		rc3[0].top = 272;	rc3[0].bottom = rc3[0].top + 48;
		rc3[1].left = 272;	rc3[1].right = rc3[1].left + 192;
		rc3[1].top = 320;	rc3[1].bottom = rc3[1].top + 32;

		for (int i = 0; i < 2; i++){
			if (PtInRect(&rc3[i],_ptMouse)){
				_nCnoice3 = i;
				break;
			}
		}

		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
			for (int i = 0; i < 2; i++){
				if (PtInRect(&rc3[i], _ptMouse)){
					if (i == 0){
						createMercenary(_nCnoice2);
					}
					else if (i == 1){
						_battleSetMode = BATTLESET_NORMAL;
						_bsMercenaryMode = BS_MERCENARY_NORMAL;
						_nCnoice = 0;
						_nCnoice2 = 0;
						_nCnoice3 = 0;
					}
				}
			}
		}

		if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			if (_pChoiceCommander->_vArms.size() > 0){
				DATAMANAGER->_money += _pChoiceCommander->_vArms[0]._price;
				_pChoiceCommander->_vArms.erase(_pChoiceCommander->_vArms.end()-1);
			}
			else{
				_bsMercenaryMode = BS_MERCENARY_COMMANDER_CHOICE;
				_nCnoice3 = 0;
			}
		}
	}

	delete[] rc;
}
void PkwBsNormalScene::battleSetShopUpdate()
{
	int commanderNum = _pCharactorPlay->_vCoCommander.size();
	RECT *rc = new RECT[commanderNum];

	for (int i = 0; i < commanderNum; i++){
		rc[i].left = 64;
		rc[i].right = rc[i].left + 130;
		rc[i].top = 80 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nCnoice = i;
	}

	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){

		for (int i = 0; i < commanderNum; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				_pChoiceCommander = &_pCharactorPlay->_vCoCommander[i];
				_battleSetMode = BATTLESET_NORMAL;
				_nCnoice = 0;
				_callbackFunctionParameter1(_obj);
				break;
			}
		}

	}
	else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_battleSetMode = BATTLESET_NORMAL;
		_nCnoice = 0;
	}

	delete[] rc;
}
void PkwBsNormalScene::battleSetCommanderPosUpdate()
{
	if (_bsCommanderPosMode == BS_COMMANDERPOS_NORMAL){
		int commanderNum = _pCharactorPlay->_vCoCommander.size();
		RECT *rc = new RECT[commanderNum];

		for (int i = 0; i < commanderNum; i++){
			rc[i].left = 64;
			rc[i].right = rc[i].left + 130;
			rc[i].top = 80 + (i * 32);
			rc[i].bottom = rc[i].top + 32;

			if (PtInRect(&rc[i], _ptMouse)) _nCnoice = i;
		}

		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){

			for (int i = 0; i < commanderNum; i++){
				if (PtInRect(&rc[i], _ptMouse)){
					_pChoiceCommander = &_pCharactorPlay->_vCoCommander[i];
					_bsCommanderPosMode = BS_COMMANDERPOS_COMMANDER_CHOICE;
					break;
				}
			}

		}
		else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_battleSetMode = BATTLESET_NORMAL;
			_nCnoice = 0;
		}

		delete[] rc;
	}
	else if (_bsCommanderPosMode == BS_COMMANDERPOS_COMMANDER_CHOICE){
		POINT pos, temp;
		bool isPosition = false;

		if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){

			pos.x = (_MapStartPos.x + _ptMouse.x) / 48;
			pos.y = (_MapStartPos.y + _ptMouse.y) / 48;

			for (int i = 0; i < DATAMANAGER->_vCommanderPos.size(); i++){

				if (pos.x == DATAMANAGER->_vCommanderPos[i].x && pos.y == DATAMANAGER->_vCommanderPos[i].y){
					
					for (int j = 0; j < _pCharactorPlay->_vCoCommander.size(); j++){
						if (pos.x == _pCharactorPlay->_vCoCommander[j]._pos.x && pos.y == _pCharactorPlay->_vCoCommander[j]._pos.y){
							temp = _pCharactorPlay->_vCoCommander[j]._pos;
							_pCharactorPlay->_vCoCommander[j]._pos = _pChoiceCommander->_pos;
							_pChoiceCommander->_pos = temp;

							isPosition = true;
							break;
						}
					}
					if (!isPosition){
						_pChoiceCommander->_pos = pos;
					}
					mercenaryRepos();
					_battleSetMode = BATTLESET_NORMAL;
					break;
				}
			}
		}
		else if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
			_bsCommanderPosMode = BS_COMMANDERPOS_NORMAL;
			//_pChoiceCommander = NULL;
			_nCnoice = 0;
		}
	}
}

//render관련 함수
void PkwBsNormalScene::BackImageDraw()
{
	//---------------------------------------------------카메라 위치에 맞는 이미지 구하기
	int loadWidth = _nWindowWidth;
	int	loadHeight = _nWindowHeight;
	int camaraSpeed = _elapsed * 1000;

	//전체 이미지(가로)가 윈도우 창보다 작을떄 가운데다 그린다 
	if (loadWidth >= _MapMaxWidth){
		_gameView.left = (loadWidth - _MapMaxWidth) / 2;
		_gameView.right = _MapMaxWidth + _gameView.left;

		_MapStartPos.x = 0;
		loadWidth = _MapMaxWidth;
	}
	//전체 이미지(가로)가 윈도우 창보다 클떄
	else{
		if (_ptMouse.x <= 10)
			_MapStartPos.x -= camaraSpeed;
		else if (_ptMouse.x >= loadWidth - 10)
			_MapStartPos.x += camaraSpeed;

		if (_MapStartPos.x <= 0)
			_MapStartPos.x = 0;
		else if (_MapStartPos.x + loadWidth >= _MapMaxWidth)
			_MapStartPos.x -= (_MapStartPos.x + loadWidth) - _MapMaxWidth;
	}

	//전체 이미지(세로)가 윈도우 창보다 작을떄 가운데다 그린다 
	if (loadHeight >= _MapManHeight){

		_gameView.top = (loadHeight - _MapManHeight) / 2;
		_gameView.bottom = _MapManHeight + _gameView.top;

		_MapStartPos.y = 0;
		loadHeight = _MapManHeight;
	}
	//전체 이미지(세로)가 윈도우 창보다 클떄
	else{
		if (_ptMouse.y <= 10)
			_MapStartPos.y -= camaraSpeed;
		else if (_ptMouse.y >= loadHeight - 10)
			_MapStartPos.y += camaraSpeed;

		if (_MapStartPos.y <= 0)
			_MapStartPos.y = 0;
		else if (_MapStartPos.y + loadHeight >= _MapManHeight)
			_MapStartPos.y -= (_MapStartPos.y + loadHeight) - _MapManHeight;
	}

	_backBuffer->render(DATAMANAGER->_mainDc, _gameView.left, _gameView.top, _MapStartPos.x, _MapStartPos.y, _gameView.right - _gameView.left, _gameView.bottom - _gameView.top);
}
void PkwBsNormalScene::CharactorRender()
{
	//아군 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		if (_pCharactorPlay->_vCoCommander[i]._pos.x != -1 && _pCharactorPlay->_vCoCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vCoCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vCoCommander[i].render(_MemDC, true);
		}
	}
	//지휘관 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		if (_pCharactorPlay->_vSupportCommander[i]._pos.x != -1 && _pCharactorPlay->_vSupportCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vSupportCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vSupportCommander[i].render(_MemDC, true);
		}
	}
	//적 지휘관 및 용병 렌더
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		if (_pCharactorPlay->_vEnemyCommander[i]._pos.x != -1 && _pCharactorPlay->_vEnemyCommander[i]._pos.y != -1){
			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++)
				_pCharactorPlay->_vEnemyCommander[i]._vArms[j].render(_MemDC, true);

			_pCharactorPlay->_vEnemyCommander[i].render(_MemDC, true);
		}
	}
}
void PkwBsNormalScene::InterfaceRender()
{
	float alpa = 150.0;
	NumDigit numDigit;

	if (_battleSetMode == BATTLESET_NORMAL){
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 32, 0, 0, 240, 64, alpa);
		IMAGEMANAGER->findImage("UI(240X64)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 32);
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 96, 0, 0, 240, 160, alpa);
		IMAGEMANAGER->findImage("UI(240X160)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 96);
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 32, 0, 0, 240, 256, alpa);
		IMAGEMANAGER->findImage("UI(240X256)")->render(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 32);

		SelectObject(_MemDC, _font[0]);
		SetTextColor(_MemDC, RGB(255, 255, 255));
		TextOut(_MemDC, _MapStartPos.x + 400, _MapStartPos.y + 48 + 2, "지휘관", strlen("지휘관"));

		SelectObject(_MemDC, _font[1]);
		
		TextOut(_MemDC, _MapStartPos.x + 48 + 4, _MapStartPos.y + 48 + 5, "소지금", strlen("소지금"));
		TextOut(_MemDC, _MapStartPos.x + 64 + 2, _MapStartPos.y + 112 + 5, "병사배속", strlen("병사배속"));
		TextOut(_MemDC, _MapStartPos.x + 64 + 2, _MapStartPos.y + 144 + 5, "아이템장비", strlen("아이템장비"));
		TextOut(_MemDC, _MapStartPos.x + 64 + 2, _MapStartPos.y + 176 + 5, "지휘관배치", strlen("지휘관배치"));
		TextOut(_MemDC, _MapStartPos.x + 64 + 2, _MapStartPos.y + 208 + 5, "출격", strlen("출격"));

		//소지금 출력
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 240, _MapStartPos.y + 64, 144, 0, 16, 16);

		numDigit = GetNumDigit(DATAMANAGER->_money);
		for (int i = 0; i < numDigit.strlen; i++){
			IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 224 - (i * 16), _MapStartPos.y + 61, 16 * numDigit.value[i], 224, 16, 19);
		}

		//지휘관 이름 출력
		//SetTextColor(_MemDC, RGB(115, 115, 115));
		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			TextOut(_MemDC, _MapStartPos.x + 400 + 2, _MapStartPos.y + 80 + (i * 32) + 5, _pCharactorPlay->_vCoCommander[i]._name, strlen(_pCharactorPlay->_vCoCommander[i]._name));
		}

		//선택 되어지고 있는 버튼 화살표 출력
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 115 + _nCnoice*32, 160, 224, 15, 26);
	}
	else if (_battleSetMode == BATTLESET_MERCENARY || _battleSetMode == BATTLESET_ITEM || _battleSetMode == BATTLESET_COMMANDERPOS){//BS_MERCENARY_COMMANDER_CHOICE
		
		if (_battleSetMode == BATTLESET_COMMANDERPOS &&_bsCommanderPosMode == BS_COMMANDERPOS_COMMANDER_CHOICE) return;

		char str[128];
		memset(str, 0, 128);
		if (_battleSetMode == BATTLESET_MERCENARY)
			memcpy(str, "병사배속", strlen("병사배속"));
		else if (_battleSetMode == BATTLESET_ITEM)
			memcpy(str, "아이템장비", strlen("아이템장비"));
		else if (_battleSetMode == BATTLESET_COMMANDERPOS)
			memcpy(str, "지휘관배치", strlen("지휘관배치"));

		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 32, 0, 0, 240, 256, alpa);
		IMAGEMANAGER->findImage("UI(240X256)")->render(_MemDC, _MapStartPos.x + 32, _MapStartPos.y + 32);
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 32, 0, 0, 240, 64, alpa);
		IMAGEMANAGER->findImage("UI(240X64)")->render(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 32);
		IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 96, 0, 0, 240, 208, alpa);
		IMAGEMANAGER->findImage("UI(240X208)")->render(_MemDC, _MapStartPos.x + 368, _MapStartPos.y + 96);

		SelectObject(_MemDC, _font[0]);
		SetTextColor(_MemDC, RGB(255, 255, 255));
		TextOut(_MemDC, _MapStartPos.x + 64, _MapStartPos.y + 48 + 2, str, strlen(str));
		
		SelectObject(_MemDC, _font[1]);
		//SetTextColor(_MemDC, RGB(77, 77, 77));

		for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
			TextOut(_MemDC, _MapStartPos.x + 64 + 2, _MapStartPos.y + 80 + (i * 32) + 5, _pCharactorPlay->_vCoCommander[i]._name, strlen(_pCharactorPlay->_vCoCommander[i]._name));
		}

		SetTextColor(_MemDC, RGB(255, 255, 255));
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 48 + 5, "소지금", strlen("소지금"));
		//캐릭터 정보 출력
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 112 + 5, _pChoiceCommander->_name, strlen(_pChoiceCommander->_name));
		TextOut(_MemDC, _MapStartPos.x + 384 + 5, _MapStartPos.y + 144 + 5, _pChoiceCommander->_chClass, strlen(_pChoiceCommander->_chClass));

		//선택 되어지고 있는 버튼 화살표 출력
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, _MapStartPos.x + 48, _MapStartPos.y + 83 + _nCnoice * 32, 160, 224, 15, 26);

		InfoRender();

		if (_battleSetMode == BATTLESET_MERCENARY && (_bsMercenaryMode == BS_MERCENARY_COMMANDER_CHOICE || _bsMercenaryMode == BS_MERCENARY_MERCENARY_CHOICE)){
			MercenaryListRender();
			if (_bsMercenaryMode == BS_MERCENARY_MERCENARY_CHOICE)
				MercenaryChoiceRender();
		}

	}
	else if (_battleSetMode == BATTLESET_BATTLESTART){

	}
}
void PkwBsNormalScene::InfoRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;

	//퍼센티지 저장
	float per;
	float width;

	//소지금 그리기
	pos.x = 576; pos.y = 64;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 144, 0, 16, 16);

	pos.y = 64 - 3;
	numDigit = GetNumDigit(DATAMANAGER->_money);
	for (int i = 0; i < numDigit.strlen; i++){
		pos.x -= 16;
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16 * numDigit.value[i], 224, 16, 19);
	}

	//레벨 그리기
	pos.x = 384; pos.y = 176;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 160, 0, 16, 16);
	pos.x = 400; pos.y = 176;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 112, 0, 16, 16);

	pos.x = 432; pos.y = 176;
	numDigit = GetNumDigit(_pChoiceCommander->_level);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//경험치 바 그리기
	pos.x = 448; pos.y = 176;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 144, 144, 16);

	per = (float)_pChoiceCommander->_curExp / (float)_pChoiceCommander->_maxExp;
	width = 112.0 * per;
	pos.x = 464; pos.y = 176;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 160, (int)width, 16);

	//공격력 그리기
	pos.x = 384; pos.y = 192;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16, 0, 16, 16);
	pos.x = 400; pos.y = 192;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32, 0, 16, 16);

	pos.x = 432; pos.y = 192;
	numDigit = GetNumDigit(_pChoiceCommander->_atk + _pChoiceCommander->_atkItem._atk + _pChoiceCommander->_DefItem._atk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//방어력 그리기
	pos.x = 464; pos.y = 192;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 48, 0, 16, 16);
	pos.x = 480; pos.y = 192;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 64, 0, 16, 16);

	pos.x = 512; pos.y = 192;
	numDigit = GetNumDigit(_pChoiceCommander->_def + _pChoiceCommander->_atkItem._def + _pChoiceCommander->_DefItem._def);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//mp그리기
	pos.x = 384; pos.y = 208;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 80, 0, 16, 16);
	pos.x = 400; pos.y = 208;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 96, 0, 16, 16);

	pos.x = 432; pos.y = 208;
	numDigit = GetNumDigit(_pChoiceCommander->_maxMp);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//아이템 그리기
	int sourX, sourY;
	pos.x = 528; pos.y = 192;
	sourX = (_pChoiceCommander->_atkItem._imageNum % 8) * 32;
	sourY = (_pChoiceCommander->_atkItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, sourX, sourY, 32, 32);
	
	pos.x = 560; pos.y = 192;
	sourX = (_pChoiceCommander->_DefItem._imageNum % 8) * 32;
	sourY = (_pChoiceCommander->_DefItem._imageNum / 8) * 32;
	IMAGEMANAGER->findImage("item")->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, sourX, sourY, 32, 32);

	//용병 정보 그리기
	if (_pChoiceCommander->_vArms.size() == 0) return;

	SelectObject(_MemDC, _font[2]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	//이름
	pos.x = 384; pos.y = 240;
	TextOut(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, _pChoiceCommander->_vArms[0]._name, strlen(_pChoiceCommander->_vArms[0]._name));
	//용병 수
	pos.x = 512; pos.y = 240;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 128, 48, 16, 16);
	pos.x = 528; pos.y = 240;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * _pChoiceCommander->_vArms.size(), 16, 16, 16);
	pos.x = 544; pos.y = 240;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32, 32, 16, 16);
	pos.x = 560; pos.y = 240;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * _pChoiceCommander->_maxArms, 16, 16, 16);
	//용병 가격
	pos.x = 560; pos.y = 256;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 96, 0, 16, 16);
	pos.x = 544; pos.y = 256;
	numDigit = GetNumDigit(_pChoiceCommander->_vArms[0]._price);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	//용병 공격력
	pos.x = 384; pos.y = 272;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16, 0, 16, 16);
	pos.x = 416; pos.y = 272;
	numDigit = GetNumDigit(_pChoiceCommander->_vArms[0]._atk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	//지휘관 지휘 공격력 
	pos.x = 432; pos.y = 272;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 16, 16, 16);
	pos.x = 464; pos.y = 272;
	numDigit = GetNumDigit(_pChoiceCommander->_commandAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	//용병 방어력
	pos.x = 496; pos.y = 272;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 48, 0, 16, 16);
	pos.x = 528; pos.y = 272;
	numDigit = GetNumDigit(_pChoiceCommander->_vArms[0]._def);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	//지휘관 지휘 방어력 
	pos.x = 544; pos.y = 272;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 16, 16, 16);
	pos.x = 576; pos.y = 272;
	numDigit = GetNumDigit(_pChoiceCommander->_commandDef);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	return;
}
void PkwBsNormalScene::MercenaryListRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	float alpa = 200.0;
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	int nCnt;
	
	pos.x = 208; pos.y = 144;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 0, 400, 176, alpa);
	IMAGEMANAGER->findImage("UI(400X176)")->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y);
	
	nCnt = 0;
	while (nCnt<3){
		int num = nCnt + (3 * _mercenaryCurList);
		if (num >= _pChoiceCommander->_vMercenaryData.size()) break;

		//용병 이미지 출력
		pos.x = 240; pos.y = 158 + (nCnt * 48);
		IMAGEMANAGER->findImage(_pChoiceCommander->_vMercenaryData[num]._imageKey)->render(_MemDC, _MapStartPos.x + pos.x - 10, _MapStartPos.y + pos.y, 180, 0, 60, 64);
		
		//용병 이름 출력
		SelectObject(_MemDC, _font[2]);
		SetTextColor(_MemDC, RGB(255, 255, 255));
		pos.x = 288; pos.y = 176 + (nCnt * 48);
		TextOut(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, _pChoiceCommander->_vMercenaryData[num]._name, strlen(_pChoiceCommander->_vMercenaryData[num]._name));

		//용병 가격
		pos.x = 496; pos.y = 176 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 96, 0, 16, 16);
		pos.x = 480; pos.y = 176 + (nCnt * 48);
		numDigit = GetNumDigit(_pChoiceCommander->_vMercenaryData[num]._price);
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}

		//용병 공격력
		pos.x = 288; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16, 0, 16, 16);
		pos.x = 304; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32, 0, 16, 16);
		pos.x = 336; pos.y = 192 + (nCnt * 48);
		numDigit = GetNumDigit(_pChoiceCommander->_vMercenaryData[num]._atk);
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}
	
		//용병 방어력
		pos.x = 368; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 48, 0, 16, 16);
		pos.x = 384; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 64, 0, 16, 16);
		pos.x = 416; pos.y = 192 + (nCnt * 48);
		numDigit = GetNumDigit(_pChoiceCommander->_vMercenaryData[num]._def);
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}

		//용병 이동력
		pos.x = 448; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 80, 0, 16, 16);
		pos.x = 464; pos.y = 192 + (nCnt * 48);
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 112, 0, 16, 16);
		pos.x = 496; pos.y = 192 + (nCnt * 48);
		numDigit = GetNumDigit(_pChoiceCommander->_vMercenaryData[num]._move);
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}

		//용병 타입
		SelectObject(_MemDC, _font[1]);
		SetTextColor(_MemDC, RGB(255, 255, 255));
		pos.x = 512; pos.y = 176 + (nCnt * 48);

		if (_pChoiceCommander->_vMercenaryData[num]._type == 0){
			TextOut(_MemDC, _MapStartPos.x + pos.x + 4, _MapStartPos.y + pos.y + 4, "보병", strlen("보병"));
		}
		else if (_pChoiceCommander->_vMercenaryData[num]._type == 1){
			TextOut(_MemDC, _MapStartPos.x + pos.x + 4, _MapStartPos.y + pos.y + 4, "기병", strlen("기병"));
		}
		else if (_pChoiceCommander->_vMercenaryData[num]._type == 2){
			TextOut(_MemDC, _MapStartPos.x + pos.x + 4, _MapStartPos.y + pos.y + 4, "수병", strlen("수병"));
		}
		else if (_pChoiceCommander->_vMercenaryData[num]._type == 3){
			TextOut(_MemDC, _MapStartPos.x + pos.x + 4, _MapStartPos.y + pos.y + 4, "비병", strlen("비병"));
		}

		nCnt++;
	}

	//페이지 버튼 출력
	if (_mercenaryCurList > 0){
		pos.x = 576; pos.y = 176;
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 240, 0, 16, 16);
	}
	if (_mercenaryCurList < _mercenaryMaxList){
		pos.x = 576; pos.y = 288;
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 224, 0, 16, 16);
	}

	//선택 되어지고 있는 용병 화살표 출력
	pos.x = 224; pos.y = 179 + _nCnoice2 * 48;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 160, 224, 15, 26);
}
void PkwBsNormalScene::MercenaryChoiceRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	float alpa = 200.0;
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	int nCnt;

	pos.x = 240; pos.y = 208;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 0, 256, 160, alpa);
	IMAGEMANAGER->findImage("UI(256X160)")->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y);

	//용병 이름 출력
	SelectObject(_MemDC, _font[2]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	pos.x = 256; pos.y = 240;
	TextOut(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, _pChoiceMercenary->_name, strlen(_pChoiceMercenary->_name));

	//용병 가격
	pos.x = 464; pos.y = 240;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 96, 0, 16, 16);
	pos.x = 448; pos.y = 240;
	numDigit = GetNumDigit(_pChoiceMercenary->_price);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//용병 공격력
	pos.x = 256; pos.y = 256;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16, 0, 16, 16);
	pos.x = 288; pos.y = 256;
	numDigit = GetNumDigit(_pChoiceMercenary->_atk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	pos.x = 304; pos.y = 256;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 16, 16, 16);
	pos.x = 336; pos.y = 256;
	numDigit = GetNumDigit(_pChoiceCommander->_commandAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//용병 방어력
	pos.x = 368; pos.y = 256;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 48, 0, 16, 16);
	pos.x = 400; pos.y = 256;
	numDigit = GetNumDigit(_pChoiceMercenary->_def);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}
	pos.x = 416; pos.y = 256;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 0, 16, 16, 16);
	pos.x = 448; pos.y = 256;
	numDigit = GetNumDigit(_pChoiceCommander->_commandDef);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
		pos.x -= 16;
	}

	//용병 이미지 출력
	pos.x = 272; pos.y = 272;
	IMAGEMANAGER->findImage(_pChoiceMercenary->_imageKey)->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 180, 0, 60, 64);

	//용병 병력 차출 수 
	pos.x = 352; pos.y = 304;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 128, 48, 16, 16);
	pos.x = 368; pos.y = 301;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 16 * _pChoiceCommander->_vArms.size(), 224, 16, 19);
	pos.x = 384; pos.y = 304;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32, 32, 16, 16);
	pos.x = 400; pos.y = 304;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 32 + (16 * _pChoiceCommander->_maxArms), 16, 16, 16);

	//용병 이름 출력
	SelectObject(_MemDC, _font[1]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	pos.x = 416; pos.y = 304;
	TextOut(_MemDC, _MapStartPos.x + pos.x + 5, _MapStartPos.y + pos.y + 5, "보병", strlen("보병"));

	//결정 버튼 출력
	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	pos.x = 288; pos.y = 324;
	TextOut(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, "결정", strlen("결정"));

	//선택 화살표 출력 _nCnoice2
	pos.x = 256; pos.y = 291 + _nCnoice3 * 32;
	infoImage->render(_MemDC, _MapStartPos.x + pos.x, _MapStartPos.y + pos.y, 160, 224, 15, 26);
}

//기능 함수
NumDigit PkwBsNormalScene::GetNumDigit(int num)
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

void PkwBsNormalScene::createMercenary(int num)
{
	int number = _mercenaryCurList * 3 + num;
	int size = _pChoiceCommander->_vArms.size();

	PkwCharactor charactor;
	MercenaryData temp = _pChoiceCommander->_vMercenaryData[number];
	POINT pos;

	if (DATAMANAGER->_money < temp._price) return;

	if (size >= 0 && size <= 5){
		charactor._commander = _pChoiceCommander;
		memcpy(charactor._name, temp._name, strlen(temp._name));
		charactor._armyType = _pChoiceCommander->_armyType;
		charactor._mercenartOrder = _pChoiceCommander->_mercenartOrder;

		if (size == 0){
			pos.x = 0;	pos.y = -1;
		}
		else if (size == 1){
			pos.x = -1;	pos.y = 0;
		}
		else if (size == 2){
			pos.x = 0;	pos.y = 1;
		}
		else if (size == 3){
			pos.x = 1;	pos.y = 0;
		}
		else if (size == 4){
			pos.x = -1;	pos.y = -1;
		}
		else if (size == 5){
			pos.x = 1;	pos.y = 1;
		}
		charactor._pos = pos;
		charactor.MercenarySet(temp);
		_pChoiceCommander->_vArms.push_back(charactor);
		DATAMANAGER->_money -= charactor._price;
	}
	else{
		//6명 이상있으면 더이상 생산 안한다.
	}
}

void PkwBsNormalScene::mercenaryRepos()
{
	POINT commander;
	POINT mercenary;

	commander = _pChoiceCommander->_pos;
	for (int i = 0; i < _pChoiceCommander->_vArms.size(); i++){
		if (i == 0){
			mercenary.x = commander.x;
			mercenary.y = commander.y - 1;
		}
		else if (i == 1){
			mercenary.x = commander.x - 1;
			mercenary.y = commander.y;
		}
		else if (i == 2){
			mercenary.x = commander.x;
			mercenary.y = commander.y + 1;
		}
		else if (i == 3){
			mercenary.x = commander.x + 1;
			mercenary.y = commander.y;
		}
		else if (i == 4){
			mercenary.x = commander.x - 1;
			mercenary.y = commander.y - 1;
		}
		else if (i == 5){
			mercenary.x = commander.x + 1;
			mercenary.y = commander.y + 1;
		}
		_pChoiceCommander->_vArms[i]._pos = mercenary;
	}
}

void PkwBsNormalScene::mouseImageFrameUpdate()
{
	_mouseImageFrame.elaspedTime += _elapsed;
	if (_mouseImageFrame.elaspedTime < _mouseImageFrame.frameDelay) return;

	_mouseImageFrame.elaspedTime = 0.0;

	if (_mouseImageFrame.frameDir == NORMAL_DIR){
		_mouseImageFrame.frameX++;
		if (_mouseImageFrame.frameX >= _mouseImageFrame.maxFrameX){
			_mouseImageFrame.frameX = _mouseImageFrame.maxFrameX - 1;
			_mouseImageFrame.frameDir = REVERSE_DIR;
		}
	}
	else if (_mouseImageFrame.frameDir == REVERSE_DIR){
		_mouseImageFrame.frameX--;
		if (_mouseImageFrame.frameX < 0){
			_mouseImageFrame.frameX = 0;
			_mouseImageFrame.frameDir = NORMAL_DIR;
		}
	}
}
