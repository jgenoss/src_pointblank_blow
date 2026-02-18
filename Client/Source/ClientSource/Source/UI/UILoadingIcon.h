#if !defined( __UI_LOADING_ICON_H__)
#define __UI_LOADING_ICON_H__

class UILoadingIcon : public i3GameNode
{
	I3_CLASS_DEFINE( UILoadingIcon, i3GameNode);

private:
	i3UIControl *			m_pFrame;
	i3UIButtonImageSet *	m_pIcon;

	static i3UITemplate *	m_pTemplate;

	REAL32			m_Delay;
	INT32			m_ImageIdx;

public:
	UILoadingIcon();
	virtual ~UILoadingIcon();

	bool			IsUsed( void)								{ return (m_pFrame!=nullptr)?true:false; }

	void			EnableLoadingIcon( i3UIControl * pCtrl, bool bVisibleChild);
	void			DisableLoadingIcon( void);

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif
