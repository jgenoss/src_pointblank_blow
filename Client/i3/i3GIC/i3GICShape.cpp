#include "stdafx.h"
#include "i3GICShape.h"
#include "i3GICDiagramContext.h"
#include "i3GICControlPoint.h"
#include "i3GICProperty.h"
#include "i3GICColorProperty.h"
#include "i3GICLinkPoint.h"

#include "i3GICShapeLine.h"

#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/ext/generic_string_cat.h"

#if defined( I3_DEBUG)

I3_CLASS_INSTANCE( i3GICShape);


i3GICShape::~i3GICShape(void)
{


	GIC_SHAPE_TREENODE_LINE_INFO * pInfo;

	for(size_t i = 0; i < m_TreeLineList.size(); i++)
	{
		pInfo = m_TreeLineList[ i];

		I3_SAFE_RELEASE( pInfo->_pLine);
		I3MEM_SAFE_FREE( pInfo);
	}

}

void i3GICShape::_GetStyleNamed( char * pszOut, INT32 len)
{
	i3::safe_string_copy( pszOut, I3STR(""), len);

	if( isStyle( I3GIC_STYLE_NOBORDERLINE) )
	{
		i3::generic_string_cat( pszOut, I3STR("_NBL"));
	}

	if( isStyle( I3GIC_STYLE_NOFILL))
	{
		i3::generic_string_cat( pszOut, I3STR("_NFL"));
	}

	if( isStyle( I3GIC_STYLE_LOCK))
	{
		i3::generic_string_cat( pszOut, I3STR("_LOCK"));
	}

	if( isStyle( I3GIC_STYLE_GDIPLUS))
	{
		i3::generic_string_cat( pszOut, I3STR("_GDI+"));
	}

	if( isStyle( I3GIC_STYLE_NOSELECT))
	{
		i3::generic_string_cat( pszOut, I3STR("_NSEL"));
	}

	if( isStyle( I3GIC_STYLE_INVISIBLE))
	{
		i3::generic_string_cat( pszOut, I3STR("_INV"));
	}

	if( isStyle( I3GIC_STYLE_NOUPDATE_SIDE_LINK))
	{
		i3::generic_string_cat( pszOut, I3STR("_NSL"));
	}

	if( isStyle( I3GIC_STYLE_NORESCALE))
	{
		i3::generic_string_cat( pszOut, I3STR("_NSC"));
	}

	if( isStyle( I3GIC_STYLE_NOHITTEST))
	{
		i3::generic_string_cat( pszOut, I3STR("_NTT"));
	}
}

I3GIC_STYLE	i3GICShape::_GetStyleByStr( const char * pszStr)
{
	I3GIC_STYLE style = 0;

	if( i3::contain_string_head( pszStr, I3STR("_NBL")) )
	{
		style |= I3GIC_STYLE_NOBORDERLINE;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_NFL")) )
	{
		style |= I3GIC_STYLE_NOFILL;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_LOCK")) )
	{
		style |= I3GIC_STYLE_LOCK;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_GDI+")) )
	{
		style |= I3GIC_STYLE_GDIPLUS;
	}
	else
	if(	i3::contain_string_head( pszStr, I3STR("_NSEL")) )
	{
		style |= I3GIC_STYLE_NOSELECT;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_INV")) )
	{
		style |= I3GIC_STYLE_INVISIBLE;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_NSL")) )
	{
		style |= I3GIC_STYLE_NOUPDATE_SIDE_LINK;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_NSC")) )
	{
		style |= I3GIC_STYLE_NORESCALE;
	}
	else
	if( i3::contain_string_head( pszStr, I3STR("_NTT")) )
	{
		style |= I3GIC_STYLE_NOHITTEST;
	}

	return style;
}

/*virtual*/ void i3GICShape::OnCreate( void)
{
	
}

void i3GICShape::AttachToShape( i3GICShape * pShape)
{
	I3ASSERT( pShape != nullptr);

	pShape->CreateTreeNodeLine( this, pShape->getChildCount() );

	I3ASSERT( getParent() == nullptr);

	{	// 좌표를 다시 맞춘다.
		VEC2D vParentPos, vChildPos, vPos;

		pShape->GetTransformedPos( &vParentPos);
		GetTransformedPos( &vChildPos);

		i3Vector::Sub( &vPos, &vChildPos, &vParentPos);

		setPos( &vPos);
	}

	pShape->AddChild( this);

	I3TRACE( I3STR("%s Attach To %s\n"), GetName(), pShape->GetName());
}

void i3GICShape::DetachFromParentShape( void)
{
	if( getParent() != nullptr)
	{
		i3GICShape * pParentShape = (i3GICShape*) getParent();

		pParentShape->DeleteTreeNodeLine( this);

		{	// 좌표를 다시 맞춘다.
			VEC2D vChildPos;

			GetTransformedPos( &vChildPos);

			setPos( &vChildPos);
		}

		//I3TRACE( "%s Detach from %s\n", GetName(), pParentShape->GetName());

		pParentShape->RemoveChild( this);
	}
}

void i3GICShape::CreateTreeNodeLine( i3GICShape * pChild, INT32 idx)
{

	VEC2D vEnd, vStart, vParentPos, vChildPos;

	i3GICShapeLine * pLine = i3GICShapeLine::new_object();

	pLine->addNodeStyle( I3_TREENODE_STYLE_VOLATILE);
	pLine->addState( I3GIC_STATE_DISABLED);
	pLine->setLineColor( RGB( 128, 128, 255));

	INT32 i;
	i3GICShape * pLineChild;
	for( i = 0; i < pLine->getChildCount(); i++)
	{
		pLineChild = (i3GICShape*) pLine->getChild( i);

		pLineChild->addState( I3GIC_STATE_DISABLED);
	}

	GetTransformedPos( &vParentPos);
	pChild->GetTransformedPos( &vChildPos);

	i3Vector::Add( &vChildPos, pChild->getWidth() * 0.5f, pChild->getHeight() * 0.5f);
	i3Vector::Add( &vParentPos, getWidth() * 0.5f, getHeight() * 0.5f);

	i3Vector::Sub( &vEnd, &vChildPos, &vParentPos);
	
	pLine->setEndPoint( &vEnd);

	i3Vector::AddX( &vStart, getWidth() * 0.5f);
	i3Vector::AddY( &vStart, getHeight() * 0.5f);
	pLine->setStartPoint( &vStart);

	GIC_SHAPE_TREENODE_LINE_INFO * pNewInfo = (GIC_SHAPE_TREENODE_LINE_INFO*) i3MemAlloc( sizeof( GIC_SHAPE_TREENODE_LINE_INFO));

	pNewInfo->_pLine = pLine;
	pNewInfo->_iChildIdx = idx;

	m_TreeLineList.push_back( pNewInfo);

}

void i3GICShape::DeleteTreeNodeLine( i3GICShape * pChild)
{

	GIC_SHAPE_TREENODE_LINE_INFO * pInfo;

	for(INT32 i = 0; i < getChildCount(); i++)
	{
		if( pChild == getChild( i))
		{
			INT32 iTreeNodeLineIdx = FindTreeNodeLine( i);

			if( iTreeNodeLineIdx != -1)
			{
				pInfo = getTreeNodeInfo( iTreeNodeLineIdx);

				I3_SAFE_RELEASE( pInfo->_pLine);
				I3MEM_SAFE_FREE( pInfo);

				m_TreeLineList.erase(m_TreeLineList.begin() + iTreeNodeLineIdx);
			}

			break;
		}
	}

}

INT32 i3GICShape::FindTreeNodeLine( INT32 iChildIdx)
{
	GIC_SHAPE_TREENODE_LINE_INFO * pInfo;

	for(size_t i = 0; i < m_TreeLineList.size(); i++)
	{
		pInfo = getTreeNodeInfo( i);
		if( pInfo->_iChildIdx == iChildIdx)
			return i;
	}

	return -1;
}

void i3GICShape::GetTransformedPos( VEC2D * pPos)
{
	i3GICShape * pParent = this;

	I3ASSERT( pPos);

	i3Vector::Zero( pPos);

	while( pParent)
	{
		if( i3::kind_of<i3GICShape*>(pParent))
		{
			i3Vector::Add( pPos, pPos, pParent->getPos());
		}

		pParent = (i3GICShape*) pParent->getParent();
	}
}

void i3GICShape::RecSetStyle( i3GICShape * pShape, I3GIC_STYLE addStyle, I3GIC_STYLE removeStyle)
{
	INT32 i;
	i3GICShape * pChild;

	pShape->addStyle( addStyle);
	pShape->removeStyle( removeStyle);

	for( i = 0; i < pShape->getChildCount(); i++)
	{
		pChild = (i3GICShape*) pShape->getChild( i);

		RecSetStyle( pChild, addStyle, removeStyle);
	}
}

void i3GICShape::RecSetState( i3GICShape * pShape, I3GIC_STATE addState, I3GIC_STATE removeState)
{
	INT32 i;
	i3GICShape * pChild;

	pShape->addState( addState);
	pShape->removeState( removeState);

	for( i = 0; i < pShape->getChildCount(); i++)
	{
		pChild = (i3GICShape*) pShape->getChild( i);

		RecSetState( pChild, addState, removeState);
	}
}

void i3GICShape::BakeReformHierarchy( i3GICShape * pShape)
{
	INT32 i;
	i3GICShape * pChild;

	i3GICShape * pRoot = nullptr;

	for( i = 0; i < pShape->getChildCount(); i++)
	{
		pChild = (i3GICShape*) pShape->getChild( i);

		if( !pChild->isStyle( I3GIC_STYLE_LINKER) &&
			!pChild->isStyle( I3GIC_STYLE_CONTROL) )
		{
			if( pRoot == nullptr)
				pRoot = pChild;
			else
			{
				BakeReformHierarchy( pChild);

				I3_SAFE_ADDREF( pChild);
				if( pChild->getParent() != nullptr)
					I3TRACE( I3STR("%s detach from %s\n"), pChild->GetName(), pChild->getParent()->GetName());
				pChild->RemoveFromParent();

				I3TRACE( I3STR("%s attach to %s\n"), pChild->GetName(), pRoot->GetName());
				pRoot->AddChild( pChild);
				i3Vector::Sub( pChild->getPos(), pChild->getPos(), pRoot->getPos());
				I3_SAFE_RELEASE( pChild);
			}
		}
	}
}

bool i3GICShape::OnSelect( bool bFlag)
{
	INT32 i;
	i3GICShape * pChild;

	if( (isStyle( I3GIC_STYLE_NOSELECT) == false) &&
		(isStyle( I3GIC_STYLE_NOSELECT_ONLY) == false) )
	{
		if( bFlag)	addState( I3GIC_STATE_SELECTED);
		else		removeState( I3GIC_STATE_SELECTED);
	}

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape *) getChild( i );
		pChild->OnSelect( bFlag);
	}

	return true;
}

/*virtual*/	void i3GICShape::OnAttach( i3GICDiagram * pDiagram)
{
}

/*virtual*/ void i3GICShape::OnDetach( i3GICDiagram * pDiagram)
{	
}

/*virtual*/ bool i3GICShape::OnBeginDrag(void)
{
	if( isState( I3GIC_STATE_DRAGGING) == false)
	{
		i3Vector::Copy( &m_vSavedPos, &m_vPos);
		i3Vector::Copy( &m_vSavedSize, &m_vSize);
		addState( I3GIC_STATE_DRAGGING);

		OnBeginDragLinkedShape();

		return true;
	}

	return false;
}

/*virtual*/ bool i3GICShape::OnEndDrag(void)
{
	if( isState( I3GIC_STATE_DRAGGING) )
	{
		removeState( I3GIC_STATE_DRAGGING );

		OnEndDragLinkedShape();

		return true;
	}

	return false;
}

/*virtual*/ bool i3GICShape::OnCancelDrag(void)
{
	if( isState( I3GIC_STATE_DRAGGING) )
	{
		i3Vector::Copy( &m_vPos, &m_vSavedPos);
		i3Vector::Copy( &m_vSize, &m_vSavedSize);

		removeState( I3GIC_STATE_DRAGGING );

		OnCancelDragLinkedShape();

		return true;
	}

	return false;
}

bool i3GICShape::OnDragCP( i3GICDiagramContext * pCtx)
{
	REAL32 val;

	VEC2D vUpdatePos;

	REAL32 cpRightPos = getPosX();
	REAL32 cpTopPos = getPosY();

	i3Vector::Copy( &vUpdatePos, getPos());

	REAL32 dcx = pCtx->getDeltaX();
	REAL32 dcy = pCtx->getDeltaY();

	// 클릭된 시점의 Hit Result를 사용한다.
	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	if( pResult->m_Semantic & I3GIC_CP_RESIZE_RIGHT)
	{
		val = getX( &m_vSavedSize) + dcx;

		setPosX( getPosX());

		cpRightPos = val + getPosX();

		if( pCtx->LinkSnapPos( this, &cpRightPos, &cpTopPos) == false)
		{
			pCtx->GridSnapPos( &cpRightPos, &cpTopPos);
		}

		val = cpRightPos - getPosX();

		if( val < 10)
			val = 10;

		setWidth( val);
	}
	else if( pResult->m_Semantic & I3GIC_CP_RESIZE_LEFT)
	{
		val = getX( &m_vSavedPos) + dcx;

		cpRightPos = val;

		if( pCtx->LinkSnapPos( this, &cpRightPos, &cpTopPos) == false)
		{
			pCtx->GridSnapPos( &cpRightPos, &cpTopPos);
		}

		REAL32 rBound = val - cpRightPos;

		val = cpRightPos;

		if( val > getX( &m_vSavedPos) + getX( &m_vSavedSize) - 10)
			val = getX( &m_vSavedPos) + getX( &m_vSavedSize) - 10;

		setPosX( val);

		val = getX( &m_vSavedSize) - dcx + rBound;

		if( val < 10)
			val = 10;

		setWidth( val);
	}

	if( pResult->m_Semantic & I3GIC_CP_RESIZE_DOWN)
	{
		val = getY( &m_vSavedSize) + dcy;

		setPosY( getPosY());

		cpTopPos = val + getPosY();

		if( pCtx->LinkSnapPos( this, &cpRightPos, &cpTopPos) == false)
		{
			pCtx->GridSnapPos( &cpRightPos, &cpTopPos);
		}

		val = cpTopPos - getPosY();

		if( val < 10)
			val = 10;

		setHeight( val);
	}
	else if( pResult->m_Semantic & I3GIC_CP_RESIZE_UP)
	{
		val = getY( &m_vSavedPos) + dcy;

		cpTopPos = val;

		if( pCtx->LinkSnapPos( this, &cpRightPos, &cpTopPos) == false)
		{
			pCtx->GridSnapPos( &cpRightPos, &cpTopPos);
		}

		REAL32 rBound = val - cpTopPos;

		val = cpTopPos;

		if( val > getY( &m_vSavedPos) + getY( &m_vSavedSize) - 10)
			val = getY( &m_vSavedPos) + getY( &m_vSavedSize) - 10;

		setPosY( val);

		val = getY( &m_vSavedSize) - dcy + rBound;
		if( val < 10)
			val = 10;

		setHeight( val);
	}

	i3Vector::Sub( &vUpdatePos, &vUpdatePos, getPos());

	pCtx->addFlags( I3GIC_CTX_FLAG_NOSNAP);
	OnDragLinkedShape( pCtx, &vUpdatePos );
	pCtx->removeFlags( I3GIC_CTX_FLAG_NOSNAP);

	return true;
}

bool i3GICShape::OnDragMoving( i3GICDiagramContext * pCtx)
{
	REAL32 dx = pCtx->getDeltaX();
	REAL32 dy = pCtx->getDeltaY();

	GIC_HIT_RESULT * pResult = pCtx->getHitResult( GIC_HIT_RESULT_MOUSEDOWN);

	// Multi object move는 CP 체크를 하지 않는다.
	if( pCtx->getMouseMode() != GIC_MOUSEMODE_MOVE_MULTIOBJ)
	{
		if( !(pResult->m_Semantic & (I3GIC_CP_MOVE_RIGHT | I3GIC_CP_MOVE_LEFT)))
			dx = 0.0f;

		if( !(pResult->m_Semantic & (I3GIC_CP_MOVE_TOP | I3GIC_CP_MOVE_DOWN)))
			dy = 0.0f;
	}

	REAL32 rMovedPosX = getX( &m_vSavedPos) + dx;
	REAL32 rMovedPosY = getY( &m_vSavedPos) + dy;

	// 모든 Linker를 확인해야한다.
	if( OnAllLinkerSnapToLink( pCtx, &rMovedPosX, &rMovedPosY) == false)
	{
		if( pCtx->GridSnapPos( &rMovedPosX, &rMovedPosY) == false)
		{
			rMovedPosX += getWidth();
			rMovedPosY += getHeight();

			pCtx->GridSnapPos( &rMovedPosX, &rMovedPosY);

			rMovedPosX -= getWidth();
			rMovedPosY -= getHeight();
		}
	}

	setPos( rMovedPosX, rMovedPosY);

	VEC2D vUpdatePos;

	i3Vector::Set( &vUpdatePos, rMovedPosX, rMovedPosY);
	i3Vector::Sub( &vUpdatePos, &vUpdatePos, &m_vSavedPos);

	// Linked shape의 좌표를 설정해준다.
	OnDragLinkedShape( pCtx, &vUpdatePos);

	return true;
}

bool i3GICShape::IsEnableDraw( i3GICDiagramContext * pCtx)
{
	if( isNodeState( I3_TREENODE_STATE_DISABLE))	return false;
	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( pCtx->getFlags() & I3GIC_CTX_FLAG_NOCONTROL)
	{
		if( i3::kind_of<i3GICControlPoint*>(this))
			return false;
	}

	return true;
}

void i3GICShape::OnDraw( i3GICDiagramContext * pCtx)
{
	
	i3GICShape * pChild;

	pCtx->getStack()->push( getPosX(), getPosY());

	for(INT32 i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape *) getChild( i);

		if( pChild->IsEnableDraw( pCtx))
			pChild->OnDraw( pCtx);
	}

	if( (pCtx->getMode() == GIC_MODE_DESIGNTREENODE)  )
	{
		VEC2D vec, vEndPoint;

		if( m_TreeLineList.size() > 0)
		{
			i3Vector::Set( &vec, getWidth() * 0.5f, getHeight() * 0.5f);

			for(size_t i = 0; i < m_TreeLineList.size(); i++)
			{
				GIC_SHAPE_TREENODE_LINE_INFO * pInfo = m_TreeLineList[ i];

				pInfo->_pLine->setStartPoint( &vec);

				pChild = (i3GICShape*) getChild( pInfo->_iChildIdx);

				i3Vector::Copy( &vEndPoint, pChild->getPos());
				i3Vector::AddX( &vEndPoint, pChild->getWidth() * 0.5f);
				i3Vector::AddY( &vEndPoint, pChild->getHeight() * 0.5f);

				pInfo->_pLine->setEndPoint( &vEndPoint);

				pInfo->_pLine->OnDraw( pCtx);
			}
		}
	}

	pCtx->getStack()->pop();
}

bool i3GICShape::IsEnableHitTest( i3GICDiagramContext * pCtx)
{
	if( pCtx->getCurrentHitResult()->m_rDepth >= getDepth())	return false;
	if( isNodeState( I3_TREENODE_STATE_DISABLE))	return false;
	if( isState( I3GIC_STATE_DISABLED))				return false;
	if( isStyle( I3GIC_STYLE_NOSELECT))				return false;
	if( pCtx->getFlags() & I3GIC_CTX_FLAG_NOLINKER)
	{
		if( i3::kind_of<i3GICLinkPoint*>(this))
		{
			return false;
		}
	}
	if( (pCtx->getFlags() & I3GIC_CTX_FLAG_CENTERLINKER_ONLY) )
	{
		if( i3::kind_of<i3GICLinkPoint* >(this))
		{
			if( i3::generic_is_iequal( GetName(), I3STR("LinkPoint_4")) == false)
				return false;
		}
		else
			return false;
	}

	return true;
}

bool i3GICShape::OnHitTest( i3GICDiagramContext * pCtx, REAL32 x, REAL32 y)
{
	INT32 i;
	i3GICShape * pChild;
	bool bHit = false;

	pCtx->getStack()->push( getPosX(), getPosY());

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape *) getChild( i);

		if( pChild->IsEnableHitTest( pCtx) )
		{
			bHit = bHit | pChild->OnHitTest( pCtx, x, y);
		}
	}

	pCtx->getStack()->pop();

	return bHit;
}

bool i3GICShape::OnHitTest( i3GICDiagramContext * pCtx, i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List)
{
	INT32 i;
	i3GICShape * pChild;
	bool bRv = false;

	pCtx->getStack()->push( getPosX(), getPosY());

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape *) getChild( i);

		if( pChild->IsEnableHitTest( pCtx))
		{
			bRv = bRv | pChild->OnHitTest( pCtx, pRect, List);
		}
	}

	pCtx->getStack()->pop();

	return bRv;
}

/*virtual*/ void i3GICShape::OnBindDiagram( bool bBind)
{
	m_bBindDiagram = bBind;

	INT32 i;

	for( i = 0; i < getChildCount(); i++)
	{
		if( i3::kind_of<i3GICShape*>(getChild( i)))
		{
			((i3GICShape*) getChild( i))->OnBindDiagram( bBind);
		}
	}
}


/*virtual*/ void i3GICShape::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3GICObject::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of< i3GICShape*>(pDest));

	i3GICShape * pDShape = (i3GICShape*) pDest;

	pDShape->setStyle( m_Style);
	pDShape->setState( m_State);

	pDShape->setPos( &m_vPos);
	pDShape->setSize( &m_vSize);

	pDShape->setSavedPos( &m_vSavedPos);
	pDShape->setSavedSize( &m_vSavedSize);

	pDShape->setDepth( m_rDepth);
}

#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9

namespace pack
{
	struct ALIGN8 GIC_OBJECT
	{
		UINT8		m_ID[4] = { 'G', 'I', 'S', '1' };
		UINT32		m_Style = 0;
		UINT32		m_State = 0;
		REAL32		m_X = 0.0f;
		REAL32		m_Y = 0.0f;
		REAL32		m_W = 0.0f;
		REAL32		m_H = 0.0f;
		COLORREF	m_Color = 0;
		UINT32		pad[16] = { 0 };
	};

	struct ALIGN8 GIC_OBJECT2
	{
		UINT8		m_ID[4] = { 'G', 'I', 'S', '2' };
		UINT32		m_Style = 0;
		UINT32		m_State = 0;
		REAL32		m_X = 0.0f;
		REAL32		m_Y = 0.0f;
		REAL32		m_W = 0.0f;
		REAL32		m_H = 0.0f;
		COLORREF	m_Color = 0;
		REAL32		m_Depth = 0.0f;
		UINT32		pad[15] = { 0 };
	};
}

UINT32 i3GICShape::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GIC_OBJECT2 data;

	Result = i3GICObject::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Style = getStyle();
	data.m_State = getState();
	data.m_X = getPosX();
	data.m_Y = getPosY();
	data.m_W = getWidth();
	data.m_H = getHeight();
	data.m_Depth = getDepth();

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GICShape::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	
	UINT8 Ver[4];

	UINT32 cur = pStream->GetPosition();

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( Ver, sizeof(Ver));
	I3_CHKIO( Rc);

	if( Ver[0] == 'G' && Ver[1] == 'I' && Ver[2] == 'O' && Ver[3] == '1')
	{
		pStream->SetPosition( cur , 1);

		Result = i3NamedElement::OnLoad( pResFile);
		I3_CHKIO( Result);

		// Old
		pack::GIC_OBJECT data;
		Rc = pStream->Read( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;

		{
			i3::pack::RECT rt;

			rt.left = data.m_X;
			rt.top = data.m_Y;
			rt.right = data.m_X + data.m_W;
			rt.bottom = data.m_Y + data.m_H;

			setRect( &rt);
		}

		setState( data.m_State);
		setStyle( data.m_Style);
		setDepth( (REAL32)data.m_Color);
	}
	else
	{
		pStream->SetPosition( cur, 1);

		Result = i3GICObject::OnLoad( pResFile);
		I3_CHKIO( Result);

		// New
		pack::GIC_OBJECT2 data2;
		Rc = pStream->Read( &data2, sizeof(data2));
		I3_CHKIO( Rc);
		Result += Rc;

		{
			i3::pack::RECT rt;

			rt.left = data2.m_X;
			rt.top = data2.m_Y;
			rt.right = data2.m_X + data2.m_W;
			rt.bottom = data2.m_Y + data2.m_H;

			setRect( &rt);
		}

		setState( data2.m_State);
		setStyle( data2.m_Style);
		setDepth( data2.m_Depth);
	}

	

	return Result;
}

/*virtual*/ bool i3GICShape::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICObject::OnSaveXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  I3STR("write fail"));
		return false;
	}

	pXML->addAttr( I3STR("Style"), getStyle());
	pXML->addAttr( I3STR("State"), getState());

	pXML->addAttr( I3STR("PosX"), getPosX());
	pXML->addAttr( I3STR("PosY"), getPosY());
	pXML->addAttr( I3STR("Width"), getWidth());
	pXML->addAttr( I3STR("Height"), getHeight());

	pXML->addAttr( I3STR("Depth"), getDepth());

	return true;
}

/*virtual*/ bool i3GICShape::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3GICObject::OnLoadXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  I3STR("read fail"));
		return false;
	}

	REAL32 rTemp;

	pXML->getAttr( I3STR("Style"), &m_Style);

	pXML->getAttr(I3STR("State"), &m_State);
	
	pXML->getAttr( I3STR("PosX"), &rTemp);
	setX( &m_vPos, rTemp);

	pXML->getAttr( I3STR("PosY"), &rTemp);
	setY( &m_vPos, rTemp);

	pXML->getAttr( I3STR("Width"), &rTemp);
	setX( &m_vSize, rTemp);

	pXML->getAttr( I3STR("Height"), &rTemp);
	setY( &m_vSize, rTemp);

	pXML->getAttr( I3STR("Depth"), &rTemp);
	setDepth( rTemp);

	if( getParent() != nullptr)
	{
		INT32 i;
		i3GICShape * pChild;

	
		for( i = 0; i < getChildCount(); i++)
		{
			pChild = (i3GICShape*) getChild( i);
			if( !i3::kind_of<i3GICLinkPoint*>(pChild)
				&& !i3::kind_of<i3GICControlPoint*>(pChild))
			{
				CreateTreeNodeLine( pChild, i);
			}
		}
	}

	return true;
}

const char * i3GICShape::_getTypeName( void)
{
	I3GIC_STYLE shapeType = getStyle() & I3GIC_STYLE_MASK_TYPE;
	
	switch( shapeType)
	{
	case I3GIC_STYLE_CONTROL :		return I3STR("Control Point");
	case I3GIC_STYLE_LINKER :		return I3STR("Link Point");
	case I3GIC_STYLE_LINE :			return I3STR("Line");
	case I3GIC_STYLE_BEZIERLINE :	return I3STR("Bezier Line");
	case I3GIC_STYLE_RECT :			return I3STR("Rectangle");
	case I3GIC_STYLE_RECTBAR :		return I3STR("Rectangle Bar");
	case I3GIC_STYLE_TEXT :			return I3STR("Text Box");
	case I3GIC_STYLE_TEXTURE :		return I3STR("Texture");
	case I3GIC_STYLE_CIRCLE :		return I3STR("Circle");
	case I3GIC_STYLE_CIRCLEBAR :	return I3STR("Circle Bar");
	case I3GIC_STYLE_TRIANGLE :		return I3STR("Triangle");
	case I3GIC_STYLE_POLYGON :		return I3STR("Polygon");
	}

	return I3STR("Unknown");
}

const char * i3GICShape::_getStateName( void)
{
	switch( getState())
	{
	case I3GIC_STATE_SELECTED :	return I3STR("Selected");
	case I3GIC_STATE_DISABLED :	return I3STR("Disabled");
	case I3GIC_STATE_DRAGGING : return I3STR("Dragging");
	}

	return I3STR("None");
}

/*virtual*/ void i3GICShape::OnProperty( CMFCPropertyGridCtrl * pCtrl)
{
	CMFCPropertyGridProperty * pProperty = new CMFCPropertyGridProperty(I3STR("Shape"));

	COleVariant varName( GetName());
	i3GICProperty * pPropName = new i3GICProperty( I3STR("Name"), varName, I3STR( "이름을 설정합니다"));
	pPropName->setShape( this);
	pPropName->setUpdateType( GIC_PROPERTY_UPDATE_NAME);
	pProperty->AddSubItem( pPropName);
	
	
	COleVariant varType( _getTypeName());
	CMFCPropertyGridProperty* pPropType = new CMFCPropertyGridProperty(I3STR("Type"), varType, I3STR("Shape Type을 표시합니다."));
	pPropType->AllowEdit( FALSE);
    pProperty->AddSubItem( pPropType);

	// Style
	char szStyle[260];

	_GetStyleNamed( szStyle, sizeof( szStyle));

	COleVariant varStyle( szStyle);
	i3GICProperty* pPropStyle = new i3GICProperty(I3STR("Style"), varStyle, I3STR("Shape Style을 표시합니다. (_NBL, _NFL, _LOCK, _GDI+, _NSEL, _INV, _NSL)"));
	pPropStyle->AllowEdit( TRUE);
	pPropStyle->setShape( this);
	pPropStyle->setUpdateType( GIC_PROPERTY_UPDATE_SHAPESTYLE);
    pProperty->AddSubItem( pPropStyle);

	// State
	COleVariant varState( _getStateName());
	CMFCPropertyGridProperty* pPropState = new CMFCPropertyGridProperty(I3STR("State"), varState, I3STR("Shape의 State를 표시합니다."));
	pPropState->AllowEdit( FALSE);
    pProperty->AddSubItem( pPropState);

	// Rect
	COleVariant varPosX( (float) getPosX() );
	COleVariant varPosY( (float) getPosY() );

	i3GICProperty* pPropPosX = new i3GICProperty(I3STR("X"), varPosX, I3STR("Shape의 X좌표"));
	i3GICProperty* pPropPosY = new i3GICProperty(I3STR("Y"), varPosY, I3STR("Shape의 Y좌표"));

	// position
	i3GICProperty* pPropPos = new i3GICProperty(I3STR("Position"));
	pPropPos->AddSubItem( pPropPosX);
	pPropPos->AddSubItem( pPropPosY);
	pPropPos->AllowEdit();
	pPropPos->setShape( this);
	pPropPos->setUpdateType( GIC_PROPERTY_UPDATE_POSITION_2D);

    pProperty->AddSubItem( pPropPos);

	// size
	COleVariant varWidth( (float) getWidth() );
	COleVariant varHeight( (float) getHeight() );

	i3GICProperty* pPropWidth = new i3GICProperty(I3STR("Width"), varWidth, I3STR("Shape의 넓이"));
	i3GICProperty* pPropHeight = new i3GICProperty(I3STR("Height"), varHeight, I3STR("Shape의 높이"));
	
	i3GICProperty* pPropSize = new i3GICProperty(I3STR("Size"));
	pPropSize->AddSubItem( pPropWidth);
	pPropSize->AddSubItem( pPropHeight);
	pPropSize->AllowEdit();
	pPropSize->setShape( this);
	pPropSize->setUpdateType( GIC_PROPERTY_UPDATE_SIZE_2D);

    pProperty->AddSubItem( pPropSize);

	// debug
#if defined( I3_DEBUG)
	COleVariant varDepth( m_rDepth);
	CMFCPropertyGridProperty * pPropDebug_Depth = new CMFCPropertyGridProperty(I3STR("Depth"), &varDepth);
    pProperty->AddSubItem( pPropDebug_Depth);
#endif

	pProperty->Show();
	pCtrl->AddProperty( pProperty);
}

/*virtual*/ void i3GICShape::OnLinkShape( i3GICLinkPoint * pChildLinker, i3GICLinkPoint * pAttachPoint)
{
#if defined( I3_DEBUG)
	INT32 i;
	bool bChildCheck = false;
	for( i = 0; i < getChildCount(); i++)
	{
		if( pChildLinker == getChild( i))
		{
			bChildCheck = true;
			break;
		}
	}

	I3ASSERT( bChildCheck );
#endif

	if( pChildLinker->findLinkedShape( pAttachPoint) == -1)
	{
		pChildLinker->addLinkedShape( pAttachPoint);
		pAttachPoint->addLinkedShape( pChildLinker);
	}
}

/*virtual*/ void i3GICShape::OnUnlinkShape( i3GICLinkPoint * pChildLinker, i3GICLinkPoint * pAttachedPoint)
{
	INT32 i;

#if defined( I3_DEBUG)
	bool bChildCheck = false;
	for( i = 0; i < getChildCount(); i++)
	{
		if( pChildLinker == getChild( i))
		{
			bChildCheck = true;
			break;
		}
	}

	I3ASSERT( bChildCheck );
#endif

	for( i = 0; i < pChildLinker->getLinkedShapeCount(); i++)
	{
		if( pChildLinker->getLinkedShape( i) == pAttachedPoint)
		{
			I3_SAFE_ADDREF( pAttachedPoint);

			pChildLinker->removeLinkedShape( pAttachedPoint);

			pAttachedPoint->removeLinkedShape( pChildLinker);

			I3_SAFE_RELEASE( pAttachedPoint);
			break;
		}
	}
}

/*virtual*/ void i3GICShape::OnUnlinkAllShape( i3GICLinkPoint * pChildLinker)
{
	INT32 i;

#if defined( I3_DEBUG)
	bool bChildCheck = false;
	for( i = 0; i < getChildCount(); i++)
	{
		if( pChildLinker == getChild( i))
		{
			bChildCheck = true;
			break;
		}
	}

	I3ASSERT( bChildCheck );
#endif

	for( i = 0; i < pChildLinker->getLinkedShapeCount(); i++)
	{
		i3GICLinkPoint * pLink = (i3GICLinkPoint*) pChildLinker->getLinkedShape( i);

		I3_SAFE_ADDREF( pChildLinker);

		pLink->removeLinkedShape( pChildLinker);
		pChildLinker->removeLinkedShape( pLink);

		I3_SAFE_RELEASE( pChildLinker);
	}
}

/*virtual*/ void i3GICShape::OnBeginDragLinkedShape( void)
{
	INT32 i;
	i3GICShape * pChild;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape*) getChild( i);

		if( i3::same_of<i3GICLinkPoint*>(pChild))
		{
			// Linked Shape들을 Update해준다.
			((i3GICLinkPoint*)pChild)->BeginDragEventToLinkedShape();
		}
		else if( !pChild->isState( I3GIC_STATE_DISABLED))
		{
			pChild->OnBeginDragLinkedShape();
		}
	}
}


/*virtual*/ void i3GICShape::OnEndDragLinkedShape( void)
{
	INT32 i;
	i3GICShape * pChild;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape*) getChild( i);

		if( i3::same_of<i3GICLinkPoint* >(pChild))
		{
			// Linked Shape들을 Update해준다.
			((i3GICLinkPoint*) pChild)->EndDragEventToLinkedShape();
		}
		else if( !pChild->isState( I3GIC_STATE_DISABLED))
		{
			pChild->OnEndDragLinkedShape();
		}
	}
}


/*virtual*/ void i3GICShape::OnCancelDragLinkedShape( void)
{
	INT32 i;
	i3GICShape * pChild;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape*) getChild( i);

		if( i3::same_of<i3GICLinkPoint*>(pChild))
		{
			// Linked Shape들을 Update해준다.
			((i3GICLinkPoint*) pChild)->CancelDragEventToLinkedShape();
		}
		else if( !pChild->isState( I3GIC_STATE_DISABLED))
		{
			pChild->OnCancelDragLinkedShape();
		}
	}
}

/*virtual*/ void i3GICShape::OnDragLinkedShape( i3GICDiagramContext * pCtx, VEC2D * pUpdatedPos)
{
	INT32 i;
	i3GICShape * pChild;

	I3ASSERT( pCtx != nullptr);
	I3ASSERT( pUpdatedPos != nullptr);

	// Link Update 활성 여부
	if( pCtx->isLinkUpdate() == false)
		return;

	for( i = 0; i < getChildCount(); i++)
	{
		pChild = (i3GICShape*) getChild( i);

		if( i3::same_of<i3GICLinkPoint*>(pChild))
		{
			// Linked Shape들을 Update해준다.
			((i3GICLinkPoint*) pChild)->UpdateLinkedShape( pCtx, pUpdatedPos);
		}
		else if( !pChild->isState( I3GIC_STATE_DISABLED))
		{
			pChild->OnDragLinkedShape( pCtx, pUpdatedPos);
		}
	}
}

/*virtual*/ void i3GICShape::OnUpdateByLink( i3GICDiagramContext * pCtx, i3GICLinkPoint * pLinker, i3GICLinkPoint * pOwnerLink)
{
	
}

#if defined( I3_DEBUG_DUMP)
void i3GICShape::Dump( void)
{
	I3TRACE( I3STR("- i3GICShape : %s\n", GetName()));

	INT32 i;

	for( i = 0; i < getChildCount(); i++)
	{
		i3GICShape * pChild = (i3GICShape*) getChild( i);

		I3TRACE( I3STR("\t└"));

		pChild->Dump();
	}
}
#endif	// I3_DEBUG_DUMP


#endif	// I3_DEBUG

