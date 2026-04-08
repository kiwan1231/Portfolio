#pragma once
#include "../gameNode.h"

class image;
class animation;


class effect : public gameNode
{
protected:
	int _x;							//Effect좌(레프트)위치
	int _y;							//Effect상(탑) 위치

	image* _effectImage;			//Effect 이미지
	animation* _effectAnimation;	//Effect 애니메이숀
	BOOL _isRunning;				//Effect 활성화 되었냐!?
	float _elapsedTime;				//이펙트 경과시간
public:

	effect();
	virtual ~effect();

	//이펙트 터트릴 이미지, 프레임가로크기, 프레임 세로크기, FPS수치, 경과시간
	virtual HRESULT init(image* effectImage, int frameW, int frameH, int fps, float elapsedTime);
	virtual void update(void);
	virtual void render(void);
	virtual void release(void);

	void startEffect(int x, int y);	//이펙트 터트릴 좌표
	virtual void killEffect(void);	//이펙트 비활성화

	BOOL getIsRunning(void) { return _isRunning; }

};

