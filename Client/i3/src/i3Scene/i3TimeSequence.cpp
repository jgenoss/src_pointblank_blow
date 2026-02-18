#include "i3SceneDef.h"
#include "i3Node.h"
#include "i3TimeSequence.h"
#include "i3SceneTracer.h"
#include "i3Particle.h"
#include "i3SceneDebug.h"
#include "i3Base/itl/vector_util.h"

I3_CLASS_INSTANCE( i3TimeSequence);

I3_TRACE_RESULT I3SG_NODE_PROC( i3TimeSequence)( i3Node * pNode, i3SceneTracer * pTracer)
{
	i3TimeSequence * pTimeSeq = (i3TimeSequence *) pNode;
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;

	pTimeSeq->Drive( pTracer);

	if( pNode->GetChildCount() > 0)
	{
		Result = i3Node::CallTraceProc( pNode, pTracer);
	}

	return Result;
}

i3TimeSequence::i3TimeSequence(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3TimeSequence));
	
	m_SeqList.reserve( 16);
}

i3TimeSequence::~i3TimeSequence(void)
{
	RemoveAllSequence();
}

INT32 i3TimeSequence::AddSequence( i3TimeSequenceInfo * pInfo)
{
	i3Node * pNode;
	INT32 idx;

	pNode = pInfo->GetNode();
	I3ASSERT( pNode != nullptr);

	I3ASSERT( i3::same_of<i3TimeSequenceInfo*>(pInfo));

	I3_SAFE_ADDREF( pInfo);

	idx = (INT32) m_SeqList.size();
	m_SeqList.push_back( pInfo);

	AddChild( pNode);

	// i3TimeSequence의 Child Node는 모두 i3timeSequenceInfo에 의해 관리되어지기 때문에
	// 어떤 Child Node도 Copy 시에 추가하지 않는다.
	pNode->AddFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_VOLATILE);

	UpdateDuration();

	return idx;
}

INT32 i3TimeSequence::InsertSequence( INT32 idx, i3TimeSequenceInfo * pInfo)
{
	i3Node * pNode;
	INT32 newidx;

	pNode = pInfo->GetNode();
	I3ASSERT( pNode != nullptr);

	I3_SAFE_ADDREF( pInfo);

	i3::vector<i3TimeSequenceInfo*>::iterator it = m_SeqList.insert(m_SeqList.begin() + idx, pInfo);
	newidx = (INT32) i3::distance(m_SeqList.begin(), it);
//	newidx = m_SeqList.insert(m_SeqList.begin() + idx, pInfo);

	AddChild( pNode);

	// i3TimeSequence의 Child Node는 모두 i3timeSequenceInfo에 의해 관리되어지기 때문에
	// 어떤 Child Node도 Copy 시에 추가하지 않는다.
	pNode->AddFlag( I3_NODEFLAG_DISABLE | I3_NODEFLAG_VOLATILE);

	UpdateDuration();

	return newidx;
}

INT32 i3TimeSequence::GetSequenceIndex( i3TimeSequenceInfo * pInfo)
{
	size_t idx = i3::vu::index_of(m_SeqList, pInfo);
	return (idx != m_SeqList.size() ) ? INT32(idx) : -1;
}

void i3TimeSequence::RemoveSequence( i3TimeSequenceInfo * pInfo)
{
	

	size_t idx = i3::vu::index_of(m_SeqList, pInfo);
	I3ASSERT( idx != m_SeqList.size() );

	DeleteSequence((INT32) idx);
}

void i3TimeSequence::DeleteSequence( INT32 idx)
{
	i3TimeSequenceInfo * pInfo = m_SeqList[idx];
	i3Node * pNode = pInfo->GetNode();

	RemoveChild( pNode);

	I3_MUST_RELEASE( pInfo);

	m_SeqList.erase(m_SeqList.begin() + idx);

	UpdateDuration();
}

void i3TimeSequence::RemoveAllSequence(void)
{
	
	i3TimeSequenceInfo * pInfo;
	i3Node * pNode;

	for(size_t i = 0; i < m_SeqList.size(); i++)
	{
		pInfo = m_SeqList[i];

		pNode = pInfo->GetNode();
		RemoveChild( pNode);

		I3_MUST_RELEASE( pInfo);
	}

	m_SeqList.clear();

	m_Duration = 0.0f;
}

void i3TimeSequence::UpdateDuration(void)
{
	
	i3TimeSequenceInfo * pInfo;

	m_Duration = 0.0f;

	m_bInfinite = false;

	for(size_t i = 0; i < m_SeqList.size(); i++)
	{
		pInfo = m_SeqList[i];

		if( pInfo->GetDuration() <= 0.0f)
			m_bInfinite = true;

		if( (pInfo->GetStartTime() + pInfo->GetDuration()) > m_Duration)
		{
			m_Duration = pInfo->GetStartTime() + pInfo->GetDuration();
		}
	}
}

void i3TimeSequence::OnChangeTime( REAL32 tm)
{
	
	i3Node * pNode;

	m_LocalTime = tm;
	m_bActive = true;

	for(size_t i = 0; i < m_SeqList.size(); i++)
	{
		i3TimeSequenceInfo * pInfo = m_SeqList[i];

		pNode = pInfo->GetNode();
		
		if( pInfo->GetStartTime() >= tm)
			pInfo->Stop();

		if( pNode->GetParent() == nullptr)
		{
			pNode->OnChangeTime( tm);
		}
	}

	i3Node::OnChangeTime( tm);
}

void i3TimeSequence::Drive( i3SceneTracer * pTracer)
{

	i3TimeSequenceInfo * pInfo;

	if(m_bActive == false)
		return;

	if( m_lastFrameID == pTracer->getFrameID())
		return;

	m_lastFrameID = pTracer->getFrameID();

	for(size_t i = 0; i < m_SeqList.size(); i++)
	{
		pInfo = m_SeqList[i];

		if( pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_DISABLE)
			continue;

		if( pInfo->GetState() & I3SG_TIMESEQ_STATE_ACTIVE)
		{
			// 활성화되어 있다.
			if( pInfo->GetDuration() > 0.0f)
			{
				REAL32 length;

				if( pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_REPEAT)
				{
					length = (pInfo->getLoopCount() + 1) * pInfo->GetDuration();
				}
				else
				{
					length = pInfo->GetDuration();
				}

				// Duration이 0이라면 Infinite 시간으로 간주한다.
				if( m_LocalTime >= (pInfo->GetStartTime() + length))
				{
					bool bStop = true;

					if( pInfo->GetStyle() & I3SG_TIMESEQ_STYLE_REPEAT)
					{
						if( pInfo->getMaxLoopCount() > 0)
						{
							// 유한 반복
							pInfo->addLoopCount();

							if( pInfo->getLoopCount() <= pInfo->getMaxLoopCount())
								bStop = false;
						}
						else
						{
							bStop = false;
						}
					}
					
					if( bStop)
					{
						pInfo->Stop();
					}
					else
						pInfo->Start();
				}
			}
		}
		else
		{
			if (pTracer->IsManualTimeUpdate())
			{
				//TimeLine 문제로 추가..
				if (pInfo->GetStartTime() > m_PrevLocalTime && pInfo->GetStartTime() < m_LocalTime)
				{
					pInfo->Start();
					pTracer->SetIsManualTimeUpdate(false);
				}
				else if( i3Math::abs( m_LocalTime - pInfo->GetStartTime()) <= pTracer->GetManualTime())
				{
					pInfo->Start();
					pTracer->SetIsManualTimeUpdate(false);
				}
			}
			else
			{
				if( i3Math::abs( m_LocalTime - pInfo->GetStartTime()) <= pTracer->GetTime())
				{
					// 시작 시간이 되었기 때문에...
					pInfo->Start();
				}
			}
		}
	}
	
	m_LocalTime += pTracer->GetTime() * m_TimeScale;

	if( m_LocalTime > m_Duration)
	{
		if( m_bInfinite == false)
		{
			// 실행을 충분히 했기 때문에...
			m_bActive = false;
		}

		if( m_bRepeat)
		{
			OnChangeTime( 0.0f);
		}
	}

	m_PrevLocalTime = m_LocalTime;
}

void i3TimeSequence::CopyTo( i3ElementBase * pNode, I3_COPY_METHOD method)
{
	
	i3TimeSequenceInfo * pInfo;
	i3TimeSequence * pDest = (i3TimeSequence *) pNode;

	i3Node::CopyTo( pNode, method);

	for( size_t i = 0; i < m_SeqList.size(); i++)
	{
		pInfo = m_SeqList[i];

		if( method == I3_COPY_REF)
		{
			pDest->AddSequence( pInfo);
		}
		else
		{
			i3TimeSequenceInfo * pNewInfo;
			bool bSkipThisSeq = false;
			// 기존 것과 비교
			
			//for( INT32 j = 0; j < pDest->GetSequenceCount(); j++)
			//{
			//	pNewInfo = pDest->GetSequence( j);
			//	if( pInfo->IsSame( pNewInfo))
			//	{	// 같은 Sequence라면 추가할 필요가 없다.
			//		bSkipThisSeq = true;				// 리턴되면 안되고, 이 Info만 스킵해야되므로 코드를 재수정합니다. (2012.11.28.수빈)
			//		break;
			//	}
			//}
			
			if (!bSkipThisSeq)
			{
				pNewInfo = i3TimeSequenceInfo::new_object_ref();
				pInfo->CopyTo( pNewInfo, method);
				pDest->AddSequence( pNewInfo);
			}
		}
	}
}

void i3TimeSequence::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	
	i3TimeSequenceInfo * pInfo;

	for(size_t i = 0; i < m_SeqList.size(); i++)
	{
		pInfo = m_SeqList[i];

		pInfo->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

UINT32 i3TimeSequence::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();

	OBJREF ref;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Count
	{
		UINT32 count = m_SeqList.size();

		Rc = pStream->Write( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TimeSequence::OnSave()", "Could not write sequence list count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( size_t i = 0; i < m_SeqList.size(); i++)
	{
		ref = (OBJREF) pResFile->GetObjectPersistID(m_SeqList[i]);

		Rc = pStream->Write( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TimeSequence::OnSave()", "Could not write sequence reference.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TimeSequence::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	UINT32 i;
	OBJREF ref;
	UINT32 count;
	i3TimeSequenceInfo * pInfo;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
		return STREAM_ERR;

	// Time Sequence의 Child는 순수하게 i3TimeSequenceInfo에 의해 관리되어지며
	// 일반적인 i3Node에서의 Child Node는 있을 수 없다!
	RemoveAllChild();

	// Count
	{
		Rc = pStream->Read( &count, sizeof(count));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TimeSequence::OnLoad()", "Could not read sequence list count.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	for( i = 0; i < count; i++)
	{
		Rc = pStream->Read( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TimeSequence::OnLoad()", "Could not read sequence reference.");
			return STREAM_ERR;
		}
		Result += Rc;

		pInfo = (i3TimeSequenceInfo *) pResFile->FindObjectByID( ref);
		I3ASSERT( pInfo != nullptr);

		AddSequence( pInfo);
	}

	return Result;
}

bool i3TimeSequence::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3Node::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	OBJREF64 ref;

	// Count
	{
		UINT32 count = m_SeqList.size();

		pXML->addAttr("Count", count);
	}
	char	conv[MAX_PATH];

	for (size_t i = 0; i < m_SeqList.size(); i++)
	{
		ref = (OBJREF)pFile->GetObjectPersistID(m_SeqList[i]);

		i3::snprintf(conv, MAX_PATH, "Ref_%d", i);
		pXML->addAttr(conv, ref);
	}

	return true;
}

bool i3TimeSequence::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3Node::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	// 일반적인 i3Node에서의 Child Node는 있을 수 없다!
	RemoveAllChild();

	// Count
	UINT32 count;
	OBJREF64 ref;
	i3TimeSequenceInfo * pInfo;

	pXML->getAttr("Count", &count);

	char	conv[MAX_PATH];

	for (int i = 0; i < (int)count; i++)
	{
		i3::snprintf(conv, MAX_PATH, "Ref_%d", i);
		pXML->getAttr(conv, &ref);

		pInfo = (i3TimeSequenceInfo *)pFile->FindObjectByID((OBJREF)ref);
		I3ASSERT(pInfo != nullptr);

		AddSequence(pInfo);
	}

	return true;
}

void i3TimeSequence::Dump(void)
{
	

	{
		I3TRACE( "############ i3TimeSequence\n");

	#if defined( I3_DEBUG)
		I3_SG_DUMP( this);
	#endif
	}

	for( size_t i = 0; i < m_SeqList.size(); i++)
	{
		i3TimeSequenceInfo * pInfo = m_SeqList[i];

		I3TRACE( "############ SEQ[%d]\n", i);

	#if defined( I3_DEBUG)
		if( pInfo->GetNode() != nullptr)
		{
			I3_SG_DUMP( pInfo->GetNode());
		}
	#endif
	}
}
