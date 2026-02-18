#include "stdafx.h"
#include "i3GICDiagram.h"
#include "i3GICDiagramView.h"
#include "i3GICLinkPoint.h"
#include "i3Base/itl/algorithm/sort.h"
#if defined( I3_DEBUG)

//#define	GIC_CLIPBOARDFORMAT			"GIC_CLIPBOARD"

I3_CLASS_INSTANCE( i3GICDiagram);


i3GICDiagram::~i3GICDiagram(void)
{
	RemoveAllShape();

	for(size_t i = 0; i < m_TemplateList.size(); i++)
	{
		i3GICShapeTemplate * pTemplate = (i3GICShapeTemplate*) m_TemplateList[ i];
		I3_SAFE_RELEASE( pTemplate);
	}
}

void i3GICDiagram::_Rec_FindSelectedShape( i3GICShape * pShape, i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	if( pShape->isSelected())
		List.push_back( pShape);

	for( i = 0; i < pShape->getChildCount(); i++)
	{
		_Rec_FindSelectedShape( (i3GICShape*)pShape->getChild( i), List);
	}
}

void i3GICDiagram::BindContext( bool bBind)
{
	INT32 i;

	m_bBindContext = bBind;

	for( i = 0; i < getShapeCount(); i++)
	{
		i3GICShape * pShapde = getShape(i);
		if (pShapde)
			pShapde->OnBindDiagram( bBind);
	}
}

////////////////////////////////////////////////////////////////////////////
//
INT32 i3GICDiagram::AddShape( i3GICShape * pParent, i3GICShape * pShape)
{
	INT32 idx = -1;
	I3ASSERT( pShape != nullptr);

	if( pParent == nullptr)
	{
		//if( m_ShapeList.Find( pShape) > -1)
		if (i3::vu::index_of(m_ShapeList, pShape) != m_ShapeList.size() )
		{
			I3PRINTLOG(I3LOG_FATAL,  _T("Already add shape.."));
			return -1;
		}

		// 최상위로 붙는 경우...
		I3_SAFE_ADDREF( pShape);
		idx = (INT32) m_ShapeList.size();
		m_ShapeList.push_back( pShape);
	}
	else
	{
		pShape->AttachToShape( pParent);

		i3TDK::Update( nullptr, I3_TDK_UPDATE_EDIT, pParent);
	}

	pShape->OnAttach( this);
	if( pShape->getDepth() == 0.f)
		pShape->setDepth( (REAL32) getShapeCount() );

	pShape->OnBindDiagram( m_bBindContext);

	i3TDK::Update( nullptr, I3_TDK_UPDATE_ADD, pShape);

	return idx;
}

void i3GICDiagram::RemoveShape( i3GICShape * pShape)
{
	I3ASSERT( pShape != nullptr);

	// Notify 하는 동안, pShape가 살아있는 것을 보장하기 위해...
	I3_SAFE_ADDREF( pShape);

	if( pShape->getParent() == nullptr)
	{
		i3::vu::remove(m_ShapeList, pShape);

		I3_SAFE_RELEASE_NONULL_POINTER( pShape);
	}
	else
	{
		pShape->DetachFromParentShape();

		i3TDK::Update( nullptr, I3_TDK_UPDATE_EDIT, pShape);
	}

	i3TDK::Update( nullptr, I3_TDK_UPDATE_REMOVE, pShape);

	pShape->OnDetach( this);

	pShape->OnBindDiagram( false);

	I3_SAFE_RELEASE( pShape);
}

void i3GICDiagram::RemoveAllShape(void)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		pShape->OnDetach( this);

		pShape->OnBindDiagram( false);

		I3_SAFE_RELEASE( pShape);
	}

	m_ShapeList.clear();

	i3TDK::Update( nullptr, I3_TDK_UPDATE_REMOVE, nullptr, i3GICShape::static_meta());
}

void i3GICDiagram::RemoveSelectedShapes(void)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		if( pShape->isSelected())
		{
			RemoveShape( pShape);

			i--;
		}
	}
}

void i3GICDiagram::SelectShape( i3GICShape * pShape)
{
	pShape->OnSelect( true);

	i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, pShape);
}

void i3GICDiagram::SelectShape( const i3::vector<i3GICShape*>& List)
{
	i3GICShape * pShape;

	for(size_t i = 0; i < List.size(); i++)
	{
		pShape = List[i];

		pShape->OnSelect( true);

		i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, pShape);
	}
}

void i3GICDiagram::SelectAllShapes(void)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape(i);

		pShape->OnSelect( true);
	}

	i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagram::UnselectShape( i3GICShape * pShape)
{
	pShape->OnSelect( false);

	i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, pShape);
}

void i3GICDiagram::UnselectShape( const i3::vector<i3GICShape*>& List)
{
	i3GICShape * pShape;

	for( size_t i = 0; i < List.size(); i++)
	{
		pShape = List[i];

		pShape->OnSelect( false);

		i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, pShape);
	}
}

void i3GICDiagram::UnselectAllShapes(void)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		getShape(i)->OnSelect( false);
	}

	i3TDK::Update( nullptr, I3_TDK_UPDATE_SELECT, nullptr, i3GICShape::static_meta());
}

void i3GICDiagram::GetSelectedShapes(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		_Rec_FindSelectedShape( getShape(i), List);
	}
}

void i3GICDiagram::BeginDrag(void)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape(i)->isSelected())
			getShape(i)->OnBeginDrag();
	}
}

void i3GICDiagram::EndDrag(void)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape(i)->isSelected())
			getShape(i)->OnEndDrag();
	}

	UpdateAllViews();
}

void i3GICDiagram::CancelDrag(void)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape(i)->isSelected())
			getShape(i)->OnCancelDrag();
	}

	UpdateAllViews();
}

void i3GICDiagram::DragCP( i3GICDiagramContext * pCtx)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape(i)->isSelected())
		{
			getShape(i)->OnDragCP( pCtx);
		}
	}

	UpdateAllViews();
}

void i3GICDiagram::DragMove( i3GICDiagramContext * pCtx)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape(i)->isSelected())
		{
			getShape(i)->OnDragMoving( pCtx);
		}
	}

	UpdateAllViews();
}

void i3GICDiagram::DragCV( i3GICDiagramContext * pCtx)
{
	INT32 i;

	for( i = 0; i < getShapeCount(); i++)
	{
		if( getShape( i)->isSelected())
		{
			getShape( i)->OnDragCV( pCtx);
		}
	}
}

static bool s_gicHitTestLinkPoint( GIC_HIT_RESULT * pSrcResult, GIC_HIT_RESULT * pDestResult, 
								  const i3::vector<i3TreeNode*>& SrcList, const i3::vector<i3TreeNode*>& DestList)
{

	i3GICLinkPoint * pSrc, * pDest;

	for(size_t i = 0; i < SrcList.size(); i++)
	{
		pSrc = (i3GICLinkPoint*) SrcList[ i];

		for( size_t j = 0; j < DestList.size(); j++)
		{
			pDest = (i3GICLinkPoint*) DestList[ j];							// i로 되어있던데 버그성으로 판단되며, j로 교체(2012.05.02.수빈)

			if( pDest->OnHitTest( nullptr, pSrc->getPosX(), pSrc->getPosY()) )
			{
				pSrcResult->m_pHitShape = pSrc;
				pSrcResult->m_rDepth = (REAL32) i;

				pDestResult->m_pHitShape = pDest;
				pDestResult->m_rDepth = (REAL32) j;
				return true;
			}
		}
	}

	return false;
}

void i3GICDiagram::LinkSnap( i3GICShape * pShape, REAL32 * pX, REAL32 * pY)
{
	INT32 i;
	i3GICShape * pDest;
	i3::vector<i3TreeNode*> list, destList;

	GIC_HIT_RESULT srcResult, destResult;

	i3TreeNode::FindNodeByExactType( pShape, i3GICLinkPoint::static_meta(), list);

	for( i = 0; i < getShapeCount(); i++)
	{
		pDest = getShape( i);

		if( pShape != pDest)
		{
			destList.clear();
			i3TreeNode::FindNodeByExactType( pDest, i3GICLinkPoint::static_meta(), destList);

			if( s_gicHitTestLinkPoint( &srcResult, &destResult, list, destList) )
			{
				REAL32 rDistX = destResult.m_pHitShape->getPosX() - srcResult.m_pHitShape->getPosX();
				REAL32 rDistY = destResult.m_pHitShape->getPosY() - srcResult.m_pHitShape->getPosY();

				*pX += rDistX;
				*pY += rDistY;

				return;
			}
		}
	}
}

i3GICShape * i3GICDiagram::FindShapeByName( const char * pszName)
{
	INT32 i;
	i3GICShape * pShape, * pResult;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		pResult = (i3GICShape *) i3TreeNode::FindNodeByName( pShape, pszName, true);
		if( pResult != nullptr)
			return pResult;
	}

	return nullptr;
}

static void s_GICShapeChangeList( i3GICShape * p1, i3GICShape * p2)
{
	i3TreeNode * pParent = p1->getParent();

	if( pParent == nullptr)
		return ;

	INT32 i, p1Idx = -1, p2Idx = -1;
	for( i = 0; i < pParent->getChildCount(); i++)
	{
		if( pParent->getChild( i) == p1)
			p1Idx = i;

		if( pParent->getChild( i) == p2)
			p2Idx = i;
	}

	if( p1Idx != -1)
		pParent->setChild( p1Idx, p2);

	if( p2Idx != -1)
		pParent->setChild( p2Idx, p1);
}
/*
static INT32 s_GICShapeSortProcByDepth( i3GICShape * p1, i3GICShape * p2)
{
	if( p1->getParent() == p2->getParent() )
	{
		if( p1->getDepth() > p2->getDepth())
		{
			s_GICShapeChangeList( p1, p2);

			return 1;
		}
		else if( p1->getDepth() < p2->getDepth())
		{
			//s_GICShapeChangeList( p1, p2);

			return -1;
		}
	}

	return 0;
}
*/
namespace
{
	bool s_GICShapeSortProcByDepth( const i3GICShape * p1, const i3GICShape * p2)
	{
		return p1->getDepth() < p2->getDepth();
	}
}


void i3GICDiagram::SortByDepth( void)
{
//	m_ShapeList.Sort( (COMPAREPROC) s_GICShapeSortProcByDepth);
	i3::sort(m_ShapeList.begin(), m_ShapeList.end(), s_GICShapeSortProcByDepth);
}

bool i3GICDiagram::SaveTemplate( const char * pszPath)
{
	i3XMLFile file;

	if( m_TemplateList.size() == 0)
		return false;

	file.SetObjects( m_TemplateList);

	if( file.Save( pszPath) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일을 저장할 수 없습니다.", pszPath);
		return false;
	}

	return true;
}


bool i3GICDiagram::LoadTemplate( const char * pszPath)
{
	i3XMLFile file;

	if( file.Load( pszPath) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "%s 파일을 로드할 수 없습니다.", pszPath);
		return false;
	}

	INT32 i;

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		i3PersistantElement * pElement = file.GetObject( i);
		if( i3::same_of<i3GICShapeTemplate*>(pElement))
		{
			addTemplate( (i3GICShapeTemplate*) pElement);
			((i3GICShapeTemplate*) pElement)->EditMode( true);
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////////
//
void i3GICDiagram::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i );

		if( pShape != nullptr)
			pShape->OnBuildObjectList( List);
	}

	i3GICObject::OnBuildObjectList( List);
}

#pragma pack(push, 8)				// [test required] 패킹 사이즈 누락으로 추가함. 2017.02.14. soon9

namespace pack
{
	struct GIC_DIAGRAM
	{
		UINT8		m_ID[4] = { 'G', 'D', 'G', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#pragma pack(pop)

UINT32	i3GICDiagram::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GIC_DIAGRAM data;

	Result = i3GICObject::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = SaveFromListToResourceFile(m_ShapeList, pResFile); //m_ShapeList.SaveTo( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3GICDiagram::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	pack::GIC_DIAGRAM data;

	Result = i3GICObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = LoadFromResourceFileToList(m_ShapeList, pResFile, true);	// m_ShapeList.LoadFrom( pResFile, true);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

/*virtual*/ bool i3GICDiagram::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3NamedElement::OnSaveXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Fail Save XML.");
		return false;
	}

//	if( m_ShapeList.SaveToXML( "ShapeList", pFile, pXML, true) == false)
	if (SaveFromListToXML(m_ShapeList, "ShapeList", pFile, pXML, true ) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Shape List Save Fail");
		return false;
	}

	return true;
}

/*virtual*/ bool i3GICDiagram::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	if( i3NamedElement::OnLoadXML( pFile, pXML) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "XML File Load fail.");
		return false;
	}

//	if( m_ShapeList.LoadFromXML( "ShapeList", pFile, pXML, true) == false)
	if (LoadFromXMLToList(m_ShapeList, "ShapeList", pFile, pXML, true) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Shape list load fail.");
		return false;
	}


	return true;
}


i3GICShapeTemplate * i3GICDiagram::FindTemplate( const char * pszName)
{
	INT32 i;

	i3GICShapeTemplate * pTemplate;

	for( i = 0; i < getTemplateCount(); i++)
	{
		pTemplate = getTemplate( i);

		if( i3::generic_is_iequal( pTemplate->GetName(), pszName) )
			return pTemplate;
	}

	return nullptr;
}


#if defined( I3_DEBUG)
void i3GICDiagram::Dump(void)
{
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		I3TRACE( "SHAPE[%02d] : %s\n", i, pShape->meta()->class_name());
	}
}
#endif

#endif	// I3_DEBUG


void i3GICDiagram::AddView( i3GICDiagramView * pView)
{
	pView->setDiagram( this);
	m_ViewList.push_back( pView);

	// Diagram이 Bind된것을 Shape에 알려주어 GDI를 활성화시킨다.
	/*INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		pShape->OnBindDiagram( true);
	}*/
}

void i3GICDiagram::RemoveAllView(void)
{
	m_ViewList.clear();

	// Diagram이 UnBind된것을 Shape에 알려주어 GDI를 비활성화시킨다.
	INT32 i;
	i3GICShape * pShape;

	for( i = 0; i < getShapeCount(); i++)
	{
		pShape = getShape( i);

		pShape->OnBindDiagram( false);
	}
}

void i3GICDiagram::UpdateAllViews(void)
{
	
	i3GICDiagramView * pView;

	for( size_t i = 0; i < m_ViewList.size(); i++)
	{
		pView = m_ViewList[i];

		pView->Invalidate();
	}
}