#if !defined( __I3_Level_LINK_H)
#define __I3_Level_LINK_H

#include "i3LevelControl.h"

class I3_EXPORT_TDK i3LevelLink : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelLink, i3LevelControl);
protected:
	i3LevelElement3D *		m_pStart = nullptr;
	i3LevelElement3D *		m_pEnd = nullptr;
	
	// Volatile Members
	i3AttrSet *				m_pAttrSet;				// [initialize at constructor]
	i3GeometryAttr *		m_pGeoAttr;				// [initialize at constructor]
	i3Geometry *			m_pGeo;					// [initialize at constructor]
	i3UserNode *			m_pUserNode;			// [initialize at constructor]

	VEC3D					m_OldStartPos;
	VEC3D					m_OldEndPos;
	bool					m_bFirst = true;

	COLOR					m_Color;				// [initialize at constructor]

public:
	i3LevelLink(void);
	virtual ~i3LevelLink(void);

	static void _LinkUpdateProc( i3SceneTracer * pTracer, void *pParam);

	i3LevelElement3D *	GetStartElement(void)		{ return m_pStart; }
	void				SetStartElement( i3LevelElement3D * pObj);

	i3LevelElement3D *	GetEndElement(void)			{ return m_pEnd; }
	void				SetEndElement( i3LevelElement3D * pObj);

	COLOR *				getColor(void)				{ return &m_Color; }
	void				setColor( UINT8 r, UINT8 g, UINT8 b, UINT8 a)
	{
		m_Color.r = r;
		m_Color.g = g;
		m_Color.b = b;
		m_Color.a = a;
	}
	void				setColor( COLOR * pCol)
	{
		i3Color::Set( &m_Color, pCol);
	}

	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
};

#endif
