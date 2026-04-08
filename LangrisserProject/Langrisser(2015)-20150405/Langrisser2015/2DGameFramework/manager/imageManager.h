#pragma once
#include "singletonBase.h"
#include "../image/image.h"
#include <map>		//키 값으로 뭔가 자료를 찾아오는 STL함쑤
#include <string>


class imageManager : public singletonBase<imageManager>
{
public:
	typedef std::map<std::string, image*> mapImageList;				//이미지 리스트 맵
	typedef std::map<std::string, image*>::iterator mapImageIter;	//이미지 리스트 이터레이터
	
private:
	mapImageList _mImageList;	//맵으로 구현된 이미지 리스트

public:
	//셋업 
	HRESULT init(void);

	//해제
	void release(void);

	//키 값으로 설정해서 초기화
	image* addImage(std::string strKey, int width, int height);

	image* addImage(std::string strKey, const DWORD resID,
		int width, int height, bool trans, COLORREF transColor);

	image* addImage(std::string strKey, const char* fileName,
		float x, float y, int width, int height, bool trans,
		COLORREF transColor);

	image* addImage(std::string strKey, const char* fileName,
		int width, int height, bool trans, COLORREF transColor);

	//키값 설정해서 프레임 이미지 초기화
	image* addFrameImage(std::string strKey, const char* fileName, float x, float y,
		int width, int height, int frameX, int frameY,
		BOOL trans, COLORREF transColor);

	image* addFrameImage(std::string strKey, const char* fileName, int width, int height,
		int frameX, int frameY, BOOL trans, COLORREF transColor);


	//이미지 찾는 함수
	image* findImage(std::string strKey);

	//이미지 지운다
	BOOL deleteImage(std::string strKey);

	//전체 이미지 지운다
	BOOL deleteAll(void);

	//렌더 함수
	void render(std::string strKey, HDC hdc);
	void render(std::string strKey, HDC hdc, int destX, int destY);
	void render(std::string strKey, HDC hdc, int sourX, int sourY,
		int sourWidth, int sourHeight);
	void render(std::string strKey, HDC hdc, int destX, int destY,
		int sourX, int sourY, int sourWidth, int sourHeight);


	//알파 렌더
	void alphaRender(std::string strKey, HDC hdc, BYTE alpha);
	void alphaRender(std::string strKey, HDC hdc, int destX, int destY, BYTE alpha);
	void alphaRender(std::string strKey, HDC hdc, int sourX, int sourY, int sourWidth, int sourHeight, BYTE alpha);
	void alphaRender(std::string strKey, HDC hdc, int destX, int destY, int sourX, int sourY, int sourWidth, int sourHeight, BYTE alpha);

	//루프 렌더 함수
	void loopRender(std::string strKey, HDC hdc, const LPRECT drawArea, int offSetX, int offSetY);
		

	imageManager();
	~imageManager();
};

