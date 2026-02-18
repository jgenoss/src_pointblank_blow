#include "i3GuiPCH.h"
#include "i3GuiControlTemplate.h"
#include "i3Base/itl/container_util.h"

I3_CLASS_INSTANCE(i3GuiControlTemplate);

i3GuiControlTemplate::~i3GuiControlTemplate(void)
{
	i3::cu::for_each_container(m_ControlDataList, i3::fu::mem_fun_safe(&i3GuiTemplateElement::Release));
}

void i3GuiControlTemplate::AddTemplate(i3GuiTemplateElement* pData, INT32 nShapeCount)
{
	INT32 i = 0;

	if(pData == nullptr)
	{
		pData = i3GuiTemplateElement::new_object();
	}

	for(i=0; i<nShapeCount; i++)
	{
		pData->AddShape();
	}

	m_ControlDataList.push_back(pData);
}

void i3GuiControlTemplate::RemoveTemplate(INT32 nIndex)
{
	i3GuiTemplateElement *pShape = m_ControlDataList[nIndex ];
	m_ControlDataList.erase(m_ControlDataList.begin() + nIndex);

	I3_SAFE_RELEASE(pShape);
}	

void i3GuiControlTemplate::RemoveAllTemplate()
{
	//m_ControlDataList.SafeReleaseAll();
	i3::cu::for_each_container(m_ControlDataList, i3::fu::mem_fun_safe(&i3GuiTemplateElement::Release));
	m_ControlDataList.clear();
}

bool i3GuiControlTemplate::AddShape(INT32 nIndex)
{
	i3GuiTemplateElement* pData = m_ControlDataList[nIndex];
	pData->AddShape();

	return true;
}

void i3GuiControlTemplate::RemoveShape(INT32 nIndex, INT32 nShape)
{
	i3GuiTemplateElement* pData = m_ControlDataList[nIndex];
	pData->RemoveShape(nShape);
}

i3GuiImage* i3GuiControlTemplate::GetTemplateImage(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape)
{
	i3GuiTemplateElement* pData = m_ControlDataList[nIndex];
	
	return pData->getImage(nShape);
}

void i3GuiControlTemplate::SetImagePos(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 x, INT32 y)
{
	i3GuiImage* pImage = GetTemplateImage(nIndex, nShape);
	pImage->m_nX = (REAL32) x;
	pImage->m_nY = (REAL32) y;
}

void i3GuiControlTemplate::SetImageUV(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 u, INT32 v)
{
	i3GuiImage* pImage = GetTemplateImage(nIndex, nShape);
	pImage->m_u = u;
	pImage->m_v = v;
}

void i3GuiControlTemplate::SetImageSize(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, INT32 w, INT32 h)
{
	i3GuiImage* pImage = GetTemplateImage(nIndex, nShape);
	pImage->m_w = w;
	pImage->m_h = h;
}

void i3GuiControlTemplate::SetImageTexture(INT32 nIndex, I3GUI_CONTROL_SHAPE nShape, i3Texture* pTex)
{
	i3GuiImage* pImage = GetTemplateImage(nIndex, nShape);
	pImage->SetTexture(pTex);
}

bool i3GuiControlTemplate::RemoveRedundantTemplate(void)
{
	
	for(size_t i = 0; i < m_ControlDataList.size(); ++i)
	{
		i3GuiTemplateElement * pElement = m_ControlDataList[i];

		for(size_t j = i; j < m_ControlDataList.size(); j++)
		{
			if( j != i)
			{
				i3GuiTemplateElement * pElm = m_ControlDataList[j];
				I3ASSERT( pElm != nullptr);

				if( (pElm->hasName() == false) || i3::generic_is_iequal( pElement->GetNameString(), pElm->GetNameString()) )
				{
					m_ControlDataList.erase(m_ControlDataList.begin() + j);
					j--;
				}
			}
		}
	}

	return true;
}

void i3GuiControlTemplate::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	
	const size_t	nTemplateCount = m_ControlDataList.size();

	for(size_t i = 0; i < nTemplateCount; ++i)
	{
		i3GuiTemplateElement * pElement = m_ControlDataList[i];

		pElement->OnBuildObjectList( List );
	}

	i3NamedElement::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct GUI_CONTROLTEMPLATE
	{
		INT32	nElementCount = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

bool i3GuiControlTemplate::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)	
		return false;

	INT32 i, iVal;
	char conv[MAX_PATH];
	INT32 nCount = (INT32)m_ControlDataList.size();

	pXML->addAttr( "Count", nCount);

	for( i = 0;i < nCount; ++i)
	{
		sprintf(conv, "data_%d", i);

		iVal = (INT32)pFile->GetObjectPersistID(m_ControlDataList[i]);

		pXML->addAttr( conv, iVal);
	}

	return true;
}

bool i3GuiControlTemplate::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 i, iVal;
	char conv[MAX_PATH];
	INT32 nCount;

	pXML->getAttr( "Count", &nCount);

	for( i = 0;i < nCount; ++i)
	{
		sprintf( conv, "data_%d", i);

		pXML->getAttr( conv, &iVal);

		if( iVal > 0)
		{
			i3GuiTemplateElement * pElm = (i3GuiTemplateElement*)pFile->FindObjectByID((OBJREF)iVal);
			I3_MUST_ADDREF( pElm);
			m_ControlDataList.push_back( pElm );
		}
	}

	return true;
}

UINT32 i3GuiControlTemplate::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3NamedElement::OnSave( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	pack::GUI_CONTROLTEMPLATE data;
	data.nElementCount = (INT32)m_ControlDataList.size();

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiControlTemplate::OnSave()", "Could not write ControlTemplate");
		return STREAM_ERR;
	}
	Result += Rc;

	for( size_t i = 0; i < m_ControlDataList.size(); ++i)
	{
		OBJREF ref = (OBJREF) pResFile->GetObjectPersistID( m_ControlDataList[i]);

		Rc = pStream->Write(&ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log("i3GuiControlTemplate::OnSave()","Could not write objref");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3GuiControlTemplate::OnLoad( i3ResourceFile * pResFile )
{
	UINT32 Rc, Result = 0;
	INT32	i;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI_CONTROLTEMPLATE data;

	Result = i3NamedElement::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read(&data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiControlTemplate::OnLoad()", "Could not read data");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0;i < data.nElementCount; ++i)
	{
		OBJREF ref;
		Rc = pStream->Read(&ref, sizeof( OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log("i3GuiControlTemplate::OnLoad()", "Could not read data");
			return STREAM_ERR;
		}
		Result += Rc;

		if( ref != 0)
		{
			i3GuiTemplateElement * pElement = (i3GuiTemplateElement*)pResFile->FindObjectByID( ref );
			I3_MUST_ADDREF( pElement);
			m_ControlDataList.push_back( pElement );
		}
	}

	return Result;
}