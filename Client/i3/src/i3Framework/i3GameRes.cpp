#include "i3FrameworkPCH.h"
#include "i3GameRes.h"
#include "i3GameObj.h"
#include "i3SceneUtil.h"
#include "i3Base/itl/vector_util.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3GameRes);

i3GameRes::i3GameRes(void)
{
	m_InstanceList.reserve( 32);
}

i3GameRes::~i3GameRes(void)
{
	Destroy();
	RemoveAllInstance();
}

void i3GameRes::Bind( i3Framework * pFramework, i3GameObjBase * pObj, bool bInstancing, bool bClone)
{
	AddInstance( pObj);
}

void i3GameRes::AddInstance( i3GameObjBase * pObj)
{
	m_InstanceList.push_back( pObj);
}

void i3GameRes::RemoveInstance( i3GameObjBase * pObj)
{
	size_t idx = i3::vu::index_of(m_InstanceList, pObj);							// m_InstanceList.IndexOf( pObj);
	if( idx < m_InstanceList.size() )
		m_InstanceList.erase(m_InstanceList.begin() + idx);
}

void i3GameRes::RemoveAllInstance(void)
{
	m_InstanceList.clear();
}


void i3GameRes::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	if( !i3::kind_of<i3GameRes* >(pDest))
		return;

	i3GameRes * pRes = (i3GameRes *) pDest;

	pRes->setResStyle( getResStyle());
	pRes->setType( getType());
}

UINT32 i3GameRes::SaveToFile( i3Stream * pStream)
{
	UINT32 Result, Rc;
	char	conv[MAX_PATH];

	i3::pack::RSC_HEADER_2 Header;
	
	i3::string_ncopy_nullpad( conv, GetNameString(), MAX_PATH -1);
	I3TRACE("%s\n", conv);

	Header.m_nType = getType();
	Header.m_nNameBufSize	= i3::generic_string_size( conv);
	Header.m_nFilePathSize	= i3::generic_string_size( conv);
	Header.m_nDataOffSet	= pStream->GetPosition();
	//Header.m_Style			= getResStyle();

	Result = pStream->Write( &Header, sizeof( Header));
	I3_CHKIO( Result);

	Rc = pStream->Write( conv, Header.m_nNameBufSize + 1);
	I3_CHKIO( Rc);
	Result += Rc;

	Rc = pStream->Write( conv, Header.m_nFilePathSize + 1);
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameRes::LoadFromFile( i3Stream * pStream, INT32 nVersion)
{	// 파일 로드는 따로 읽어서 분석합니다.
	// 여기서는 아무것도 읽지 않음..
	return 0;
}
UINT32 i3GameRes::LoadFromFile(i3IniParser * pParser, INT32 nVersion, INT32 nType)
{	// 파일 로드는 따로 읽어서 분석합니다.
	// 여기서는 아무것도 읽지 않음..
	return 0;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 GAMERES
	{
		UINT8	m_ID[4] = { 'R', 'E', 'S', '1' };
		UINT32	m_Style = 0;
		INT32	m_Type = 0;

		UINT32	pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32 i3GameRes::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES data;

	Result = i3ResourceObject::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_Style = getResStyle();
	data.m_Type = getType();

	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3GameRes::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::GAMERES data;

	Result = i3ResourceObject::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	setResStyle( static_cast<I3_RES_STYLE>(data.m_Style));
	setType( data.m_Type);

	return Result;
}

void i3GameRes::OnLostDevice( bool bLostDevice)
{
}

void i3GameRes::OnRevive( i3RenderContext * pCtx)
{
}
