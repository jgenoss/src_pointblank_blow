#include "i3FrameworkPCH.h"
#include "i3StageInfo.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/smart_ptr/make_shared.h"
#include "i3Base/i3StringListBuffer.h"

I3_CLASS_INSTANCE( i3StageInstanceInfo);
I3_CLASS_INSTANCE( i3StageInfo);

void i3StageInstanceInfo::SetClassNameByMeta( i3ClassMeta * pMeta)
{
	SetName( pMeta->class_name().c_str());
}

i3ClassMeta * i3StageInstanceInfo::GetClassMetaByName()
{
	return i3ClassMetaSystem::i()->find_class_meta(GetNameString());
}

#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct INSTANCE_INFO
	{
		INT32					m_nCount = 0;
		INT32					m_bAllocCountFixed = 0;
		REAL32					m_rUpdateInterval = 0.0f;
		INT32					pad[5] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3StageInstanceInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::INSTANCE_INFO	data;

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof( data));

	data.m_rUpdateInterval	= m_rUpdateInterval;
	data.m_nCount			= m_nCount;
	data.m_bAllocCountFixed = m_bAllocCountFixed;

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3StageInstanceInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::INSTANCE_INFO	data;

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_rUpdateInterval	=	data.m_rUpdateInterval;
	m_nCount			=	data.m_nCount;
	m_bAllocCountFixed	=	data.m_bAllocCountFixed;

	return Result;
}

////////////////////////////////////////////////////////////////////////////

i3StageInfo::~i3StageInfo(void)
{
	I3_SAFE_RELEASE( m_pNextStage);

	if( m_spInstanceManagerInfoList != nullptr)
	{
		i3::cu::for_each_container(*m_spInstanceManagerInfoList, i3::fu::mem_fun_safe(&i3StageInstanceInfo::Release));
		m_spInstanceManagerInfoList.reset();
		m_ElementInstanceManagerInfoList.clear();
		//m_pInstanceManagerInfoList->SafeReleaseAll();
		//I3_SAFE_RELEASE( m_pInstanceManagerInfoList);
	}
}

INT32 i3StageInfo::getInstanceManagerInfoCount()
{
	if( m_spInstanceManagerInfoList != nullptr)
	{
		return (INT32)m_spInstanceManagerInfoList->size();
	}
	else
	{
		return 0;
	}
}

void i3StageInfo::setInstanceManagerInfoList(const StageInstanceInfoListPtr& spList)
{
	if( m_spInstanceManagerInfoList != nullptr)
	{
	//	m_pInstanceManagerInfoList->SafeReleaseAll();
		i3::cu::for_each_container(*m_spInstanceManagerInfoList, i3::fu::mem_fun_safe(&i3StageInstanceInfo::Release));
	}
	
	m_spInstanceManagerInfoList = spList;

	m_ElementInstanceManagerInfoList = spList.get();

	//I3_REF_CHANGE( m_pInstanceManagerInfoList, pList);
}

void i3StageInfo::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_spInstanceManagerInfoList != nullptr)
	{
		const size_t nCount = m_spInstanceManagerInfoList->size();
		for( size_t i = 0; i <  nCount; ++i)
		{
			i3StageInstanceInfo * pInfo = (*m_spInstanceManagerInfoList)[i];
			pInfo->OnBuildObjectList( List);
		}
	}

	i3NamedElement::OnBuildObjectList( List);
}
	
void i3StageInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	i3StageInfo * pInfo = (i3StageInfo *) pDest;

	pInfo->setStageClassName( getStageClassName());
	pInfo->setFadeInColor( getFadeInColor());
	pInfo->setFadeInTime( getFadeInTime());
	pInfo->setFadeOutColor( getFadeOutColor());
	pInfo->setFadeOutTime( getFadeOutTime());
	pInfo->setNextStage( getNextStage());
	pInfo->setJumpType( getJumpType());
	pInfo->setInstanceManagerInfoList( getInstanceManagerInfoListPtr());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct STAGE_INFO
	{
		UINT8			m_ID[4] = { 'S', 'T', 'G', '1' };
		char			m_szClassName[64] = { 0 };
		UINT32			m_colorFI = 0;
		REAL32			m_timeFI = 0.0f;
		UINT32			m_colorFO = 0;
		REAL32			m_timeFO = 0.0f;
		OBJREF			m_pNextStage = 0;
		UINT32			m_JumpType = 0;
		INT32			m_RSCBufferSize = 0;
		INT32			m_nInstanceManagerInfoCount = 0;
		UINT32			pad[15] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3StageInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::STAGE_INFO data;

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	i3::safe_string_copy( data.m_szClassName, getStageClassName(), 64);
	data.m_colorFI =	i3Color::ConvertRGBA32( getFadeInColor());
	data.m_timeFI =		getFadeInTime();
	data.m_colorFO =	i3Color::ConvertRGBA32( getFadeOutColor());
	data.m_timeFO =		getFadeOutTime();
	data.m_pNextStage = (OBJREF) pResFile->GetObjectPersistID( getNextStage());
	data.m_JumpType =	(UINT32) getJumpType();
	data.m_RSCBufferSize =	GetStringListBufferSize(m_RSCPath);	//	m_RSCPath.GetBufferSize();
	data.m_nInstanceManagerInfoCount = getInstanceManagerInfoCount();

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data");
		return STREAM_ERR;
	}
	Result += Rc;

	//	RSC Path
	{
		INT32 size = data.m_RSCBufferSize;
		if( size > 0)
		{
			char * pszBuffer = (char *) i3MemAlloc( size);
		//	m_RSCPath.CopyToBuffer( pszBuffer, size);
			CopyFromStringListToBuffer(m_RSCPath, pszBuffer, size);

			Rc = pStream->Write( pszBuffer, size);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not write buffer");
				I3MEM_SAFE_FREE(pszBuffer);
				return STREAM_ERR;
			}
			Result += Rc;

			I3MEM_SAFE_FREE( pszBuffer);
		}
	}

	//	Instance Info list :::::::::::
	{
		for( INT32 i = 0; i < getInstanceManagerInfoCount(); ++i)
		{
			i3StageInstanceInfo * pInfo = (*m_spInstanceManagerInfoList)[i];
			OBJREF64 obj = (OBJREF64)pResFile->GetObjectPersistID( pInfo);

			Rc = pStream->Write( &obj, sizeof( OBJREF64));
			I3_CHKIO( Rc);
			Result += Rc;
		}
	}

	return Result;
}

UINT32 i3StageInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::STAGE_INFO data;

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read data");
		return STREAM_ERR;
	}
	Result += Rc;

	setStageClassName( data.m_szClassName);
	
	i3Color::ConvertRGBA32( data.m_colorFI, &m_colorFI);
	setFadeInTime( data.m_timeFI);

	i3Color::ConvertARGB32( data.m_colorFO, &m_colorFO);
	setFadeOutTime( data.m_timeFO);

	if( data.m_pNextStage != 0)
	{
		i3StageInfo * pNext = (i3StageInfo *) pResFile->FindObjectByID( data.m_pNextStage);
		I3ASSERT( pNext != nullptr);

		setNextStage( pNext);
	}

	setJumpType( (I3FRM_STAGE_JUMP_TYPE) data.m_JumpType);

	//	RSC Path
	if( data.m_RSCBufferSize > 0)
	{		
		INT32 size = data.m_RSCBufferSize;
		m_RSCPath.clear();
		if( size > 0)
		{
			char * pszBuffer = (char *) i3MemAlloc( size);

			Rc = pStream->Read( pszBuffer, size);
			if( Rc == STREAM_ERR)
			{
				I3PRINTLOG(I3LOG_FATAL,  "Could not read buffer");
				I3MEM_SAFE_FREE(pszBuffer);
				return STREAM_ERR;
			}
			Result += Rc;

		//	m_RSCPath.CopyFromBuffer( pszBuffer);
			CopyFromBufferToStringList(m_RSCPath, pszBuffer);

			I3MEM_SAFE_FREE( pszBuffer);
		}
	}

	//	Instance Info list :::::::::::
	{
	//	List * pStageInsInfoList = List::new_object_ref();
		StageInstanceInfoListPtr spStageInsInfoList = i3::make_shared<StageInstanceInfoList>();

		for( INT32 i = 0; i < data.m_nInstanceManagerInfoCount; ++i)
		{
			OBJREF64 obj;
			Rc = pStream->Read( &obj, sizeof( OBJREF64));
			I3_CHKIO( Rc);
			Result += Rc;

			if( obj != 0)
			{
				i3StageInstanceInfo * pInfo = (i3StageInstanceInfo*)pResFile->FindObjectByID( (OBJREF)obj);
				I3ASSERT( pInfo != nullptr);

				spStageInsInfoList->push_back( pInfo);
				I3_SAFE_ADDREF( pInfo);
			}
		}

		setInstanceManagerInfoList( spStageInsInfoList);
	}

	return Result;
}
