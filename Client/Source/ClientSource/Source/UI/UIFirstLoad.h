#if !defined( __UI_FIRST_LOAD_H__)
#define __UI_FIRST_LOAD_H__
#define MAX_LOAD_TEXT 8

#include "UIBase.h"


class UIFirstLoad : public UIBase
{
	I3_CLASS_DEFINE( UIFirstLoad, UIBase);
private:
	INT32			m_ImageCount;
	INT32			m_ImageIdx;
	REAL32			m_ChangeImageTime;
	REAL32			m_rLoadingPercent;

	i3UIFrameWnd*			m_pLoadingBackGround;
	i3UIButtonImageSet *	m_pLoadingRotate;
	i3UIEditBox	*			m_pLoadingTextCtrl;

//	WCHAR16					m_pLoadingText[MAX_LOAD_TEXT*2];
//	char					m_pConv[MAX_LOAD_TEXT];

	vector<i3UIImageBoxEx*>	m_pIconArray;

	// Loading progress bar
	i3Sprite2D*				m_pLoadingProgressbar;
	i3AttrSet*				m_pAttrSetLoading;

	// ProgressBar Coord.
	REAL32 m_fScreenWidthRate;
	REAL32 m_fScreenHeightRate;
	REAL32 m_rProg_PosX;
	REAL32 m_rProg_PosY;
	REAL32 m_rProg_Height;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	bool			AddTradeIcon(char* Path);
public:
	UIFirstLoad();
	virtual ~UIFirstLoad();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnRevive( i3RenderContext * pCtx) override;
};

#endif
