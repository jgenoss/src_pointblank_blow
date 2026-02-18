#ifndef __I3_FADE_EFFECT_H__
#define __I3_FADE_EFFECT_H__

#include "i3FadeEffectType.h"			// 내포 enum타입이 클래스 정의 의존성을 만드는데, i3FadeEffect는 익스포트클래스가 아니라서 바깥으로 뺍니다. (2012.08.30.수빈)
// 익스포트추가->다시 제거. (12.09.19.수빈)
class i3FadeEffect : public i3ElementBase
{
	I3_CLASS_DEFINE( i3FadeEffect, i3ElementBase );
protected:
	bool				m_bEnable = false;
	REAL32			m_rElapsedTime = 0.0f;

	// 다음 프레임에서 Disable이 필요하다고 설정하기.
	// 외부에서 TreatPendingDisable()을 호출해 주어야 한다.
	bool				m_bPendingDisable = false;

	i3AttrSet			*m_pRoot = nullptr;

	i3ColorAttr			*m_pFadeColorAttr = nullptr;
	i3GeometryAttr *	m_pGeoAttr = nullptr;

	// 실제 Fade 효과가 진행되는 시간 비율. 
	// 범위는 0.0 ~ 1.0
	// 1.0 - 완전한 암흑이 보장되는 시간.
	REAL32			m_rRealFadeRatio = 0.75f;

	REAL32			m_rTotalTime = 0.0f;

	REAL32			m_rFadeTime = 0.0f;
	REAL32			m_rRealFadeTime = 0.0f;
	REAL32			m_rOutTime = 0.0f;
	REAL32			m_rSlope = 0.0f;

	REAL32			m_rDarkenTime = 0.0f; // FADE_OUT_IN 에서 검은 화면 지속시간

	i3FadeEffectType	m_FadeType = I3_FADE_EFFECT_FADE_IN;

	REAL32			m_rRatio = 0.0f;

protected:
	void				_CreateVertex(void);

public:
	~i3FadeEffect();

	bool Create( i3Node *pParentNode );
	i3Node *GetRootNode(){ return m_pRoot; }

	bool IsEnable(){ return m_bEnable; }
	void SetEnable( bool bTrue = true );

	void SetPendingDisable() { m_bPendingDisable = true; }
	void TreatPendingDisable()
	{
		if( m_bPendingDisable )
		{
			SetEnable(false);
		}
	}

	// Fade 효과 시작하기
	// FAD_IN, FADE_OUT 에서느 rFadeTime 전체 이펙트 시간과 동일
	// FADE_OUT_IN에서는 rFadeTime * 2 + rDarkenTime 만큼의 시간이 소요.
	// rDarkenTime는 검은화면 지속 시간. FADE_OUT, FADE_OUT_IN에만 적용.
	// bReusePrevTime은 현재 화면을 카피해서 재사용하는 방식을 말함 (구현유보)
	void Start( 
		i3FadeEffectType FadeType, 
		REAL32 rFadeTime = 0.5f,
		REAL32 rDarkenTime = 0.0f,
		bool bReusePrevFrame = false
		);

	bool OnUpdate( REAL32 rDeltaSeconds );

	void		OnDestroy(void);
	void		OnRevive(void);
};

#endif //__I3_FADE_EFFECT_H__
