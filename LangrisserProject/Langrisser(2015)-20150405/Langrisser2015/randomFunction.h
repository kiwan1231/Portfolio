#pragma once

#include "./2DGameFramework/manager/singletonBase.h"
#include <time.h>

//======================================
// ## 2014.12.15 ## randomFunction ##
//======================================

class randomFunction : public singletonBase <randomFunction>
{
public:
	randomFunction(void)
	{
		srand(GetTickCount());
	}
	~randomFunction(void)
	{
	}

	//int형 랜덤값 가져오자
	inline int getInt(int num) { return rand() % num; }
	inline int getFromIntTo(int fromNum, int toNum) { return rand() % (toNum - fromNum + 1) + fromNum;  }
};
