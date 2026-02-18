#include "i3FrameworkPCH.h"
#include "i3AIState.h"
#include "i3AIContext.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3AIState);

i3AIState::i3AIState( void)
{
	setStyle( I3_AISTATE_STYLE_CALL_FUNC);		//	기본으로 콜백 호출

}
i3AIState::~i3AIState( void)
{
	I3_SAFE_RELEASE( m_pAnim);
	I3_SAFE_RELEASE( m_pNext);

	I3_SAFE_RELEASE( m_pTimeSeq);

}

void i3AIState::SetAnim( i3Animation * pAnim)
{
	I3_REF_CHANGE( m_pAnim, pAnim);
}

#if defined( ENABLE_LUAFUNC)
void i3AIState::SetRunLuaProcName( const char * pszName)
{
	m_strRunLuaFuncName.clear();

	if( pszName != nullptr)
	{
		m_strRunLuaFuncName = pszName;
	}
}

void i3AIState::SetFinishLuaProcName( const char * pszName)
{
	m_strFinishLuaFuncName.clear();
	if( pszName != nullptr)
	{
		m_strFinishLuaFuncName = pszName;
	}
}
#endif

void i3AIState::OnChangeTime( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pTimeSeq != nullptr)
	{
		m_pTimeSeq->OnChangeTime( tm);
	}
}

void i3AIState::OnActiveState( i3AIContext * pCtx, REAL32 timeDelta)
{	
	if( getStyle() & I3_AISTATE_STYLE_CALL_LUA)
	{
		//	나중에 Lua 추가시
	}

	if( m_pTimeSeq != nullptr)
	{
		m_pTimeSeq->ActivateNode();
	}
}

void i3AIState::OnFinishState( i3AIContext * pCtx, REAL32 timeDelta)
{
	if( getStyle() & I3_AISTATE_STYLE_CALL_LUA)
	{
		//	나중에 Lua 추가시
	}

	if( getTimeSeq() != nullptr)
	{
		getTimeSeq()->DeactivateNode();
	}
}

void	i3AIState::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	// Recursive하게 AI State 들을 연결한 경우에 대한 예외처리.
	// 반드시 자신을 가장 먼저 List에 추가한 후, m_pNext에 대한
	// 처리를 진행해야 한다.
	i3ResourceObject::OnBuildObjectList( List);

	if( m_pAnim != nullptr)
	{
		m_pAnim->OnBuildObjectList( List);
	}

	if( m_pNext != nullptr)
	{
//		if( pList->IndexOf( m_pNext) == -1)
		if ( i3::find(List.begin(), List.end(), m_pNext) == List.end() )
		{
			// Recursive Next로 설정된 경우를 위한 검사.
			m_pNext->OnBuildObjectList( List);
		}
	}

	if( m_pTimeSeq != nullptr)
	{
		m_pTimeSeq->OnBuildObjectList( List);
	}
}

void	i3AIState::OnQueryStrings( i3::vector<i3::rc_string>& StrList)
{
	i3ResourceObject::OnQueryStrings( StrList);

#if defined( ENABLE_LUAFUNC)
	if( !m_strRunLuaFuncName.empty())
	{
		StrList.push_back( m_strRunLuaFuncName);
	}

	if( !m_strFinishLuaFuncName.empty())
	{
		StrList.push_back( m_strFinishLuaFuncName);
	}
#endif
}

void	i3AIState::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3ResourceObject::CopyTo( pDest, method);

	I3ASSERT( i3::same_of< i3AIState* >(pDest));

	i3AIState * pState = (i3AIState *) pDest;

	pState->setStyle( getStyle());
	pState->setID( getID());
	pState->SetAnim( (i3Animation *) getAnim());
	pState->setBlendTime( getBlendTime());
	pState->setTimeScale( getTimeScale());
	pState->SetRunLuaProcName( (char *) getRunLuaProcName());
	pState->SetFinishLuaProcName( (char *) getFinishLuaProcName());
	pState->setDuration( getDuration());
	pState->setNext( getNext());
	pState->setAnimName( getAnimNameString());

	if( m_pTimeSeq != nullptr)
	{
		if( method == I3_COPY_REF)
		{
			pState->m_pTimeSeq = m_pTimeSeq;
			I3_SAFE_ADDREF( m_pTimeSeq);
		}
		else
		{
			if( pState->m_pTimeSeq == nullptr)
			{
				pState->m_pTimeSeq = (i3TimeSequence *) i3Scene::CreateClone( m_pTimeSeq, I3_COPY_INSTANCE);
				I3_MUST_ADDREF( pState->m_pTimeSeq);
			}
			else
			{
				m_pTimeSeq->CopyTo( pState->m_pTimeSeq, I3_COPY_INSTANCE);
			}
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 AI_STATE
	{
		UINT8			m_sig[4] = { 'A', 'I', 'S', '1' };

		UINT32			m_Style = 0;
		INT32			m_ID = 0;

		OBJREF64		m_pAnim = 0;
		REAL32			m_timeScale = 0.0f;
		REAL32			m_timeBlend = 0.0f;
		INT32			pad0 = 0;

		INT32			m_RunLuaFuncName = 0;
		INT32			m_FinishLuaFuncName = 0;

		REAL32			m_timeLength = 0.0f;

		OBJREF64		m_pNext = 0;
		OBJREF64		m_pTimeSeq = 0;

		UINT32			m_AnimNameLen = 0;

		UINT32			pad[13] = { 0 };

	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3AIState::OnSave( i3ResourceFile * pResFile)
{
	pack::AI_STATE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream  = pResFile->GetStream();

	Result = i3ResourceObject::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_Style			= getStyle();
	data.m_ID				= getID();
	data.m_pAnim			= (OBJREF64) pResFile->GetObjectPersistID( (i3PersistantElement *) getAnim());
	data.m_timeScale		= getTimeScale();
	data.m_timeBlend		= getBlendTime();

	if( getRunLuaProcName() != nullptr)
	{
		size_t idx = i3::vu::index_of(pResFile->GetStringTable(), (char *) getRunLuaProcName() );

		data.m_RunLuaFuncName	= (idx != pResFile->GetStringTable().size() ) ? INT32(idx) : -1;
			// pResFile->GetStringTable()->IndexOf( (char *) getRunLuaProcName());
	}
	else
	{
		data.m_RunLuaFuncName	= -1;
	}

	if( getFinishLuaProcName() != nullptr)
	{
		size_t idx = i3::vu::index_of(pResFile->GetStringTable(), (char *) getFinishLuaProcName() );

		data.m_FinishLuaFuncName	= (idx != pResFile->GetStringTable().size() ) ? INT32(idx) : -1;
			//pResFile->GetStringTable()->IndexOf( (char *) getFinishLuaProcName());
	}
	else
	{
		data.m_FinishLuaFuncName	= -1;
	}

	data.m_timeLength		= getDuration();
	data.m_pNext			= (OBJREF64) pResFile->GetObjectPersistID( getNext());
	data.m_pTimeSeq			= (OBJREF64) pResFile->GetObjectPersistID( getTimeSeq());
	data.m_AnimNameLen		= i3::generic_string_size( getAnimNameString());

	if( data.m_AnimNameLen == 0)
	{
		if( getAnim() != nullptr)
		{
			setAnimName( getAnim()->GetNameString());
			data.m_AnimNameLen = getAnim()->GetNameString().size();
		}
	}

	Rc = pStream->Write( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_AnimNameLen > 0)
	{
		Rc = pStream->Write( getAnimName(), data.m_AnimNameLen);
		I3_CHKIO( Rc);
		Result += Rc;
	}
	

	return Result;
}

UINT32 i3AIState::OnLoad( i3ResourceFile * pResFile)
{
	pack::AI_STATE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream  = pResFile->GetStream();
	const i3::vector<i3::rc_string>& StrList  = pResFile->GetStringTable();

	Result = i3ResourceObject::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof( data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;
	
	setStyle( data.m_Style);
	setID( data.m_ID);
	
	if( data.m_pAnim != 0)
	{
		i3Animation * pAnim = (i3Animation *) pResFile->FindObjectByID( (OBJREF) data.m_pAnim);

		SetAnim( pAnim);

		if( data.m_timeScale > 0.f)
			setTimeScale( data.m_timeScale);
		setBlendTime( data.m_timeBlend);
	}
	
	if( data.m_RunLuaFuncName != -1)
	{
		SetRunLuaProcName( StrList[ data.m_RunLuaFuncName].c_str());
	}

	if( data.m_FinishLuaFuncName != -1)
	{
		SetFinishLuaProcName( StrList[ data.m_FinishLuaFuncName].c_str());
	}

	setDuration( data.m_timeLength);

	if( data.m_pNext != 0)
	{
		i3AIState * pNext = (i3AIState *) pResFile->FindObjectByID( (OBJREF) data.m_pNext);
		I3ASSERT( pNext != nullptr);

		setNext( pNext);
	}

	if( data.m_pTimeSeq != 0)
	{
		i3TimeSequence * pTimeSeq = (i3TimeSequence *) pResFile->FindObjectByID( (OBJREF) data.m_pTimeSeq);
		I3ASSERT( pTimeSeq != nullptr);

		setTimeSeq( pTimeSeq);
	}

	if( data.m_AnimNameLen > 0)
	{
		i3::vector<char>	char_buff(data.m_AnimNameLen + 1);		// +1은 널공간으로 추가할당..
		Rc = pStream->Read( &char_buff[0], data.m_AnimNameLen);
		setAnimName(&char_buff[0]);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

bool i3AIState::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->addAttr("Style", getStyle());
	pXML->addAttr("ID", getID());
	pXML->addAttr("AnimID", (OBJREF64)pFile->GetObjectPersistID((i3PersistantElement *)getAnim()));
	pXML->addAttr("TimeScale", getTimeScale());
	pXML->addAttr("BlendTime", getBlendTime());
	pXML->addAttr("RunLuaFuncName", -1);
	pXML->addAttr("FinishLuaFuncName", -1);
	pXML->addAttr("Duration", getDuration());
	pXML->addAttr("NextID", (OBJREF64)pFile->GetObjectPersistID(getNext()));
	pXML->addAttr("TimeSeqID", (OBJREF64)pFile->GetObjectPersistID(getTimeSeq()));

	size_t AnimNameLen = i3::generic_string_size(getAnimNameString());
	if (AnimNameLen == 0)
	{
		if (getAnim() != nullptr)
		{
			setAnimName(getAnim()->GetNameString());
			AnimNameLen = getAnim()->GetNameString().size();
		}
	}

	pXML->addAttr("AnimNameLen", AnimNameLen);

	if (AnimNameLen > 0)
	{
		pXML->addAttr("AnimName", getAnimName());
	}
	
	
	return true;
}

bool i3AIState::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3ResourceObject::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	UINT32 style;
	pXML->getAttr("Style", &style);
	setStyle(style);
	
	UINT32 id;
	pXML->getAttr("ID", &id);
	setID(id);

	OBJREF64 temp;
	pXML->getAttr("AnimID", &temp);
	if (temp != 0)
	{
		i3Animation * pAnim = (i3Animation *)pFile->FindObjectByID((OBJREF)temp);
		SetAnim(pAnim);
	}

	REAL32 timescale;
	pXML->getAttr("TimeScale", &timescale);
	setTimeScale(timescale);

	REAL32 blendtime;
	pXML->getAttr("BlendTime", &blendtime);
	setBlendTime(blendtime);

	REAL32 duration;
	pXML->getAttr("Duration", &duration);
	setDuration(duration);

	pXML->getAttr("NextID", &temp);
	if (temp != 0)
	{
		i3AIState * pNext = (i3AIState *)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT(pNext != nullptr);
		setNext(pNext);
	}
	
	pXML->getAttr("TimeSeqID", &temp);
	if (temp != 0)
	{
		i3TimeSequence * pTimeSeq = (i3TimeSequence *)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT(pTimeSeq != nullptr);
		setTimeSeq(pTimeSeq);
	}

	size_t AnimNameLen;
	pXML->getAttr("AnimNameLen", &AnimNameLen);

	if (AnimNameLen > 0)
	{
		i3::rc_string str = pXML->getAttr("AnimName");
		setAnimName(str.c_str());
	}
	

	return true;
}