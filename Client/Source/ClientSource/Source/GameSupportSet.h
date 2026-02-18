#ifndef _GAMESUPPORTSET_H__
#define _GAMESUPPORTSET_H__

#define MAX_DISK_FRAME	23

class CGameSupportSet :	public i3ElementBase
{
	I3_CLASS_DEFINE( CGameSupportSet, i3ElementBase );

protected:
	// Loading Sub Sprite (Training or CCMode)
	i3Sprite2D				*		m_pSpriteTraining;
	i3Texture				*		m_pTextureTraining;
	
	// Loading BackGround
	i3Sprite2D				*		m_pSpriteBG;
	i3Texture				*		m_pTextureBG;

	// Loading Progress Bar
	i3Sprite2D				*		m_pSpriteProgress;
	REAL32							m_rLoadingPercent;
	
	// Loading Tip
	i3TextNodeDX2			*		m_pTextTip;
	
	i3AttrSet *						m_pRoot;

protected:
	void _SetLoadingBG(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate);
	void _SetSubLoadingImg(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate);
	void _SetLoadingTip(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate);
	void _SetLoadingProgress(REAL32 fScreenWidthRate, REAL32 fScreenHeightRate);
public:
	CGameSupportSet(void);
	~CGameSupportSet(void);

	void SetLoadingSpr(void);
	void UpdateLoadingProgress( REAL32 rDeltaSeconds);	
	void ReleaseLoadingSpr(void);

	void SaveScreenShot(void);
	void SaveAllFontTexture(void);

	REAL32 GetLoadingPercent(void) const	{ return m_rLoadingPercent; }
	void SetLoadingPercent(REAL32 rValue)	{ m_rLoadingPercent = rValue; }
};

#endif //_GAMESUPPORTSET_H__