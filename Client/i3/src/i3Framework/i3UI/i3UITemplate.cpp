#include "i3FrameworkPCH.h"
#include "i3UITemplate.h"
#include "i3Base/itl/vector_util.h"

#include "i3UITemplate_Button.h"
#include "i3UITemplate_ButtonComposed3.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UITemplate);

i3UITemplate::~i3UITemplate()
{
	
	for(size_t i = 0;i < m_List.size(); ++i)
	{
		i3UITemplate * pTemplate = m_List[i];

		I3_SAFE_RELEASE( pTemplate);
	}
	m_List.clear();
}

void i3UITemplate::Create(void)
{
}

INT32			i3UITemplate::getSubTemplateCount(void)
{
	return (INT32)m_List.size();
}

i3UITemplate *	i3UITemplate::getSubTemplate( INT32 idx)
{
	I3_BOUNDCHK( idx, (INT32)m_List.size());

	return m_List[idx];
}

const char *	i3UITemplate::getSubTemplateComment( INT32 idx)
{
	i3UITemplate * pTemplate = getSubTemplate( idx);

	if( pTemplate != nullptr)
		return pTemplate->getComment();

	return "Invalid sub-template index";
}

void i3UITemplate::removeSubTemplate( i3UITemplate * pTemplate)
{
	I3ASSERT( pTemplate != nullptr);

//	INT32 idx = m_List.Find( pTemplate);
	size_t idx = i3::vu::index_of(m_List, pTemplate);

	if( idx != m_List.size() )
	{
		I3_MUST_RELEASE( pTemplate);
		m_List.erase( m_List.begin() + idx);
	}
}

INT32 i3UITemplate::addSubTemplate( i3UITemplate * pTemplate)
{
	if( pTemplate == nullptr)
		return -1;

	I3_SAFE_ADDREF( pTemplate);

	m_List.push_back( pTemplate);
	return m_List.size() - 1;
}

void i3UITemplate::setSubTemplate( INT32 idx, i3UITemplate * pTemplate)
{
	I3_BOUNDCHK( idx, (INT32)m_List.size());

	I3_SAFE_ADDREF( pTemplate);

//	m_List.SetItem( idx, pTemplate);
	i3::vu::set_value_force(m_List, idx, pTemplate);
}

void i3UITemplate::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	INT32 i;

	i3ResourceObject::CopyTo( pDest, method);

	I3ASSERT( pDest->kind_of( i3UITemplate::static_meta()));

	i3UITemplate * pTemplate;

	pTemplate = (i3UITemplate *) pDest;

	// Shape List 복사
	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pSrcShape, * pDestShape;

		pSrcShape = getShape( i);
		pDestShape = pTemplate->getShape( i);

		pSrcShape->CopyTo( pDestShape, method);
	}

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pSrcTemp, * pDestTemp;

		pSrcTemp = getSubTemplate( i);
		pDestTemp = pTemplate->getSubTemplate( i);

		if( method == I3_COPY_INSTANCE)
		{
			if( pSrcTemp != nullptr)
			{
				if( pDestTemp == nullptr)
				{
					pDestTemp =	pSrcTemp->create_instance(); //	(i3UITemplate *) CREATEINSTANCE( pSrcTemp->GetMeta());

					pTemplate->setSubTemplate( i, pDestTemp);
				}

				pSrcTemp->CopyTo( pDestTemp, method);
			}
		}
		else
		{
			pTemplate->setSubTemplate( i, pSrcTemp);
		}
	}
}

void i3UITemplate::RefreshOffset( i3VirtualTexture * pVTex)
{
	if( pVTex == nullptr)
		return;

	
	for(INT32 i = 0;i < getShapeCount(); ++i)
	{
		i3UIShape * pShape = getShape(i);

		I3VT_TEXINFO * info = pVTex->FindRefTexture( pShape->getTextureResPath());

		if(info)
		{
			pShape->setTexcoordOffset( info->m_X, info->m_Y);
		}
		else
		{
			I3TRACE( "Texture가 설정되지 않은 Shape가 존재 합니다. i3UITemplate_Name : %s i3UIShape_TexPath = %s\n", GetName(), pShape->getTextureResPath() );
		}
	}

	i3UITemplate * pSubTemp;
	for(INT32 i = 0; i < getSubTemplateCount(); i++)
	{
		pSubTemp = getSubTemplate( i);

		pSubTemp->RefreshOffset( pVTex);
	}
}

void i3UITemplate::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;
	i3UITemplate * pSub;

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		pSub = getSubTemplate( i);

		pSub->OnBuildObjectList( List);
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct UITEMPLATE
	{
		UINT8		m_ID[4] = { 'U', 'T', 'P', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UITemplate::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::UITEMPLATE data;
	INT32 i;

	result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// Shape Data
	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);

		rc = pShape->OnSave( pResFile);
		I3_CHKIO( rc);
		result += rc;
	}
	
	// SubTemplate Reference
	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemplate = getSubTemplate( i);
		OBJREF ref;

		ref = pResFile->GetObjectPersistID( pTemplate);

		rc = pResFile->Write( &ref, sizeof(ref));
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32 i3UITemplate::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 rc, result;
	pack::UITEMPLATE data;
	INT32 i;

	result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// Shape Data
	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);

		rc = pShape->OnLoad( pResFile);
		I3_CHKIO( rc);
		result += rc;
	}
	
	// SubTemplate Reference
	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemplate;
		OBJREF ref;

		rc = pResFile->Read( &ref, sizeof(ref));
		I3_CHKIO( rc);
		result += rc;

		if( ref != 0)
		{
			pTemplate = (i3UITemplate *) pResFile->FindObjectByID( ref);
			I3ASSERT( pTemplate != nullptr);

			setSubTemplate( i, pTemplate);
		}
	}

	i3::pack::RECT rt;
	GetControlRect(&rt);
	AdjustLayout( rt.right - rt.left, rt.bottom - rt.top);

	return result;
}

bool i3UITemplate::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 i;
	char conv[256];
	
	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);

		i3::snprintf( conv, sizeof( conv), "Shape%d", i);

		i3XMLElement * pNewXML = new i3XMLElement( conv);
		NetworkDump_Malloc( pNewXML, sizeof( i3XMLElement), __FILE__, __LINE__);
		pNewXML = (i3XMLElement*) pXML->LinkEndChild( pNewXML);
		I3ASSERT( pNewXML != nullptr);

		pShape->OnSaveXML( pFile, pNewXML);
	}

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemp = getSubTemplate( i);
		OBJREF ref;

		ref = pFile->GetObjectPersistID( pTemp);
		
		i3::snprintf( conv, sizeof( conv), "SubTemplate%d", i);
		pXML->addAttr( conv, ref);
	}

	return true;
}

bool i3UITemplate::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
i3_prof_func();

	bool bRv = i3ResourceObject::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 i;
	char conv[256];

	for( i = 0; i < getShapeCount(); i++)
	{
		i3UIShape * pShape = getShape( i);

		i3::snprintf( conv, sizeof(conv), "Shape%d", i);

		i3XMLElement * pChild = (i3XMLElement *) pXML->FirstChild( conv);

		if( pChild != nullptr)
			pShape->OnLoadXML( pFile, pChild);
		else
			I3TRACE("i3UITemplate::OnLoadXML: pChild == nullptr    %s\n", conv);
	}

	for( i = 0; i < getSubTemplateCount(); i++)
	{
		i3UITemplate * pTemp;
		OBJREF ref;

		i3::snprintf( conv, sizeof( conv), "SubTemplate%d", i);

		if( pXML->getAttr( conv, &ref) != nullptr)
		{
			pTemp = (i3UITemplate *) pFile->FindObjectByID( ref);

			I3ASSERT( pTemp != nullptr);
		
			setSubTemplate(i, pTemp);
		}
	}

	i3::pack::RECT rt;
	GetControlRect(&rt);
	AdjustLayout( rt.right - rt.left, rt.bottom - rt.top);

	return true;
}

static char 	s_szTemplateName[I3UI_CATEGORY_MAX][64] =
{
	"Button",			// I3UI_CATEGORY_BUTTON = 0,
	"Check Box",		// I3UI_CATEGORY_CHECKBOX,
	"Radio Button",		// i3UI_CATEGORY_RADIO,
	"Edit Box",			// I3UI_CATEGORY_EDITBOX,
	"List Box",			// I3UI_CATEGORY_LISTBOX,
	"Combo Box",		// I3UI_CATEGORY_COMBO,
	"Scroll",			// I3UI_CATEGORY_SCROLL,
	"ListView Box",		// I3UI_CATEGORY_LISTVIEWBOX,
	"ListView Item",	// I3UI_CATEGORY_LISTVIEWITEM,
	"ListView Cell",	// I3UI_CATEGORY_LISTVIEWCELL,
	"ListView Sep",		// I3UI_CATEGORY_LISTVIEWSEP,
	"ListView Header",	// I3UI_CATEGORY_LISTVIEWHEADER
	"Tree View",		// I3UI_CATEGORY_TREEVIEW,
	"Image",			// I3UI_CATEGORY_STATIC_IMAGE,
	"Slider",			// I3UI_CATEGORY_SLIDER,
	"Socket",			// I3UI_CATEGORY_SOCKET,
	"Progress",			// I3UI_CATEGORY_PROGRESS,
	"Progress Image",	// I3UI_CATEGORY_PROGRESS_IMAGE,
	"Progress Split",	// I3UI_CATEGORY_PROGRESS_SPLIT,
	"Frame Window",		// 
	"Spin",				// 
	"Tab",				// I3UI_CATEGORY_TAB,
	"Tool tip",			// I3UI_CATEGORY_TOOLTIP
	"Static Text",		// 
	"Horizental Bar",
	"CustomControl",
	"Double Text",
	"ButtonCombo Box",		// I3UI_CATEGORY_BUTTONCOMBO,
	"Combo Box (No EditBox)"	// I3UI_CATEGORY_COMBO_NOEDITBOX
};

const char * i3UITemplate::getTemplateCategoryName( I3UI_CATEGORY cate)
{
	if( (cate < 0) || (cate >= I3UI_CATEGORY_MAX))
		return "Unknown";

	return (const char *) s_szTemplateName[ cate];
}

i3::pack::RECT * i3UITemplate::_getTargetRect( INT32 idx, i3::pack::RECT * pRects)
{
	I3_BOUNDCHK( idx, getShapeCount());

	i3::pack::RECT * pSrcRect = (i3::pack::RECT *) getShape( idx)->getRect();
	if( pRects == nullptr)
		return pSrcRect;

	i3::pack::RECT * pDestRect = &pRects[idx];

	*pDestRect = *pSrcRect;

	return pDestRect;
}

bool i3UITemplate::GetTextArea( i3::pack::RECT* pIn, i3::pack::RECT* pOut)
{
	//Text를 출력하는 영역을 갖는 i3UITemplate은 이 함수를 재정의하여 true를 리턴한다..
	return false;
}

INT32 i3UITemplate::getShapeOffsetX(UINT32 shapeIdx)
{
	INT32 xOffset = 0;
	if(i3::same_of<i3UITemplate_Button*>(this))
	{
		i3UIShape * pShape = getShape(shapeIdx);
		xOffset += pShape->getTextOffsetX();
	}
	if(i3::same_of<i3UITemplate_ButtonComposed3*>(this))
	{
		i3UIShape * pShape1 = getShape(shapeIdx + 0);
		i3UIShape * pShape2 = getShape(shapeIdx + 1);
		i3UIShape * pShape3 = getShape(shapeIdx + 2);
		xOffset += pShape1->getTextOffsetX();
		xOffset += pShape2->getTextOffsetX();
		xOffset += pShape3->getTextOffsetX();
	}
	return xOffset;
}

INT32 i3UITemplate::getShapeOffsetY(UINT32 shapeIdx)
{
	INT32 yOffset = 0;
	if(i3::same_of<i3UITemplate_Button*>(this))
	{
		i3UIShape * pShape = getShape(shapeIdx);
		yOffset += pShape->getTextOffsetY();
	}
	if(i3::same_of<i3UITemplate_ButtonComposed3*>(this))
	{
		i3UIShape * pShape1 = getShape(shapeIdx + 0);
		i3UIShape * pShape2 = getShape(shapeIdx + 1);
		i3UIShape * pShape3 = getShape(shapeIdx + 2);
		yOffset += pShape1->getTextOffsetY();
		yOffset += pShape2->getTextOffsetY();
		yOffset += pShape3->getTextOffsetY();
	}
	return yOffset;
}