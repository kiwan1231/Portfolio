#include "../../stdafx.h"
#include "effectManager.h"
#include "../effect/effect.h"

effectManager::effectManager()
{
}


effectManager::~effectManager()
{
}

HRESULT effectManager::init(void)
{
	return S_OK;
}
void effectManager::release(void)
{
	iterTotalEffect vIter;
	iterEffect mIter;

	//이펙트를 모두 넣어준 벡터를 검사
	vIter = _vTotalEffects.begin();

	for (vIter; vIter != _vTotalEffects.end(); ++vIter)
	{
		mIter = vIter->begin();
		for (; mIter != vIter->end();)
		{
			if (mIter->second.size() != 0)
			{
				iterEffects vArrIter = mIter->second.begin();
				for (vArrIter; vArrIter != mIter->second.end(); ++vArrIter)
				{
					(*vArrIter)->release();
					delete *vArrIter;
					vArrIter = mIter->second.erase(vArrIter);

				}
				mIter->second.clear();
			}
			else
			{
				++mIter;
			}
		}
	}
}
void effectManager::update(void)
{
	iterTotalEffect vIter;
	iterEffect mIter;

	//이펙트를 모두 넣어둔 벡터를 검사
	for (vIter = _vTotalEffects.begin(); vIter != _vTotalEffects.end(); ++vIter)
	{
		//토탈이펙트 벡터 안에 있는 맵의 이펙트 키 검사
		for (mIter = vIter->begin(); mIter != vIter->end(); ++mIter)
		{
			iterEffects vArrIter;
			for (vArrIter = mIter->second.begin(); vArrIter != mIter->second.end(); ++vArrIter)
			{
				(*vArrIter)->update();
			}
		}
	}
}
void effectManager::render(void)
{
	iterTotalEffect vIter;
	iterEffect mIter;

	//이펙트를 모두 넣어둔 벡터를 검사
	for (vIter = _vTotalEffects.begin(); vIter != _vTotalEffects.end(); ++vIter)
	{
		//토탈이펙트 벡터 안에 있는 맵의 이펙트 키 검사
		for (mIter = vIter->begin(); mIter != vIter->end(); ++mIter)
		{
			iterEffects vArrIter;
			for (vArrIter = mIter->second.begin(); vArrIter != mIter->second.end(); ++vArrIter)
			{
				(*vArrIter)->render();
			}
		}
	}

}

void effectManager::addEffect(string effectName, const char* imageName, int imageWidth,
	int imageHeight, int effectWidth, int effectHeight, int fps, float elapsed, int buffer)
{
	image* img;
	arrEffects vEffectBuffer;	//이펙트 버퍼
	arrEffect mArrEffect;		//키값과 벡터 이펙트 담는 STL구조 중 map을 씀

	//등록된 이미지가 매니져에 이미 있으면
	if (IMAGEMANAGER->findImage(imageName))
	{
		img = IMAGEMANAGER->findImage(imageName);
	}
	//없으면
	else
	{
		//이미지 등록
		img = IMAGEMANAGER->addImage(imageName, imageName, imageWidth, imageHeight, true, RGB(255, 0, 255));
	}

	//벡터만큼 돌려서 벡터안에 넣어주는 부분
	for (int i = 0; i < buffer; i++)
	{
		vEffectBuffer.push_back(new effect);
		vEffectBuffer[i]->init(img, effectWidth, effectHeight, fps, elapsed);
	}

	//이펙트 버퍼를 맵에 담자
	mArrEffect.insert(pair<string, arrEffects>(effectName, vEffectBuffer));

	//키 값과 버퍼를 담은 맵을 다시 토탈맵에 넣어줌
	_vTotalEffects.push_back(mArrEffect);
}

void effectManager::play(string effectName, int x, int y)
{
	iterTotalEffect vIter;
	iterEffect mIter;

	//이펙트를 모두 넣어둔 벡터를 검사
	for (vIter = _vTotalEffects.begin(); vIter != _vTotalEffects.end(); ++vIter)
	{
		//토탈 이펙트 벡터 안에 있는 맵의 이펙트 키 검사
		for (mIter = vIter->begin(); mIter != vIter->end(); ++mIter)
		{
			//이펙트 이름을 비교해서 같지않으면 다음 맵으로~ 넘어감
			if (!(mIter->first == effectName)) break;

			//이펙트 키와 일치하면 이펙트를 실행시킨다
			iterEffects vArrIter;
			for (vArrIter = mIter->second.begin(); vArrIter != mIter->second.end(); ++vArrIter)
			{
				if ((*vArrIter)->getIsRunning()) continue;
				(*vArrIter)->startEffect(x, y);
				return;
			}
		}
	}
}