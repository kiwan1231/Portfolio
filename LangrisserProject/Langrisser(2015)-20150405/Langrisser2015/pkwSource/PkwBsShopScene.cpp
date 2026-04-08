#include "../stdafx.h"
#include "PkwBsShopScene.h"


PkwBsShopScene::PkwBsShopScene()
{
}


PkwBsShopScene::~PkwBsShopScene()
{
}

HRESULT PkwBsShopScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();
	_shopMode = SHOP_NORMAL;
	_itemMaxPage = 0;
	_itemCurPage = 0;
	_nChoice = 0;
	_elapsed = 0.0;

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;
	_gameView.left = 0; _gameView.right = _nWindowWidth;
	_gameView.top = 0; _gameView.bottom = _nWindowHeight;

	
	_backBuffer = NULL;
	_backBuffer = new image;
	if (FAILED(_backBuffer->init(_nWindowWidth, _nWindowHeight))){
		SAFE_DELETE(_backBuffer);		_backBuffer = NULL;
		return E_FAIL;
	}

	_MemDC = _backBuffer->getMemDC();

	SetBkMode(_MemDC, TRANSPARENT);

	// 폰트 set
	_font[0] = CreateFont(28, 0, 0, 0, FW_EXTRALIGHT, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
	_font[1] = CreateFont(22, 0, 0, 0, FW_THIN, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "굴림");
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
void PkwBsShopScene::release(void)
{
	for (int i = 0; i < 10; i++){
		if (_font[i] != NULL)	DeleteObject(_font[i]);
	}

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
	}
}
void PkwBsShopScene::update(void)
{
	_elapsed = TIMEMANAGER->getElapsedTime();
	mouseImageFrameUpdate();

	if (_shopMode == SHOP_NORMAL){
		_itemMaxPage = (DATAMANAGER->_vPlayerItem.size() - 1) / 5;
		shopNormalUpdate();
	}
	else if (_shopMode == SHOP_EQUIP_ATTACKITEM){
		int atkItemSize = 0;
		for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
			if (DATAMANAGER->_vPlayerItem[i]._type == 0)//공격아이템 일경우
				atkItemSize++;
		}
		_itemMaxPage = (atkItemSize - 1) / 5;
		shopEquipAtkItemUpdate();
	}
	else if (_shopMode == SHOP_EQUIP_DEFENCEITEM){
		int defItemSize = 0;
		for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
			if (DATAMANAGER->_vPlayerItem[i]._type == 1)//방어아이템 일경우
				defItemSize++;
		}
		_itemMaxPage = (defItemSize - 1) / 5;
		shopEquipDefenceItemUpdate();
	}
	else if (_shopMode == SHOP_BUY){
		_itemMaxPage = (DATAMANAGER->_vShopData.size() - 1) / 5;
		shopBuyUpdate();
	}
	else if (_shopMode == SHOP_SELL){
		_itemMaxPage = (DATAMANAGER->_vPlayerItem.size() - 1) / 5;
		shopSellUpdate();
	}
}
void PkwBsShopScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	//맵 그리기
	IMAGEMANAGER->findImage("shop")->render(_MemDC);

	//인터페이스 그리기
	InterfaceRender();

	// 경과 시간 그리기 getHDC()
	sprintf_s(str, "battle경과시간 : %f ptX : %d  ptY : %d", _elapsed, _ptMouse.x, _ptMouse.y);
	TextOut(_MemDC, 0, 0, str, strlen(str));

	//커서 그리기
	IMAGEMANAGER->findImage("titleCursor")->render(_MemDC, _ptMouse.x, _ptMouse.y, _mouseImageFrame.frameX * 46, 0, 46, 46);

	//백 이미지 그려주기
	_backBuffer->render(DATAMANAGER->_mainDc);
}

//update관련 함수
void PkwBsShopScene::shopNormalUpdate()
{
	RECT rc[5];
	for (int i = 0; i < 3; i++){
		rc[i].left = 304;
		rc[i].right = rc[i].left + 80;
		rc[i].top = 48 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}

	rc[3].left = 272;	rc[3].right = rc[3].left + 16;
	rc[3].top = 272;	rc[3].bottom = rc[3].top + 16;
	rc[4].left = 272;	rc[4].right = rc[4].left + 16;
	rc[4].top = 416;	rc[4].bottom = rc[4].top + 16;


	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < 5; i++){
			if (PtInRect(&rc[i],_ptMouse)){
				if (i == 0){
					_shopMode = SHOP_EQUIP_ATTACKITEM;
					_itemCurPage = 0;
				}
				else if (i == 1){
					_shopMode = SHOP_BUY;
					_itemCurPage = 0;
				}
				else if (i == 2){
					_shopMode = SHOP_SELL;
					_itemCurPage = 0;
				}
				else if (i == 3){
					_itemCurPage--;
					if (_itemCurPage < 0)	_itemCurPage++;
				}
				else if (i == 4){
					_itemCurPage++;
					if (_itemCurPage > _itemMaxPage)	_itemCurPage--;
				}
				_nChoice = 0;
			}
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_shopMode = SHOP_NORMAL;
		_itemMaxPage = 0;
		_itemCurPage = 0;
		_nChoice = 0;

		_callbackFunctionParameter(_obj);
	}
}
void PkwBsShopScene::shopEquipAtkItemUpdate()
{
	int atkItemNum = 0;
	int showItemNum = 0;

	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (DATAMANAGER->_vPlayerItem[i]._type == 0){//공격 아이템 일 경우
			atkItemNum++;
		}
	}

	showItemNum = atkItemNum - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	RECT *rc = new RECT[showItemNum + 2];// +2는 이전 페이지 다음 페이지 버튼 렉트 이다.
	for (int i = 0; i < showItemNum; i++){
		rc[i].left = 48;
		rc[i].right = rc[i].left + 208;
		rc[i].top = 272 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}
	rc[showItemNum].left = 272;		rc[showItemNum].right = rc[showItemNum].left + 16;
	rc[showItemNum].top = 272;		rc[showItemNum].bottom = rc[showItemNum].top + 16;
	rc[showItemNum + 1].left = 272;	rc[showItemNum + 1].right = rc[showItemNum + 1].left + 16;
	rc[showItemNum + 1].top = 416;	rc[showItemNum + 1].bottom = rc[showItemNum + 1].top + 16;

	// 입력받기
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showItemNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showItemNum){
					_itemCurPage--;
					if (_itemCurPage < 0)	_itemCurPage++;
				}
				else if (i == showItemNum + 1){
					_itemCurPage++;
					if (_itemCurPage > _itemMaxPage)	_itemCurPage--;
				}
				else{
					itemSetUp(i, true);
					_shopMode = SHOP_EQUIP_DEFENCEITEM;
					_itemCurPage = 0;
					_nChoice = 0;
				}
			}
		}
	}

	if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_shopMode = SHOP_NORMAL;
		_itemCurPage = 0;
		_nChoice = 0;
	}
	
	delete[] rc;
}
void PkwBsShopScene::shopEquipDefenceItemUpdate()
{
	int defItemNum = 0;
	int showItemNum = 0;

	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (DATAMANAGER->_vPlayerItem[i]._type == 1){//기타 아이템 일 경우
			defItemNum++;
		}
	}

	showItemNum = defItemNum - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	RECT *rc = new RECT[showItemNum + 2];// +2는 이전 페이지 다음 페이지 버튼 렉트 이다.
	for (int i = 0; i < showItemNum; i++){
		rc[i].left = 48;
		rc[i].right = rc[i].left + 208;
		rc[i].top = 272 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}
	rc[showItemNum].left = 272;		rc[showItemNum].right = rc[showItemNum].left + 16;
	rc[showItemNum].top = 272;		rc[showItemNum].bottom = rc[showItemNum].top + 16;
	rc[showItemNum + 1].left = 272;	rc[showItemNum + 1].right = rc[showItemNum + 1].left + 16;
	rc[showItemNum + 1].top = 416;	rc[showItemNum + 1].bottom = rc[showItemNum + 1].top + 16;

	// 입력받기
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showItemNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showItemNum){
					_itemCurPage--;
					if (_itemCurPage < 0)	_itemCurPage++;
				}
				else if (i == showItemNum + 1){
					_itemCurPage++;
					if (_itemCurPage > _itemMaxPage)	_itemCurPage--;
				}
				else{
					itemSetUp(i, false);
					_shopMode = SHOP_NORMAL;
					_itemCurPage = 0;
					_nChoice = 0;
				}
			}
		}
	}

	if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_shopMode = SHOP_NORMAL;
		_itemCurPage = 0;
		_nChoice = 0;
	}

	delete[] rc;
}
void PkwBsShopScene::shopBuyUpdate()
{
	int showItemNum = 0;

	showItemNum = DATAMANAGER->_vShopData.size() - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	RECT *rc = new RECT[showItemNum + 2];// +2는 이전 페이지 다음 페이지 버튼 렉트 이다.
	for (int i = 0; i < showItemNum; i++){
		rc[i].left = 48;
		rc[i].right = rc[i].left + 208;
		rc[i].top = 272 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}
	rc[showItemNum].left = 272;		rc[showItemNum].right = rc[showItemNum].left + 16;
	rc[showItemNum].top = 272;		rc[showItemNum].bottom = rc[showItemNum].top + 16;
	rc[showItemNum + 1].left = 272;	rc[showItemNum + 1].right = rc[showItemNum + 1].left + 16;
	rc[showItemNum + 1].top = 416;	rc[showItemNum + 1].bottom = rc[showItemNum + 1].top + 16;

	// 입력받기
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showItemNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showItemNum){
					_itemCurPage--;
					if (_itemCurPage < 0)	_itemCurPage++;
					break;
				}
				else if (i == showItemNum + 1){
					_itemCurPage++;
					if (_itemCurPage > _itemMaxPage)	_itemCurPage--;
					break;
				}
				else{
					//아이템 을 산다
					int number = (_itemCurPage * 5) + i;
					
					if (DATAMANAGER->_money >= DATAMANAGER->_vShopData[number]._money){
						DATAMANAGER->_money -= DATAMANAGER->_vShopData[number]._money;
						DATAMANAGER->_vPlayerItem.push_back(DATAMANAGER->_vShopData[number]);
					}
					break;
				}
			}
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_shopMode = SHOP_NORMAL;
		_itemCurPage = 0;
		_nChoice = 0;
	}
	delete[] rc;
}
void PkwBsShopScene::shopSellUpdate()
{
	int showItemNum = 0;

	showItemNum = DATAMANAGER->_vPlayerItem.size() - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	RECT *rc = new RECT[showItemNum + 2];// +2는 이전 페이지 다음 페이지 버튼 렉트 이다.

	for (int i = 0; i < showItemNum; i++){
		rc[i].left = 48;
		rc[i].right = rc[i].left + 208;
		rc[i].top = 272 + (i * 32);
		rc[i].bottom = rc[i].top + 32;

		if (PtInRect(&rc[i], _ptMouse)) _nChoice = i;
	}
	rc[showItemNum].left = 272;		rc[showItemNum].right = rc[showItemNum].left + 16;
	rc[showItemNum].top = 272;		rc[showItemNum].bottom = rc[showItemNum].top + 16;
	rc[showItemNum + 1].left = 272;	rc[showItemNum + 1].right = rc[showItemNum + 1].left + 16;
	rc[showItemNum + 1].top = 416;	rc[showItemNum + 1].bottom = rc[showItemNum + 1].top + 16;

	// 입력받기
	if (KEYMANAGER->isOnceKeyDown(VK_LBUTTON)){
		for (int i = 0; i < showItemNum + 2; i++){
			if (PtInRect(&rc[i], _ptMouse)){
				if (i == showItemNum){
					_itemCurPage--;
					if (_itemCurPage < 0)	_itemCurPage++;
					break;
				}
				else if (i == showItemNum + 1){
					_itemCurPage++;
					if (_itemCurPage > _itemMaxPage)	_itemCurPage--;
					break;
				}
				else{
					//아이템 을 판다
					vector<ItemData>::iterator	viPlayerData;
					viPlayerData = DATAMANAGER->_vPlayerItem.begin();
					int number = (_itemCurPage * 5) + i;
					for (int j = 0; j < number; j++)
						viPlayerData++;

					DATAMANAGER->_money += DATAMANAGER->_vPlayerItem[number]._money;
					DATAMANAGER->_vPlayerItem.erase(viPlayerData);
					break;
				}
			}
		}
	}
	if (KEYMANAGER->isOnceKeyDown(VK_RBUTTON)){
		_shopMode = SHOP_NORMAL;
		_itemCurPage = 0;
		_nChoice = 0;
	}
	delete[] rc;
}

//render관련 함수
void PkwBsShopScene::InterfaceRender()
{
	//공통적으로 그려줘야 하는 이미지 그려주기
	InfoRender();

	if (_shopMode == SHOP_NORMAL){
		shopNormalRender();
	}
	else if (_shopMode == SHOP_EQUIP_ATTACKITEM){
		shopEquipAtkItemRender();
	}
	else if (_shopMode == SHOP_EQUIP_DEFENCEITEM){
		shopEquipDefenceItemRender();
	}
	else if (_shopMode == SHOP_BUY){
		shopBuyRender();
	}
	else if (_shopMode == SHOP_SELL){
		shopSellRender();
	}
}
void PkwBsShopScene::InfoRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));
	TextOut(_MemDC, 48 + 2, 48 + 2, "아이템장비", strlen("아이템장비"));

	//선택된 지휘관 이미지 출력
	pos.x = 48; pos.y = 96;
	IMAGEMANAGER->findImage(_pChoiceCommander->_imageKey)->render(_MemDC, pos.x - 12, pos.y, 180, 0, 60, 64);

	//선택된 지휘관 이름 출력
	SelectObject(_MemDC, _font[1]);
	pos.x = 96; pos.y = 96;
	TextOut(_MemDC, pos.x + 5, pos.x + 5, _pChoiceCommander->_name, strlen(_pChoiceCommander->_name));

	//선택된 지휘관 클레스 출력
	pos.x = 128; pos.y = 128;
	TextOut(_MemDC, pos.x + 5, pos.x + 5, _pChoiceCommander->_chClass, strlen(_pChoiceCommander->_chClass));

	//선택된 지휘관 공격력
	pos.x = 48; pos.y = 160;
	infoImage->render(_MemDC, pos.x, pos.y, 16, 0, 16, 16);//a출력
	pos.x = 64; pos.y = 160;
	infoImage->render(_MemDC, pos.x, pos.y, 32, 0, 16, 16);//t출력

	pos.x = 112; pos.y = 160;
	numDigit = GetNumDigit(_pChoiceCommander->_atk + _pChoiceCommander->_atkItem._atk + _pChoiceCommander->_DefItem._atk);
	for (int i = 0; i < numDigit.strlen; i++){
		//공격력 숫자 출력
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//선택된 지휘관 방어력
	pos.x = 48; pos.y = 176;
	infoImage->render(_MemDC, pos.x, pos.y, 48, 0, 16, 16);//d출력
	pos.x = 64; pos.y = 176;
	infoImage->render(_MemDC, pos.x, pos.y, 64, 0, 16, 16);//f출력

	pos.x = 112; pos.y = 176;
	numDigit = GetNumDigit(_pChoiceCommander->_def + _pChoiceCommander->_atkItem._def + _pChoiceCommander->_DefItem._def);
	for (int i = 0; i < numDigit.strlen; i++){
		//방여력 숫자 출력
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//선택된 지휘관 마나
	pos.x = 144; pos.y = 160;
	infoImage->render(_MemDC, pos.x, pos.y, 80, 0, 16, 16);//m출력
	pos.x = 160; pos.y = 160;
	infoImage->render(_MemDC, pos.x, pos.y, 96, 0, 16, 16);//p출력

	//현재 마나 숫자 출력
	pos.x = 208; pos.y = 160;
	numDigit = GetNumDigit(_pChoiceCommander->_curMp);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	pos.x = 224; pos.y = 160;
	infoImage->render(_MemDC, pos.x, pos.y, 32, 32, 16, 16);// '/'출력

	//최대 마나 숫자 출력
	pos.x = 256; pos.y = 160;
	numDigit = GetNumDigit(_pChoiceCommander->_maxMp);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//선택된 지휘관 이동력
	pos.x = 144; pos.y = 176;
	infoImage->render(_MemDC, pos.x, pos.y, 80, 0, 16, 16);//m출력
	pos.x = 160; pos.y = 176;
	infoImage->render(_MemDC, pos.x, pos.y, 112, 0, 16, 16);//v출력

	//이동력 숫자 출력
	pos.x = 208; pos.y = 176;
	numDigit = GetNumDigit(_pChoiceCommander->_move + _pChoiceCommander->_DefItem._move);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//지휘범위 출력
	pos.x = 64; pos.y = 192;
	infoImage->render(_MemDC, pos.x, pos.y, 0, 48, 64, 16);//'지휘범위'출력
	//지휘범위 숫자 출력
	pos.x = 96; pos.y = 208;
	numDigit = GetNumDigit(_pChoiceCommander->_commandRange + _pChoiceCommander->_DefItem._commandRange);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//수정 출력
	pos.x = 176; pos.y = 192;
	infoImage->render(_MemDC, pos.x, pos.y, 64, 48, 32, 16);
	
	//공격 수정치 출력
	pos.x = 128; pos.y = 208;
	infoImage->render(_MemDC, pos.x, pos.y, 16, 0, 16, 16);//'A'출력
	pos.x = 144; pos.y = 208;
	infoImage->render(_MemDC, pos.x, pos.y, 0, 16, 16, 16);//'+'출력
	//공격 수정치 숫자 출력
	pos.x = 176; pos.y = 208;
	numDigit = GetNumDigit(_pChoiceCommander->_commandAtk + _pChoiceCommander->_atkItem._mercenaryAtk + _pChoiceCommander->_DefItem._mercenaryAtk);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	//방어 수정치 출력
	pos.x = 208; pos.y = 208;
	infoImage->render(_MemDC, pos.x, pos.y, 48, 0, 16, 16);//'D'출력
	pos.x = 224; pos.y = 208;
	infoImage->render(_MemDC, pos.x, pos.y, 0, 16, 16, 16);//'+'출력
	//방어 수정치 숫자 출력
	pos.x = 256; pos.y = 208;
	numDigit = GetNumDigit(_pChoiceCommander->_commandDef + _pChoiceCommander->_atkItem._mercenaryDef + _pChoiceCommander->_DefItem._mercenaryDef);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 32 + (16 * numDigit.value[i]), 16, 16, 16);
		pos.x -= 16;
	}

	int sourX, sourY, posX;
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		pos.x = 416; pos.y = 16 + (i * 48);
		//지휘관 이미지 출력
		IMAGEMANAGER->findImage(_pCharactorPlay->_vCoCommander[i]._imageKey)->render(_MemDC, pos.x - 12, pos.y, 180, 0, 60, 64);

		//공격 아이템 출력
		sourX = (_pCharactorPlay->_vCoCommander[i]._atkItem._imageNum % 8) * 32;
		sourY = (_pCharactorPlay->_vCoCommander[i]._atkItem._imageNum / 8) * 32;
		IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x + 48, pos.y + 16, sourX, sourY, 32, 32);
		
		//방어 아이템 출력
		sourX = (_pCharactorPlay->_vCoCommander[i]._DefItem._imageNum % 8) * 32;
		sourY = (_pCharactorPlay->_vCoCommander[i]._DefItem._imageNum / 8) * 32;
		IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x + 80, pos.y + 16, sourX, sourY, 32, 32);

		//공격  출력
		infoImage->render(_MemDC, pos.x + 128, pos.y + 16, 16, 0, 16, 16);//'A'출력

		posX = 160;
		numDigit = GetNumDigit(_pCharactorPlay->_vCoCommander[i]._atk + _pCharactorPlay->_vCoCommander[i]._atkItem._atk + _pCharactorPlay->_vCoCommander[i]._DefItem._atk);
		for (int j = 0; j < numDigit.strlen; j++){
			infoImage->render(_MemDC, pos.x + posX, pos.y + 16, 32 + (16 * numDigit.value[j]), 16, 16, 16);
			posX -= 16;
		}

		//방어 출력
		infoImage->render(_MemDC, pos.x + 128, pos.y + 32, 48, 0, 16, 16);//'D'출력

		posX = 160;
		numDigit = GetNumDigit(_pCharactorPlay->_vCoCommander[i]._def + _pCharactorPlay->_vCoCommander[i]._atkItem._def + _pCharactorPlay->_vCoCommander[i]._DefItem._def);
		for (int j = 0; j < numDigit.strlen; j++){
			infoImage->render(_MemDC, pos.x + posX, pos.y + 32, 32 + (16 * numDigit.value[j]), 16, 16, 16);
			posX -= 16;
		}
	}
}
void PkwBsShopScene::shopNormalRender()
{
	float alpa = 150.0;
	POINT pos;

	pos.x = 288; pos.y = 32;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 112, 128, alpa);
	IMAGEMANAGER->findImage("UI(112X128)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	pos.x = 320; pos.y = 48;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "장비", strlen("장비"));
	pos.x = 320; pos.y = 80;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "사기", strlen("사기"));
	pos.x = 320; pos.y = 112;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "팔기", strlen("팔기"));

	SelectObject(_MemDC, _font[1]);

	//아이템 창 출력
	pos.x = 16; pos.y = 224;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 288, 224, alpa);
	IMAGEMANAGER->findImage("UI(288X224)")->render(_MemDC, pos.x, pos.y);
	pos.x = 32; pos.y = 240;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "가지고 있는 아이템", strlen("가지고 있는 아이템"));

	int itemStart = _itemCurPage * 5;
	int sourX, sourY;
	for (int i = itemStart; i < itemStart + 5; i++){
		if (i >= DATAMANAGER->_vPlayerItem.size()) break;

		pos.x = 48; pos.y = 272 + (i - itemStart) * 32;

		//아이템 이미지 출력
		sourX = (DATAMANAGER->_vPlayerItem[i]._imageNum % 8) * 32;
		sourY = (DATAMANAGER->_vPlayerItem[i]._imageNum / 8) * 32;
		IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x, pos.y, sourX, sourY, 32, 32);
		//아이템 이름 출력
		TextOut(_MemDC, pos.x + 32 + 5, pos.y + 5, DATAMANAGER->_vPlayerItem[i]._name, strlen(DATAMANAGER->_vPlayerItem[i]._name));
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	pos.x = 304; pos.y = 48 + (_nChoice * 32);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_itemCurPage != 0){
		pos.x = 272; pos.y = 272;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_itemCurPage != _itemMaxPage){
		pos.x = 272; pos.y = 416;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 224, 0, 16, 16);
	}
}
void PkwBsShopScene::shopEquipAtkItemRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	pos.x = 272; pos.y = 32;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 128, 64, alpa);
	IMAGEMANAGER->findImage("UI(128X64)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	pos.x = 304; pos.y = 48;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "장비", strlen("장비"));

	pos.x = 16; pos.y = 224;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 288, 224, alpa);
	IMAGEMANAGER->findImage("UI(288X224)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[1]);

	pos.x = 32; pos.y = 240;
	TextOut(_MemDC, pos.x + 5, pos.y + 5, "무기의 장비", strlen("무기의 장비"));

	//공격 아이템 출력
	int atkItemNum = 0;
	int showItemNum = 0;
	int itemStart;
	int sourX, sourY;

	//공격 아이템 총 갯수 저장
	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (DATAMANAGER->_vPlayerItem[i]._type == 0){
			atkItemNum++;
		}
	}

	//출력되어지는 공격 아이템 갯수 저장
	showItemNum = atkItemNum - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	//출력되어지는 공격 아이템 시작 위치
	itemStart = _itemCurPage * 5;

	//기타 아이템 정보 출력
	int cnt = 0;
	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (cnt - itemStart >= 5) break;

		if (DATAMANAGER->_vPlayerItem[i]._type == 0){
			if (cnt >= itemStart){
				pos.x = 48; pos.y = 272 + (cnt - itemStart) * 32;
				//아이템 이미지 출력
				sourX = (DATAMANAGER->_vPlayerItem[i]._imageNum % 8) * 32;
				sourY = (DATAMANAGER->_vPlayerItem[i]._imageNum / 8) * 32;
				IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x, pos.y, sourX, sourY, 32, 32);
				//아이템 이름 출력
				TextOut(_MemDC, pos.x + 32 + 5, pos.y + 5, DATAMANAGER->_vPlayerItem[i]._name, strlen(DATAMANAGER->_vPlayerItem[i]._name));
			}
			cnt++;
		}
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	pos.x = 32; pos.y = 272 + (_nChoice * 32);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_itemCurPage != 0){
		pos.x = 272; pos.y = 272;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_itemCurPage != _itemMaxPage){
		pos.x = 272; pos.y = 416;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 224, 0, 16, 16);
	}
}
void PkwBsShopScene::shopEquipDefenceItemRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	pos.x = 272; pos.y = 32;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 128, 64, alpa);
	IMAGEMANAGER->findImage("UI(128X64)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	pos.x = 304; pos.y = 48;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "장비", strlen("장비"));

	pos.x = 16; pos.y = 224;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 288, 224, alpa);
	IMAGEMANAGER->findImage("UI(288X224)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[1]);

	pos.x = 32; pos.y = 240;
	TextOut(_MemDC, pos.x + 5, pos.y + 5, "기타의 장비", strlen("기타의 장비"));

	//기타 아이템 출력
	int defItemNum = 0;
	int showItemNum = 0;
	int itemStart;
	int sourX, sourY;

	//공격 아이템 총 갯수 저장
	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (DATAMANAGER->_vPlayerItem[i]._type == 1){
			defItemNum++;
		}
	}
	//출력되어지는 공격 아이템 갯수 저장
	showItemNum = defItemNum - (_itemCurPage * 5);
	if (showItemNum > 5)	showItemNum = 5;

	//출력되어지는 공격 아이템 시작 위치
	itemStart = _itemCurPage * 5;

	//기타 아이템 정보 출력
	int cnt = 0;
	for (int i = 0; i < DATAMANAGER->_vPlayerItem.size(); i++){
		if (cnt - itemStart >= 5) break;

		if (DATAMANAGER->_vPlayerItem[i]._type == 1){
			if (cnt >= itemStart){
				pos.x = 48; pos.y = 272 + (cnt - itemStart) * 32;
				//아이템 이미지 출력
				sourX = (DATAMANAGER->_vPlayerItem[i]._imageNum % 8) * 32;
				sourY = (DATAMANAGER->_vPlayerItem[i]._imageNum / 8) * 32;
				IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x, pos.y, sourX, sourY, 32, 32);
				//아이템 이름 출력
				TextOut(_MemDC, pos.x + 32 + 5, pos.y + 5, DATAMANAGER->_vPlayerItem[i]._name, strlen(DATAMANAGER->_vPlayerItem[i]._name));
			}
			cnt++;
		}
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	pos.x = 32; pos.y = 272 + (_nChoice * 32);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_itemCurPage != 0){
		pos.x = 272; pos.y = 272;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_itemCurPage != _itemMaxPage){
		pos.x = 272; pos.y = 416;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 224, 0, 16, 16);
	}
}
void PkwBsShopScene::shopBuyRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	pos.x = 272; pos.y = 32;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 128, 64, alpa);
	IMAGEMANAGER->findImage("UI(128X64)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	pos.x = 304; pos.y = 48;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "사기", strlen("사기"));

	//아이템 창 출력
	pos.x = 16; pos.y = 224;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 384, 224, alpa);
	IMAGEMANAGER->findImage("UI(384X224)")->render(_MemDC, pos.x, pos.y);


	SelectObject(_MemDC, _font[1]);
	pos.x = 32; pos.y = 240;
	TextOut(_MemDC, pos.x + 2, pos.y + 5, "구입리스트", strlen("구입리스트"));
	pos.x = 176; pos.y = 240;
	TextOut(_MemDC, pos.x + 2, pos.y + 5, "소지금", strlen("소지금"));

	//소지금 그리기
	pos.x = 368; pos.y = 256;
	infoImage->render(_MemDC, pos.x, pos.y, 144, 0, 16, 16);

	pos.x = 352; pos.y = 256 - 3;
	numDigit = GetNumDigit(DATAMANAGER->_money);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 16 * numDigit.value[i], 224, 16, 19);
		pos.x -= 16;
	}


	int itemStart = _itemCurPage * 5;
	int sourX, sourY;
	
	for (int i = itemStart; i < itemStart + 5; i++){
		if (i >= DATAMANAGER->_vShopData.size()) break;

		pos.x = 48; pos.y = 272 + (i - itemStart) * 32;

		//아이템 이미지 출력
		sourX = (DATAMANAGER->_vShopData[i]._imageNum % 8) * 32;
		sourY = (DATAMANAGER->_vShopData[i]._imageNum / 8) * 32;
		IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x, pos.y, sourX, sourY, 32, 32);
		//아이템 이름 출력
		TextOut(_MemDC, pos.x + 32 + 5, pos.y + 5, DATAMANAGER->_vShopData[i]._name, strlen(DATAMANAGER->_vShopData[i]._name));

		//아이템 가격 출력
		infoImage->render(_MemDC, pos.x + 320, pos.y + 16, 144, 0, 16, 16);
		numDigit = GetNumDigit(DATAMANAGER->_vShopData[i]._money);
		pos.x = 352; pos.y = 288 + (i - itemStart) * 32;
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, pos.x, pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	pos.x = 32; pos.y = 272 + (_nChoice * 32);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_itemCurPage != 0){
		pos.x = 272; pos.y = 272;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_itemCurPage != _itemMaxPage){
		pos.x = 272; pos.y = 416;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 224, 0, 16, 16);
	}
}
void PkwBsShopScene::shopSellRender()
{
	//이미지 포인터 저장
	image* infoImage = IMAGEMANAGER->findImage("charactorUI");
	//이미지 그리는 위치
	POINT pos;
	//해당 숫자가 몇자리수인지 저장
	NumDigit numDigit;
	// 알파값
	float alpa = 150.0;

	pos.x = 272; pos.y = 32;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 128, 64, alpa);
	IMAGEMANAGER->findImage("UI(128X64)")->render(_MemDC, pos.x, pos.y);

	SelectObject(_MemDC, _font[0]);
	SetTextColor(_MemDC, RGB(255, 255, 255));

	pos.x = 304; pos.y = 48;
	TextOut(_MemDC, pos.x + 2, pos.y + 2, "팔기", strlen("팔기"));

	//아이템 창 출력
	pos.x = 16; pos.y = 224;
	IMAGEMANAGER->findImage("UI_BLUE_BACK")->alphaRender(_MemDC, pos.x, pos.y, 0, 0, 384, 224, alpa);
	IMAGEMANAGER->findImage("UI(384X224)")->render(_MemDC, pos.x, pos.y);


	SelectObject(_MemDC, _font[1]);
	pos.x = 32; pos.y = 240;
	TextOut(_MemDC, pos.x + 2, pos.y + 5, "매각리스트", strlen("매각리스트"));
	pos.x = 176; pos.y = 240;
	TextOut(_MemDC, pos.x + 2, pos.y + 5, "소지금", strlen("소지금"));

	//소지금 그리기
	pos.x = 368; pos.y = 256;
	infoImage->render(_MemDC, pos.x, pos.y, 144, 0, 16, 16);

	pos.x = 352; pos.y = 256 - 3;
	numDigit = GetNumDigit(DATAMANAGER->_money);
	for (int i = 0; i < numDigit.strlen; i++){
		infoImage->render(_MemDC, pos.x, pos.y, 16 * numDigit.value[i], 224, 16, 19);
		pos.x -= 16;
	}


	int itemStart = _itemCurPage * 5;
	int sourX, sourY;

	for (int i = itemStart; i < itemStart + 5; i++){
		if (i >= DATAMANAGER->_vPlayerItem.size()) break;

		pos.x = 48; pos.y = 272 + (i - itemStart) * 32;

		//아이템 이미지 출력
		sourX = (DATAMANAGER->_vPlayerItem[i]._imageNum % 8) * 32;
		sourY = (DATAMANAGER->_vPlayerItem[i]._imageNum / 8) * 32;
		IMAGEMANAGER->findImage("item")->render(_MemDC, pos.x, pos.y, sourX, sourY, 32, 32);
		//아이템 이름 출력
		TextOut(_MemDC, pos.x + 32 + 5, pos.y + 5, DATAMANAGER->_vPlayerItem[i]._name, strlen(DATAMANAGER->_vPlayerItem[i]._name));

		//아이템 가격 출력
		infoImage->render(_MemDC, pos.x + 320, pos.y + 16, 144, 0, 16, 16);
		numDigit = GetNumDigit(DATAMANAGER->_vPlayerItem[i]._money);

		pos.x = 352; pos.y = 288 + (i - itemStart) * 32;
		for (int i = 0; i < numDigit.strlen; i++){
			infoImage->render(_MemDC, pos.x, pos.y, 32 + 16 * numDigit.value[i], 16, 16, 16);
			pos.x -= 16;
		}
	}

	// 장비, 사기, 팔기 선택 화살표 출력
	pos.x = 32; pos.y = 272 + (_nChoice * 32);
	IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y + 3, 160, 224, 15, 26);

	// 아이템 창 전 버튼 출력
	if (_itemCurPage != 0){
		pos.x = 272; pos.y = 272;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 240, 0, 16, 16);
	}

	// 아이템 창 다음 버튼 출력
	if (_itemCurPage != _itemMaxPage){
		pos.x = 272; pos.y = 416;
		IMAGEMANAGER->findImage("charactorUI")->render(_MemDC, pos.x, pos.y, 224, 0, 16, 16);
	}
}

//기타 함수
NumDigit PkwBsShopScene::GetNumDigit(int num)
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

void PkwBsShopScene::itemSetUp(int num, bool isAtkItem)
{
	int number = (_itemCurPage * 5) + num;
	int cnt = 0;

	ItemData temp;
	vector<ItemData>::iterator ivPlayerData;

	if (isAtkItem){// 공격 아이템 장착
		for (ivPlayerData = DATAMANAGER->_vPlayerItem.begin(); ivPlayerData != DATAMANAGER->_vPlayerItem.end(); ivPlayerData++){
			if (ivPlayerData->_type == 0){
				if (cnt == number){
					temp = _pChoiceCommander->_atkItem;				// 선택 유닛 공격 아이템 임시 저장
					_pChoiceCommander->_atkItem = *ivPlayerData;	// 선택 유닛에 선택한 아이템 장착
					DATAMANAGER->_vPlayerItem.erase(ivPlayerData);	// 선택한 아이템 플레이어 아이템 창고에서 삭제
					DATAMANAGER->_vPlayerItem.push_back(temp);		// 유닛이 가지고 있던 아이템 아이템 창고에 넣기
					
					break;
				}
				else cnt++;
			}
		}
	}
	else{// 기타 아이템 장착
		for (ivPlayerData = DATAMANAGER->_vPlayerItem.begin(); ivPlayerData != DATAMANAGER->_vPlayerItem.end(); ivPlayerData++){
			if (ivPlayerData->_type == 1){
				if (cnt == number){
					temp = _pChoiceCommander->_DefItem;				// 선택 유닛 기타 아이템 임시 저장
					_pChoiceCommander->_DefItem = *ivPlayerData;	// 선택 유닛에 선택한 아이템 장착
					DATAMANAGER->_vPlayerItem.erase(ivPlayerData);	// 선택한 아이템 플레이어 아이템 창고에서 삭제
					DATAMANAGER->_vPlayerItem.push_back(temp);		// 유닛이 가지고 있던 아이템 아이템 창고에 넣기

					break;
				}
				else cnt++;
			}
		}
	}
}

void PkwBsShopScene::commanderListRender(int value, int type)
{

}

void PkwBsShopScene::mouseImageFrameUpdate()
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
