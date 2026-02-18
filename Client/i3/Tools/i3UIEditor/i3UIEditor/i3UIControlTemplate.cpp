#include "stdafx.h"
#include "i3UIControlTemplate.h"

I3_CLASS_INSTANCE(i3UIControlTemplate, i3NamedElement);

i3UIControlTemplate::i3UIControlTemplate(void)
{
}

i3UIControlTemplate::~i3UIControlTemplate(void)
{
	m_ControlDataList.SafeReleaseAll();	
}

void i3UIControlTemplate::AddTemplate(i3UITemplate* pData)
{
	I3ASSERT( pData != NULL);

	m_ControlDataList.Add(pData);
}

void i3UIControlTemplate::RemoveTemplate(INT32 nIndex)
{
	i3UITemplate *pShape = (i3UITemplate*)m_ControlDataList.GetItem( nIndex );
	m_ControlDataList.Delete(nIndex);

	I3_SAFE_RELEASE(pShape);
}	

void i3UIControlTemplate::RemoveAllTemplate()
{
	m_ControlDataList.SafeReleaseAll();
}

i3UIImage* i3UIControlTemplate::GetTemplateShape(INT32 nIndex, INT32 nShape)
{
	i3UITemplate* pData =(i3UITemplate*)m_ControlDataList.GetItem(nIndex);
	
	return pData->getShape(nShape);
}

void i3UIControlTemplate::SetImagePos(INT32 nIndex, INT32 nShape, INT32 x, INT32 y)
{
	i3UIImage* pImage = GetTemplateShape(nIndex, nShape);

	pImage->m_nX = (REAL32) x;
	pImage->m_nX = (REAL32) y;
}

void i3UIControlTemplate::SetImageUV(INT32 nIndex, INT32 nShape, INT32 u, INT32 v)
{
	i3UIImage* pImage = GetTemplateShape(nIndex, nShape);
	pImage->m_u = u;
	pImage->m_v = v;
}

void i3UIControlTemplate::SetImageSize(INT32 nIndex, INT32 nShape, INT32 w, INT32 h)
{
	i3UIImage* pImage = GetTemplateShape(nIndex, nShape);
	pImage->m_w = w;
	pImage->m_h = h;
}

void i3UIControlTemplate::SetImageTexture(INT32 nIndex, INT32 nShape, i3Texture* pTex)
{
	i3UIImage* pImage = GetTemplateShape(nIndex, nShape);
	pImage->SetTexture(pTex);
}

void i3UIControlTemplate::OnBuildObjectList( i3List * pList )
{
	INT32	i = 0;
	INT32	nTemplateCount = m_ControlDataList.GetCount();

	for( i = 0; i < nTemplateCount; ++i)
	{
		i3UITemplate * pElement = (i3UITemplate*)m_ControlDataList.GetItem(i);

		pElement->OnBuildObjectList( pList );
	}

	i3NamedElement::OnBuildObjectList( pList );
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

typedef struct _tag_GuiControlTemplate
{
	INT32	nElementCount;
}I3_PERSIST_UI_CONTROLTEMPLATE;
#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

bool i3UIControlTemplate::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)	
		return false;

	INT32 i, iVal;
	char conv[MAX_PATH];
	INT32 nCount = m_ControlDataList.GetCount();

	pXML->addAttr( "Count", nCount);

	for( i = 0;i < nCount; ++i)
	{
		sprintf(conv, "data_%d", i);

		iVal = (INT32)pFile->GetObjectPersistID((i3PersistantElement*)m_ControlDataList.GetItem( i));

		pXML->addAttr( conv, iVal);
	}

	return true;
}

bool i3UIControlTemplate::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 iVal;
	char conv[MAX_PATH];
	INT32 nCount;

	pXML->getAttr( "Count", &nCount);

	for( INT32 i = 0;i < nCount; ++i)
	{
		sprintf( conv, "data_%d", i);

		pXML->getAttr( conv, &iVal);

		if( iVal != 0)
		{
			i3UITemplate * pElm = (i3UITemplate*)pFile->FindObjectByID((OBJREF)iVal);
			I3ASSERT( pElm != NULL);

			pElm->AddRef();
			m_ControlDataList.Add( pElm );
		}
	}

	return true;
}

UINT32 i3UIControlTemplate::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	INT32	i;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_UI_CONTROLTEMPLATE data;

	Result = i3NamedElement::OnSave( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	memset(&data, 0, sizeof(data));
	data.nElementCount = m_ControlDataList.GetCount();

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3UIControlTemplate::OnSave()", "Could not write ControlTemplate");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0; i < m_ControlDataList.GetCount(); ++i)
	{
		OBJREF ref = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_ControlDataList.GetItem(i));

		Rc = pStream->Write(&ref, sizeof(OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log("i3UIControlTemplate::OnSave()","Could not write objref");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3UIControlTemplate::OnLoad( i3ResourceFile * pResFile )
{
	UINT32 Rc, Result = 0;
	INT32	i;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_UI_CONTROLTEMPLATE data;

	Result = i3NamedElement::OnLoad( pResFile );
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read(&data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3UIControlTemplate::OnLoad()", "Could not read data");
		return STREAM_ERR;
	}
	Result += Rc;

	for( i = 0;i < data.nElementCount; ++i)
	{
		OBJREF ref;
		Rc = pStream->Read(&ref, sizeof( OBJREF));
		if( Rc == STREAM_ERR)
		{
			i3Log("i3UIControlTemplate::OnLoad()", "Could not read data");
			return STREAM_ERR;
		}
		Result += Rc;

		if( ref != 0)
		{
			i3UITemplate * pElement = (i3UITemplate*)pResFile->FindObjectByID( ref );
			I3ASSERT( pElement != NULL );

			{
				pElement->AddRef();
				m_ControlDataList.Add( pElement );
			}
		}
	}

	return Result;
}