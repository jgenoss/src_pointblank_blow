#pragma once

class CHUD_TextEtc : public i3GameObjBase
{
	I3_CLASS_DEFINE(CHUD_TextEtc, i3GameObjBase);

public:
	CHUD_TextEtc(void);
	~CHUD_TextEtc(void);

public:
	static CHUD_TextEtc* instance();

public:
	void CreateHUD(i3AttrSet* pHudRoot, i3Texture* pTexture);
	void OnUpdate( REAL32 rDeltaSeconds );
	void SetEtcText( const i3::wliteral_range& wTextRng);

private:
	i3TextNodeDX2 *	m_pTextEtc;
	//i3Sprite2D *	m_pSpriteEtcMessageBG;
	REAL32 m_fEtcTextTime;
};
