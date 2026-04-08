#pragma once

#include <cmath>

#define DEG_TO_RAD 0.017453F //1도의 라디안 값
#define PI 3.141592654F
#define PI2 (PI * 2)
#define PI4 float(PI / 4.0f)
#define PI6 float(PI / 6.0f)
#define FLOAT_EPSILON 0.001f //실수의 가장 작은 단위
#define FLOAT_TO_INT(f1) static_cast<int>(f1 + FLOAT_EPSILON) //실수형을 정수형으로 비교
#define FLOAT_EQUAL(f1, f2) (fabs(f1, f2) <= FLOAT_EPSILON) //두 실수가 같은지 비교해준다

namespace MY_UTIL //<--- 자기가 쓰고싶어하는 이름으로 바꿔도 무방
{
	float getAngle(float startX, float startY, float endX, float endY);
	float getDistance(float startX, float startY, float endX, float endY);
}