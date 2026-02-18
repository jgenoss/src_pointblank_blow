#include "i3SceneDef.h"
#include "i3Collider.h"
#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3CollideeSphere.h"
#include "i3CollideeCylinder.h"
#include "i3CollideeMesh.h"
#include "i3CollideeCapsule.h"

I3_CLASS_INSTANCE( i3Collider);

i3Collider::~i3Collider(void)
{
	I3_SAFE_RELEASE( m_pLocal);
	I3_SAFE_RELEASE( m_pWorld);
}

void	i3Collider::SetCollidee( i3Collidee * pCol)
{
	I3_REF_CHANGE( m_pLocal, pCol);

	if( pCol != nullptr)
	{
		if( i3::same_of<i3CollideeLine*>(pCol))
			SetType( I3_COLLIDER_LINE);
		else if( i3::same_of<i3CollideeSphere*>(pCol))
			SetType( I3_COLLIDER_SPHERE);
		else if( i3::same_of<i3CollideeCylinder*>(pCol))
			SetType( I3_COLLIDER_CYLINDER);
		else if( i3::same_of<i3CollideeMesh*>(pCol))
			SetType( I3_COLLIDER_MESH);
		else if( i3::same_of<i3CollideeCapsule*>(pCol))
			SetType( I3_COLLIDER_CAPSULE);
	}
}

void	i3Collider::SetStyle( UINT32 style)
{
	m_Style = style;

	if((style & I3_COLLIDER_STYLE_STATIC))
	{
		I3_SAFE_RELEASE( m_pWorld);
	}
}

void i3Collider::Transform( MATRIX * pMatrix)
{
	I3ASSERT( m_pLocal != nullptr);

	if( m_Style & I3_COLLIDER_STYLE_STATIC)
	{
		return;
	}

	if((m_Style & I3_COLLIDER_STYLE_ONCE_TRANSFORM) && (m_Style & I3_COLLIDER_STYLE_TRANSFORMED))
		return;

	if( m_pWorld == nullptr)
	{
		m_pWorld = m_pLocal->create_instance();
		I3_MUST_ADDREF( m_pWorld);
	}

	m_pLocal->Transform( pMatrix, m_pWorld);

	m_Style |= I3_COLLIDER_STYLE_TRANSFORMED;
}

void i3Collider::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	if( !i3::kind_of<i3Collider*>(pDest))
		return;

	i3Collider * pColl = (i3Collider *) pDest;

	pColl->SetType( GetType());
	pColl->SetCollidee( GetCollidee());
	pColl->SetStyle( GetStyle());

	if( method == I3_COPY_INSTANCE)
	{
		pColl->SetStyle( GetStyle() & ~I3_COLLIDER_STYLE_TRANSFORMED);
	}
	else
	{
		pColl->m_pWorld = m_pWorld;
		I3_SAFE_ADDREF( m_pWorld);
	}
}

void i3Collider::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pLocal != nullptr)
	{
		m_pLocal->OnBuildObjectList( List);
	}

	// m_pWorld는 Volatile인 멤버이기 때문에 저장할 필요가 없다.
	i3PersistantElement::OnBuildObjectList( List);
}

UINT32 i3Collider::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	// Version
	{
		UINT16 version = 1;

		Rc = pStream->Write( &version, sizeof(version));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::OnSave()", "Could not write version information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Style
	{
		Rc = pStream->Write( &m_Style, sizeof(m_Style));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::OnSave()", "Could not write style data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Local Collidee Object Reference
	{
		OBJREF ref;

		ref = (OBJREF) pResFile->GetObjectPersistID( m_pLocal);

		Rc = pStream->Write( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::OnSave()", "Could not write collidee object reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3Collider::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	// Version
	{
		UINT16 version;

		Rc = pStream->Read( &version, sizeof(version));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::OnLoad()", "Could not read version information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Style
	{
		Rc = pStream->Read( &m_Style, sizeof(m_Style));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::OnLoad()", "Could not read style data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Local Collidee Object Reference.
	{
		OBJREF ref;

		Rc = pStream->Read( &ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3Collider::onLoad()", "Could not read collidee object reference.");
			return STREAM_ERR;
		}

		Result += Rc;

		if( ref != 0)
		{
			i3Collidee * pCol;

			pCol = (i3Collidee *) pResFile->FindObjectByID( (UINT32) ref);
			I3ASSERT( pCol);

			SetCollidee( pCol);
		}
	}

	return Result;
}
