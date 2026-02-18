#if !defined( __GAME_CHARA_BONE_DEFINE_H__)
#define __GAME_CHARA_BONE_DEFINE_H__

enum BONES
{
	BONE_ROOT = 0,
	BONE_PELVIS,
	BONE_IK,
	BONE_SPINE1,
	BONE_SPINE2,
	BONE_SPINE3,

	BONE_NECK,
	BONE_HEAD,
	BONE_LFOOT,
	BONE_RFOOT,
	BONE_LDARM,
	BONE_RDARM,

	BONE_LLEGU,
	BONE_RLEGU,
	BONE_LLEGD,
	BONE_RLEGD,

	BONE_MAX
};

enum RAPTOR_BONES
{
	RAPTOR_BONE_ROOT = 0,
	RAPTOR_BONE_PELVIS,
	RAPTOR_BONE_IK,
	RAPTOR_BONE_SPINE,
	RAPTOR_BONE_SPINE1,
	RAPTOR_BONE_SPINE2,
	
	RAPTOR_BONE_NECK,
	RAPTOR_BONE_NECK1,
	RAPTOR_BONE_NECK2,
	RAPTOR_BONE_HEAD,

	RAPTOR_BONE_FOOT_LEFT,
	RAPTOR_BONE_FOOT_RIGHT,

	RAPTOR_BONE_PIVOTCAM,

	RAPTOR_BONE_MAX
};

enum TREX_BONES
{
	TREX_BONE_ROOT = 0,
	TREX_BONE_PELVIS,
	TREX_BONE_IK,
	TREX_BONE_SPINE,
	TREX_BONE_SPINE1,
	TREX_BONE_SPINE2,
	
	TREX_BONE_IKNECK,
	TREX_BONE_NECK,
	TREX_BONE_NECK1,
	TREX_BONE_NECK2,
	TREX_BONE_IK_HEAD,
	TREX_BONE_HEAD,

	TREX_BONE_FOOT_LEFT,
	TREX_BONE_FOOT_RIGHT,

	TREX_BONE_PIVOTCAM,

	TREX_BONE_MAX,
};

#ifdef DOMI_DINO
enum DOMI_RAPTOR_BONES
{
	DOMI_RAPTOR_BONE_ROOT = 0,
	DOMI_RAPTOR_BONE_PELVIS,
	DOMI_RAPTOR_BONE_IK,
	DOMI_RAPTOR_BONE_SPINE,
	DOMI_RAPTOR_BONE_SPINE1,

	DOMI_RAPTOR_BONE_NECK,
	DOMI_RAPTOR_BONE_NECK1,
	DOMI_RAPTOR_BONE_HEAD,

	DOMI_RAPTOR_BONE_FOOT_LEFT,
	DOMI_RAPTOR_BONE_FOOT_RIGHT,

	DOMI_RAPTOR_BONE_PIVOTCAM,

	DOMI_RAPTOR_BONE_MAX
};

enum MUTANT_TREX_BONES
{
	DOMI_MUTANTREX_BONE_ROOT = 0,
	DOMI_MUTANTREX_BONE_PELVIS,
	DOMI_MUTANTREX_BONE_IK,
	DOMI_MUTANTREX_BONE_SPINE,
	DOMI_MUTANTREX_BONE_SPINE1,

	DOMI_MUTANTREX_BONE_IKNECK,
	DOMI_MUTANTREX_BONE_NECK,
	DOMI_MUTANTREX_BONE_NECK1,
	DOMI_MUTANTREX_BONE_IK_HEAD,
	DOMI_MUTANTREX_BONE_HEAD,

	DOMI_MUTANTREX_BONE_FOOT_LEFT,
	DOMI_MUTANTREX_BONE_FOOT_RIGHT,

	DOMI_MUTANTREX_BONE_PIVOTCAM,

	DOMI_MUTANTREX_BONE_MAX,
};
#endif


enum DEVIATION_STATE
{
	DEVIATION_FIT = 0,
	DEVIATION_CHANGE,
};

enum RECOIL_HORZ_STATE
{
	RECOIL_HORZ_NA = 0,
	RECOIL_HORZ_FIRING,
	RECOIL_HORZ_RESTORE
};

struct CHARAPLAYER_RECOIL
{
	VAR_LR(REAL32, 236)	m_RecoilTime = 0.0f;
	VAR_LR(REAL32, 237)	m_RecoilTargetTime = 0.0f;
	VAR_LR(REAL32, 238)	m_RecoilAngle = 0.0f;		//	수직 반동각
	VAR_LR(REAL32, 239)	m_RecoilTargetAngle = 0.0f;

	VAR_LR(REAL32, 240)	m_RecoilHorzAngle = 0.0f;
	VAR_LR(REAL32, 241)	m_RecoilHorzTime = 0.0f;
	VAR_LR(REAL32, 242)	m_RecoilHorzTimeTarget = 0.0f;
	VAR_LR(REAL32, 243)	m_rRecoilHorzTimeTarget = 0.0f;

	VAR_LR(RECOIL_HORZ_STATE, 244)	m_HorzState = RECOIL_HORZ_NA;
	VAR_LR(REAL32, 245)	m_HorzStartAngle = 0.0f;
	VAR_LR(REAL32, 246)	m_timeHorzRestore = 0.0f;

	VAR_LR(REAL32, 247)	m_RecoilHorzAngle_Recovertime = 1.0f;
	VAR_LR(REAL32, 248)	m_RecoilVertAngle_Recovertime = 1.0f;

	void	Reset() {
		m_RecoilTime = 0.f;
		m_RecoilTargetTime = 0.f;
		m_RecoilAngle = 0.f;
		m_RecoilTargetAngle = 0.f;
		m_RecoilHorzAngle = 0.f;
		m_RecoilHorzTime = 0.f;
		m_RecoilHorzTimeTarget = 0.f;
		m_rRecoilHorzTimeTarget = 0.f;
		m_HorzState = RECOIL_HORZ_NA;
		m_HorzStartAngle = 0.0f;
		m_timeHorzRestore = 0.0f;
		m_RecoilHorzAngle_Recovertime = 1.0f;
		m_RecoilVertAngle_Recovertime = 1.0f;
	}

	void	cancelRecoilHorz(void)
	{
		m_timeHorzRestore = 0.0f;
		m_HorzState = RECOIL_HORZ_NA;
	}

	void	_calcRecoil(REAL32 tm) {
		if (m_RecoilAngle > 0.f) {
			m_RecoilTime = tm + m_RecoilTime;
			if (m_RecoilTargetTime > 0.0f) {
				REAL32 cos_val;
				cos_val = i3Math::vcos(1.0f + (m_RecoilTime / m_RecoilTargetTime));
				m_RecoilAngle = m_RecoilTargetAngle + (cos_val * m_RecoilTargetAngle);
				if (m_RecoilAngle < 0.0f)	m_RecoilAngle = 0.0f;
			}
			if (m_RecoilTargetTime < m_RecoilTime) {
				m_RecoilAngle = 0.0f;	m_RecoilTime = 0.0f;
			}
		}
	}
};

struct CHARAPLAYER_DEVIATION
{
	REAL32			m_DeviationTargetAngle = 0.0f;
	REAL32			m_DeviationStartAngle = 0.0f;
	REAL32			m_DeviationAngle = 0.0f;
	REAL32			m_FireDeviation = 0.0f;
	REAL32			m_DeviationTargetTime = 3.0f;
	REAL32			m_DeviationTime = 3.0f;
	DEVIATION_STATE	m_DeviationState = DEVIATION_FIT;

	void	Reset() {
		m_DeviationTargetAngle = m_DeviationStartAngle = m_DeviationAngle = 0.f;
		m_DeviationTargetTime = m_DeviationTime = 3.f;
		m_DeviationState = DEVIATION_FIT;
		m_FireDeviation = 0.0f;
	}

	void	_calcDeviation(REAL32 tm) {
		if (m_DeviationState == DEVIATION_CHANGE) {
			REAL32 cos_val;
			m_DeviationTime += tm;
			if (m_DeviationTargetTime > I3_EPS) {
				cos_val = i3Math::vsin_libm((m_DeviationTime / m_DeviationTargetTime));
				m_DeviationAngle = m_DeviationStartAngle + (cos_val * (m_DeviationTargetAngle - m_DeviationStartAngle));
			}

			//I3TRACE( "%f, %f\n", m_DeviationTime, m_DeviationAngle);

			if (m_DeviationTargetTime < m_DeviationTime) {
				m_DeviationAngle = m_DeviationTargetAngle;
				m_DeviationStartAngle = m_DeviationTargetAngle;
				m_DeviationTime = 0.0f;
				m_DeviationState = DEVIATION_FIT;
			}
		}
	}
};

struct CHARAPLAYER_SCOPEWAVE
{
	REAL32	m_timeScopeWaveUpdate = 0.0f;
	REAL32	m_ScopeWaveTargetTheta = 0.0f;
	REAL32	m_ScopeWaveTheta = 0.0f;
	REAL32	m_ScopeWaveThetaVel = 0.0f;
	REAL32	m_ScopeWaveTargetRho = 0.0f;
	REAL32	m_ScopeWaveRho = 0.0f;
	REAL32	m_ScopeWaveRhoVel = 0.0f;
	INT32	m_nCurScopeLevel = 0;	//	현재 줌 단계

	void	Reset() {
		m_timeScopeWaveUpdate = m_ScopeWaveTargetTheta = m_ScopeWaveTheta = m_ScopeWaveThetaVel = 0.f;
		m_ScopeWaveTargetRho = m_ScopeWaveRho = m_ScopeWaveRhoVel = 0.f;
		m_nCurScopeLevel = 0;
	}

	void	_calcScopeWave(REAL32 tm) {
		if ((m_ScopeWaveTheta - m_ScopeWaveTargetTheta) > 0.0f) m_ScopeWaveThetaVel -= 0.00001f;
		else	m_ScopeWaveThetaVel += 0.00001f;
		if ((m_ScopeWaveRho - m_ScopeWaveTargetRho) > 0.0f) m_ScopeWaveRhoVel -= 0.001f;
		else	m_ScopeWaveRhoVel += 0.001f;
		m_ScopeWaveTheta += m_ScopeWaveThetaVel;
		m_ScopeWaveRho += m_ScopeWaveRhoVel;
		m_timeScopeWaveUpdate += tm;
		if (m_timeScopeWaveUpdate < 1.0f)	return;
		m_timeScopeWaveUpdate = 0.0f;
		m_ScopeWaveTargetTheta = i3Math::vsin(i3Math::Randf2()) * 0.05f;
		m_ScopeWaveTargetRho = i3Math::vcos(i3Math::Randf2()) * 0.1f;
	}
};

#endif
