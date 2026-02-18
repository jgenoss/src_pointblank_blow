#if !defined( __I3_LEVEL_LAYER_H)
#define __I3_LEVEL_LAYER_H

class i3LevelElement3D;

#define	I3LV_LAYER_STATE_SELECTED			0x00000001
#define I3LV_LAYER_STATE_COLLAPSED			0x00000002
#define I3LV_LAYER_STATE_HIDED				0x00000004
#define I3LV_LAYER_STATE_FREEZED			0x00000008

class I3_EXPORT_TDK i3LevelLayer : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3LevelLayer, i3NamedElement);
protected:
	UINT32			m_State = 0;
	i3::vector<i3LevelElement3D*>	m_ElmList;

public:
	virtual ~i3LevelLayer(void);

	UINT32					getState(void)							{ return m_State; }
	void					setState( UINT32 state)					{ m_State = state; }
	void					addState( UINT32 mask)					{ m_State |= mask; }
	void					removeState( UINT32 mask)				{ m_State &= ~mask; }

	INT32					getElementCount(void)					{ return (INT32)m_ElmList.size(); }
	i3LevelElement3D *		getElement( INT32 idx)					
	{ 
		I3ASSERT( (idx >= 0) && (idx < (INT32)m_ElmList.size()));

		return m_ElmList[idx]; 
	}

	void					AddElement( i3LevelElement3D * pObj);
	void					RemoveElement( i3LevelElement3D * pObj);
	INT32					FindElement( i3LevelElement3D * pObj);
	void					RemoveAllElement(void);

	void					Show( BOOL bShow);
	void					Freeze( BOOL bFreeze);

	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;
	virtual bool			OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool			OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
