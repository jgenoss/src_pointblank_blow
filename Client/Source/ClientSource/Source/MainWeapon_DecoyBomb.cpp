#include "pch.h"
#include "MainWeapon_DecoyBomb.h"
#include "i3Base/string/ext/utf16_to_utf8.h"

//
// DecoySndIntervalGenerator
//==================================================================================

MainWeapon_DecoyBomb::DecoySndIntervalGenerator::DecoySndIntervalGenerator(
	const WeaponDecoySndInterval* intervalData)
	: m_intervalData(intervalData)
{
	Reset();
}

MainWeapon_DecoyBomb::DecoySndIntervalGenerator::~DecoySndIntervalGenerator()
{
}

INT32 MainWeapon_DecoyBomb::DecoySndIntervalGenerator::Update(REAL32 dSecond)
{
	if(!m_intervalData || m_intervalData->GetNum() == 0)
		return -1;

	if(m_elapsed == -1.0f)
	{
		m_elapsed = dSecond;
		m_lastIntervalChange = 0.0f;

		return 1;
	}

	REAL32 lastElapsed = 0.0f;

	m_elapsed += dSecond;
	lastElapsed = m_elapsed - m_lastIntervalChange;

	REAL32 curInterval = m_intervalData->GetInterval(m_curIndexInterval);

	if(lastElapsed > curInterval)
	{
		m_curIndexInterval++;
		m_lastIntervalChange = m_elapsed;

		if(m_intervalData->GetNum() < m_curIndexInterval+1)
		{
			if(m_intervalData->GetLoop())
			{
				Reset(true);
				return 1;
			}
			else
				return -1; // 종료
		}
		else
			return 1; // 다음 interval변경되었음
	}

	return 0; // 계속 진행
}

void MainWeapon_DecoyBomb::DecoySndIntervalGenerator::Reset(bool loop)
{
	m_indexInterval = 0;
	m_elapsed = -1.0f;
	m_lastIntervalChange = 0.0f;
	m_curIndexInterval = 0;
}

//==================================================================================

//
// WeaponDecoyBomb
//==================================================================================

MainWeapon_DecoyBomb::MainWeapon_DecoyBomb(WeaponBase* p) : MainWeapon_Grenade(p), 
															m_intervalGenerator(nullptr), m_decoySndPlayState(nullptr), m_decoySndPlayInfo(nullptr), 
															m_decoyEffect(nullptr)
{
	m_decoyEffectIndex = g_pEffectManager->FindEffect("Bomb_Light_Red");
}

MainWeapon_DecoyBomb::~MainWeapon_DecoyBomb(void)
{
	ResetDecoyStatus();
}

void MainWeapon_DecoyBomb::Explosion()
{
	MainWeapon_Grenade::Explosion();
	// 강제종료
	UpdateDecoying(-1.0f);
}

void MainWeapon_DecoyBomb::OnSettled()
{
	if(m_isSetted)
		return;

	if(!isBombState(WEAPON_BOMBSTATE_SETTLED))
		return;

	m_isSetted = true;
	m_isDecoying = true;
	m_decoyLifeTime = m_pWeaponBase->GetExplosionTime();

	if(m_decoyLifeTime < 0.01)
		m_decoyLifeTime = 0.01f;

	m_intervalGenerator = new DecoySndIntervalGenerator( _GetSndInterval() );

	VEC3D* pos = m_pWeaponBase->GetPos();
	i3Matrix::SetTranslate(&m_decoySndTM, pos->x, pos->y, pos->z);

	// 이펙트시작
	if(m_decoyEffectIndex != -1)
		m_decoyEffect = g_pEffectManager->AddEffect(m_decoyEffectIndex, m_pWeaponBase->GetPos(), &I3_YAXIS);
}

void MainWeapon_DecoyBomb::ResetDecoyStatus()
{
	m_isDecoying = false;
	m_decoyLifeTime = 0.0f;
	m_decoyTime = 0.0f;

	I3_SAFE_DELETE(m_intervalGenerator);
	m_oneTimeDecoySndCreate = false;

	if(m_decoySndPlayState)
		m_decoySndPlayState->Stop();

	m_decoySndPlayState = nullptr;
	I3_SAFE_RELEASE(m_decoySndPlayInfo);

	WEAPON_EFFECT_FORCE_DELETE(m_decoyEffect);

	i3Matrix::Identity(&m_decoySndTM);
}

void MainWeapon_DecoyBomb::Reset()
{
	MainWeapon_Grenade::Reset();

	m_isSetted = false;
	ResetDecoyStatus();
}

WeaponBase* MainWeapon_DecoyBomb::GetOwnerWeapon( WEAPON_SLOT_TYPE slot)
{
	CGameCharaBase* owner = m_pWeaponBase->getOwner();

	if(!owner)
		return nullptr;

	return owner->GetCharaWeapon( slot);
}


bool MainWeapon_DecoyBomb::UpdateDecoying(REAL32 dSecond)
{
	if( isBombState(WEAPON_BOMBSTATE_SETTLED) )
	{
		OnSettled();
	}

	if(!m_isDecoying)
		return false;

	bool forceEndingDecoy = false;

	if(dSecond == -1.0f)
		forceEndingDecoy = true;
	else
	{	
		m_decoyTime += dSecond;

		if(m_decoyTime > m_decoyLifeTime)
			forceEndingDecoy = true;
	}

	if(forceEndingDecoy)
	{
		if(m_decoySndPlayState)
		{
			m_decoySndPlayState->Stop();
			m_decoySndPlayState = nullptr;
		}

		I3_SAFE_RELEASE(m_decoySndPlayInfo);
		WEAPON_EFFECT_FORCE_DELETE(m_decoyEffect);

		m_isDecoying = false;
		return false;
	}

	//
	// Decoy sound
	//

	// 난사처리이기에 시간이 밀려서 elapsed타임이 커질경우 
	// 정확히 interval간격에 따라 한틱에 여러번호출하지는않음

	i3SoundContext* ctxSnd = g_pViewer->GetSoundContext();
	bool generateSnd = false;
	bool skipSndUpdate = false;

	// 0 = 진행중, 1=interval이 끝나서 다음interval시작, -1:종료
	INT32 intervalOper = m_intervalGenerator->Update(dSecond);

	if(!ctxSnd || intervalOper == -1)
		skipSndUpdate = true;

	if(intervalOper == 1)
		generateSnd = true;

	if(!skipSndUpdate && generateSnd)
	{
		if(!m_decoySndPlayInfo && !m_oneTimeDecoySndCreate)
		{
			i3SoundPlayInfo* decoySndPlayInfo = _GetSoundPlayInfo();

			if(decoySndPlayInfo)
			{
				m_decoySndPlayInfo = i3SoundPlayInfo::new_object();
				decoySndPlayInfo->CopyTo(m_decoySndPlayInfo, I3_COPY_INSTANCE);
				m_decoySndPlayInfo->setMuteAtMaxDistance(true);
			}

			m_oneTimeDecoySndCreate = true;
		}

		if(ctxSnd)
		{
			if(m_decoySndPlayInfo)
			{
				if(m_decoySndPlayState && m_decoySndPlayState->getPlayInfo() == m_decoySndPlayInfo)
				{	// 이전과 동일사운드 생성이면 재사용
					m_decoySndPlayState = ctxSnd->RePlay(m_decoySndPlayState, m_decoySndPlayInfo, &m_decoySndTM);
				}
				else
				{
					// 신규사운드
					m_decoySndPlayInfo->set3DMode(I3SND_RT_3DMODE_WORLD_SPACE);
					m_decoySndPlayState = ctxSnd->Play(m_decoySndPlayInfo, &m_decoySndTM);
				}

				skipSndUpdate = true;
			}
		}
	}

	if(!skipSndUpdate)
	{
		// 움직일일이없을테니
		//i3Matrix::SetTranslate(&m_decoySndTM, pPos);
		//m_decoySndPlayState->UpdatePos( &mtx);
	}

	return true;
}

i3SoundPlayInfo* MainWeapon_DecoyBomb::GetDefaultDecoySound()
{
	i3::rc_wstring wstrDefaultSound;
	m_pWeaponBase->getWeaponInfo()->GetDefaultDecoySound(wstrDefaultSound);

	i3::stack_string strDefaultSound;
	i3::utf16_to_utf8(wstrDefaultSound, strDefaultSound);

	i3GameResSound * pSoundRes = (i3GameResSound *)g_pFramework->QuaryResource(strDefaultSound.c_str()); //왜인지 FindResourceByOnlyName로는 안찾아짐

	if(!pSoundRes)
		return nullptr;

	return pSoundRes->getSound();
}

/*virtual*/const WeaponDecoySndInterval * MainWeapon_DecoyBomb::_GetSndInterval(void)
{
	return g_pWeaponManager->GetDecoySndInterval();
}

/*virtual*/ i3SoundPlayInfo * MainWeapon_DecoyBomb::_GetSoundPlayInfo(void)
{
	WeaponBase* weapon = GetOwnerWeapon(WEAPON_SLOT_PRIMARY);
	if(weapon)
		return weapon->GetFireSoundPlayInfo(WEAPON::FIRE_SOUND_NORMAL);

	return GetDefaultDecoySound();
}

//==================================================================================