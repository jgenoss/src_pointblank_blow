#if !defined( __TOOL_TIP_H__)
#define __TOOL_TIP_H__

#if JuuL_0

class CToolTip
{
public:
	CToolTip();
	~CToolTip();

	void	Init();
	void	Set(UINT32 idxTooltip);
	void	Update(REAL32 rDeltaSeconds);
	void	OnChangeVideo();

private:
	void CalcTooltipSize();
	void	CalcPosition();


private:
	UINT32	m_idxCurTooltip;
	BOOL	m_bEnableTooltip;
	i3TextNodeDX2 *	m_pTooltipText;
	i3Texture *		m_pTooltipTexture;
	i3Texture *		m_pTooltipTexture2;
	i3Sprite2D *	m_pTooltipSprite;
	i3Sprite2D *	m_pTooltipSprite2;

	static const REAL32 _TooltipDefalultWidth;
	static const REAL32 _TooltipDefalultHeight;
	REAL32 m_TooltipWidth;
	REAL32 m_TooltipHeight;
	REAL32 m_TooltipWidthRatio;
	REAL32 m_TooltipHeightRatio;
	REAL32 m_CurrentGuiResolutionWidth;
	REAL32 m_CurrentGuiResolutionHeight;
};


#endif
#endif
