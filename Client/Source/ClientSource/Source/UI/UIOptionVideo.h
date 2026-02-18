#if !defined( __UI_OPTION_Video_H__)
#define __UI_OPTION_Video_H__

#include "UIOptionBase.h"
#include "ProgressEdit.h"
#include "VideoUtil.h" 

class UIOptionVideo : public UIOptionBase
{
	I3_CLASS_DEFINE( UIOptionVideo, UIOptionBase );

	enum{ MAX_PE = 2, };

public:
	void clicked(int idx1, int idx2);

private:
	VideoUtil m_Video;

	//screen 관련
	INT32 m_nScreenWidth;	//해상도
	INT32 m_nScreenHeight;
	BOOL m_bFullScreen; //전체모드

	INT32 m_AA;	//안티앨리어싱
	INT32 m_AAQ;
	BOOL m_bVSync;	//수직 동기화
	BOOL m_bTriLinearFilter;	//부드러운 원근 처리

	//next time 관련
	INT32 m_nTextureQualityType;	//텍스쳐 퀄리티
	INT32 m_nSpecularQualityType;	//빛반사 효과
	INT32 m_nEffectQuality;	//이펙트 품질
	BOOL m_bEnableNormalMap;	//세부 재질 표현

	//Shader가 해당 GPU에서 사용 가능한지 확인이 필요!!
	INT32 m_nShadowQualityType;	//그림자 효과
	BOOL	m_bDynamicLight;	//광원효과

	//즉시 변경
	INT32 m_nVideoResolution; //동영상 해상도
	//INT32 m_Quick_Setup;		// 비디오 품질 설정
	INT32 m_nQualityType;		// 비디오 품질 설정

	REAL32 m_fGammaVal;	//화면 밝기
	REAL32 m_fFovValue;	//시야각
	IntProgressEdit mPEdit[MAX_PE];
	
	BOOL m_bEnableBulletTrace;	//총알궤적
	BOOL m_bEnableBulletSmoke;	//총알궤적 이펙트
	BOOL m_bEnableTerrainEffect;	//지형 이펙트
	BOOL m_bEnablePhysX;	//물리 효과
	// 
	BOOL m_bDisplayOtherParts; //상대방 파츠 표시
	BOOL m_bFixedViewCharacter; //상대방 캐릭터 표시

	/*****************************/
	INT32 m_nScreenHz;
	INT32 m_ScreenDepth;
	BOOL m_b16BitsDepth;
	BOOL m_bEnableBloom;

private:
	//void set_quick_setup();
	void setupQuality(const INT32 type);
	void adjust_max_value();	//그래픽 카드 지원여부에 따라 값 변경

	void adjust_refresh_rate();	//해상도를 변경하면 해당 함수로 변경해줘야함.

public:
	void check_quick_setup_level();

private:
	void cb_edit_change1( INT32 value );
	void cb_edit_change2( INT32 value );
	IntProgress_Callback< UIOptionVideo, &UIOptionVideo::cb_edit_change1 > m_Callback1;
	IntProgress_Callback< UIOptionVideo, &UIOptionVideo::cb_edit_change2 > m_Callback2;

private:
	virtual void load_value() override;
	void load_detail_value();

	virtual void save_value() override;
	
	virtual void default_value() override;
	void default_detail_value();
	
	virtual void apply_game() override;
	virtual void apply_lua() override;

public:
	UIOptionVideo();
	virtual ~UIOptionVideo();

	virtual void _InitializeAtLoad( i3UIScene * pScene ) override;
	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual bool OnKey_Enter() override;
	virtual bool OnKey_Escape() override;
	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool is_modified() override;
	bool is_modified_screen() const;	//screen에 영향을 주는 값 변경.. 유저에게 물어본다.
	bool is_diff_default_RefreshRate() const;
	
	virtual bool key_escape() override;
	bool clicked_default();	//리턴값이 false 이면 원래 행동 한다.
	bool clicked_ok();	//리턴값이 false 이면 원래 행동 한다.
	bool clicked_cancel();	//리턴값이 false 이면 원래 행동 한다.
};

#endif
