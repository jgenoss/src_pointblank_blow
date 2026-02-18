#if !defined( __LOD_H)
#define __LOD_H

#define		SG_STYLE_SKIN		0x00000001

class CLOD : public i3ResourceObject
{
	I3_CLASS_DEFINE( CLOD, i3ResourceObject);
protected:
	i3SceneGraphInfo *		m_pSg;
	INT32					m_Level;
	bool					m_bGather;

	// Volatile Members
	i3LOD	*				m_pLOD;

	// 이하의 Member들은 Scene Graph가 설정될 때마다
	// 내부에서 계산되어 Update되는 값들이다.
	// 외부 입장에서는 Read-Only.
	INT32					m_BoneCount;
	INT32					m_TriCount;
	UINT32					m_SgStyle;

protected:
	INT32					_GetTriangleCount( i3GeometryAttr * pGeoAttr);
	void					_SetLOD(void);

public:
	CLOD(void);
	virtual ~CLOD(void);

	i3SceneGraphInfo *		getSceneGraph(void)			{ return m_pSg; }
	void					SetSceneGraph( i3SceneGraphInfo * pInfo, INT32 Lv, bool bGather);

	i3LOD *					getLOD(void)				{ return m_pLOD; }

	INT32					getBoneCount(void)			{ return m_BoneCount; }
	INT32					getTriCount(void)			{ return m_TriCount; }
	UINT32					getSgStyle(void)			{ return m_SgStyle; }

	void					BuildCollider();

	virtual void			CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void			OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
	virtual bool			OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool			OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);

};

#endif
