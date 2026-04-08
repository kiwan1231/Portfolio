#pragma once

class timer
{
private:
	bool _isHardware;				//고성능 타이머를 쓰겠니?
	float _timeScale;				//시간 경과량
	float _timeElapsed;				//마지막 시간과 현재 시간의 경과량
	__int64 _curTime;				//현재 시간
	__int64 _lastTime;				//마지막 시간
	__int64 _periodFrequency;		//시간을 주자

	unsigned long _frameRate;		//FPS(!=First Person SHooting && Frame Per Second)
	unsigned long _FPSFrameCount;	//FPS카운트
	float _FPSTimeElapsed;			//FPS 마지막 시간과 현재 시간의 경과량
	float _worldTime;				//월드타임

public:
	HRESULT init(void);
	void tick(float lockFPS = 0.0f);
	unsigned long getFrameRate(char* str = NULL) const;
	inline float getElapsedTime(void) const { return _timeElapsed; }
	inline float getWorldTime(void) const { return _worldTime; }

	timer();
	~timer();
};

