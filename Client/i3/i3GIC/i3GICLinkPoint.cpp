#include "stdafx.h"
#include "i3GICLinkPoint.h"
#include "i3GICDiagramContext.h"
#include "i3GICShapeFigure.h"
#include "i3GICProperty.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICLinkPoint);

HPEN i3GICLinkPoint::m_hpenLinkPoint = nullptr;
INT32 i3GICLinkPoint::m_hpenRefCnt = 0;

i3GICLinkPoint::i3GICLinkPoint()
{
	setStyle( I3GIC_STYLE_LINKER | I3GIC_STYLE_NOSELECT);
}

i3GICLinkPoint::~i3GICLinkPoint()
{
	_DeleteGDILinkPointObject();

	
	i3GICLinkPoint * pShape;

	for(size_t i = 0; i < m_LinkedShapeList.size(); i++)
	{
		pShape = getLinkedShape( i);
		pShape->removeLinkedShape( this);
	}
}

void i3GICLinkPoint::_CreateGDILinkPointObject( void)
{
	m_hpenRefCnt++;

	if( m_hpenLinkPoint != nullptr)
		return;

	m_hpenLinkPoint = CreatePen( PS_SOLID, 1, RGB( 50, 50, 255));

	I3ASSERT( m_hpenLinkPoint != nullptr);
}

void i3GICLinkPoint::_DeleteGDILinkPointObject( void)
{
	m_hpenRefCnt--;

	if((m_hpenRefCnt == 0) && (m_hpenLinkPoint != nullptr))
	{
		DeleteObject( m_hpenLinkPoint);
		m_hpenLinkPoint = nullptr;
	}
}

/*virtual*/	bool i3GICLinkPoint::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	i3::pack::RECT rt;
	POINT sz, pos;

	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOHITTEST))			return false;
	
	sz.x = CPSZ;
	sz.y = CPSZ;
	pCtx->toLP( &sz);

	pos.x = (INT32) getPosX();
	pos.y = (INT32) getPosY();

	pCtx->GetDrawPos( &pos);

	rt.left		= (REAL32)(pos.x - sz.x);
	rt.top		= (REAL32)(pos.y - sz.y);
	rt.right	= (REAL32)(pos.x + sz.x);
	rt.bottom	= (REAL32)(pos.y + sz.y);

	i3Math::NormalizeRect( &rt);

	VEC2D cpos;
	cpos.x = x;
	cpos.y = y;

	if( i3Math::PtInRect( &rt, &cpos))
	{
		pCtx->getCurrentHitResult()->m_rDepth = getDepth();
		pCtx->getCurrentHitResult()->m_pHitShape = this;
		pCtx->getCurrentHitResult()->m_Semantic = I3GIC_CP_LINKER;
		return true;
	}

	return false;
}


/*virtual*/ void i3GICLinkPoint::OnDraw( i3GICDiagramContext * pCtx)
{
	POINT pt;
	//HGDIOBJ oldhpenLinkPoint;

	if( m_hpenLinkPoint == nullptr)
		return;

	if( !isStyle( I3GIC_STYLE_INVISIBLE) )
	{
		pt.x = (INT32) getPosX();
		pt.y = (INT32) getPosY();

		//m_hpenLinkPoint = CreatePen(PS_SOLID, 1, RGB(50, 50, 255));

		//oldhpenLinkPoint = pCtx->selectObject( m_hpenLinkPoint);
		pCtx->moveTo( pt.x - 5, pt.y - 5);
		pCtx->lineTo( pt.x + 5, pt.y + 6);
		pCtx->moveTo( pt.x - 5, pt.y + 5);
		pCtx->lineTo( pt.x + 5, pt.y - 6);

		//pCtx->selectObject(oldhpenLinkPoint);
		//DeleteObject(m_hpenLinkPoint);
		//m_hpenLinkPoint = CreatePen(PS_SOLID, 1, RGB(50, 50, 255));
	}

	i3GICShape::OnDraw( pCtx);
}

void i3GICLinkPoint::BeginDragEventToLinkedShape( void)
{
	INT32 i;
	// 연결된 모든 Shape을 BeginDrag한다.
	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) getLinkedShape( i)->getParent();

		pShape->OnBeginDrag();
	}
}

void i3GICLinkPoint::EndDragEventToLinkedShape( void)
{
	INT32 i;
	// 연결된 모든 Shape을 EndDrag한다.
	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) getLinkedShape( i)->getParent();

		pShape->OnEndDrag();
	}
}

void i3GICLinkPoint::CancelDragEventToLinkedShape( void)
{
	INT32 i;
	// 연결된 모든 Shape을 CancelDrag한다.
	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) getLinkedShape( i)->getParent();

		pShape->OnCancelDrag();
	}
}

void i3GICLinkPoint::UpdateLinkedShape( i3GICDiagramContext * pCtx, VEC2D * pUpdatedPos)
{
	INT32 i;

	if( getLinkFlags() & GIC_LINK_FLAG_UPDATED)
		return;

	setUpdatedPos( pUpdatedPos);

	addLinkFlags( GIC_LINK_FLAG_UPDATED);

	// 연결된 모든 Shape을 Update한다.
	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		i3GICShape * pShape = (i3GICShape*) getLinkedShape( i)->getParent();

		//if( pShape->isState( I3GIC_STATE_SELECTED) == false &&
		/*if(
			pShape->isState( I3GIC_STATE_DRAGGING) )*/
		{
			pShape->OnUpdateByLink( pCtx, this, getLinkedShape( i));

			if( pShape->isStyle( I3GIC_STYLE_NOUPDATE_SIDE_LINK) == false)
				pShape->OnDragLinkedShape( pCtx, pUpdatedPos);
		}
	}

	removeLinkFlags( GIC_LINK_FLAG_UPDATED);
}

/*virtual*/ void i3GICLinkPoint::OnBindDiagram( bool bBind)
{
	i3GICShape::OnBindDiagram( bBind);

	if( bBind)	_CreateGDILinkPointObject();
	else		_DeleteGDILinkPointObject();
}



#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct GIC_LINK_POINT_HEAD
	{
		UINT8	m_ID[4] = { 'G', 'L', 'P', '1' };
		UINT32	m_LinkFlags = 0;
		UINT32	m_LinkCount = 0;
		UINT32	pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


/*virtual*/	UINT32 i3GICLinkPoint::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::GIC_LINK_POINT_HEAD head;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShape::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;
	
	head.m_LinkFlags = m_LinkFlags;
	head.m_LinkCount = getLinkedShapeCount();

	Rc = pStream->Write( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	INT32 i;
	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		OBJREF ref = (OBJREF)pResFile->GetObjectPersistID( getLinkedShape( i));
		Rc = pStream->Write( &ref, sizeof( OBJREF));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

/*virtual*/	UINT32 i3GICLinkPoint::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 i;
	OBJREF ref;
	i3GICLinkPoint * pLink;
	UINT32 Rc, Result = 0;
	pack::GIC_LINK_POINT_HEAD head;

	i3Stream * pStream = pResFile->GetStream();

	Rc = i3GICShape::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Read( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	m_LinkFlags = head.m_LinkFlags;

	if( head.m_LinkCount > 0)
	{
		for( i = 0; i < head.m_LinkCount; i++)
		{	
			Rc = pStream->Read( &ref, sizeof( ref));
			I3_CHKIO( Rc);
			Result += Rc;

			if( ref != 0)
			{
				pLink = (i3GICLinkPoint*) pResFile->FindObjectByID( ref);
				m_LinkedShapeList.push_back( pLink);
			}
		}
	}

	return Result;
}

/*virtual*/ bool i3GICLinkPoint::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 i;
	char conv[260];

	if( i3GICShape::OnSaveXML( pFile, pXML) == false)
		return false;

	pXML->addAttr( _T("LinkFlags"), getLinkFlags());
	pXML->addAttr( _T("LinkCount"), getLinkedShapeCount());

	for( i = 0; i < getLinkedShapeCount(); i++)
	{
		OBJREF ref = (OBJREF)pFile->GetObjectPersistID( getLinkedShape( i));

		i3::snprintf( conv, _countof( conv), _T("LinkPoint%d"), i);
		pXML->addAttr( conv, (INT32) ref);
	}

	return true;
}

/*virtual*/ bool i3GICLinkPoint::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	UINT32 i;
	UINT32 nTemp;
	OBJREF ref;
	char conv[260];

	if( i3GICShape::OnLoadXML( pFile, pXML) == false)
		return false;

	pXML->getAttr( _T("LinkFlags"), &nTemp);
	setLinkFlags( (GIC_LINK_FLAG) nTemp);

	pXML->getAttr( _T("LinkCount"), &nTemp);
	m_LinkedShapeList.resize( nTemp);

	for( i = 0; i < nTemp; i++)
	{
		i3::snprintf( conv, _countof( conv), _T("LinkPoint%d"), i);
		if( pXML->getAttr( conv, (INT32*) &ref) != nullptr)
		{
			i3GICLinkPoint * pLink = (i3GICLinkPoint*) pFile->FindObjectByID( ref);

			if( pLink != nullptr)
			{
				m_LinkedShapeList[i] = pLink;			// 미리 resize되어 있어 강제로 늘릴 필요는 없음..
			//	m_LinkedShapeList.SetItem( i, pLink);
			}
		}
	}

	return true;
}

/*virtual*/ void i3GICLinkPoint::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	char szName[ 260];
	char szStyle[ 260];

	i3::snprintf( szName, _countof( szName), _T("LinkPoint_%s"), GetName());

	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(szName);

	{
		COleVariant varName( GetName());
		i3GICProperty * pPropName = new i3GICProperty( _T("Name"), varName, _T( "Link Point의 Name을 표시합니다."));
		pPropName->AllowEdit();
		pPropName->setShape( this);
		pPropName->setUpdateType( GIC_PROPERTY_UPDATE_NAME);
		pProperty->AddSubItem( pPropName);
	}

	{
		_GetStyleNamed( szStyle, _countof( szStyle));

		COleVariant varStyle( szStyle);
		i3GICProperty* pPropStyle = new i3GICProperty(_T("Style"), varStyle, _T("Shape Style을 표시합니다. (_NBL, _NFL, _LOCK, _GDI+, _NSEL, _INV, _NSL)"));
		pPropStyle->AllowEdit( TRUE);
		pPropStyle->setShape( this);
		pPropStyle->setUpdateType( GIC_PROPERTY_UPDATE_SHAPESTYLE);
		pProperty->AddSubItem( pPropStyle);
	}

	pCtrl->AddProperty( pProperty);
}



#endif	// I3_DEBUG
