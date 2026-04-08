#include "../stdafx.h"
#include "PkwEffectPlay.h"

PkwEffectPlay::PkwEffectPlay()
{
}
PkwEffectPlay::~PkwEffectPlay()
{
}

void PkwEffectPlay::Init()
{
	_vEffect.clear();
}
void PkwEffectPlay::release()
{
	for (int i = 0; i < _vEffect.size(); i++){
		if (_vEffect[i] != NULL){
			_vEffect[i]->release();
			delete _vEffect[i];
		}
	}
	_vEffect.clear();
}
void PkwEffectPlay::update()
{
	int effectSize = _vEffect.size();

	//이펙트 업데이트
	for (int i = 0; i < effectSize; ){
		// 이펙트가 다 끝났을 경우 마법 효과를 처리 하고 벡터에서 제거 한다.
		if (_vEffect[i]->getIsPlay() == false){
			_vEffect[i]->magicEffect();
			_vEffect[i]->release();
			delete _vEffect[i];
			_vEffect.erase(_vEffect.begin() + i);
			//이펙트를 제거하면 총 도는 이펙트 사이즈를 하나 줄여준다
			// magicEffect함수를 호출할때 추가된 이펙트들은 이번 업데이트에서 업데이트 해주지 않는다
			effectSize--;
		}
		// 이펙트를 업데이트
		else{
			_vEffect[i]->update();
			i++;
		}
	}
}
void PkwEffectPlay::render(HDC memDC)
{
	//이펙트 그리기
	for (int i = 0; i < _vEffect.size(); i++){
		_vEffect[i]->render(memDC);
	}
}
void PkwEffectPlay::AddEffet(PkwCharactor*	attackUnit, PkwCharactor*	targetUnit, POINT targetPos, SkillData skill)
{
	if (skill._number == -1){//치료 커맨드 발동
		PkwHealEffect* temp = new PkwHealEffect;
		temp->init(NULL, NULL, targetUnit, targetPos, skill, "effectHeal", 48, 48, 10, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 0){//매직 애로우
		
	}
	else if (skill._number == 1000){//파이어볼
		PkwFireBallMagic* temp = new PkwFireBallMagic;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "effectFireball", 32, 32, 4, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 1010){//메테오
		PkwMeteorMagic* temp = new PkwMeteorMagic;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "effectMeteo", 128, 128, 5, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 2000){//힐1
		PkwHeal_1Effect* temp = new PkwHeal_1Effect;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill,"effectHeal1", 16, 16, 6, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 3000){//어택1
		PkwAtkMagic* temp = new PkwAtkMagic;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "effectAtkMagic", 64, 64, 4, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 3002){//프로텍션1
		PkwProtectionMagic* temp = new PkwProtectionMagic;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "effectDefMagic", 64, 64, 4, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 5000){//텔레포트
		PkwTeleportMagic* temp = new PkwTeleportMagic;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "effectTeleport", 48, 48, 4, 0.15, false);
		_vEffect.push_back(temp);
	}
	else if (skill._number == 6000){//소환
		PkwPrayerSummon* temp = new PkwPrayerSummon;
		temp->init(&_vEffect, attackUnit, targetUnit, targetPos, skill, "23_prayer", 60, 64, 6, 0.15, false);
		_vEffect.push_back(temp);
	}
}
int PkwEffectPlay::getEffectSize()
{
	return _vEffect.size();
}
