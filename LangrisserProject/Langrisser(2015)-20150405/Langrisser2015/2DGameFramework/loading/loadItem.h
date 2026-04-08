#pragma once

//로딩 아이템 종류

enum LOADING_KIND
{
	LOADING_KIND_IMAGE_00,
	LOADING_KIND_IMAGE_01,
	LOADING_KIND_IMAGE_02,
	LOADING_KIND_FRAMEIMAGE_00,
	LOADING_KIND_FRAMEIMAGE_01,
	LOADING_KIND_SOUND_00,
	LOADING_KIND_END
};

//이미지 리소스 구조체
struct tagImageResource
{
	string keyName;
	const char* fileName;
	int x, y;
	int width, height;
	int frameX, frameY;
	bool trans;
	COLORREF transColor;
};

class loadItem
{
private:
	LOADING_KIND _kind;
	tagImageResource _imageResource;
public:
	//이미지 로드 아이템
	HRESULT initForImage(string keyName, int width, int height);
	HRESULT initForImage(string keyName, const char* fileName, int x, int y, int width, int height, bool trans, COLORREF transColor);
	HRESULT initForImage(string keyName, const char* fileName, int width, int height, bool trans, COLORREF transColor);
	HRESULT initForFrameImage(string keyName, const char* fileName, int x, int y, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor);
	HRESULT initForFrameImage(string keyName, const char* fileName, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor);

	LOADING_KIND getLoadingKind() { return _kind; }
	tagImageResource getImageResource(void) { return _imageResource; }

	loadItem();
	~loadItem();
};

