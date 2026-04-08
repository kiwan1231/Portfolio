#pragma once

#include "../2DGameFramework/gameNode.h"
#include"../common.h"
#include "../2DGameFramework/loading/loadItem.h"

class PkwLoadingScene : public gameNode
{
private:
	typedef vector<loadItem*> arrLoadItem;
	typedef vector<loadItem*>::iterator arrLoadItemIter;
private:
	void* _obj;
	CALLBACK_FUNCTION_PARAMETER _callbackFunctionParameter;

	arrLoadItem _vLoadItem;
	int _current;

	image*				_backBuffer;
	HDC					_MemDC;
	float				_elapsed;
	int					_nWindowWidth;
	int					_nWindowHeight;
public:
	PkwLoadingScene();
	~PkwLoadingScene();

	virtual HRESULT init(CALLBACK_FUNCTION_PARAMETER cbFunction, void* obj);
	virtual void release(void);
	virtual void update(void);
	virtual void render(void);

	void loadImage(string keyName, int width, int height);
	void loadImage(string keyName, const char* fileName, int x, int y, int width, int height, bool trans, COLORREF transColor);
	void loadImage(string keyName, const char* fileName, int width, int height, bool trans, COLORREF transColor);
	void loadFrameImage(string keyName, const char* fileName, int x, int y, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor);
	void loadFrameImage(string keyName, const char* fileName, int width, int height, int frameX, int frameY, bool trans, COLORREF transColor);
	void loadSound();
	BOOL loadNext(void);
	void ImageDataLoad();
};

