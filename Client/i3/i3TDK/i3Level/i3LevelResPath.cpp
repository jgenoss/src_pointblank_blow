#include "stdafx.h"
#include "i3LevelResPath.h"
#include "i3LevelGlobalVariable.h"

//I3_CLASS_INSTANCE( i3LevelResPath, i3LevelRes);
I3_CLASS_INSTANCE( i3LevelResPath);

i3LevelResPath::i3LevelResPath()
{
	m_ResType		= I3LV_TYPE_PATH;
}

i3LevelResPath::~i3LevelResPath()
{
	I3_SAFE_RELEASE( m_pPath);
}

void i3LevelResPath::Create(void)
{
	m_pPath = i3GamePath::new_object();

	m_pPath->SetExternRes( TRUE);
}

bool i3LevelResPath::OnValidateBrief( bool bForce)
{
	if( bForce == false)
	{
		if( isBriefLoaded())
			return true;
	}

	char szFull[ MAX_PATH];

	getFullPath( szFull);

	i3GamePath * pPath = i3GamePath::LoadFromFile( szFull);

	if( pPath == nullptr)
	{
		return false;
	}

//	pPath->AddRef();	// LoadFromFile()의 레퍼런스카운팅이 생겼으므로 이제 이건 지운다..(2012.11.02.수빈)
	
	m_PointCount = pPath->getPointCount();
	m_LineCount = pPath->getLinkCount();

	I3_MUST_RELEASE(pPath);

	addResState( I3LV_RES_STATE_BRIEF_LOADED);

	return true;
}

bool i3LevelResPath::OnValidate( i3LevelScene * pScene, bool bForce)
{
	if( bForce == false)
	{
		if( isLoaded())
			return true;
	}

	char szFull[ MAX_PATH];

	getFullPath( szFull);

	BeginValidate();

//		i3GamePath * pPath = i3GamePath::LoadFromFile( szFull);
//		I3_REF_CHANGE( m_pPath, pPath);
	m_pPath = i3GamePath::LoadFromFile( szFull);	
	// 이와 같이 고친다..LoadFromFile은 최소한 내부에서 레퍼런스카운팅이 1확보된다.(2012.09.13.수빈)
		
	EndValidate();

	if( m_pPath == nullptr)
		return false;

//	m_pPath->AddRef();	// 지금의 AddRef()는 릭원인이 될뿐이다..무조건..주석..(2012.09.13.수빈)
	m_pPath->SetExternRes( TRUE);

	addResState( I3LV_RES_STATE_LOADED);
	
	return true;
}

bool i3LevelResPath::OnSerialize(void)
{
	i3ResourceFile file;

	if( m_pPath == nullptr)
		return true;

	file.setKeyObject( m_pPath);

	char szFull[MAX_PATH];

	sprintf( szFull, "%s/%s", g_szResPath, getPath());

	if( file.Save( szFull) == STREAM_ERR)
		return false;

	return true;
}

i3GameRes * i3LevelResPath::CreateGameRes(void)
{
	i3GameResPath * pRes;

	pRes = i3GameResPath::new_object();
	pRes->setPath( m_pPath);

	return pRes;
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)			// [test required] 패킹사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct RES_PATH
	{
		UINT8		m_ID[4] = { 'R', 'S', 'P', '1' };
		INT32		m_PointCount = 0;
		INT32		m_LineCount = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3LevelResPath::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_PATH	data;

	Result = i3LevelRes::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_PointCount	= m_PointCount;
	data.m_LineCount		= m_LineCount;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}
	
UINT32 i3LevelResPath::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::RES_PATH data;

	Result = i3LevelRes::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Result);
	Result += Rc;

	m_PointCount = data.m_PointCount;
	m_LineCount = data.m_LineCount;

	return Result;
}

i3LevelElement3D *	i3LevelResPath::CreateInstance(i3LevelScene * pScene)
{
	i3LevelPath * pObj;

	OnValidate(pScene, false);

	pObj = i3LevelPath::new_object_ref();
	pObj->BindRes(this);

	pScene->AddElement(pObj, GetName());

	return pObj;
}