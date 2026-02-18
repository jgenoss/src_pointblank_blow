#if !defined( __I3_OBJECT_EDITOR_FRAMEWORK_H)
#define __I3_OBJECT_EDITOR_FRAMEWORK_H

class i3ObjectEditorFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3ObjectEditorFramework, i3ViewerFramework);
protected:

	i3Node *			m_pCaster;
	i3Node *			m_pReceiver;
	i3Transform2 *		m_pFloorRoot;
	i3PhysixShapeSet *	m_pShapeSet;

	i3Transform *		m_pEmitEffectIcon;

protected:
	void				_CreateFloor(void);
	void				_ResizeFloor(void);

public:
	i3ObjectEditorFramework(void);
	virtual ~i3ObjectEditorFramework(void);

	void				setEmitEffectIconEnable( bool bFlag);
	void				setEmitEffectIconPos( MATRIX * pMtx);

	virtual void		OnCreate(void) override;
	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes) override;
	virtual		void	OnChangeBound(void) override;

	virtual void		OnUpdate( REAL32 tm) override;
	virtual void		OnChangeTime( REAL32 tm) override;
	virtual bool		OnStartPlay(void) override;
	virtual bool		OnStartPause(void) override;
};

#endif
