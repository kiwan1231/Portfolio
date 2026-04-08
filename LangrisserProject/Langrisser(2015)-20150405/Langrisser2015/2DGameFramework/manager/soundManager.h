#pragma once

#include "singletonbase.h"

#include <map>
#include <string>

//fmod.hpp추가
#include "inc/fmod.hpp"

//lib 링크
#pragma comment (lib, "2DGameFramework/manager/lib/fmodex_vc.lib")

//여유분의 채널 갯수 설정 (버퍼임)
#define EXTRA_SOUND_CHANNEL 5
#define SOUNDBUFFER 10

//총 사운드 갯수 (알지? 사운드 씹히지마라고 해주는거 촬리얘두라아아아앙)
#define TOTAL_SOUND_CHANNEL SOUNDBUFFER + EXTRA_SOUND_CHANNEL

using namespace FMOD;

class soundManager : public singletonBase <soundManager>
{
private:
	typedef map<string, Sound**> arrSounds;
	typedef map<string, Sound**>::iterator arrSoundsIter;
	typedef map<string, Channel**> arrChannels;
	typedef map<string, Channel**>::iterator arrChannelIter;

private:
	System* _system;
	Sound** _sound;
	Channel** _channel;

	arrSounds _mTotalSounds;
public:
	char		_curBGSound[128];
public:
	HRESULT init(void);
	void release(void);

	//사운드 추가
	void addSound(string keyName, string soundName, bool background = false, bool loop = false);

	//사운드 플레이
	void play(string keyName, float volume);

	//사운드 정지
	void stop(string keyName);

	//사운드 일시정지
	void pause(string keyName);

	//사운드 재시작
	void resume(string keyName);

	//일시정지 되었냐?
	bool isPauseSound(string keyName);

	//플레이 중이냐?
	bool isPlaySound(string keyName);


	//Fmod 시스템을 갱신
	void update(void);

	int getSoundLength(string keyName);
	int getCurSoundPos(string keyName);
	void setCurSoundPos(string keyName, int pos);
	char* getSoundName(string keyName);

	soundManager(void);
	~soundManager(void);
};
