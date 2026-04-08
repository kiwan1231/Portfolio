#include "../stdafx.h"
#include "PkwImageLoad.h"
PkwImage::PkwImage(void)
{
	_image = NULL;
}
PkwImage::~PkwImage(void)
{

}


void PkwImage::ImageLoad(char* fileName, IMAGE_TYPE type, int xSize, int ySize)
{
	USES_CONVERSION;
	WCHAR* wFileName = A2W(fileName);
	_image = Gdiplus::Image::FromFile(wFileName);

	_imageType = type;
	_xMaxSize = _image->GetWidth();
	_yMaxSize = _image->GetHeight();
	_xSize = xSize;
	_ySize = ySize;

	//---초기화 한후 이미지 그릴떄 처리
	_CharacNumber = 0;
	_ImageNumber = 0;
	_AniNumber = 0;

	return;
}
void PkwImage::ImageDelete()
{
	SAFE_DELETE(_image);
}
void PkwImage::DrawImage(Graphics* graphics, int xPos, int yPos, int width, int height, ANI_TYPE aniType, int aniNum, float rTransparency)
{
	

	Rect destRect(xPos, yPos, width, height);//(출력위치xy,출력길이wh)
	int xImagePos, yImagePos;

	if (_image == NULL)		return;
	
	if (_imageType == NORMAL){

		ImageAttributes   IA;
		ColorMatrix colorMatrix = { 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 0.0f, rTransparency, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		IA.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

		graphics->DrawImage(_image, destRect, 0, 0, _image->GetWidth(), _image->GetHeight(), UnitPixel, &IA);
	}
	else if (_imageType == ITEM){
		xImagePos = (aniNum % 8)*_xSize;
		yImagePos = (aniNum / 8)*_ySize;
		graphics->DrawImage(_image, destRect, xImagePos, yImagePos, _xSize, _ySize, UnitPixel, NULL);
	}
	else if (_imageType == CHAT_FACE){
		graphics->DrawImage(_image, destRect, 0, 0, _image->GetWidth(), _image->GetHeight(), UnitPixel, NULL);
	}
	else if (_imageType == BATTLE_FACE){
	}
	else if (_imageType == BATTLE_ANI){

		if (aniType == STAND){
			xImagePos = 0;
			yImagePos = _ySize;
		}
		else if (aniType == RIGHT_WALK){
			xImagePos = _xSize*(aniNum);
			yImagePos = 0;
		}
		else if (aniType == LEFT_WALK){//RIGHT_WALK를 반대로 출력
			xImagePos = _xSize*(aniNum + 1);
			yImagePos = 0;
			width *= -1;
		}
		else if (aniType == TOP_WALK){
			xImagePos = _xSize*(aniNum + 2);
			yImagePos = _ySize;
		}
		else if (aniType == BOTTOM_WALK){
			xImagePos = _xSize*(aniNum);
			yImagePos = _ySize;
		}
		else if (aniType == ATTACK){
		}
		else if (aniType == DEATH){
			xImagePos = _xSize * 2;
			yImagePos = 0;
		}
		graphics->DrawImage(_image, destRect, xImagePos, yImagePos, _xSize, _ySize, UnitPixel, NULL);
	}
	else if (_imageType == MAP){
		graphics->DrawImage(_image, destRect, 0, 0, _image->GetWidth(), _image->GetHeight(), UnitPixel, NULL);
	}
	return;
}

//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------

PkwImageLoad::PkwImageLoad()
{
}
PkwImageLoad::~PkwImageLoad()
{
}
/*---
캐릭터 애니메이션 이미지 예외 좌표
x = 
---*/
void PkwImageLoad::ImageLoad()
{
	_CharacAni[0].ImageLoad("image\\battleAni\\0_Enwin.png", BATTLE_ANI, 64, 64);
	_CharacAni[1].ImageLoad("image\\battleAni\\1_Hein.png", BATTLE_ANI, 64, 64);
	_CharacAni[2].ImageLoad("image\\battleAni\\2_liana.png", BATTLE_ANI, 64, 64);
	_CharacAni[3].ImageLoad("image\\battleAni\\3_Sherry.png", BATTLE_ANI, 64, 64);
	_CharacAni[4].ImageLoad("image\\battleAni\\4_Lana.png", BATTLE_ANI, 64, 64);
	_CharacAni[5].ImageLoad("image\\battleAni\\5_Lowe.png", BATTLE_ANI, 64, 64);
	_CharacAni[6].ImageLoad("image\\battleAni\\6_Scott.png", BATTLE_ANI, 64, 64);
	_CharacAni[7].ImageLoad("image\\battleAni\\7_Kiss.png", BATTLE_ANI, 64, 64);
	_CharacAni[8].ImageLoad("image\\battleAni\\8_Leicester.png", BATTLE_ANI, 64, 64);
	_CharacAni[9].ImageLoad("image\\battleAni\\9_Aaron.png", BATTLE_ANI, 64, 64);

	_CharacAni[10].ImageLoad("image\\battleAni\\10_Sonia.png", BATTLE_ANI, 64, 64);
	_CharacAni[11].ImageLoad("image\\battleAni\\11_Leon.png", BATTLE_ANI, 64, 64);
	_CharacAni[12].ImageLoad("image\\battleAni\\12_Egbert.png", BATTLE_ANI, 64, 64);
	_CharacAni[13].ImageLoad("image\\battleAni\\13_Valgas.png", BATTLE_ANI, 64, 64);
	_CharacAni[14].ImageLoad("image\\battleAni\\14_Imelda.png", BATTLE_ANI, 64, 64);
	_CharacAni[15].ImageLoad("image\\battleAni\\15_Bernhard.png", BATTLE_ANI, 64, 64);
	_CharacAni[16].ImageLoad("image\\battleAni\\16_Jessica.png", BATTLE_ANI, 64, 64);
	_CharacAni[17].ImageLoad("image\\battleAni\\17_Ladin.png", BATTLE_ANI, 64, 64);
	_CharacAni[18].ImageLoad("image\\battleAni\\18_Dieharte.png", BATTLE_ANI, 64, 64);
	_CharacAni[19].ImageLoad("image\\battleAni\\19_souldier.png", BATTLE_ANI, 64, 64);

	_CharacAni[20].ImageLoad("image\\battleAni\\20_trooper.png", BATTLE_ANI, 64, 64);
	_CharacAni[21].ImageLoad("image\\battleAni\\21_harpv.png", BATTLE_ANI, 64, 64);
	_CharacAni[22].ImageLoad("image\\battleAni\\22_merman.png", BATTLE_ANI, 64, 64);
	_CharacAni[23].ImageLoad("image\\battleAni\\23_prayer.png", BATTLE_ANI, 64, 64);
	
	_CharacFaceImage[0].ImageLoad("image\\charactorFace\\0_Enwin_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[1].ImageLoad("image\\charactorFace\\1_Hein_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[2].ImageLoad("image\\charactorFace\\2_liana_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[3].ImageLoad("image\\charactorFace\\3_Sherry_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[4].ImageLoad("image\\charactorFace\\4_Lana_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[5].ImageLoad("image\\charactorFace\\5_Lowe_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[6].ImageLoad("image\\charactorFace\\6_Scott_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[7].ImageLoad("image\\charactorFace\\7_Kiss_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[8].ImageLoad("image\\charactorFace\\8_Leicester_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[9].ImageLoad("image\\charactorFace\\9_Aaron_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[10].ImageLoad("image\\charactorFace\\10_Sonia_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[11].ImageLoad("image\\charactorFace\\11_Leon_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[12].ImageLoad("image\\charactorFace\\12_Egbert.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[13].ImageLoad("image\\charactorFace\\13_Valgas_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[14].ImageLoad("image\\charactorFace\\14_Imelda_face.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[15].ImageLoad("image\\charactorFace\\15_Bernhard.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[16].ImageLoad("image\\charactorFace\\16_Jessica.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[17].ImageLoad("image\\charactorFace\\17_Ladin.png", CHAT_FACE, 80, 96);
	_CharacFaceImage[18].ImageLoad("image\\charactorFace\\18_Dieharte.png", CHAT_FACE, 80, 96);

	_MapImage[0].ImageLoad("image\\map\\map1(64).png", MAP, 1728, 2816);
	
	_UserInterfaceImage[0].ImageLoad("image\\userInterface\\UI_BLUE_BACK.png", NORMAL, -1, -1);
	_UserInterfaceImage[1].ImageLoad("image\\userInterface\\UI_GREEN_BACK.png", NORMAL, -1, -1);
	_UserInterfaceImage[2].ImageLoad("image\\userInterface\\UI_RED_BACK.png", NORMAL, -1, -1);
	_UserInterfaceImage[3].ImageLoad("image\\userInterface\\UI(160X400).png", NORMAL, 160, 400);
	_UserInterfaceImage[4].ImageLoad("image\\userInterface\\UI(400X320).png", NORMAL, 400, 320);
	_UserInterfaceImage[5].ImageLoad("image\\userInterface\\charactorfaceWin.png", NORMAL, 112, 128);
	_UserInterfaceImage[6].ImageLoad("image\\userInterface\\choiceArrow.png", NORMAL, 15, 30);
	_UserInterfaceImage[7].ImageLoad("image\\userInterface\\UI(160X192).png", NORMAL, 160, 192);

	_ItemImage.ImageLoad("image\\item\\item.png", ITEM, 32, 32);
}
void PkwImageLoad::ImageDelete()
{
	for (int i = 0; i < CHARACTOR_ANI_IMAGE; i++)
		_CharacAni[i].ImageDelete();
	for (int i = 0; i < MAP_IMAGE; i++)
		_MapImage[i].ImageDelete();
	for (int i = 0; i < CHARACTOR_FACE_IMAGE; i++)
		_CharacFaceImage[i].ImageDelete();
	for (int i = 0; i < USER_INTERFACE_IMAGE; i++)
		_UserInterfaceImage[i].ImageDelete();

	_ItemImage.ImageDelete();
}