#include "i3GuiPCH.h"
#include "i3GuiBase.h"
#include "i3Base/itl/container_util.h"

#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE(i3GuiControlData);
I3_CLASS_INSTANCE(i3GuiTemplateElement);
I3_CLASS_INSTANCE(i3GuiImage);
I3_CLASS_INSTANCE(I3GUI_EVENT_DATA);


bool I3GUI_EVENT_MOVE_DATA::SaveToXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement *  pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "FactorType", m_nFactorType);
	pChild->addAttr( "CountTime", m_fCountTime);
	pChild->addAttr( "DestPosX", m_fDestPosX);
	pChild->addAttr( "DestPosY", m_fDestPosY);
	pChild->addAttr( "SpeedX", m_fSpeedX);
	pChild->addAttr( "SpeedY", m_fSpeedY);
	
	return true;
}

bool I3GUI_EVENT_ROTATE_DATA::SaveToXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement *  pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*) pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "FactorType",	m_nFactorType);
	pChild->addAttr( "CountTime",		m_fCountTime);
	pChild->addAttr( "Speed",			m_fSpeed);
	pChild->addAttr( "LoopRotate",	m_bLoopRotate);
	pChild->addAttr( "IsSpin",		m_bSpin);
	pChild->addAttr( "SpinDirection", m_nSpinDirection);
	
	return true;
}

bool I3GUI_EVENT_SCALE_DATA::SaveToXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement *  pChild = new i3XMLElement( pszName);
	pChild = (i3XMLElement*)pXML->LinkEndChild( pChild);
	I3ASSERT( pChild != nullptr);

	pChild->addAttr( "FactorType",	m_nFactorType);
	pChild->addAttr( "CountTime",		m_fCountTime);
	pChild->addAttr( "ScaleX",		m_fDestScaleX);
	pChild->addAttr( "ScaleY",		m_fDestScaleY);
	pChild->addAttr( "OldScaleX",		m_fOldScaleX);
	pChild->addAttr( "OldScaleY",		m_fOldScaleY);
	pChild->addAttr( "SpeedScaleX",	m_fSpeedScaleX);
	pChild->addAttr( "SpeedScaleY",	m_fSpeedScaleY);
	pChild->addAttr( "LoopScale",		m_bLoopScale);
	
	return true;
}

bool I3GUI_EVENT_MOVE_DATA::LoadFromXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	I3ASSERT( pChild != nullptr);

	pChild->getAttr( "FactorType", &m_nFactorType);
	pChild->getAttr( "CountTime", &m_fCountTime);
	pChild->getAttr( "DestPosX", &m_fDestPosX);
	pChild->getAttr( "DestPosY", &m_fDestPosY);
	pChild->getAttr( "SpeedX", &m_fSpeedX);
	pChild->getAttr( "SpeedY", &m_fSpeedY);
	
	return true;
}

bool I3GUI_EVENT_ROTATE_DATA::LoadFromXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	I3ASSERT( pChild != nullptr);

	pChild->getAttr( "FactorType",	&m_nFactorType);
	pChild->getAttr( "CountTime",	&m_fCountTime);
	pChild->getAttr( "Speed",		&m_fSpeed);
	pChild->getAttr( "LoopRotate",	&m_bLoopRotate);
	pChild->getAttr( "IsSpin",		&m_bSpin);
	pChild->getAttr( "SpinDirection", &m_nSpinDirection);
	
	return true;
}

bool I3GUI_EVENT_SCALE_DATA::LoadFromXML( i3XMLFile * pFile, const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *) pXML->FirstChild( pszName);
	I3ASSERT( pChild != nullptr);

	pChild->getAttr( "FactorType",	&m_nFactorType);
	pChild->getAttr( "CountTime",	&m_fCountTime);
	pChild->getAttr( "ScaleX",		&m_fDestScaleX);
	pChild->getAttr( "ScaleY",		&m_fDestScaleY);
	pChild->getAttr( "OldScaleX",	&m_fOldScaleX);
	pChild->getAttr( "OldScaleY",	&m_fOldScaleY);
	pChild->getAttr( "SpeedScaleX",	&m_fSpeedScaleX);
	pChild->getAttr( "SpeedScaleY",	&m_fSpeedScaleY);
	pChild->getAttr( "LoopScale",	&m_bLoopScale);
	
	return true;
}

I3GUI_EVENT_DATA::~I3GUI_EVENT_DATA()
{
	I3MEM_SAFE_FREE( m_pMoveData);
	I3MEM_SAFE_FREE( m_pRotateData);
	I3MEM_SAFE_FREE( m_pScaleData);

	I3_SAFE_RELEASE( m_pSoundNode);
}

void I3GUI_EVENT_DATA::Init()
{
	m_nEventStyle	= 0;
	m_pMoveData		= nullptr;
	m_pRotateData	= nullptr;
	m_pScaleData	= nullptr;
	m_pSoundNode	= nullptr;
}

void I3GUI_EVENT_DATA::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	if( !i3::kind_of< I3GUI_EVENT_DATA* >(pDest))
	{
		return;
	}

	I3GUI_EVENT_DATA * pEvent = (I3GUI_EVENT_DATA*)pDest;

	pEvent->m_nEventStyle = m_nEventStyle;

	if( m_pSoundNode != nullptr)
	{
		I3_SAFE_RELEASE( pEvent->m_pSoundNode);

		pEvent->m_pSoundNode	= i3SoundNode::new_object();

		m_pSoundNode->CopyTo( pEvent->m_pSoundNode, I3_COPY_INSTANCE);
	}

	if( m_nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3ASSERT( m_pMoveData != nullptr);

		I3MEM_SAFE_FREE( pEvent->m_pMoveData);

		pEvent->m_pMoveData = (I3GUI_EVENT_MOVE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_MOVE_DATA));

		i3mem::Copy( pEvent->m_pMoveData, m_pMoveData, sizeof( I3GUI_EVENT_MOVE_DATA) );
	}

	if( m_nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3ASSERT( m_pRotateData != nullptr);

		I3MEM_SAFE_FREE( pEvent->m_pRotateData);

		pEvent->m_pRotateData = (I3GUI_EVENT_ROTATE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_ROTATE_DATA));

		i3mem::Copy( pEvent->m_pRotateData, m_pRotateData, sizeof( I3GUI_EVENT_ROTATE_DATA));
	}

	if( m_nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3ASSERT( m_pScaleData != nullptr);

		I3MEM_SAFE_FREE( pEvent->m_pScaleData);

		pEvent->m_pScaleData = (I3GUI_EVENT_SCALE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_SCALE_DATA));

		i3mem::Copy( pEvent->m_pScaleData, m_pScaleData, sizeof( I3GUI_EVENT_SCALE_DATA));
	}
}

#if defined (I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 EVENT_DATA
	{
		UINT32		nStyle = 0;		//EventStyle
		OBJREF64	pSound = 0;
		UINT32		pad[3] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif
void	I3GUI_EVENT_DATA::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pSoundNode != nullptr)
	{
		m_pSoundNode->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

bool I3GUI_EVENT_DATA::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3PersistantElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->addAttr("EventStyle", m_nEventStyle);

	if( m_nEventStyle & I3GUI_EVENT_SOUND)
	{
		INT32 iVal = (INT32)pFile->GetObjectPersistID( m_pSoundNode);

		pXML->addAttr("SoundNode", iVal);
	}

	if( m_nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3ASSERT( m_pMoveData != nullptr);

		m_pMoveData->SaveToXML( pFile, "MoveData", pXML);
	}

	if( m_nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3ASSERT( m_pRotateData != nullptr);

		m_pRotateData->SaveToXML( pFile, "RotateData", pXML);
	}

	if( m_nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3ASSERT( m_pScaleData != nullptr);

		m_pScaleData->SaveToXML( pFile, "ScaleData", pXML);
	}

	return true;
}

bool I3GUI_EVENT_DATA::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3PersistantElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 iVal;

	pXML->getAttr("EventStyle", &m_nEventStyle);

	if( m_nEventStyle & I3GUI_EVENT_SOUND)
	{
		pXML->getAttr("SoundNode", &iVal);

		if( iVal > 0)
		{
			i3SoundNode * pNode = (i3SoundNode*)pFile->FindObjectByID((OBJREF)iVal);
			I3ASSERT( pNode != nullptr);

			I3_REF_CHANGE( m_pSoundNode, pNode);
		}
	}

	if( m_nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3ASSERT( m_pMoveData == nullptr);

		m_pMoveData = (I3GUI_EVENT_MOVE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_MOVE_DATA));
		m_pMoveData->LoadFromXML( pFile, "MoveData", pXML);
	}

	if( m_nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3ASSERT( m_pRotateData == nullptr);

		m_pRotateData = (I3GUI_EVENT_ROTATE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_ROTATE_DATA));
		m_pRotateData->LoadFromXML( pFile, "RotateData", pXML);
	}

	if( m_nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3ASSERT( m_pScaleData == nullptr);

		m_pScaleData = (I3GUI_EVENT_SCALE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_SCALE_DATA));
		m_pScaleData->LoadFromXML( pFile, "ScaleData", pXML);
	}

	return true;
}

UINT32	I3GUI_EVENT_DATA::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;
	pack::EVENT_DATA	data;
	i3Stream	* pStream = pResFile->GetStream();

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	//i3mem::FillZero( &data, sizeof( pack::EVENT_DATA));

	data.nStyle = m_nEventStyle;
	if( m_pSoundNode != nullptr)
	{
		data.pSound	= (OBJREF64)pResFile->GetObjectPersistID( m_pSoundNode);
	}

	Rc = pStream->Write( &data, sizeof( pack::EVENT_DATA));
	I3_CHKIO( Rc);
	Result += Rc;

	if( m_nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3ASSERT( m_pMoveData != nullptr);

		Rc = pStream->Write( m_pMoveData, sizeof( I3GUI_EVENT_MOVE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( m_nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3ASSERT( m_pRotateData != nullptr);

		Rc = pStream->Write( m_pRotateData, sizeof( I3GUI_EVENT_ROTATE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( m_nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3ASSERT( m_pScaleData != nullptr);

		Rc = pStream->Write( m_pScaleData, sizeof( I3GUI_EVENT_SCALE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32	I3GUI_EVENT_DATA::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;
	pack::EVENT_DATA data;
	i3Stream	* pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data ));
	I3_CHKIO( Result);
	Result += Rc;

	m_nEventStyle = data.nStyle;
	if( data.pSound != 0)
	{
		m_pSoundNode = (i3SoundNode*)pResFile->FindObjectByID((OBJREF)data.pSound);
		I3_MUST_ADDREF( m_pSoundNode);
	}

	if( m_nEventStyle & I3GUI_EVENT_MOVE)
	{
		I3ASSERT( m_pMoveData == nullptr);
		m_pMoveData = (I3GUI_EVENT_MOVE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_MOVE_DATA));

		Rc = pStream->Read( m_pMoveData, sizeof( I3GUI_EVENT_MOVE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( m_nEventStyle & I3GUI_EVENT_ROTATE)
	{
		I3ASSERT( m_pRotateData == nullptr);
		m_pRotateData = (I3GUI_EVENT_ROTATE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_ROTATE_DATA));

		Rc = pStream->Read( m_pRotateData, sizeof( I3GUI_EVENT_ROTATE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( m_nEventStyle & I3GUI_EVENT_SCALE)
	{
		I3ASSERT( m_pScaleData == nullptr);
		m_pScaleData = (I3GUI_EVENT_SCALE_DATA*)i3MemAlloc( sizeof( I3GUI_EVENT_SCALE_DATA));

		Rc = pStream->Read( m_pScaleData, sizeof( I3GUI_EVENT_SCALE_DATA));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
i3GuiImage::~i3GuiImage()
{
	I3_SAFE_RELEASE( m_pTexture );
}

void i3GuiImage::Reset(void)
{
	m_nX = m_nOriginalX;
	m_nY = m_nOriginalY;
	m_nWidth = m_nOriginalWidth;
	m_nHeight = m_nOriginalHeight;
}

void i3GuiImage::SetTexture( i3Texture *pTex )
{
	I3_REF_CHANGE( m_pTexture, pTex);
}

void i3GuiImage::SetTexture( i3Texture * pTex, INT32 u, INT32 v, INT32 w, INT32 h)
{
	SetTexture(pTex);

	m_u = u;
	m_v = v;
	m_w = w;
	m_h = h;
}

void i3GuiImage::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	//Texture는 TextureList에서 저장하므로 OnSave()리스트에 넣지 않습니다.
	if( m_pTexture != nullptr)
	{
		m_pTexture->OnBuildObjectList( List );
	}

	i3NamedElement::OnBuildObjectList( List );
}

void i3GuiImage::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3GuiImage * pImage = (i3GuiImage*)pDest;

	pImage->SetTexture( m_pTexture );
	pImage->m_nX	= m_nX;
	pImage->m_nY	= m_nY;
	pImage->m_nOriginalX = m_nX;
	pImage->m_nOriginalY = m_nY;
	pImage->m_u		= m_u;
	pImage->m_v		= m_v;
	pImage->m_w		= m_w;
	pImage->m_h		= m_h;
	pImage->m_nOriginalWidth = m_nWidth;
	pImage->m_nOriginalHeight= m_nHeight;
	pImage->m_nWidth = m_nWidth;
	pImage->m_nHeight= m_nHeight;
	pImage->m_bFlipX = m_bFlipX;
	pImage->m_bFlipY = m_bFlipY;
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUI_IMAGE
	{
		UINT8		m_ID[4] = { 'G', 'I', 'M', '1' };
		OBJREF64	m_pTexture = 0;
		INT32		m_nX = 0, m_nY = 0;
		INT32		m_u = 0, m_v = 0, m_w = 0, m_h = 0;
		INT32		m_nWidth = 0, m_nHeight = 0;
		INT32		m_bFlipX = 0, m_bFlipY = 0;
		INT8		Reserved[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiImage::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI_IMAGE data;

	Result = i3NamedElement::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_pTexture	= (OBJREF64) pResFile->GetObjectPersistID( m_pTexture);
	data.m_nX		= (INT32) m_nX;
	data.m_nY		= (INT32)m_nY;
	data.m_u		= m_u;
	data.m_v		= m_v;
	data.m_w		= m_w;
	data.m_h		= m_h;
	data.m_nWidth	= (INT32)m_nWidth;
	data.m_nHeight	= (INT32)m_nHeight;
	data.m_bFlipX	= (INT32)m_bFlipX;
	data.m_bFlipY	= (INT32)m_bFlipY;

	Rc = pStream->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3GuiImage::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	pack::GUI_IMAGE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data ));
	I3_CHKIO( Result);
	Result += Rc;

	//Texture
	if( data.m_pTexture != 0)
	{
		i3Texture * pTexture = (i3Texture*)pResFile->FindObjectByID( (OBJREF)data.m_pTexture);
		I3ASSERT( pTexture != nullptr);
			
		SetTexture( pTexture);
	}

	m_nX		= (REAL32) data.m_nX;
	m_nY		= (REAL32) data.m_nY;
	m_nOriginalX= (REAL32) data.m_nX;
	m_nOriginalY= (REAL32) data.m_nY;
	m_u			= data.m_u;
	m_v			= data.m_v;
	m_w			= data.m_w;
	m_h			= data.m_h;
	m_nWidth	= (REAL32) (data.m_nWidth);
	m_nHeight	= (REAL32) (data.m_nHeight);
	m_nOriginalWidth= (REAL32) (data.m_nWidth);
	m_nOriginalHeight= (REAL32) (data.m_nHeight);
	m_bFlipX	= data.m_bFlipX != 0;
	m_bFlipY	= data.m_bFlipY != 0;

	return Result;
}

bool i3GuiImage::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->SetAttribute( "ID", "GIM1");

	INT32 iTexture = pFile->GetObjectPersistID( m_pTexture);
	pXML->addAttr( "Texture", iTexture);

	pXML->addAttr( "nX", m_nX);
	pXML->addAttr( "nY", m_nY);
	pXML->addAttr( "nWidth", m_nWidth);
	pXML->addAttr( "nHeight", m_nHeight);

	pXML->addAttr( "u", m_u);
	pXML->addAttr( "v", m_v);
	pXML->addAttr( "w", m_w);
	pXML->addAttr( "h", m_h);

	pXML->addAttr( "FlipX", m_bFlipX);
	pXML->addAttr( "FlipY", m_bFlipY);

	return true;
}

bool i3GuiImage::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	const char * pszID = pXML->Attribute( "ID");
	if( i3::generic_is_niequal( "GIM1", pszID, 4) == false)
	{
		I3TRACE("invalid i3guiimage id\n");
		return false;
	}

	INT32 iTexture;
	pXML->getAttr( "Texture", &iTexture);
	if( iTexture > 0)
	{
		i3Texture * pTex = (i3Texture*)pFile->FindObjectByID((OBJREF)iTexture);
		I3ASSERT( pTex != nullptr);

		SetTexture( pTex);
	}

	pXML->getAttr( "nX", &m_nX);
	pXML->getAttr( "nY", &m_nY);
	pXML->getAttr( "nWidth", &m_nWidth);
	pXML->getAttr( "nHeight", &m_nHeight);

	pXML->getAttr( "u", &m_u);
	pXML->getAttr( "v", &m_v);
	pXML->getAttr( "w", &m_w);
	pXML->getAttr( "h", &m_h);

	pXML->getAttr( "FlipX", &m_bFlipX);
	pXML->getAttr( "FlipY", &m_bFlipY);

	m_nOriginalX		= m_nX;
	m_nOriginalY		= m_nY;
	m_nOriginalWidth	= m_nWidth;
	m_nOriginalHeight	= m_nHeight;

	return true;
}
////////////////////
///////////////////
///////////////////
i3GuiTemplateElement::~i3GuiTemplateElement()
{
	
//	m_ImageList.SafeReleaseAll();
	i3::cu::for_each_container(m_ImageList, i3::fu::mem_fun_safe(&i3GuiImage::Release));

}

void i3GuiTemplateElement::AddShape()
{
	i3GuiImage* pImage = i3GuiImage::new_object();

	m_ImageList.push_back(pImage);
}

void i3GuiTemplateElement::AddShape(i3GuiImage * pImage)
{
	if( pImage != nullptr )
	{
		I3_MUST_ADDREF( pImage);
		m_ImageList.push_back( pImage );
	}
}

void i3GuiTemplateElement::RemoveShape(INT32 nIndex)
{
	i3GuiImage* pImage = m_ImageList[nIndex];
//	m_ImageList.Remove(pImage);
	i3::vu::remove(m_ImageList, pImage);
 
	I3_SAFE_RELEASE(pImage);
}

void i3GuiTemplateElement::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3GuiTemplateElement * pElement = (i3GuiTemplateElement*)pDest;

	pElement->setFixedCountShapeType( m_bFixedCountShapeType);

	INT32	i;
	for( i = 0; i< getShapeCount(); ++i)
	{
		i3GuiImage* pSrcImage = getImage(i);
		if( pSrcImage != nullptr )
		{
			pElement->AddShape();
			i3GuiImage* pImage = pElement->getImage(i);
		
			pSrcImage->CopyTo( pImage, method);
		}
	}
}


void i3GuiTemplateElement::OnBuildObjectList(i3::vector<i3PersistantElement*>& List )
{
	INT32 nCount = getShapeCount();

	if( nCount > 0 )
	{
		for( INT32	i= 0; i < nCount; ++i)
		{
			i3GuiImage * pImage = getImage(i);
			if( pImage != nullptr )
				pImage->OnBuildObjectList( List );
		}
	}

	i3NamedElement::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUI_TEMPLATEELEMENT
	{
		INT32			m_nShapeCount = 0;
	};

	struct ALIGN4 GUI_TEMPLATEELEMENT2
	{
		UINT8			nID[4] = { 'G', 'T', 'E', '1' };
		UINT32			nVersion = 1;

		INT32			nShapeCount = 0;
		INT32			bFixedCountShapeType = 0;
		INT32			pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiTemplateElement::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI_TEMPLATEELEMENT2 data;

	Result = i3NamedElement::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.nShapeCount	= getShapeCount();
	data.bFixedCountShapeType = ( m_bFixedCountShapeType == true) ? 1 : 0;

	{
		Rc = pStream->Write( &data, sizeof( data ));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	for( INT32	i = 0; i < getShapeCount(); ++i)
	{
		OBJREF pRef = (OBJREF) pResFile->GetObjectPersistID( getImage(i));
		Rc = pStream->Write( &pRef, sizeof(OBJREF));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3GuiTemplateElement::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile );
	I3_CHKIO( Result);

	UINT32 nCurPos = pStream->GetPosition();

	char conv[4];
	Rc = pResFile->Read( conv, 4);
	I3_CHKIO( Rc);
	
	Rc = pStream->SetPosition( nCurPos, STREAM_BEGIN);
	I3_CHKIO( Rc);

	INT32 nShapeCount = 0;
	if( i3::generic_is_niequal( conv, "GTE1", 4) == false)
	{
		pack::GUI_TEMPLATEELEMENT data;

		Rc = pStream->Read( &data, sizeof( data));
		I3_CHKIO( Rc);
		Result += Rc;

		nShapeCount = data.m_nShapeCount;
	}
	else
	{
		pack::GUI_TEMPLATEELEMENT2 data;

		Rc = pStream->Read( &data, sizeof( data));
		I3_CHKIO( Rc);
		Result += Rc;

		if( data.nVersion == 1)
		{
			nShapeCount				= data.nShapeCount;
			m_bFixedCountShapeType	= data.bFixedCountShapeType != 0 ? true : false;
		}
	}

    for( INT32	i = 0; i < nShapeCount; ++i)
	{
		OBJREF ref;

		Rc = pStream->Read( &ref, sizeof( OBJREF));
		I3_CHKIO( Rc);
		Result += Rc;

		if( ref != 0 )
		{
			i3GuiImage * pImage = (i3GuiImage*)pResFile->FindObjectByID( ref );
			if( pImage != nullptr )
			{
				AddShape(pImage);
			}
		}
	}

	return Result;
}

bool i3GuiTemplateElement::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 nVer = 1, iVal = 0;
	INT32 nShapeCount = getShapeCount();

	pXML->SetAttribute( "ID", "GTE1");

	pXML->addAttr("Version", nVer);
	pXML->addAttr("ShapeCount", nShapeCount);

	INT32 isFixed = ( m_bFixedCountShapeType == true) ? 1 : 0;
	pXML->addAttr("isFixed", isFixed);

	char conv[MAX_PATH];
	for( INT32	i = 0; i < nShapeCount; ++i)
	{
		iVal = 0;

		sprintf( conv, "Image_%d", i);
		iVal = (INT32)pFile->GetObjectPersistID( getImage(i));

		pXML->addAttr( conv, iVal);
	}

	return true;
}

bool i3GuiTemplateElement::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	const char *pszID = pXML->Attribute("ID");
	if( i3::generic_is_niequal( pszID, "GTE1", 4) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "invalid i3GuiTemplate ID");
		return false;
	}

	INT32 nVer = 1, iVal = 0;
	INT32 nShapeCount = getShapeCount();
	INT32 isFixed;

	pXML->getAttr( "Version", &nVer);
	pXML->getAttr( "ShapeCount", &nShapeCount);
	pXML->getAttr( "isFixed", &isFixed);

	char conv[MAX_PATH];
	for( INT32	i = 0; i < nShapeCount; ++i)
	{
		sprintf( conv, "Image_%d", i);

		pXML->getAttr( conv, &iVal);
		if( iVal > 0)
		{
			i3GuiImage * pImage = (i3GuiImage *)pFile->FindObjectByID( (OBJREF)iVal);
			I3ASSERT( pImage != nullptr);

			AddShape( pImage);
		}
	}

	return true;
}
////////////////////
///////////////////
///////////////////

i3GuiControlData::i3GuiControlData()
{
	Init();
}

i3GuiControlData::~i3GuiControlData()
{
	I3_SAFE_RELEASE( m_pShape );
}


void i3GuiControlData::Init()
{
	m_pShape = nullptr;

	m_nAlignX = 0;
	m_nAlignY = 0;
	m_fRotate = 0.0f;
	m_fRotateCenterX = m_fRotateCenterY = 0.0f;
	m_fScaleX = m_fScaleY = 1.0f;
	m_bFlipX = m_bFlipY = false;

	INT32 i;
	for( i = 0; i < I3_GUI_SHAPE_COUNT; i++ )
	{
		i3Color::Set( &m_Color[i], (UINT8) 255, 255, 255, 255 );
	}

	m_bEnable = true;	// Enable
}

void i3GuiControlData::SetShape( i3GuiTemplateElement * pShape)
{
	if( m_pShape != pShape)
	{
		I3_REF_CHANGE( m_pShape, pShape);
	}
}


void i3GuiControlData::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3GuiControlData * pData = (i3GuiControlData*)pDest;

	if( m_pShape != nullptr )
	{
		i3GuiTemplateElement * pElement = i3GuiTemplateElement::new_object_ref();
		m_pShape->CopyTo( pElement, method);
		pData->SetShape( pElement );
	}

	memcpy( pData->m_Color, m_Color, sizeof(I3COLOR) * I3_GUI_SHAPE_COUNT);
	pData->m_bEnable		= m_bEnable;
	pData->m_nAlignX		= m_nAlignX;
	pData->m_nAlignY		= m_nAlignY;
	pData->m_fScaleX		= m_fScaleX;
	pData->m_fScaleY		= m_fScaleY;
	pData->m_fRotate		= m_fRotate;
	pData->m_fRotateCenterX = m_fRotateCenterX;
	pData->m_fRotateCenterY = m_fRotateCenterY;
	pData->m_bFlipX			= m_bFlipX;
	pData->m_bFlipY			= m_bFlipY;
}

void i3GuiControlData::OnBuildObjectList( i3::vector<i3PersistantElement*>& List )
{
	//i3GuiControlTemplate이 멤버로 가지고있는 리스트에서 Shape에대한 내용을 저장합니다. 따라서 여기서는 빠집니다.
	if( m_pShape )
		m_pShape->OnBuildObjectList( List );

	i3NamedElement::OnBuildObjectList( List );
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

namespace pack
{
	struct ALIGN4 GUI_CONTROLDATA
	{
		OBJREF		m_pShape = 0;
		I3COLOR		m_Color[I3_GUI_SHAPE_COUNT];
		INT32		m_bEnable = 0;
		INT32		m_nAlignX = 0, m_nAlignY = 0;
		REAL32		m_fScaleX = 0.0f, m_fScaleY = 0.0f;
		REAL32		m_fRotate = 0.0f;
		REAL32		m_fRotateCenterX = 0.0f, m_fRotateCenterY = 0.0f;
		INT32		m_bFlipX = 0, m_bFlipY = 0;	// U,v 좌표를 뒤집음.
		INT8		Reserved[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3GuiControlData::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3NamedElement::OnSave( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	pack::GUI_CONTROLDATA data;

	data.m_pShape = (OBJREF) pResFile->GetObjectPersistID( (i3PersistantElement *) m_pShape);
	memcpy( data.m_Color, m_Color, sizeof(I3COLOR) * I3_GUI_SHAPE_COUNT);
	data.m_bEnable	= (INT32)m_bEnable;
	data.m_nAlignX	= m_nAlignX;
	data.m_nAlignY	= m_nAlignY;
	data.m_fScaleX	= m_fScaleX;
	data.m_fScaleY	= m_fScaleY;
	data.m_fRotate	= m_fRotate;
	data.m_fRotateCenterX = m_fRotateCenterX;
	data.m_fRotateCenterY = m_fRotateCenterY;
	data.m_bFlipX	= (INT32)m_bFlipX;
	data.m_bFlipY	= (INT32)m_bFlipY;

	Rc = pStream->Write( &data, sizeof( data ));
	if( Rc == STREAM_ERR)
	{
		i3Log("i3GuiControlData::OnSave()", "Could not save file");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3GuiControlData::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GUI_CONTROLDATA	data;

	Result = i3NamedElement::OnLoad( pResFile );
	if( Result == STREAM_ERR )
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data ));
	if( Rc == STREAM_ERR )
	{
		i3Log("i3GuiControlData::OnLoad", "Could Not Read file");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_pShape != 0)
	{
		i3GuiTemplateElement * pShape = (i3GuiTemplateElement*)pResFile->FindObjectByID( data.m_pShape);
		if( pShape != nullptr )
		{
			SetShape( pShape );
		}
	}

	memcpy( m_Color, data.m_Color, sizeof(I3COLOR) * I3_GUI_SHAPE_COUNT);

	m_bEnable		= 	data.m_bEnable != 0;	
	m_nAlignX		= 	data.m_nAlignX;
	m_nAlignY		= 	data.m_nAlignY;
	m_fScaleX		= 	data.m_fScaleX;
	m_fScaleY		= 	data.m_fScaleY;
	m_fRotate		= 	data.m_fRotate;
	m_fRotateCenterX= 	data.m_fRotateCenterX;
	m_fRotateCenterY= 	data.m_fRotateCenterY;
	m_bFlipX		= 	data.m_bFlipX != 0;
	m_bFlipY		= 	data.m_bFlipY != 0;

	return Result;
}

bool i3GuiControlData::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	INT32 nShape = (INT32)pFile->GetObjectPersistID( (i3PersistantElement*)m_pShape);

	pXML->addAttr( "Shape", nShape);
	pXML->addAttr( "Enable", m_bEnable);
	pXML->addAttr( "AlignX", m_nAlignX);
	pXML->addAttr( "nAlignY",m_nAlignY);
	pXML->addAttr( "ScaleX",m_fScaleX);
	pXML->addAttr( "ScaleY",m_fScaleY);
	pXML->addAttr( "Rotate",m_fRotate);
	pXML->addAttr( "RotateCenterX", m_fRotateCenterX);
	pXML->addAttr( "RotateCenterY", m_fRotateCenterY);
	pXML->addAttr( "FlipX", m_bFlipX);
	pXML->addAttr( "FlipY", m_bFlipY);

	char conv[MAX_PATH];
	for( INT32 i = 0; i<I3_GUI_SHAPE_COUNT; ++i)
	{
		sprintf( conv, "Color_%d", i);
		i3Color::SetToXML( pXML, conv, &m_Color[i]);
	}

	return true;
}

bool i3GuiControlData::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

    INT32 nShape;
	pXML->getAttr( "Shape", &nShape);
	if( nShape > 0)
	{
		i3GuiTemplateElement * pShape = (i3GuiTemplateElement*)pFile->FindObjectByID((OBJREF)nShape);
		I3ASSERT( pShape != nullptr);

		SetShape( pShape);
	}

	pXML->getAttr( "Enable", &m_bEnable);
	pXML->getAttr( "AlignX", &m_nAlignX);
	pXML->getAttr( "nAlignY",&m_nAlignY);
	pXML->getAttr( "ScaleX",&m_fScaleX);
	pXML->getAttr( "ScaleY",&m_fScaleY);
	pXML->getAttr( "Rotate",&m_fRotate);
	pXML->getAttr( "RotateCenterX", &m_fRotateCenterX);
	pXML->getAttr( "RotateCenterY", &m_fRotateCenterY);
	pXML->getAttr( "FlipX", &m_bFlipX);
	pXML->getAttr( "FlipY", &m_bFlipY);

	char conv[MAX_PATH];
	for( INT32 i = 0; i<I3_GUI_SHAPE_COUNT; ++i)
	{
		sprintf( conv, "Color_%d", i);

		i3Color::GetFromXML( pXML, conv, &m_Color[i]);
	}

	return true;
}

I3_EXPORT_GUI
const char * i3Gui::GetDefaultFontName(void)
{
	UINT32 nCodePage = i3Language::GetCurrentCodePage();

	switch( nCodePage)
	{
	case I3_LANG_CODE_KOREAN:		return "맑은 고딕";	break;
	case I3_LANG_CODE_THAI:			return "Tahoma";	break;
	default:						return i3Language::GetDefaultFontName( nCodePage);	break;
	}
}

