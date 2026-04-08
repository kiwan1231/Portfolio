#include "../stdafx.h"
#include "PkwBattleMapScene.h"
#include "PkwEventManager.h"

PkwEventManager::PkwEventManager()
{
}
PkwEventManager::~PkwEventManager()
{
}

HRESULT PkwEventManager::init(PkwBattleMapScene* pBttleMapScene)
{
	_pBttleMapScene = pBttleMapScene;
	_pBattleInfo = &_pBttleMapScene->_battleInfo;

	_vBattleEvent.clear();

	_isEventManagerAction = false;
	
	//유닛 데스 이벤트 클래스는 미리 등록한다
	unitDeathEvent* pUnitDeathEvent;
	pUnitDeathEvent = new unitDeathEvent();
	pUnitDeathEvent->init(DEATH_EVENT, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight);
	_vBattleEvent.push_back(pUnitDeathEvent);

	//유닛 레벨업 이벤트 클래스는 미리 등록한다
	levelUpEvent* plevelUpEvent;
	plevelUpEvent = new levelUpEvent();
	plevelUpEvent->init(DEATH_EVENT, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight);
	_vBattleEvent.push_back(plevelUpEvent);

	//유닛 레벨업 이벤트 클래스는 미리 등록한다
	classChangeEvent* pClassChangeEvent;
	pClassChangeEvent = new classChangeEvent();
	pClassChangeEvent->init(DEATH_EVENT, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight);
	_vBattleEvent.push_back(pClassChangeEvent);

	//서로 대화하는 이벤트는 미리 등록한다
	talkEventCreate();

	//게임 클리어 게임오버 확인 이벤트 클래스는 미리 등록한다
	battleEndEvent* pBattleEndEvent;
	pBattleEndEvent = new battleEndEvent();
	pBattleEndEvent->init(DEATH_EVENT, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight, _pBattleInfo);
	_vBattleEvent.push_back(pBattleEndEvent);

	return S_OK;
}
void PkwEventManager::release(void)
{
	vector<battleEvent*>::iterator		viBattleEvent;

	for (viBattleEvent = _vBattleEvent.begin(); viBattleEvent != _vBattleEvent.end(); viBattleEvent++){
		(*viBattleEvent)->release();
		delete (*viBattleEvent);
	}
	_vBattleEvent.clear();
}
void PkwEventManager::update(float elapsedTime)
{
	battleEvent* pEvent = NULL;

	vector<battleEvent*>::iterator		viBattleEvent;
	
	for (viBattleEvent = _vBattleEvent.begin(); viBattleEvent != _vBattleEvent.end();){
		//처리 해야할 이벤트가 종료 상태이면 벡터 안에서 없애준다
		if ( (*viBattleEvent)->_eventStatus == EVENT_END){
			(*viBattleEvent)->release();
			delete (*viBattleEvent);
			viBattleEvent = _vBattleEvent.erase(viBattleEvent);
		}
		else if ((*viBattleEvent)->_eventStatus == EVENT_DEACTIVE && _isEventManagerAction == false){
			//비활성화 이벤트는 체크를 해주다가 조건이 되면 활성화 된다
			(*viBattleEvent)->checkEvent();
			viBattleEvent++;
		}
		else{
			//액티브인 녀석은 아무것도 안하고 패스한다
			viBattleEvent++;
		}
	}

	//활성화된 이벤트를 찾는다(가장 앞에 있는 벡터 부터)
	for (int i = 0; i < _vBattleEvent.size(); i++){
		if (_vBattleEvent[i]->_eventStatus == EVENT_ACTIVE){
			pEvent = _vBattleEvent[i];
			break;
		}
	}

	//이벤트 벡터에 활성화된 이벤트가 아무것도 없으면 이벤트 매니져 비활성화 
	if (pEvent == NULL){
		_isEventManagerAction = false;
	}
	//이벤트 매니져 활성화 그리고 받은 이벤트만 업데이트
	else{
		_isEventManagerAction = true;
		pEvent->update(elapsedTime);
	}
}
void PkwEventManager::render(HDC memDC)
{
	battleEvent* pEvent = NULL;

	//활성화된 이벤트를 찾는다(가장 앞에 있는 벡터 부터)
	for (int i = 0; i < _vBattleEvent.size(); i++){
		if (_vBattleEvent[i]->_eventStatus == EVENT_ACTIVE){
			pEvent = _vBattleEvent[i];
			break;
		}
	}

	//이벤트 벡터에 활성화된 이벤트가 없으면 아무것도 안한다
	if (pEvent == NULL) return;
	//벡터에 있으면 가장 앞에 있는 이벤트 만 그려준다
	else  pEvent->render(memDC);
}

void PkwEventManager::addEvent(int eventType)
{
	//턴 변화 이벤트
	if (eventType == 0){
		turnChangeEvent* turnEvent;
		turnEvent = new turnChangeEvent();
		turnEvent->init(TURN_CHANGE_EVENT, _pBattleInfo->nTurn, _pBattleInfo->battleStatus, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight);
		_vBattleEvent.push_back(turnEvent);
	}
	//레벨 업 이벤트
	else if (eventType == 1){

	}
	//클래스 체인지 이벤트
	else if (eventType == 2){

	}
	//유닛 죽음 이벤트
	else if (eventType == 2){

	}
	//토크 이벤트
	else if (eventType == 3){

	}
	//게임 오버 이벤트
	else if (eventType == 4){

	}
	//게임 클리어 이벤트
	else if (eventType == 5){

	}
}
void PkwEventManager::talkEventCreate()
{
	ifstream file;
	char temp[512];
	int type;
	talkEvent* pTalkEvent = NULL;

	//데이터넣울줄떄마다 올라가는 카운터
	int talkCnt = -1;
	int nCnt = 0;

	file.open(DATAMANAGER->_stageDataFile[DATAMANAGER->_stageNum].stageTalkFile);

	while (!file.eof()){
		file.getline(temp, 512);
		if (strcmp(temp, "#talkDataStart") == 0){
			talkCnt = -1;
			type = 0;
			//토크 이벤트 생성
			pTalkEvent = new talkEvent();
			pTalkEvent->init(TALK_EVENT, &_pBttleMapScene->_MapStartPos, _pBttleMapScene->_MapMaxWidth, _pBttleMapScene->_MapManHeight, _pBattleInfo);
		}
		else if (strcmp(temp, "#talkTerms") == 0){
			nCnt = 0;
			type = 1;
		}
		else if (strcmp(temp, "#talkNumber") == 0){
			type = 2;
		}
		else if (strcmp(temp, "#talkCommander") == 0){
			talkCnt++;
			nCnt = 0;
			type = 3;
		}
		else if (strcmp(temp, "#talkDataEnd") == 0){
			//토크 데이터를 모두 넣었음으로 저장
			_vBattleEvent.push_back(pTalkEvent);
		}
		else if (temp[0] == '#') continue;
		else{
			if (type == 1){
				// 토크 조건 넣어주기
				pTalkEvent->readTalkTerms(temp, nCnt);
				nCnt++;
			}
			else if (type == 2){
				//데이터 갯수 넣어주기
				pTalkEvent->createTalkData(temp);
			}
			else if (type == 3){
				//토크 데이터를 모두 넣었음으로 저장
				pTalkEvent->readTalkData(talkCnt, temp, nCnt);
				nCnt++;
			}
		}
	}

	file.close();

	return;
	
}
void PkwEventManager::addTalkEvent()
{
	
}