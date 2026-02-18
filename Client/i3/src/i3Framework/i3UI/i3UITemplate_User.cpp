#include "i3FrameworkPCH.h"
#include "i3UITemplate_User.h"
#include "i3UIControl_User.h"
#include "i3Base/itl/vector_util.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UITemplate_User);

i3UITemplate_User::i3UITemplate_User()
{
	m_ShapeList.reserve( 32);
}

i3UITemplate_User::~i3UITemplate_User()
{
	INT32 i;
	for( i = 0;i < getShapeCount(); ++i)
	{
		I3UI_SHAPE_INFO * pInfo = m_ShapeList[i];
			
		I3_SAFE_RELEASE( pInfo->m_pShape);
		I3_MUST_DELETE( pInfo);
	}
	m_ShapeList.clear();
}

void i3UITemplate_User::resetTemplate(void)
{
	INT32 i;

	// Shape 정리
	for( i = 0;i < getShapeCount(); ++i)
	{
		I3UI_SHAPE_INFO * pInfo = m_ShapeList[i];
			
		I3_SAFE_RELEASE( pInfo->m_pShape);
		I3_MUST_DELETE( pInfo);
	}
	m_ShapeList.clear();

	// Sub-Template 정리.
	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemplate = getSubTemplate( i);

		if( pTemplate->isBuiltInSubTemplate() == false)			// ButtonImageSet Class에서 사용
		{
			removeSubTemplate( pTemplate);
			i--;
		}
	}
}

INT32 i3UITemplate_User::RemoveShape( I3UI_SHAPE_INFO * pInfo)
{
	INT32 idx = (INT32) i3::vu::index_of(m_ShapeList, pInfo);		//m_ShapeList.Find((void*)pInfo);
	if ((UINT32)idx == m_ShapeList.size()) idx = -1;
		
	if( idx != -1)
	{
		I3_SAFE_RELEASE( pInfo->m_pShape);
		I3_MUST_DELETE( pInfo);

		m_ShapeList.erase( m_ShapeList.begin() + idx);
	}

	return idx;
}

INT32 i3UITemplate_User::RemoveShape( i3UIShape * pShape)
{
	INT32 i;
	for( i = 0;i < getShapeCount(); ++i)
	{
		I3UI_SHAPE_INFO * pInfo = m_ShapeList[i];
		if( pInfo->m_pShape == pShape)
		{
			I3_SAFE_RELEASE( pInfo->m_pShape);
			I3_MUST_DELETE( pInfo);

			m_ShapeList.erase( m_ShapeList.begin() + i);

			return i;
		}
	}

	return -1;
}

INT32 i3UITemplate_User::AddNewShape( I3UI_SHAPE_INFO * pInfo)
{
	I3ASSERT( pInfo != nullptr);

	m_ShapeList.push_back(pInfo);

	return (INT32)m_ShapeList.size() - 1;
}

const char *	i3UITemplate_User::getComment(void)
{
	return "Custom Control";
}

const char *	i3UITemplate_User::getTemplateName(void)
{
	return "CustomControl";
}

I3UI_CATEGORY	i3UITemplate_User::getTemplateCategory(void)
{
	return I3UI_CATEGORY_CUSTOM;
}

i3ClassMeta	*	i3UITemplate_User::GetInstanceMeta()
{
	return i3UIUserControl::static_meta();
}

void i3UITemplate_User::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3UITemplate_User* >(pDest));

	i3UITemplate_User * pDestUser = (i3UITemplate_User*)pDest;
	if( pDestUser->getShapeCount() != getShapeCount())
	{
		pDestUser->resetTemplate();

		INT32 i;
		for( i = 0;i < getShapeCount();++i)
		{
			I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO;

			NetworkDump_Malloc( pInfo, sizeof( I3UI_SHAPE_INFO), __FILE__, __LINE__);
			pInfo->m_pShape = i3UIShape::new_object();

			pDestUser->AddNewShape( pInfo);
		}
	}

	INT32 i;
	for( i = 0;i < getShapeCount();++i)
	{
		i3UIShape * pSrcShape = getShape(i);

		I3UI_SHAPE_INFO * pInfo = pDestUser->getShapeInfo(i);

		pSrcShape->CopyTo( pInfo->m_pShape, method);
		
		pInfo->m_strInfoString = m_ShapeList[i]->m_strInfoString;
	}

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pSrcTemp, * pDestTemp;

		pSrcTemp = getSubTemplate( i);
		pDestTemp = pDestUser->getSubTemplate( i);

		if( method == I3_COPY_INSTANCE)
		{
			if( pSrcTemp != nullptr)
			{
				if( pDestTemp == nullptr || pDestTemp->same_of( pSrcTemp->meta()))
				{
					pDestTemp =	pSrcTemp->create_instance(); //	(i3UITemplate *) CREATEINSTANCE( pSrcTemp->GetMeta());

					pDestUser->setSubTemplate( i, pDestTemp);
				}

				pSrcTemp->CopyTo( pDestUser, method);
			}
		}
		else
		{
			pDestUser->setSubTemplate( i, pSrcTemp);
		}
	}
}

void i3UITemplate_User::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3UITemplate::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct UITEMPLATE
	{
		UINT8		m_ID[4] = { 'u', 's', 'e', 'r' };
		INT32		m_ShapeCount = 0;
		INT32		m_SubTemplateCount = 0;
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UITemplate_User::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;

	pack::UITEMPLATE data;
		
	data.m_ShapeCount = getShapeCount();
	data.m_SubTemplateCount = getSubTemplateCount();

	Result = pResFile->Write(&data, sizeof( data));
	I3_CHKIO( Result);

	Rc = i3UITemplate::OnSave( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UITemplate_User::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Result, Rc;
	pack::UITEMPLATE data;

	Result = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Result);

	// user template는 기본적으로 shape를 가지고 있지 않기 때문에 load할때 만들어 주어야 한다.
	INT32 i;
	for( i = 0;i < data.m_ShapeCount;++i)
	{
		I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO;
		NetworkDump_Malloc( pInfo, sizeof( I3UI_SHAPE_INFO), __FILE__, __LINE__);
		pInfo->m_pShape = i3UIShape::new_object();

		AddNewShape( pInfo);
	}

	m_List.resize( data.m_SubTemplateCount);

	Rc = i3UITemplate::OnLoad( pResFile);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

bool i3UITemplate_User::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnSaveXML( pFile, pXML);
	if( bRv == false)	return false;

	pXML->addAttr("usershapeCount", getShapeCount());
	pXML->addAttr("usersubtemplateCount", getSubTemplateCount());

	INT32 i;
	char conv[MAX_PATH];

	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);

		i3::snprintf( conv, MAX_PATH, "Shape%d", i);

		i3XMLElement * pNewXML = new i3XMLElement( conv);
		NetworkDump_Malloc( pNewXML, sizeof( i3XMLElement), __FILE__, __LINE__);
		pNewXML = (i3XMLElement*) pXML->LinkEndChild( pNewXML);
		I3ASSERT( pNewXML != nullptr);

		pShape->OnSaveXML( pFile, pNewXML);

		pNewXML->addAttr( "shapeString", getShapeInfoString(i));
	}

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemp = getSubTemplate( i);
		OBJREF ref;

		ref = pFile->GetObjectPersistID( pTemp);
		
		i3::snprintf( conv, MAX_PATH, "SubTemplate%d", i);
		pXML->addAttr( conv, ref);
	}

	return bRv;
}

bool i3UITemplate_User::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3_prof_func();

	bool bRv = i3ResourceObject::OnLoadXML( pFile, pXML);
	if( bRv == false)	return false;

	INT32 shapeCount, subTemplatecount;
	char conv[MAX_PATH];

	pXML->getAttr("usershapeCount", &shapeCount);
	pXML->getAttr("usersubtemplateCount", &subTemplatecount);

	// user template는 기본적으로 shape를 가지고 있지 않기 때문에 load할때 만들어 주어야 한다.
	INT32 i;
	for( i = 0;i < shapeCount;++i)
	{
		I3UI_SHAPE_INFO * pInfo = new I3UI_SHAPE_INFO;
		NetworkDump_Malloc( pInfo, sizeof( I3UI_SHAPE_INFO), __FILE__, __LINE__);
		pInfo->m_pShape = i3UIShape::new_object();

		AddNewShape( pInfo);
	}

	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);
		I3UI_SHAPE_INFO * pInfo = getShapeInfo(i);

		i3::snprintf( conv, MAX_PATH, "Shape%d", i);

		i3XMLElement * pChild = (i3XMLElement *) pXML->FirstChild( conv);

		if( pChild != nullptr)
		{
			pShape->OnLoadXML( pFile, pChild);

			const char * pszShapeString = pChild->getAttr("shapeString");
			pInfo->m_strInfoString = pszShapeString;
		}
		else
		{
			I3TRACE("i3UITemplate::OnLoadXML: pChild == nullptr    %s\n", conv);
		}
	}

	for( i = 0; i < subTemplatecount; i++)
	{
		i3UITemplate * pTemp;
		OBJREF ref;

		i3::snprintf( conv, MAX_PATH, "SubTemplate%d", i);

		if( pXML->getAttr( conv, &ref) != nullptr)
		{
			pTemp = (i3UITemplate *) pFile->FindObjectByID( ref);

			I3ASSERT( pTemp != nullptr);
		
			addSubTemplate(pTemp);
		}
	}

	i3::pack::RECT rt;
	GetControlRect(&rt);
	AdjustLayout( rt.right - rt.left, rt.bottom - rt.top);

	return bRv;
}

bool i3UITemplate_User::GetControlRect( i3::pack::RECT * pRect)
{
	if( getShapeCount() <= 0)
	{
		pRect->left = pRect->top = 0.0f;
		pRect->right = pRect->bottom = 10.0f;
		return false;
	}

	INT32 i;
	for( i = 0;i < getShapeCount(); ++i)
	{
		i3UIShape * pShape = getShape( i);

		if( i == 0)
		{
			*pRect = *pShape->getRect();
			pRect->right += pShape->getPosX();
			pRect->bottom += pShape->getPosY();
		}
		else
		{
			pRect->left		= i3Math::Min( pRect->left, pShape->getPosX());
			pRect->top		= i3Math::Min( pRect->top, pShape->getPosY());
			pRect->right	= i3Math::Max( pRect->right, pShape->getPosX() + pShape->getWidth());
			pRect->bottom	= i3Math::Max( pRect->bottom, pShape->getPosY() + pShape->getHeight());
		}
	}

	pRect->Normalize();

	return true;
}

void i3UITemplate_User::InitializeLayout(void)
{

}

bool i3UITemplate_User::AdjustLayout(REAL32 cx, REAL32 cy, i3::pack::RECT * pRects, INT32 shapeCount)
{
	INT32 i;
	INT32 totalCount = getShapeCount();

	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0;i < totalCount; ++i)
	{
		i3UIShape		* pShape = getShape( i);
		i3::pack::RECT			* pRect	 = _getTargetRect( i, pRects);
		const i3::pack::RECT	* pSrcRect = pShape->getRect();

		*pRect = *pSrcRect;
	/*
		REAL32		rateX = 1.0f;
		if( cx > I3_EPS)	rateX = cx / (pSrcRect->right - pSrcRect->left);

		REAL32		rateY = 1.0f;
		if( cy > I3_EPS) rateY = cy / (pSrcRect->bottom - pSrcRect->top);

		pRect->left = pSrcRect->left * rateX;
		pRect->right= pSrcRect->right * rateX;

		pRect->top = pSrcRect->top * rateY;
		pRect->bottom = pSrcRect->bottom * rateY;
	*/
	}

	return true;
}

void i3UITemplate_User::SpreadLayout( REAL32 * pCX, REAL32 * pCY, i3::pack::RECT * pRects, INT32 shapeCount)
{
	*pCX = 0.0f;
	*pCY = 0.0f;

	REAL32 left = 0.0f;
	REAL32 top = 0.0f;

	INT32 i;
	INT32 totalCount = getShapeCount();
	if( shapeCount > 0)
		totalCount = MIN( totalCount, shapeCount);

	for( i = 0;i < totalCount; ++i)
	{
		i3::pack::RECT * pRect = _getTargetRect( i, pRects);

		pRect->left = left;
		pRect->top	= top;

		left += pRect->right + I3UI_SPREAD_MARGIN;

		*pCX = i3Math::Max( left, *pCX);
		
		if( i%5 == 4)
		{
			left = 0.0f;
			top += pRect->bottom + I3UI_SPREAD_MARGIN;
		}
		
		*pCY = i3Math::Max( pRect->bottom, *pCX);
	}

	return ;
}
