#include "stdafx.h"
#include "Tool_i3UIImage.h"
/*
I3_CONCRETE_CLASS_INSTANCE(FT_i3UIImage, i3UIImage);

///////////////////////////////////////////////////////////////////////////////////////////////////////
FT_i3UIImage::FT_i3UIImage()
{
	m_Path[0] = 0;
	i3Color::Set( &m_ColorKey, 0,0,0,0);
}

FT_i3UIImage::~FT_i3UIImage()
{
}

void FT_i3UIImage::GetTDKGridProperties(i3List * pList)
{
	// 먼저 Base Class의 GetTDKGridProperties를 호출
	i3UIImage::GetTDKGridProperties( pList);

	// BeginGroup은 EndGroup과 짝을 맞추어야 합니다.
	PROPERTYINFO* pProp = GetGridProp_BeginGroup( "Resource Information");
	pList->Add( pProp);

	pProp = GetGridProp_File( m_Path, MAX_PATH, "File");
	pList->Add( pProp);

	pProp = GetGridProp_Color( &m_ColorKey, "Color Key", "Color Key");
	pList->Add( pProp);

	// EndGroup
	pProp = GetGridProp_EndGroup( );
	pList->Add( pProp);
}

void FT_i3UIImage::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIImage::CopyTo( pDest, method);

	FT_i3UIImage * pImage = (FT_i3UIImage*)pDest;

	pImage->setResPath( m_Path);
	pImage->setColorKey( &m_ColorKey);
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

typedef struct ALIGN4 _tag_GuiImage
{
	UINT8		m_ID[4];
	UINT8		m_Path[MAX_PATH];
	COLOR		m_ColorKey;
	UINT32		pad[4];
}I3_PERSIST_TOOL_UI_IMAGE;

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 FT_i3UIImage::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_TOOL_UI_IMAGE data;

	Result = i3UIImage::OnSave( pResFile );
	I3_CHKIO( Result);

	memset( &data, 0, sizeof( data ));
	memcpy( data.m_ID, "TIM1", 4);

	i3String::NCopy( (char*)data.m_Path, m_Path, MAX_PATH-1);
	i3Color::Copy( &data.m_ColorKey, &m_ColorKey);

	Rc = pStream->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 FT_i3UIImage::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	I3_PERSIST_TOOL_UI_IMAGE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3UIImage::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data ));
	I3_CHKIO( Result);
	Result += Rc;

	setResPath( (char*)data.m_Path);
	setColorKey( &data.m_ColorKey);

	return Result;
}

bool FT_i3UIImage::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	if( bRv == false)
		return false;

	pXML->SetAttribute( "ID", "TIM1");
	pXML->SetAttribute( "path", m_Path);

	i3Color::SetToXML( pXML, "ColorKey", &m_ColorKey);

	return true;
}

bool FT_i3UIImage::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	const char * pszID = pXML->Attribute( "ID");
	if( i3String::NCompare( "TIM1", pszID, 4) != 0)
	{
		I3TRACE("invalid i3guiimage id\n");
		return false;
	}

	const char * pszPath = pXML->Attribute( "path");
	i3String::NCopy( m_Path, pszPath, MAX_PATH -1);

	i3Color::GetFromXML( pXML, "ColorKey", &m_ColorKey);

	return true;
}
*/