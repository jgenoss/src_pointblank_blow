#if !defined( __I3_LEVEL_PATHLINK_H)
#define __I3_LEVEL_PATHLINK_H

#include "i3LevelControl.h"

class i3LevelPath;

class I3_EXPORT_TDK i3LevelPathLink : public i3LevelControl
{
	I3_EXPORT_CLASS_DEFINE( i3LevelPathLink, i3LevelControl);
protected:
	i3LevelControl *		m_pStart = nullptr;
	i3LevelControl *		m_pEnd = nullptr;

	UINT32					m_PathStyle = i3PathLine::STYLE_BIDIRECTION;
	REAL32					m_Cost = 0.0f;
	
	// Volatile Members
	i3AttrSet *				m_pAttrSet;				// [initialize at constructor]
	i3Billboard *			m_pBillboard;			// [initialize at constructor]

	INT32					m_Depth = 0;

public:
	void				UpdateAll(void);

public:
	i3LevelPathLink(void);
	virtual ~i3LevelPathLink(void);

	UINT32				getPathStyle(void)				{ return m_PathStyle; }
	void				setPathStyle( UINT32 style)		{ m_PathStyle = style; }
	void				addPathStyle( UINT32 mask)		{ m_PathStyle |= mask; }
	void				removePathStyle( UINT32 mask)	{ m_PathStyle &= ~mask; }

	bool				isBidirectional(void)		{ return (m_PathStyle & i3PathLine::STYLE_BIDIRECTION) != 0; }
	void				setBidirectional( bool bFlag);

	INT32				getDepth(void)					{ return m_Depth; }
	void				setDepth( INT32 depth)			{ m_Depth = depth; }

	REAL32				getCost(void)					{ return m_Cost; }
	void				setCost( REAL32 val)			{ m_Cost = val; }

	i3LevelControl *	GetStartElement(void)		{ return m_pStart; }
	void				SetStartElement( i3LevelControl * pObj);

	i3LevelControl *	GetEndElement(void)			{ return m_pEnd; }
	void				SetEndElement( i3LevelControl * pObj);

	virtual void		UpdateLink(void);

	virtual REAL32		OnTouch( VEC3D * pStart, VEC3D * pEnd, MATRIX * pViewProj, bool bPick, bool bSelect, SEL_MODE mode) override;

	virtual void		OnSelected( BOOL bSelected) override;

	virtual void		OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32		OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32		OnLoad( i3ResourceFile * pResFile) override;
	virtual bool		OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
};

#endif
