#include "../stdafx.h"
#include "PkwLoadingScene.h"


PkwLoadingScene::PkwLoadingScene()
{
}


PkwLoadingScene::~PkwLoadingScene()
{
}

HRESULT PkwLoadingScene::init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj)
{
	_obj = obj;
	_callbackFunctionParameter = static_cast<CALLBACK_FUNCTION_PARAMETER>(cbFunction);

	_nWindowWidth = _winResolutionRc.right - _winResolutionRc.left;
	_nWindowHeight = _winResolutionRc.bottom - _winResolutionRc.top;

	_backBuffer = NULL;
	_backBuffer = new image;
	
	if (FAILED(_backBuffer->init(_nWindowWidth, _nWindowHeight))){
		SAFE_DELETE(_backBuffer);	_backBuffer = NULL;
		return E_FAIL;
	}

	//dc 얻어오기
	_MemDC = _backBuffer->getMemDC();

	//글자 배경모드(TRANSPARENT : 투명 QPAQUEL : 불투명)
	SetBkMode(_MemDC, TRANSPARENT);

	// 폰트 set
	HFONT				font;
	font = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, 3, 2, 1, VARIABLE_PITCH | FF_ROMAN, "Times New Roman");
	SelectObject(_MemDC, font);
	SetTextColor(_MemDC, RGB(0, 0, 0));
	DeleteObject(font);

	//---------------------------------------scene image Load--------------------------------------------------
	IMAGEMANAGER->addImage("blackBack", "image\\scene\\blackBack.bmp", 640, 480, true, RGB(255, 0, 255));
	//---------------------------------------loadingScene image Load--------------------------------------------------
	IMAGEMANAGER->addImage("loadingScene", "image\\scene\\loadingScene.bmp",640, 480, true, RGB(255, 0, 255));

	_current = 0;
	ImageDataLoad();
}
void PkwLoadingScene::release(void)
{
	for (int i = 0; i < _vLoadItem.size(); i++){
		delete _vLoadItem[i];
		_vLoadItem[i] = NULL;
	}
	_vLoadItem.clear();

	if (_backBuffer != NULL){
		SAFE_DELETE(_backBuffer);
		_backBuffer = NULL;
	}
}
void PkwLoadingScene::update(void)
{
	_elapsed = TIMEMANAGER->getElapsedTime();

	if (!loadNext()){
		loadSound();
		_callbackFunctionParameter(_obj);
	}
}
void PkwLoadingScene::render(void)
{
	char str[128];

	if (_backBuffer == NULL) return;

	//배경 이미지 그리기
	IMAGEMANAGER->findImage("loadingScene")->render(_MemDC, 0, 0);
	
	// 경과 시간 그리기 getHDC()
	SetTextColor(_MemDC, RGB(250, 250, 250));
	sprintf_s(str, "loading Scene - elapse time : %f  ", _elapsed);
	TextOut(_MemDC, 0, 0, str, strlen(str));

	SetTextColor(_MemDC, RGB(0, 0, 0));
	sprintf_s(str, "%d / %d", _current, _vLoadItem.size());
	TextOut(_MemDC, 540, 300, str, strlen(str));

	_backBuffer->render(DATAMANAGER->_mainDc);
}

void PkwLoadingScene::loadImage(string keyName, int width, int height)
{
	loadItem* item = new loadItem;
	item->initForImage(keyName, width, height);
	_vLoadItem.push_back(item);
}

void PkwLoadingScene::loadImage(string keyName, const char* fileName, int x, int y, int width, int height, bool trans, COLORREF transColor)
{
	loadItem* item = new loadItem;
	item->initForImage(keyName, fileName, x, y, width, height, trans, transColor);
	_vLoadItem.push_back(item);
}

void PkwLoadingScene::loadImage(string keyName, const char* fileName, int width, int height, bool trans, COLORREF transColor)
{
	loadItem* item = new loadItem;
	item->initForImage(keyName, fileName, width, height, trans, transColor);
	_vLoadItem.push_back(item);
}

void PkwLoadingScene::loadFrameImage(string keyName, const char* fileName, int x, int y, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor)
{
	loadItem* item = new loadItem;
	item->initForFrameImage(keyName, fileName, x, y, width, height, frameX, frameY, trans, transColor);
	_vLoadItem.push_back(item);
}

void PkwLoadingScene::loadFrameImage(string keyName, const char* fileName, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor)
{
	loadItem* item = new loadItem;
	item->initForFrameImage(keyName, fileName, width, height, frameX, frameY, trans, transColor);
	_vLoadItem.push_back(item);
}

BOOL PkwLoadingScene::loadNext(void)
{
	//아이템 갯수가 벡터 사이즈 이상 넘어가면 뻘스
	if (_current >= _vLoadItem.size())
	{
		return false;
	}

	//로딩중
	loadItem* item = _vLoadItem[_current];

	switch (item->getLoadingKind())
	{
		case LOADING_KIND_IMAGE_00:
		{
		  tagImageResource imageResource = item->getImageResource();
		  IMAGEMANAGER->addImage(imageResource.keyName, imageResource.width, imageResource.height);
		}
		break;

		case LOADING_KIND_IMAGE_01:
		{
			tagImageResource imageResource = item->getImageResource();
			IMAGEMANAGER->addImage(imageResource.keyName,
				imageResource.fileName, imageResource.x, imageResource.y,
				imageResource.width, imageResource.height,
				imageResource.trans, imageResource.transColor);
		}
		break;

		case LOADING_KIND_IMAGE_02:
		{
			tagImageResource imageResource = item->getImageResource();
			IMAGEMANAGER->addImage(imageResource.keyName,
				imageResource.fileName,
				imageResource.width, imageResource.height,
				imageResource.trans, imageResource.transColor);
		}
		break;

		case LOADING_KIND_FRAMEIMAGE_00:
		{
			tagImageResource imageResource = item->getImageResource();
			IMAGEMANAGER->addFrameImage(imageResource.keyName,
				imageResource.fileName, imageResource.x, imageResource.y,
				imageResource.width, imageResource.height,
				imageResource.frameX, imageResource.frameY,
				imageResource.trans, imageResource.transColor);
		}
		break;

		case LOADING_KIND_FRAMEIMAGE_01:
		{
			tagImageResource imageResource = item->getImageResource();
			IMAGEMANAGER->addFrameImage(imageResource.keyName,
				imageResource.fileName, imageResource.width,
				imageResource.height, imageResource.frameX,
				imageResource.frameY, imageResource.trans,
				imageResource.transColor);
		}
		break;
		case LOADING_KIND_SOUND_00:
		{

		}
		break;
		case LOADING_KIND_END:
		{

		}
		break;
	}

	//로딩바 이미지 변경
	//_loadingBar->setGuage(_current, _vLoadItem.size());

	//카운트(아이템 로드 카운트)
	_current++;

	return true;
}

void PkwLoadingScene::ImageDataLoad()
{
	//---------------------------------------openingSceneImage Load--------------------------------------------------
	loadImage("titleBack", "image\\scene\\titleScene.bmp",640, 480, true, RGB(255, 0, 255));
	loadFrameImage("titleCursor", "image\\scene\\openingCursor.bmp",0,0,460, 46, 10, 1, true, RGB(255, 0, 255));
	loadFrameImage("titleChoice", "image\\scene\\openingChoice.bmp", 0, 0, 300, 35, 6, 1, true, RGB(255, 0, 255));
	//---------------------------------------------------------------------------------------------------------------
	//---------------------------------------scenarioSceneImage Load--------------------------------------------------
	loadImage("scenarioScene(ori)", "image\\scene\\scenarioScene(ori).bmp", 816, 1088, true, RGB(255, 0, 255));
	loadImage("scenarioScene(black)", "image\\scene\\scenarioScene(black).bmp", 816, 1088, true, RGB(255, 0, 255));
	//---------------------------------------------------------------------------------------------------------------
	//맵 이미지 로드 
	loadImage("backImage", "image\\map\\backImage.bmp", 2000, 3000, true, RGB(255, 0, 255));
	loadImage("map1", "image\\map\\map1(48).bmp", 1296, 2112, true, RGB(255, 0, 255));

	//캐릭터 애니 이미지 로드
	//지휘관
	loadFrameImage("0_Enwin", "image\\battleAni\\0_Enwin.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("1_Hein", "image\\battleAni\\1_Hein.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("2_liana", "image\\battleAni\\2_liana.bmp", 0, 0, 360, 512, 6, 8, true, RGB(255, 0, 255));
	/*IMAGEMANAGER->addFrameImage("3_Sherry", "image\\battleAni\\3_Sherry.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("4_Lana", "image\\battleAni\\4_Lana.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("5_Lowe", "image\\battleAni\\5_Lowe.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("6_Scott", "image\\battleAni\\6_Scott.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("7_Kiss", "image\\battleAni\\7_Kiss.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("8_Leicester", "image\\battleAni\\8_Leicester.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("9_Aaron", "image\\battleAni\\9_Aaron.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));

	IMAGEMANAGER->addFrameImage("10_Sonia", "image\\battleAni\\10_Sonia.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));*/
	loadFrameImage("11_Leon", "image\\battleAni\\11_Leon.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	/*IMAGEMANAGER->addFrameImage("12_Egbert", "image\\battleAni\\12_Egbert.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("13_Valgas", "image\\battleAni\\13_Valgas.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("14_Imelda", "image\\battleAni\\14_Imelda.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("15_Bernhard", "image\\battleAni\\15_Bernhard.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("16_Jessica", "image\\battleAni\\16_Jessica.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("17_Ladin", "image\\battleAni\\17_Ladin.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));
	IMAGEMANAGER->addFrameImage("18_Dieharte", "image\\battleAni\\18_Dieharte.bmp", 0, 0, 256, 512, 4, 8, true, RGB(255, 0, 255));*/
	
	// 일반 지휘관
	loadFrameImage("commanderGeneral", "image\\battleAni\\commanderGeneral.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("commanderKnight", "image\\battleAni\\commanderKnight.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("commanderSister", "image\\battleAni\\commanderSister.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));

	//용병
	loadFrameImage("19_souldier", "image\\battleAni\\19_souldier.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("20_trooper", "image\\battleAni\\20_trooper.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("21_harpv", "image\\battleAni\\21_harpv.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("22_merman", "image\\battleAni\\22_merman.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("spearman", "image\\battleAni\\spearman.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("monk", "image\\battleAni\\monk.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));
	loadFrameImage("civilian", "image\\battleAni\\civilian.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));

	//소환수
	loadFrameImage("23_prayer", "image\\battleAni\\23_prayer.bmp", 0, 0, 360, 128, 6, 2, true, RGB(255, 0, 255));

	//캐릭터 얼굴 이미지 로드
	loadImage("0_Enwin_face", "image\\charactorFace\\0_Enwin_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("1_Hein_face", "image\\charactorFace\\1_Hein_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("2_liana_face", "image\\charactorFace\\2_liana_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("3_Sherry_face", "image\\charactorFace\\3_Sherry_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("4_Lana_face", "image\\charactorFace\\4_Lana_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("5_Lowe_face", "image\\charactorFace\\5_Lowe_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("6_Scott_face", "image\\charactorFace\\6_Scott_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("7_Kiss_face", "image\\charactorFace\\7_Kiss_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("8_Leicester_face", "image\\charactorFace\\8_Leicester_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("9_Aaron_face", "image\\charactorFace\\9_Aaron_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("10_Sonia_face", "image\\charactorFace\\10_Sonia_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("11_Leon_face", "image\\charactorFace\\11_Leon_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("12_Egbert", "image\\charactorFace\\12_Egbert.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("13_Valgas_face", "image\\charactorFace\\13_Valgas_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("14_Imelda_face", "image\\charactorFace\\14_Imelda_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("15_Bernhard", "image\\charactorFace\\15_Bernhard.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("16_Jessica", "image\\charactorFace\\16_Jessica.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("17_Ladin", "image\\charactorFace\\17_Ladin.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("18_Dieharte", "image\\charactorFace\\18_Dieharte.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("19_Rihard_face", "image\\charactorFace\\19_Rihard_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("20_Balde_face", "image\\charactorFace\\20_Balde_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("21_Robin_face", "image\\charactorFace\\21_Robin_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("22_Evangeline_face", "image\\charactorFace\\22_Evangeline_face.bmp", 80, 96, true, RGB(255, 0, 255));

	//캐릭터 토크 이미지 로드
	loadImage("0_Enwin_talk", "image\\talkFace\\0_Enwin_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("1_Hein_talk", "image\\talkFace\\1_Hein_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("2_liana_talk", "image\\talkFace\\2_liana_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("3_Sherry_talk", "image\\talkFace\\3_Sherry_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("4_Lana_talk", "image\\talkFace\\4_Lana_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("5_Lowe_talk", "image\\talkFace\\5_Lowe_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("6_Scott_talk", "image\\talkFace\\6_Scott_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("7_Kiss_talk", "image\\talkFace\\7_Kiss_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("8_Leicester_talk", "image\\talkFace\\8_Leicester_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("9_Aaron_talk", "image\\talkFace\\9_Aaron_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("10_Sonia_talk", "image\\talkFace\\10_Sonia_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("11_Leon_talk", "image\\talkFace\\11_Leon_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("12_Egbert_talk", "image\\talkFace\\12_Egbert_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("13_Valgas_talk", "image\\talkFace\\13_Valgas_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("14_Imelda_talk", "image\\talkFace\\14_Imelda_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("15_Bernhard_talk", "image\\talkFace\\15_Bernhard_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("16_Jessica_talk", "image\\talkFace\\16_Jessica_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("17_Ladin_talk", "image\\talkFace\\17_Ladin_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("18_Dieharte_talk", "image\\talkFace\\18_Dieharte_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("19_Rihard_talk", "image\\talkFace\\19_Rihard_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("20_Balde_talk", "image\\talkFace\\20_Balde_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("21_Robin_talk", "image\\talkFace\\21_Robin_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("22_Evangeline_talk", "image\\talkFace\\22_Evangeline_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("General_talk", "image\\talkFace\\General_talk.bmp", 144, 238, true, RGB(255, 0, 255));
	loadImage("sister_talk", "image\\talkFace\\sister_talk.bmp", 144, 238, true, RGB(255, 0, 255));



	//----일반 지휘관 페이스 charactorAction 
	loadImage("General_face", "image\\charactorFace\\General_face.bmp", 80, 96, true, RGB(255, 0, 255));
	loadImage("sister_face", "image\\charactorFace\\sister_face.bmp", 80, 96, true, RGB(255, 0, 255));
	
	//유저인터페이스 이미지 로드
	loadImage("UI_BLUE_BACK", "image\\userInterface\\UI_BLUE_BACK.bmp", 640, 480, true, RGB(255, 0, 255));
	loadImage("UI_GREEN_BACK", "image\\userInterface\\UI_GREEN_BACK.bmp", 640, 480, true, RGB(255, 0, 255));
	loadImage("UI_RED_BACK", "image\\userInterface\\UI_RED_BACK.bmp", 640, 480, true, RGB(255, 0, 255));

	loadImage("UI(112X128)", "image\\userInterface\\UI(112X128).bmp", 112, 128, true, RGB(255, 0, 255));
	loadImage("UI(112X208)", "image\\userInterface\\UI(112X208).bmp", 112, 208, true, RGB(255, 0, 255));
	loadImage("UI(128X64)", "image\\userInterface\\UI(128X64).bmp", 128, 64, true, RGB(255, 0, 255));
	loadImage("UI(128X96)", "image\\userInterface\\UI(128X96).bmp", 128, 96, true, RGB(255, 0, 255));
	loadImage("UI(128X112)", "image\\userInterface\\UI(128X112).bmp", 128, 112, true, RGB(255, 0, 255));
	loadImage("UI(128X160)", "image\\userInterface\\UI(128X160).bmp", 128, 160, true, RGB(255, 0, 255));
	loadImage("UI(128X208)", "image\\userInterface\\UI(128X208).bmp", 128, 208, true, RGB(255, 0, 255));
	loadImage("UI(128X256)", "image\\userInterface\\UI(128X256).bmp", 128, 256, true, RGB(255, 0, 255));
	loadImage("UI(128X304)", "image\\userInterface\\UI(128X304).bmp", 128, 304, true, RGB(255, 0, 255));
	loadImage("UI(144X64)", "image\\userInterface\\UI(144X64).bmp", 144, 64, true, RGB(255, 0, 255));
	loadImage("UI(208X64)", "image\\userInterface\\UI(208X64).bmp", 208, 64, true, RGB(255, 0, 255));
	loadImage("UI(240X64)", "image\\userInterface\\UI(240X64).bmp", 240, 64, true, RGB(255, 0, 255));
	loadImage("UI(240X160)", "image\\userInterface\\UI(240X160).bmp", 240, 160, true, RGB(255, 0, 255));
	loadImage("UI(240X208)", "image\\userInterface\\UI(240X208).bmp", 240, 208, true, RGB(255, 0, 255));
	loadImage("UI(240X256)", "image\\userInterface\\UI(240X256).bmp", 240, 256, true, RGB(255, 0, 255));
	loadImage("UI(256X160)", "image\\userInterface\\UI(256X160).bmp", 256, 160, true, RGB(255, 0, 255));
	loadImage("UI(256X192)", "image\\userInterface\\UI(256X192).bmp", 256, 192, true, RGB(255, 0, 255));
	loadImage("UI(288X224)", "image\\userInterface\\UI(288X224).bmp", 288, 224, true, RGB(255, 0, 255));
	loadImage("UI(320X208)", "image\\userInterface\\UI(320X208).bmp", 320, 208, true, RGB(255, 0, 255));
	loadImage("UI(352X192)", "image\\userInterface\\UI(352X192).bmp", 352, 192, true, RGB(255, 0, 255));
	loadImage("UI(384X224)", "image\\userInterface\\UI(384X224).bmp", 384, 224, true, RGB(255, 0, 255));
	loadImage("UI(400X176)", "image\\userInterface\\UI(400X176).bmp", 400, 176, true, RGB(255, 0, 255));
	loadImage("UI(480X304)", "image\\userInterface\\UI(480X304).bmp", 480, 304, true, RGB(255, 0, 255));
	loadImage("UI(528X128)", "image\\userInterface\\UI(528X128).bmp", 528, 128, true, RGB(255, 0, 255));
	loadImage("UI(576X160)", "image\\userInterface\\UI(576X160).bmp", 576, 160, true, RGB(255, 0, 255));

	loadImage("UI(160X192)", "image\\userInterface\\UI(160X192).bmp", 160, 192, true, RGB(255, 0, 255));
	loadImage("UI(160X400)", "image\\userInterface\\UI(160X400).bmp", 160, 400, true, RGB(255, 0, 255));
	
	loadImage("UI(400X320)", "image\\userInterface\\UI(400X320).bmp", 400, 320, true, RGB(255, 0, 255));

	loadImage("charactorfaceWin", "image\\userInterface\\charactorfaceWin.bmp", 112, 128, true, RGB(255, 0, 255));
	loadImage("choiceArrow", "image\\userInterface\\choiceArrow.bmp", 15, 30, true, RGB(255, 0, 255));
	loadImage("charactorUI", "image\\userInterface\\charactorUI.bmp", 256, 256, true, RGB(255, 0, 255));
	loadImage("charactorAction", "image\\userInterface\\charactorAction.bmp", 384, 256, true, RGB(255, 0, 255));

	loadImage("scenarioClear", "image\\userInterface\\scenarioClear.bmp", 448, 64, true, RGB(255, 0, 255));
	loadImage("gameOver", "image\\userInterface\\gameOver.bmp", 288, 64, true, RGB(255, 0, 255));

	loadImage("downBar", "image\\userInterface\\downBar.bmp", 640, 80, true, RGB(255, 67, 255));
	loadImage("groundIcon", "image\\userInterface\\groundIcon.bmp", 480, 32, true, RGB(255, 0, 255));

	loadImage("shop", "image\\scene\\shop.bmp", 640, 480, true, RGB(255, 0, 255));

	//아이템 이미지 로드
	loadFrameImage("item", "image\\item\\item.bmp", 0, 0, 256, 160, 8, 5, true, RGB(255, 0, 255));

	//배틀셋 씬 이미지 로드
	loadFrameImage("commanderPos", "image\\userInterface\\commanderPos.bmp", 0, 0, 48, 144, 1, 3, true, RGB(255, 0, 255));

	//이펙트 이미지
	loadFrameImage("effectHeal", "image\\effect\\effectHeal.bmp", 0, 0, 48, 480, 1, 10, true, RGB(255, 0, 255));
	loadFrameImage("effectHeal1", "image\\effect\\effectHeal1.bmp", 0, 0, 16, 96, 1, 6, true, RGB(255, 0, 255));
	loadFrameImage("effectAtkMagic", "image\\effect\\effectAtkMagic.bmp", 0, 0, 256, 64, 4, 1, true, RGB(255, 0, 255));
	loadFrameImage("effectDefMagic", "image\\effect\\effectDefMagic.bmp", 0, 0, 256, 64, 4, 1, true, RGB(255, 0, 255));
	loadFrameImage("effectMeteo", "image\\effect\\effectMeteo.bmp", 0, 0, 640, 128, 5, 1, true, RGB(255, 0, 255));
	loadFrameImage("effectExplosion", "image\\effect\\effectExplosion.bmp", 0, 0, 64, 320, 1, 5, true, RGB(255, 0, 255));
	loadFrameImage("effectFireball", "image\\effect\\effectFireball.bmp", 0, 0, 128, 32, 4, 1, true, RGB(255, 0, 255));
	loadFrameImage("effectTeleport", "image\\effect\\effectTeleport.bmp", 0, 0, 192, 48, 4, 1, true, RGB(255, 0, 255));
	loadFrameImage("summonEffect2", "image\\effect\\summonEffect2.bmp", 0, 0, 384, 64, 6, 1, true, RGB(255, 0, 255));
	loadFrameImage("summonEffect3", "image\\effect\\summonEffect3.bmp", 0, 0, 64, 64, 4, 1, true, RGB(255, 0, 255));

	loadImage("summonEffect1", "image\\effect\\summonEffect1.bmp", 128, 128, true, RGB(255, 0, 255));//모든 마법 다 이걸로 한다 일단

	//배틀씬쇼 이미지
	loadImage("bsBack10", "image\\battleScene\\bsBack10.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack11", "image\\battleScene\\bsBack11.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack12", "image\\battleScene\\bsBack12.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack13", "image\\battleScene\\bsBack13.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack15", "image\\battleScene\\bsBack15.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack16", "image\\battleScene\\bsBack16.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack17", "image\\battleScene\\bsBack17.bmp", 640, 272, true, RGB(255, 0, 255));
	loadImage("bsBack18", "image\\battleScene\\bsBack18.bmp", 640, 272, true, RGB(255, 0, 255));

	loadImage("bsUnitBar1", "image\\battleScene\\bsUnitBar1.bmp", 640, 208, true, RGB(255, 0, 255));
	loadImage("bsUnitBar2", "image\\battleScene\\bsUnitBar2.bmp", 640, 208, true, RGB(255, 0, 255));

	loadImage("divideBar", "image\\battleScene\\divideBar.bmp", 48, 480, true, RGB(255, 0, 255));

	loadFrameImage("bsNumImage", "image\\battleScene\\bsNumImage.bmp", 0, 0, 528, 48, 11, 1, true, RGB(255, 0, 255));
}
void PkwLoadingScene::loadSound()
{
	SOUNDMANAGER->addSound("1_opening", "sound\\1_opening.wav", true, true);
	SOUNDMANAGER->addSound("2_scenario", "sound\\2_scenario.wav", true, true);
	SOUNDMANAGER->addSound("3_battleSet", "sound\\3_battleSet.wav", true, true);
	SOUNDMANAGER->addSound("4_coTurn1", "sound\\4_coTurn1.wav", true, true);
	SOUNDMANAGER->addSound("5_coTurn2", "sound\\5_coTurn2.wav", true, true);
	SOUNDMANAGER->addSound("6_suportTurn", "sound\\6_suportTurn.wav", true, true);
	SOUNDMANAGER->addSound("7_enemyTurn1", "sound\\7_enemyTurn1.wav", true, true);
	SOUNDMANAGER->addSound("8_enemyTurn2", "sound\\8_enemyTurn2.wav", true, true);
	SOUNDMANAGER->addSound("9_gameClear", "sound\\9_gameClear.wav", false, false);
	SOUNDMANAGER->addSound("10_ending", "sound\\10_ending.wav", true, true);
	SOUNDMANAGER->addSound("11_gameEvent1", "sound\\11_gameEvent1.wav", true, true);
}