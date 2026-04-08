#pragma once
#include"../common.h"

#define MAP_IMAGE					1
#define CHARACTOR_ANI_IMAGE			24
#define USER_INTERFACE_IMAGE		8
#define CHARACTOR_FACE_IMAGE		19

class PkwImage
{
public:
	PkwImage(void);
	~PkwImage(void);
public:
	void ImageLoad(char* fileName, IMAGE_TYPE type, int xSize, int ySize);
	void ImageDelete();
	void DrawImage(Graphics*, int xPos, int yPos, int width, int height, ANI_TYPE aniType, int aniNum, float rTransparency);
public:
	IMAGE_TYPE			_imageType;		//이미지 종류(배틀이미지,얼굴이미지 등등)
//	ANI_TYPE			_aniType;		//애니 종류(서있기,왼쪽 오른쪽 걷기 등등)
	Gdiplus::Image*		_image;			//이미지 저장
	int					_xMaxSize;		//이미지 x 길이
	int					_yMaxSize;		//이미지 y 길이
	int					_xSize;			// 출력시키는 캐릭터 이미지 x 크기
	int					_ySize;			// 출력시키는 캐릭터 이미지 y 크기
	int					_ImageNumber;	// 출려시키는 이미지 위치(이미지 전체 크기와 출력크기에 맞춰서 위치 지정을 한다)
	int					_CharacNumber;	// 출력시키는 캐릭터 색깔(아군0 ~ 적군1)
	int					_AniNumber;		// 출력시키는 애니 번호(0~1)
};

class PkwImageLoad
{
public:
	PkwImageLoad(void);
	~PkwImageLoad(void);
public:
	PkwImage			_MapImage[MAP_IMAGE];
	PkwImage			_CharacAni[CHARACTOR_ANI_IMAGE];
	PkwImage			_CharacFaceImage[CHARACTOR_FACE_IMAGE];
	PkwImage			_UserInterfaceImage[USER_INTERFACE_IMAGE];
	PkwImage			_ItemImage;

public:
	void ImageLoad();
	void ImageDelete();
};


