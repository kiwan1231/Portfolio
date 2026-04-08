#include "../stdafx.h"
#include "unitDeathEvent.h"

unitDeathEvent::unitDeathEvent()
{
}
unitDeathEvent::~unitDeathEvent()
{
}

void unitDeathEvent::init(BATTLE_EVENT_TYPE eventType, POINT* pMapStartPos, int MapMaxWidth, int MapManHeight)
{
	battleEvent::init(eventType, pMapStartPos, MapMaxWidth, MapManHeight);

	_pCharactorPlay = DATAMANAGER->getCharactorPlay();

	_vDeathUnit.clear();
	_vDeathEffect.clear();

	_eventStatus = EVENT_DEACTIVE;
}
void unitDeathEvent::release(void)
{
	battleEvent::release();

	_vDeathUnit.clear();
}
void unitDeathEvent::update(float elapsedTime)
{
	_delayTime += elapsedTime;

	//0.1초마다 업데이트를 해준다
	if (_delayTime < 0.1) return;
	_delayTime = 0.0;

	//이벤트가 모두 종료 되었으면 비활성화 시킨다
	if (_vDeathUnit.size() == 0 && _vDeathEffect.size() == 0){
		_eventStatus = EVENT_DEACTIVE;
	}

	//죽은 유닛 폭발 이미지 벡터 업데이트
	vector<deathEffect>::iterator		viDeathEffect;
	for (viDeathEffect = _vDeathEffect.begin(); viDeathEffect != _vDeathEffect.end();){
		//처리 해야할 이벤트가 종료 상태이면 벡터 안에서 없애준다
		if ( (*viDeathEffect).isEnd == true){
			viDeathEffect = _vDeathEffect.erase(viDeathEffect);
		}
		else{
			(*viDeathEffect).frameX++;
			if ( (*viDeathEffect).frameX >= 4){
				(*viDeathEffect).frameX = 3;
				(*viDeathEffect).isEnd = true;
			}
			viDeathEffect++;
		}
	}
	
	//죽는 유닛 벡터 업데이트
	bool						isDelete = false;
	vector<POINT>::iterator		viDeathUnit;
	vector<PkwCharactor>::iterator		viArms;

	//아무것도 없으면 업데이트 안함
	if (_vDeathUnit.size() == 0) return;
	viDeathUnit = _vDeathUnit.begin();

	//아군 지휘관 체크
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		//병사들에게서 죽은자 있는지 체크 있으면 제거 하고 함수를 종료한다
		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.x == viDeathUnit->x && _pCharactorPlay->_vCoCommander[i]._vArms[j]._pos.y == viDeathUnit->y){
				addEffect(*viDeathUnit);
				_pCharactorPlay->_vCoCommander[i]._vArms.erase(_pCharactorPlay->_vCoCommander[i]._vArms.begin() + j);
				_vDeathUnit.erase(viDeathUnit);
				return;
			}
		}
		//아군 지휘관이 죽었는지
		if (_pCharactorPlay->_vCoCommander[i]._pos.x == viDeathUnit->x && _pCharactorPlay->_vCoCommander[i]._pos.y == viDeathUnit->y){
			//아군 지휘관이 죽으면 그 아래에 있는 병사들을 먼저 죽인다. 
			for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
				//이미 hp인 녀석은 check함수에서 벡터안에 이미 들어가 있으므로 hp가 0이 아닌 녀석만 죽인다.
				if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._curHp <= 0) continue;
				addEffect(_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos);
				_pCharactorPlay->_vCoCommander[i]._vArms.erase(_pCharactorPlay->_vCoCommander[i]._vArms.begin() + j);
				return;
			}
			//이미 벡터 안에 있는 애들을 제외한 다른 병사들을 모두 죽이면 지휘관을 죽인다.
			addEffect(*viDeathUnit);
			_pCharactorPlay->_vCoCommander.erase(_pCharactorPlay->_vCoCommander.begin() + i);
			_vDeathUnit.erase(viDeathUnit);
			return;
		}
	}

	//지원군 체크
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		
		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.x == viDeathUnit->x && _pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos.y == viDeathUnit->y){
				addEffect(*viDeathUnit);
				_pCharactorPlay->_vSupportCommander[i]._vArms.erase(_pCharactorPlay->_vSupportCommander[i]._vArms.begin() + j);
				_vDeathUnit.erase(viDeathUnit);
				return;
			}
		}

		if (_pCharactorPlay->_vSupportCommander[i]._pos.x == viDeathUnit->x && _pCharactorPlay->_vSupportCommander[i]._pos.y == viDeathUnit->y){

			for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){

				if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp <= 0) continue;
				addEffect(_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos);
				_pCharactorPlay->_vSupportCommander[i]._vArms.erase(_pCharactorPlay->_vSupportCommander[i]._vArms.begin() + j);
				return;
			}
			addEffect(*viDeathUnit);
			_pCharactorPlay->_vSupportCommander.erase(_pCharactorPlay->_vSupportCommander.begin() + i);
			_vDeathUnit.erase(viDeathUnit);
			return;
		}
	}

	//적군 체크
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		
		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos.x == viDeathUnit->x && _pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos.y == viDeathUnit->y){
				addEffect(*viDeathUnit);
				_pCharactorPlay->_vEnemyCommander[i]._vArms.erase(_pCharactorPlay->_vEnemyCommander[i]._vArms.begin() + j);
				_vDeathUnit.erase(viDeathUnit);
				return;
			}
		}

		if (_pCharactorPlay->_vEnemyCommander[i]._pos.x == viDeathUnit->x && _pCharactorPlay->_vEnemyCommander[i]._pos.y == viDeathUnit->y){

			for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){

				if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._curHp <= 0) continue;
				addEffect(_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos);
				_pCharactorPlay->_vEnemyCommander[i]._vArms.erase(_pCharactorPlay->_vEnemyCommander[i]._vArms.begin() + j);
				return;
			}
			addEffect(*viDeathUnit);
			_pCharactorPlay->_vEnemyCommander.erase(_pCharactorPlay->_vEnemyCommander.begin() + i);
			_vDeathUnit.erase(viDeathUnit);
			return;
		}
	}
}
void unitDeathEvent::render(HDC memDC)
{
	int frameWidth, frameHeight;
	frameWidth = 64;
	frameHeight = 64;

	//폭발하는 이미지 출력
	for (int i = 0; i < _vDeathEffect.size(); i++){
		IMAGEMANAGER->findImage("charactorAction")->render(memDC, _vDeathEffect[i].pos.x * UNITSIZEX - 8, _vDeathEffect[i].pos.y * UNITSIZEY - 8, _vDeathEffect[i].frameX * frameWidth, 192, frameWidth, frameHeight);
	}
}
void unitDeathEvent::checkEvent()
{	
	//아군 체크
	for (int i = 0; i < _pCharactorPlay->_vCoCommander.size(); i++){
		//지휘관이 가지고 있는 유닛 부터 죽었으면 먼저 벡터에 넣는다 안그럼 지휘관이 죽고 자기 안에 있는 유닛들을 또 죽는 벡터에 넣는데 혹시라도 중복되면 큰일
		for (int j = 0; j < _pCharactorPlay->_vCoCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vCoCommander[i]._vArms[j]._curHp <= 0)
				_vDeathUnit.push_back(_pCharactorPlay->_vCoCommander[i]._vArms[j]._pos);
		}

		if (_pCharactorPlay->_vCoCommander[i]._curHp <= 0)
			_vDeathUnit.push_back(_pCharactorPlay->_vCoCommander[i]._pos);
	}

	//지원군 체크
	for (int i = 0; i < _pCharactorPlay->_vSupportCommander.size(); i++){
		//지휘관이 가지고 있는 유닛 부터 죽었으면 먼저 벡터에 넣는다 안그럼 지휘관이 죽고 자기 안에 있는 유닛들을 또 죽는 벡터에 넣는데 혹시라도 중복되면 큰일
		for (int j = 0; j < _pCharactorPlay->_vSupportCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vSupportCommander[i]._vArms[j]._curHp <= 0)
				_vDeathUnit.push_back(_pCharactorPlay->_vSupportCommander[i]._vArms[j]._pos);
		}

		if (_pCharactorPlay->_vSupportCommander[i]._curHp <= 0)
			_vDeathUnit.push_back(_pCharactorPlay->_vSupportCommander[i]._pos);
	}

	//적군 체크
	for (int i = 0; i < _pCharactorPlay->_vEnemyCommander.size(); i++){
		//지휘관이 가지고 있는 유닛 부터 죽었으면 먼저 벡터에 넣는다 안그럼 지휘관이 죽고 자기 안에 있는 유닛들을 또 죽는 벡터에 넣는데 혹시라도 중복되면 큰일
		for (int j = 0; j < _pCharactorPlay->_vEnemyCommander[i]._vArms.size(); j++){
			if (_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._curHp <= 0)
				_vDeathUnit.push_back(_pCharactorPlay->_vEnemyCommander[i]._vArms[j]._pos);
		}

		if (_pCharactorPlay->_vEnemyCommander[i]._curHp <= 0)
			_vDeathUnit.push_back(_pCharactorPlay->_vEnemyCommander[i]._pos);
	}

	if (_vDeathUnit.size() != 0)
		_eventStatus = EVENT_ACTIVE;
}

void unitDeathEvent::addEffect(POINT pos)
{
	deathEffect temp;
	temp.pos.x = pos.x;
	temp.pos.y = pos.y;
	temp.frameX = 0;
	temp.delay = 0.0;
	temp.isEnd = false;
	_vDeathEffect.push_back(temp);

	if (DATAMANAGER->_choiceUnit->_number < 1000)
		DATAMANAGER->_choiceUnit->_curExp += 10;
	else
		DATAMANAGER->_choiceUnit->_commander->_curExp += 10;
}
