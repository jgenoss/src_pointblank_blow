#include "stdafx.h"
#include "i3LevelPathLink.h"
#include "i3LevelGlobalVariable.h"

//I3_CLASS_INSTANCE( i3LevelPathLink, i3LevelControl);
I3_CLASS_INSTANCE( i3LevelPathLink);

i3LevelPathLink::i3LevelPathLink(void)
{
	AddStyle( I3_LEVEL_STYLE_NOMOVE | I3_LEVEL_STYLE_NOSCALE | I3_LEVEL_STYLE_NOROTATE);

	RemoveStyle( I3_LEVEL_STYLE_VOLATILE);

	m_pBillboard = i3Billboard::new_object_ref();
	m_pBillboard->SetCount( 1);
	m_pBillboard->setRotateUVState( true);

	m_pBillboard->SetEnable( 0, FALSE);
	m_pBillboard->SetSize( 0, 0.3f);
	m_pBillboard->SetSrcBlend( I3G_BLEND_SRCALPHA);
	m_pBillboard->SetDestBlend( I3G_BLEND_INVSRCALPHA);

	m_pBillboard->SetColor( 0, 255, 255, 255, 255);
	m_pBillboard->SetTexture( g_pScene->getCommonArrowTexResource()->getTexture());

	this->SetMaterialDiffuse( 0.0f, 0.4f, 0.7f, 1.0f);

	m_pAttrSet = i3AttrSet::new_object();
	m_pAttrSet->AddChild( m_pBillboard);

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( FALSE);
		m_pAttrSet->AddAttr( pAttr);
	}

	m_pScene->AddChild( m_pAttrSet);
}

i3LevelPathLink::~i3LevelPathLink(void)
{
	I3_SAFE_RELEASE( m_pStart);
	I3_SAFE_RELEASE( m_pEnd);
	I3_SAFE_RELEASE( m_pAttrSet);
}

inline REAL32 _u( INT32 val)
{
	return val / 256.0f;
}

inline REAL32 _v( INT32 val)
{
	return val / 128.0f;
}

void i3LevelPathLink::UpdateAll(void)
{
	if( (m_pStart == nullptr) || (m_pEnd == nullptr))
	{
		m_pBillboard->SetEnable( 0, FALSE);
		return;
	}

	m_pBillboard->SetEnable( 0, TRUE);

	UpdateLink();

	VEC2D vmin, vmax;

	if( this->isBidirectional())
	{
		i3Vector::Set( &vmin, _u(0), _v(16));
		i3Vector::Set( &vmax, _u(256), _v(32));
	}
	else
	{
		i3Vector::Set( &vmin, _u(0), _v(0));
		i3Vector::Set( &vmax, _u(256), _v(16));
	}

	m_pBillboard->SetTextureCoord( 0, &vmin, &vmax);
}

void i3LevelPathLink::UpdateLink(void)
{
	VEC3D pos;
	VEC3D dir;

	i3Vector::Sub( &dir, m_pStart->getPos(), m_pEnd->getPos());
	i3Vector::Normalize( &dir, &dir);
	i3Vector::Scale( &dir, &dir, 0.25f);

	i3Vector::Copy( &pos, m_pStart->getPos());
	i3Vector::Sub( &pos, &pos, &dir);
	m_pBillboard->SetStart( 0, &pos);

	i3Vector::Copy( &pos, m_pEnd->getPos());
	i3Vector::Add( &pos, &pos, &dir);
	m_pBillboard->SetEnd( 0, &pos);

	i3Vector::Minimize( &m_BoundMin, m_pStart->getPos(), m_pEnd->getPos());
	i3Vector::Maximize( &m_BoundMax, m_pStart->getPos(), m_pEnd->getPos());
}

void i3LevelPathLink::setBidirectional( bool bFlag)
{
	if( bFlag)
		addPathStyle( i3PathLine::STYLE_BIDIRECTION);
	else
		removePathStyle( i3PathLine::STYLE_BIDIRECTION);

	UpdateAll();
}

void i3LevelPathLink::SetStartElement( i3LevelControl * pObj)
{
	I3_REF_CHANGE( m_pStart, pObj);

	UpdateAll();
}

void i3LevelPathLink::SetEndElement( i3LevelControl * pObj)
{
	I3_REF_CHANGE( m_pEnd, pObj);

	UpdateAll();
}

void i3LevelPathLink::OnSelected( BOOL bSelected)
{
	i3LevelControl::OnSelected( bSelected);

	if( bSelected)
		SetMaterialDiffuse( 1.0f, 0.4f, 0.0f, 1.0f);
	else
		SetMaterialDiffuse( 0.0f, 0.4f, 0.7f, 1.0f);
}

REAL32	i3LevelPathLink::OnTouch( VEC3D * pStart, VEC3D * pEnd, MATRIX * pViewProj, bool bPick, bool bSelect, SEL_MODE mode)
{
	VEC3D s, e;
	VEC3D dir1, dir2;
	REAL32 t1, t2;

	I3ASSERT( m_pStart != nullptr);
	I3ASSERT( m_pEnd != nullptr);

	i3Vector::TransformCoord( &s, m_pStart->getPos(), pViewProj);
	i3Vector::TransformCoord( &e, m_pEnd->getPos(), pViewProj);

	s.z = 0.0f;
	e.z = 0.0f;

	if( i3Math::GetPerpendicularPoint( &s, &e, pStart, &dir1, &t1))
	{
		i3Vector::Sub( &dir2, &dir1, pStart);
		
		t2 = i3Vector::Length( &dir2);

		if( t2 <= 0.015f)
			return t2;

		return 0.0f;
	}
	
	return 0.0f;

	///REAL32 dist = i3Math::GetSqrDistanceLineToLine( m_pStart->getPos(), &dir1, pStart, &dir2, &t1, &t2);

	//I3TRACE( "dist : %f\n", dist);

	//return dist;
}

void i3LevelPathLink::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	I3_SAFE_ADDREF( m_pStart);
	I3_SAFE_ADDREF( m_pEnd);

	i3LevelControl::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)						// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct PATHLINK
	{
		UINT8			m_ID[4] = { 'P', 'H', 'L', '1' };
		OBJREF			m_pStart = 0;
		OBJREF			m_pEnd = 0;
		UINT32			m_PathStyle = 0;
		REAL32			m_Cost = 0.0f;

		UINT32			pad[32] = { 0 };

	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelPathLink::OnSave( i3ResourceFile * pResFile)
{
	pack::PATHLINK data;
	UINT32 rc, result;

	result = i3LevelControl::OnSave( pResFile);
	I3_CHKIO( result);
	
	if( m_pStart != nullptr)
		data.m_pStart = pResFile->GetObjectPersistID( m_pStart);

	if( m_pEnd != nullptr)
		data.m_pEnd = pResFile->GetObjectPersistID( m_pEnd);

	data.m_PathStyle = m_PathStyle;
	data.m_Cost = m_Cost;

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3LevelPathLink::OnLoad( i3ResourceFile * pResFile)
{
	pack::PATHLINK data;
	UINT32 rc, result;

	result = i3LevelControl::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	if( data.m_pStart != 0)
	{
		i3LevelControl * pStart = (i3LevelControl *) pResFile->FindObjectByID( data.m_pStart);
		I3ASSERT( pStart != nullptr);

		SetStartElement(  pStart);
	}

	if( data.m_pEnd != 0)
	{
		i3LevelControl * pEnd = (i3LevelControl *) pResFile->FindObjectByID( data.m_pEnd);
		I3ASSERT( pEnd != nullptr);

		SetEndElement(  pEnd);
	}

	m_PathStyle = data.m_PathStyle;
	m_Cost = data.m_Cost;

	return result;
}

bool i3LevelPathLink::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	OBJREF ref = 0;

	i3LevelControl::OnSaveXML( pFile, pXML);

	if( m_pStart != nullptr)
		ref = pFile->GetObjectPersistID( m_pStart);

	pXML->addAttr( "StartElement", ref);

	ref = 0;

	if( m_pEnd != nullptr)
		ref = pFile->GetObjectPersistID( m_pEnd);

	pXML->addAttr( "EndElement", ref);

	pXML->addAttr( "PathStyle", m_PathStyle);
	pXML->addAttr( "Cost",		m_Cost);

	return true;
}

bool i3LevelPathLink::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	OBJREF ref;

	i3LevelControl::OnLoadXML( pFile, pXML);

	pXML->getAttr( "StartElement", &ref);

	if( ref != 0)
	{
		i3LevelControl * pElm = (i3LevelControl *) pFile->FindObjectByID( ref);
		I3ASSERT( pElm != nullptr);

		SetStartElement( pElm);
	}

	pXML->getAttr( "EndElement", &ref);
	if( ref != 0)
	{
		i3LevelControl * pElm = (i3LevelControl *) pFile->FindObjectByID( ref);
		I3ASSERT( pElm != nullptr);

		SetEndElement( pElm);
	}

	pXML->getAttr( "PathStyle", &m_PathStyle);
	pXML->getAttr( "Cost", &m_Cost);

	return true;
}
