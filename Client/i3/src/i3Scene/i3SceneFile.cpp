#include "i3SceneDef.h"
#include "i3SceneFile.h"
#include "i3SceneObject.h"

I3_CLASS_INSTANCE( i3SceneFile);

i3SceneFile::~i3SceneFile(void)
{
	I3_SAFE_RELEASE( m_pInfo);
}

void i3SceneFile::SetSceneGraph( i3Node * pNode)
{
	if( m_pInfo == nullptr)
	{
		m_pInfo = i3SceneGraphInfo::new_object();
	}

	m_pInfo->setInstanceSg( pNode);
}

void i3SceneFile::SetCommonSceneGraph( i3Node * pNode)
{
	if( m_pInfo == nullptr)
	{
		m_pInfo = i3SceneGraphInfo::new_object();
	}

	m_pInfo->setCommonSg( pNode);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED SCENEFILE_HEADER
	{
		UINT8		m_Version[2] = { 0 };
		UINT32		m_RootID = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3SceneFile::Load( i3Stream * pStream, bool bCommonRes)
{
	UINT32 Result = 0, Rc;
	pack::SCENEFILE_HEADER header;
	bool bOldVersion = false;

	// Header
	{
		UINT8 id[4];
		UINT64 oldOffset = pStream->GetPosition();

		Rc = pStream->Read( id, 4);
		I3_CHKIO( Rc);
		Result += Rc;

		header.m_RootID = 0;

		if( memcmp( id, "I3SG", 4) == 0)
		{
			bOldVersion = true;

			Rc = pStream->Read( &header, sizeof(pack::SCENEFILE_HEADER));
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read Scene Graph File Header.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
		else
		{
			pStream->SetPosition( (INT32) oldOffset, STREAM_BEGIN);
		}
	}
	
	Rc = i3ResourceFile::Load( pStream, bCommonRes);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	if( bOldVersion == true)
	{
		i3Node * pNode;

		pNode = (i3Node *) FindObjectByID( header.m_RootID);

		SetSceneGraph( pNode);
	}
	else
	{
		I3_SAFE_RELEASE( m_pInfo);
		m_pInfo = (i3SceneGraphInfo *) getKeyObject();
		I3_MUST_ADDREF( m_pInfo);
	}
	
	m_pInfo->SetExternTextureSkipped(m_bExtTexSkipped);

	return Result;
}

UINT32	i3SceneFile::Save( i3Stream * pStream)
{
	UINT32 Result = 0, Rc;

	// Header
	m_ObjList.push_back( m_pInfo);
	I3_MUST_ADDREF( m_pInfo);	// m_ObjList에 넣기전에는 카운팅필요..(2012.09.17.수빈)
	setKeyObject( m_pInfo);

	Rc = i3ResourceFile::Save( pStream);
	if( Rc == STREAM_ERR)
	{
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3SceneFile::Load( const char * pszPath, bool bCommonRes)
{
	return i3ResourceFile::Load( pszPath, bCommonRes);
}

UINT32 i3SceneFile::Save( const char * pszPath)
{
	return i3ResourceFile::Save( pszPath);
}

bool	i3SceneFile::CheckExternNeedLoad(i3ClassMeta* RscClassMeta) const
{
	// i3s파일의 경우, 텍스쳐 공유하는 다른 메시에서 레퍼런스 메시를 읽을때는 텍스쳐로딩을 생략하도록 유도한다..
	// bCommonRes를 이용하는 방법도 좋아보이긴 하지만, 역할 중복을 피하는게 좋을것 같아 따로 구성한다..
	if (!m_bExtTexSkipped) return true;
	return !i3::kind_of<i3Texture*>(RscClassMeta);
}
