#include "../../stdafx.h"
#include "effect.h"
#include "../image/image.h"
#include "../animation/animation.h"


effect::effect() : _effectImage(NULL), _effectAnimation(NULL),
_isRunning(false), _x(0), _y(0)
{
}


effect::~effect()
{
}


HRESULT effect::init(image* effectImage, int frameW, int frameH, int fps, float elapsedTime)
{

	//만약에 이미지가 없으면 리턴
	if (!effectImage) return E_FAIL;

	//활성화 여부 초기화
	_isRunning = false;

	//이펙트 이미지 정보대입
	_effectImage = effectImage;

	//이펙트 경과시간
	_elapsedTime = elapsedTime;

	//이미지 애니메이션 객체가 없으면 생성해줌
	if (!_effectAnimation) _effectAnimation = new animation;

	//애니메이션 정보 셋팅
	_effectAnimation->init(_effectImage->getWidth(), _effectImage->getHeight(), frameW, frameH);
	_effectAnimation->setDefPlayFrame(false, false);
	_effectAnimation->setFPS(fps);
	_effectAnimation->stop();


	return S_OK;
}
void effect::update(void)
{
	//활성화 되지 않으면 실행시켜주지 않는다
	if (!_isRunning) return;
	_effectAnimation->frameUpdate(_elapsedTime);

	//이펙트 애니메이션이 종료가 되면 Kill해준다
	if (!_effectAnimation->isPlay())killEffect();
}
void effect::render(void)
{
	//실행이 안되면 그려주면 안됨
	if (!_isRunning) return;

	//Effect 애니메이션 렌더링~
	_effectImage->aniRender(getMemDC(), _x, _y, _effectAnimation);
}
void effect::release(void)
{
	SAFE_DELETE(_effectAnimation);
}

//이펙트 터트릴 좌표
void effect::startEffect(int x, int y)
{
	//정보가 초기화 되어있지 않다면 실행되지 않는다.
	if (!_effectImage || !_effectAnimation) return;

	//중앙좌표로 이동해줌줌
	_x = x - (_effectAnimation->getFrameWidth() / 2);
	_y = y - (_effectAnimation->getFrameHeight() / 2);

	_isRunning = true;
	_effectAnimation->start();
}

//이펙트 비활성화
void effect::killEffect(void)
{
	_isRunning = false;
}