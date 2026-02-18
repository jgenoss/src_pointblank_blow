#if !defined( __I3VTEX_FRAMEWORK_H)
#define __I3VTEX_FRAMEWORK_H

class i3VTexFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3VTexFramework);

protected:
	i3UIText *		m_pText;
	i3UIFont *		m_pFont;
	i3UIEditBox*	m_pEdit;


public:
	i3VTexFramework(void);
	virtual ~i3VTexFramework(void);

	virtual void		OnCreate(void);

	virtual	void	OnKeyDown( UINT32 nKey);
	virtual void	OnKeyUp( UINT32 nKey);
};

#endif
