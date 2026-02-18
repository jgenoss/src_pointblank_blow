#include "i3MathType.h"
#include "i3Animation.h"
#include "i3Base/string/ext/safe_string_copy.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3Animation);

i3Animation::~i3Animation(void)
{
	INT32 i;
	i3TransformSequence * pTransSeq;
	i3::pack::animation::TRACK_INFO * pTrack;

	pTrack = m_pTracks;

	for( i = 0; i < m_TrackCount; i++)
	{
		pTransSeq = pTrack->m_pSeq;
		I3_SAFE_RELEASE( pTransSeq);

		pTrack++;
	}

	if( m_pBindBuf == nullptr)
	{
		i3::static_pool_free( m_pTracks ); 
	}
	else
	{
		I3_SAFE_RELEASE( m_pBindBuf);
	}
}

bool i3Animation::Create( INT32 TrackCount)
{
	m_TrackCount = TrackCount;	
	
	if( m_TrackCount > 0)
	{
		m_pTracks = (i3::pack::animation::TRACK_INFO *) i3::static_pool_malloc( sizeof(i3::pack::animation::TRACK_INFO) * TrackCount);
		I3ASSERT( m_pTracks );

		memset( m_pTracks, 0, sizeof(i3::pack::animation::TRACK_INFO) * TrackCount);
	}

	return true;
}

void i3Animation::Bind( i3::pack::animation::TRACK_INFO * pTracks, INT32 Count, REAL32 Duration, i3MemoryBuffer * pMemBuf, INT32 offset)
{
	INT32 i;

	I3_REF_CHANGE( m_pBindBuf, pMemBuf);

	m_offMemory = offset;
	m_TrackCount = Count;
	m_pTracks = pTracks;
	m_Duration = Duration;

	for( i = 0; i < Count; i++)
	{
		m_pTracks[i].m_pSeq = nullptr;
	}
}

void i3Animation::SetTrack( INT32 idx, i3TransformSequence * pSeq, const char * pszBoneName)
{
	if( pSeq != nullptr)
	{
		I3_MUST_ADDREF( pSeq);
		m_Duration = MAX( m_Duration, pSeq->GetDuration());
	}

	I3_SAFE_RELEASE( m_pTracks[idx].m_pSeq);

	// Transform Sequence
	m_pTracks[idx].m_pSeq = pSeq;

	// Bone Name
	strncpy( m_pTracks[idx].m_szBone, pszBoneName, MAX_NAME - 1);
	m_pTracks[idx].m_szBone[ MAX_NAME - 1] = 0;
}

INT32 i3Animation::AddTrack( i3TransformSequence * pSeq, char * pszBoneName)
{
	i3::pack::animation::TRACK_INFO * pNewTrack;

	if( pSeq != nullptr)
	{
		I3_MUST_ADDREF( pSeq);
		m_Duration = MAX( m_Duration, pSeq->GetDuration());
	}

	pNewTrack = (i3::pack::animation::TRACK_INFO *) i3::static_pool_malloc( sizeof(i3::pack::animation::TRACK_INFO) * (m_TrackCount + 1));

	if( m_pTracks != nullptr)
	{
		i3mem::Copy( pNewTrack, m_pTracks, sizeof(i3::pack::animation::TRACK_INFO) * m_TrackCount);

		// 교체
		i3::static_pool_free( m_pTracks);
	}

	m_pTracks = pNewTrack;

	INT32 idx = m_TrackCount;

	m_pTracks[idx].m_pSeq = pSeq;

	i3::string_ncopy_nullpad( m_pTracks[idx].m_szBone, pszBoneName, MAX_NAME - 1);
	m_pTracks[idx].m_szBone[ MAX_NAME - 1] = 0;

	m_TrackCount++;

	return idx;
}

UINT32 i3Animation::GetInterpolatedKeyframe( INT32 idxTrack, I3MATRIXINFO * pInfo, bool bFirst)
{
	return m_pTracks[idxTrack].m_pSeq->GetInterpolatedKeyframe( pInfo, bFirst);
}

void i3Animation::GetKeyframe( INT32 idxTrack, INT32 idxKey, MATRIX * pMtx)
{
	m_pTracks[ idxTrack].m_pSeq->GetKeyMatrix( idxKey, pMtx);
}

void i3Animation::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	if( IsExternRes() == false)
	{
		for( i = 0; i < m_TrackCount; i++)
		{
			if( m_pTracks[i].m_pSeq != nullptr)
			{
				m_pTracks[i].m_pSeq->OnBuildObjectList( List);
			}
		}
	}

	i3ResourceObject::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct ALIGN4 TRACKINFO
	{
		OBJREF		m_pSeq = 0;
		char		m_szBone[MAX_NAME] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3Animation::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();

	// Named Element
	Result = i3ResourceObject::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Track Count
	Rc = pStream->Write( &m_TrackCount, sizeof(m_TrackCount));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Animation::OnSave()", "Could not write track count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Track Data
	//
	// i3TransformSequence에 대한 포인터를 그대로 저장한다.
	// 이는 Loading 시에 적당한 Runtime Pointer로 Rebind되기 때문에 문제가 되지 않는다.
	for( INT32 i = 0; i < m_TrackCount; i++)
	{
		pack::TRACKINFO data;

		data.m_pSeq = pResFile->GetObjectPersistID( m_pTracks[i].m_pSeq);
		i3::safe_string_copy( data.m_szBone, m_pTracks[i].m_szBone, MAX_NAME);

		Rc = pResFile->Write( &data, sizeof(data));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	
	return Result;
}

UINT32 i3Animation::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3Animation::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	UINT32 Result = 0, Rc;
	i3Stream * pStream = pResFile->GetStream();
	INT32 i;

	// Named Element
	Result = i3ResourceObject::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Track Count
	Rc = pStream->Read( &m_TrackCount, sizeof(m_TrackCount));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Animation::OnLoad()", "Could not read track count data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( m_TrackCount > 0)
	{
		Create( m_TrackCount);

		// Track References
		for( i = 0; i < m_TrackCount; i++)
		{
			pack::TRACKINFO data;

			Rc = pStream->Read( &data, sizeof(data));
			I3_CHKIO( Rc);
			Result += Rc;
		
			if( data.m_pSeq != 0)
			{
				i3TransformSequence * pSeq;

				pSeq = (i3TransformSequence *) pResFile->FindObjectByID( (UINT32) data.m_pSeq);
				I3ASSERT( pSeq != nullptr);

				SetTrack( i, pSeq, data.m_szBone);
			}
		}
	}

	return Result;
}

bool i3Animation::SaveToFile( const char * pszPath)
{
	i3FileStream Stream;
	bool Result;

	if( Stream.Create( pszPath, STREAM_WRITE) == false)
	{
		i3Log( "i3Animation::SaveToFile()", "Could not create %s file.", pszPath);
		return false;
	}

	Result = SaveToFile( &Stream);

	Stream.Close();

	return Result;
}

i3Animation * i3Animation::LoadFromFile( const char * pszPath, UINT32 * pSize)
{
	i3FileStream Stream;
	i3Animation * pResult;

	if( Stream.Open( pszPath, STREAM_READ | STREAM_SHAREREAD) == false)
	{
		i3Log( "i3Animation::LoadFromFile()", "Could not open %s animation file.", pszPath);
		return nullptr;
	}

	pResult = LoadFromFile( &Stream);

#if defined( I3_DEBUG)
	if( pResult != nullptr)
		pResult->m_FileSize = Stream.GetSize();
	if( pSize != nullptr)
		*pSize = Stream.GetSize();
#endif

	Stream.Close();

	return pResult;
}

bool i3Animation::SaveToFile( i3Stream * pStream)
{
	i3ResourceFile file;
	i3::vector<i3PersistantElement*> TempList;
	UINT32 Rc;

	TempList.push_back( this);

	file.SetStream( pStream);

	file.SetObjects(TempList);

	Rc = file.Save( pStream);

	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Animation::SaveToFile()", "Could not write transform sequences.");
		return false;
	}

	return true;
}

i3Animation * i3Animation::LoadFromFile( i3Stream * pStream)
{
	i3Animation * pResult = nullptr;
	i3ResourceFile file;
	INT32 i;
	UINT32 Rc;

	file.SetStream( pStream);

	Rc = file.Load( pStream);
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3Animation::LoadFromFile()", "Could not read transform sequences.");
		return nullptr;
	}

	for( i = 0; i < file.GetObjectCount(); i++)
	{
		i3PersistantElement * pObj = file.GetObject( i);

		if( i3::kind_of<i3Animation*>(pObj))
		{
			pResult = (i3Animation *) pObj;
		}
	}

	I3ASSERT( pResult != nullptr);

	I3_SAFE_ADDREF( pResult);	// i3ResourceFile이 소거되기 전에 AddRef()..이 경우 다른곳에 Add될때 Release해줘야한다...
								// (2012.09.11.수빈.)
	
	return pResult;
}
