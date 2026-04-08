#include "../../stdafx.h"
#include "animation.h"


animation::animation()
:_frameNum(0),
_frameWidth(0),
_frameHeight(0),
_loop(FALSE),
_frameUpdateSec(0),
_elapsedSec(0),
_nowPlayIdx(0),
_play(FALSE)
{
}


animation::~animation()
{
}

//이미지 총 가로세로크기와 한 프레임의 가로세로 크기로 셋팅
HRESULT animation::init(int totalW, int totalH, int frameW, int frameH)
{
	//가로 프레임 수
	_frameWidth = frameW;
	int _frameNumWidth = totalW / _frameWidth;

	//세로 프레임 수
	_frameHeight = frameH;
	int _frameNumHeight = totalH / _frameHeight;

	//총 프레임 수 계산
	_frameNum = _frameNumWidth * _frameNumHeight;

	//프레임 위치 리스트를 셋팅을 해준다
	_frameList.clear();
	for (int i = 0; i < _frameNumHeight; i++)
	{
		for (int j = 0; j < _frameNumWidth; j++)
		{
			POINT framePos;
			framePos.x = j * _frameWidth;
			framePos.y = i * _frameHeight;

			//프레임 위치 리스트에 추가
			_frameList.push_back(framePos);
		}
	}

	//기본 프레임으로 셋팅
	setDefPlayFrame();

	return S_OK;
}

void  animation::release(void)
{

}

//기본 셋팅
void animation::setDefPlayFrame(BOOL reverse, BOOL loop)
{
	//루프 여부 대입
	_loop = loop;

	//플레이 리스트 클리어(초기화)
	_playList.clear();

	//다시 되돌아 오는(Reverse) 애니메이션이면?
	if (reverse)
	{
		//거기에 루프 애니메이션이면?
		if (_loop)
		{
			//정방향 애니메이션
			for (int i = 0; i < _frameNum; i++)
			{
				_playList.push_back(i);
			}

			//역방향 애니메이션
			for (int i = _frameNum - 1; i > 0; i--)
			{
				_playList.push_back(i);
			}
		}
		//루프 애니메이션이 아니면
		else
		{
			//갈때 프레임
			for (int i = 0; i < _frameNum; i++)
			{
				_playList.push_back(i);
			}

			for (int i = _frameNum - 1; i >= 0; i--)
			{
				_playList.push_back(i);
			}
		}
	}
	//다시 되돌아 오는 애니메이션이 아니면
	else
	{
		//갈때 저장
		for (int i = 0; i < _frameNum; i++)
		{
			_playList.push_back(i);
		}
	}
}

//플레이 프레임 배열로 셋팅
void animation::setPlayFrame(int* playArr, int arrLen, BOOL loop)
{
	//루프 여부 대입
	_loop = loop;

	//플레이 리스트 클리어
	_playList.clear();

	//정방향 프레임 셋팅
	for (int i = 0; i < arrLen; i++)
	{
		_playList.push_back(playArr[i]);
	}
}

//플레이 프레임 시작과 종료로 셋팅
void animation::setPlayFrame(int start, int end, BOOL reverse, BOOL loop)
{
	//루프 여부 대입
	_loop = loop;

	//플레이 리스트 클리어
	_playList.clear();

	if (start == end)
	{
		//시작 프레임과 끝 프레임이 같으면
		_playList.clear();
		stop();
		return;
	}

	//시작 프레임이 종료 프레임 보다 큰 경우
	if (start > end)
	{
		//다시 되돌아 오는 애니메이션이면
		if (reverse)
		{
			//루프 애니메이션이면
			if (_loop)
			{
				for (int i = start; i >= end; i--)
				{
					_playList.push_back(i);
				}

				for (int i = end; i < start; i++)
				{
					_playList.push_back(i);
				}
			}

			//루프 애니메이션이 아니면
			if (!_loop)
			{
				for (int i = start; i >= end; i--)
				{
					_playList.push_back(i);
				}

				for (int i = end + 1; i <= start; i++)
				{
					_playList.push_back(i);
				}
			}
		}
		else
		{
			for (int i = start; i >= end; i--)
			{
				_playList.push_back(i);
			}
		}
	}
	//다시 되돌아 오는 애니메이션이면
	if (reverse)
	{
		//루프 애니메이숀이면
		if (_loop)
		{
			for (int i = start; i <= end; i++)
			{
				_playList.push_back(i);
			}

			for (int i = end - 1; i > start; i--)
			{
				_playList.push_back(i);
			}
		}
		else
		{
			for (int i = start; i <= end; i++)
			{
				_playList.push_back(i);
			}

			for (int i = end - 1; i > start; i--)
			{
				_playList.push_back(i);
			}
		}

	}
	else
	{
		for (int i = start; i <= end; i++)
		{
			_playList.push_back(i);
		}
	}
}

//초당 갱신 횟수
void animation::setFPS(int framePerSec)
{
	_frameUpdateSec = 1.0f / framePerSec;
}

//프레임 업데이트
void animation::frameUpdate(float elpasedTime)
{
	if (_play)
	{
		_elapsedSec += elpasedTime;

		//프레임 업데이트 시간이 되었다면
		if (_elapsedSec >= _frameUpdateSec)
		{
			_elapsedSec -= _frameUpdateSec;
			_nowPlayIdx++;
			if (_nowPlayIdx == _playList.size())
			{
				//루프 플레이면
				if (_loop)
				{
					_nowPlayIdx = 0;
				}
				else
				{
					_nowPlayIdx--;
					_play = FALSE;
				}
			}
		}
	}
}

//플레이 시작
void animation::start(void)
{
	_play = TRUE;
	_nowPlayIdx = 0;
}

void animation::stop(void)
{
	_play = FALSE;
	_nowPlayIdx = 0;
}

void animation::pause(void)
{
	_play = FALSE;
}

void animation::resume(void)
{
	_play = TRUE;
}