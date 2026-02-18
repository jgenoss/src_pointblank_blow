#include "stdafx.h"
#include "i3LevelLink.h"

I3_CLASS_INSTANCE( i3LevelLink);

void i3LevelLink::_LinkUpdateProc( i3SceneTracer * pTracer, void *pParam )
{
	i3LevelLink * pLink = (i3LevelLink *) pParam;
	i3VertexArray * pVA = pLink->m_pGeoAttr->GetVertexArray();
	i3LevelElement3D * pStart = pLink->GetStartElement();
	i3LevelElement3D * pEnd = pLink->GetEndElement();
	VEC3D start, end, vmin, vmax;
	COLOR col;

	if((pStart == nullptr) || (pEnd == nullptr))
	{
		pLink->m_pGeo->AddFlag( I3_NODEFLAG_DISABLE);
		return;
	}
	else
	{
		pLink->m_pGeo->RemoveFlag( I3_NODEFLAG_DISABLE);
	}

	if( pStart->IsSelected() || pEnd->IsSelected())
		i3Color::Set( &col, (UINT8) 255, 128, 0, 255);
	else
		i3Color::Set( &col, &pLink->m_Color);

	i3Matrix::TransformPoints( &vmin, pStart->GetBoundMin(), 1, pStart->GetTransform());
	i3Matrix::TransformPoints( &vmax, pStart->GetBoundMax(), 1, pStart->GetTransform());
	i3Vector::Add( &start, &vmin, &vmax);
	i3Vector::Scale( &start, &start, 0.5f);

	i3Matrix::TransformPoints( &vmin, pEnd->GetBoundMin(), 1, pEnd->GetTransform());
	i3Matrix::TransformPoints( &vmax, pEnd->GetBoundMax(), 1, pEnd->GetTransform());
	i3Vector::Add( &end, &vmin, &vmax);
	i3Vector::Scale( &end, &end, 0.5f);

	if( pLink->m_bFirst == false)
	{
		if( (i3Vector::IsSame( &start, &pLink->m_OldStartPos) == TRUE) &&
			(i3Vector::IsSame( &end, &pLink->m_OldEndPos) == TRUE))
			return;
	}
	else
	{
		pLink->m_bFirst = false;
	}

	pVA->Lock();
		pVA->SetPosition( 0, &start);
		pVA->SetPosition( 1, &end);
		pVA->SetColor( 0, 2, &col);
	pVA->Unlock();

	i3Vector::Copy( &pLink->m_OldStartPos, &start);
	i3Vector::Copy( &pLink->m_OldEndPos, &end);
}

i3LevelLink::i3LevelLink(void)
{
	AddStyle( I3_LEVEL_STYLE_NOMOVE | I3_LEVEL_STYLE_NOSCALE | I3_LEVEL_STYLE_NOROTATE | I3_LEVEL_STYLE_UNSELECT);

	{
		i3VertexArray * pVA = i3VertexArray::new_object_ref();
		i3VertexFormat fmt;

		fmt.SetHasPosition( TRUE);
		fmt.SetHasColor( TRUE);

		pVA->Create( &fmt, 2, 0);

		m_pGeoAttr = i3GeometryAttr::new_object();
		m_pGeoAttr->SetVertexArray( pVA, I3G_PRIM_LINELIST, 1, 0);

		m_pUserNode = i3UserNode::new_object();
		m_pUserNode->SetCallBack( (I3USERNODEPROC) _LinkUpdateProc, (void *) this);

		m_pGeo = i3Geometry::new_object();
		m_pGeo->AppendGeometryAttr( m_pGeoAttr);
		m_pUserNode->AddChild( m_pGeo);
	}

	{
		m_pAttrSet = i3AttrSet::new_object();

		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set( FALSE);
			m_pAttrSet->AddAttr( pAttr);
		}

		m_pAttrSet->AddChild( m_pUserNode);
	}

	m_pScene->AddChild( m_pAttrSet);
}

i3LevelLink::~i3LevelLink(void)
{
	I3_SAFE_RELEASE( m_pStart);
	I3_SAFE_RELEASE( m_pEnd);

	I3_SAFE_RELEASE( m_pGeoAttr);
	I3_SAFE_RELEASE( m_pGeo);
	I3_SAFE_RELEASE( m_pUserNode);
	I3_SAFE_RELEASE( m_pAttrSet);
}

void i3LevelLink::SetStartElement( i3LevelElement3D * pObj)
{
	I3_REF_CHANGE(m_pStart, pObj);
}

void i3LevelLink::SetEndElement( i3LevelElement3D * pObj)
{
	I3_REF_CHANGE(m_pEnd, pObj);
}

void i3LevelLink::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	I3_SAFE_ADDREF(m_pStart);
	I3_SAFE_ADDREF(m_pEnd);
}

UINT32 i3LevelLink::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3LevelLink::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}
