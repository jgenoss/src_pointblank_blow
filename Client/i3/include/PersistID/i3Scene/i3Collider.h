#if !defined( __I3_COLLIDER_H)
#define __I3_COLLIDER_H

#include "i3Collidee.h"

#define			I3_COLLIDER_STYLE_STATIC				0x00000001
#define			I3_COLLIDER_STYLE_ONCE_TRANSFORM		0x00000002
#define			I3_COLLIDER_STYLE_TRANSFORMED			0x00000004

enum I3_COLLIDER_TYPE
{
	I3_COLLIDER_NONE	= 0,
	I3_COLLIDER_LINE,
	I3_COLLIDER_SPHERE,
	I3_COLLIDER_CYLINDER,
	I3_COLLIDER_MESH,
	I3_COLLIDER_CAPSULE,
};

class I3_EXPORT_SCENE i3Collider : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3Collider);

protected:
	i3Collidee *		m_pLocal;
	i3Collidee *		m_pWorld;

	UINT32				m_Style;

	I3_COLLIDER_TYPE	m_Type;

public:
	i3Collider(void);
	virtual ~i3Collider(void);

	I3_COLLIDER_TYPE	GetType(void)						{ return m_Type; }
	void				SetType( I3_COLLIDER_TYPE type)		{ m_Type = type; }

	i3Collidee	*	GetCollidee(void)				{ return m_pLocal; }
	void			SetCollidee( i3Collidee * pCol);

	i3Collidee *	GetTransformedCollidee(void)
	{
		if( m_Style & I3_COLLIDER_STYLE_STATIC)
			return m_pLocal;
		
		return m_pWorld; 
	}

	UINT32			GetStyle(void)					{ return m_Style; }
	void			SetStyle( UINT32 style);

	void			Transform( MATRIX * pMatrix);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);

};

#endif
