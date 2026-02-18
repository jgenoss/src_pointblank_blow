#ifndef __I3_FADE_EFFECT_H__
#define __I3_FADE_EFFECT_H__

class i3FadeEffect : public i3ElementBase
{
	I3_CLASS_DEFINE( i3FadeEffect );
public:
	enum FADE_TYPE
	{
		FADE_IN,
		FADE_OUT,
		FADE_OUT_IN,
	};

protected:
	BOOL				m_bEnable;
	RT_REAL32			m_rElapsedTime;

	// 다음 프레임에서 Disable이 필요하다고 설정하기.
	// 외부에서 TreatPendingDisable()을 호출해 주어야 한다.
	BOOL				m_bPendingDisable;

	i3AttrSet			*m_pRoot;

	i3ColorAttr			*m_pFadeColorAttr;

	// 실제 Fade 효과가 진행되는 시간 비율. 
	// 범위는 0.0 ~ 1.0
	// 1.0 - 완전한 암흑이 보장되는 시간.
	RT_REAL32			m_rRealFadeRatio;	

	RT_REAL32			m_rTotalTime;

	RT_REAL32			m_rFadeTime;
	RT_REAL32			m_rRealFadeTime;
	RT_REAL32			m_rOutTime;
	RT_REAL32			m_rSlope;

	RT_REAL32			m_rDarkenTime; // FADE_OUT_IN 에서 검은 화면 지속시간

	FADE_TYPE			m_FadeType;

	RT_REAL32			m_rRatio;

public:
	i3FadeEffect();
	~i3FadeEffect();

	BOOL Create( i3Node *pParentNode );
	i3Node *GetRootNode(){ return m_pRoot; }

	BOOL IsEnable(){ return m_bEnable; }
	void SetEnable( BOOL bTrue = TRUE );

	void SetPendingDisable(){ m_bPendingDisable = TRUE; }
	void TreatPendingDisable()
	{
		if( m_bPendingDisable )
		{
			SetEnable( FALSE );
		}
	}

	// Fade 효과 시작하기
	// FAD_IN, FADE_OUT 에서느 rFadeTime 전체 이펙트 시간과 동일
	// FADE_OUT_IN에서는 rFadeTime * 2 + rDarkenTime 만큼의 시간이 소요.
	// rDarkenTime는 검은화면 지속 시간. FADE_OUT, FADE_OUT_IN에만 적용.
	// bReusePrevTime은 현재 화면을 카피해서 재사용하는 방식을 말함 (구현유보)
	void Start( 
		FADE_TYPE FadeType, 
		RT_REAL32 rFadeTime = __RT( 0.5f ),
		RT_REAL32 rDarkenTime = __RT_0,
		BOOL bReusePrevFrame = FALSE
		);

	BOOL OnUpdate( RT_REAL32 rDeltaSeconds );
};

#endif //__I3_FADE_EFFECT_H__
