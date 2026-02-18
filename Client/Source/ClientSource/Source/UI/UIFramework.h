#if !defined( __UI_FRAMEWORK_H__)
#define __UI_FRAMEWORK_H__


class UIFramework : public i3Stage
{
	I3_CLASS_DEFINE( UIFramework, i3Stage);

public:
	UIMainFrame *	GetUIMainframe( void)			{ return m_pMainfram; }

public:
	UIFramework();
	virtual ~UIFramework();

	virtual void	OnCreate( void) override;
	virtual void	OnLoadEnd( void) override;
	virtual bool	OnFinish( void) override;
	virtual bool	OnEvent( UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds ) override;

	bool OnQueryLoad(INT32 numStage);
	void OnEscape( void);

private:
	UIMainFrame *	m_pMainfram;

};

#endif
