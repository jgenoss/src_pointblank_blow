#include "i3SceneDef.h"
#include "i3SoundNode.h"
#include "i3SceneTracer.h"
#include "i3Base/string/ext/extract_filename.h"

I3_CLASS_INSTANCE( i3SoundNode);

I3_TRACE_RESULT I3SG_NODE_PROC( i3SoundNode)( i3Node * pNode, i3SceneTracer * pTracer)
{
	I3_TRACE_RESULT Result = I3_TRACE_RESULT_CONTINUE;
	i3SoundNode * pSndNode = (i3SoundNode *) pNode;

	pSndNode->Drive( pTracer);

	return Result;
}

void		i3SoundNode::PreTrace(i3SceneTracer * pTracer)
{
	/////
}

i3SoundNode::i3SoundNode(void)
{
	_setTraceProc( I3SG_NODE_PROC( i3SoundNode));
}

i3SoundNode::~i3SoundNode(void)
{
	_StopSound();

	I3_SAFE_RELEASE( m_pSndPlayInfo);
}

void i3SoundNode::_StopSound(bool bForce)
{
	if( m_pSndPlayState != nullptr)
	{
		if( bForce || m_pSndPlayState->isLoop())
		{
			m_pSndPlayState->Stop();
		}
		
		I3_SAFE_RELEASE( m_pSndPlayState);
	}

	Reset();
}

bool i3SoundNode::OnQueryDeactivateNode()
{
	if( m_pSndPlayState == nullptr)
		return true;
	else
	{
		if( m_pSndPlayState->isLoop() || !m_pSndPlayState->isListnerSound() || m_pSndPlayInfo->isStopOnReset())
			return true;
		else
			return false;
	}
}

void i3SoundNode::setSoundPlayInfo( i3SoundPlayInfo * pSnd, i3ResourceFile * pFile)
{
	_StopSound();
	
	I3_REF_CHANGE( m_pSndPlayInfo, pSnd);

	if( m_pSndPlayInfo != nullptr)
	{
		i3Sound* pS = m_pSndPlayInfo->getSound();
		if( pS == nullptr && pFile != nullptr)
		{
			m_pSndPlayInfo->LoadFromBackground( pFile);
			pS = m_pSndPlayInfo->getSound();
		}

		if( pS != nullptr)
		{
			i3::rc_string conv;
			i3::extract_filename(pS->GetNameString(), conv);
			SetName( conv);
		}
	}
}

void i3SoundNode::setSoundPlayInfoFromXML(i3SoundPlayInfo * pSnd, i3XMLFile * pFile)
{
	_StopSound();

	I3_REF_CHANGE(m_pSndPlayInfo, pSnd);

	if (m_pSndPlayInfo != nullptr)
	{
		i3Sound* pS = m_pSndPlayInfo->getSound();
		if (pS == nullptr && pFile != nullptr)
		{
			//m_pSndPlayInfo->LoadFromBackground(pFile);
			//pS = m_pSndPlayInfo->getSound();
		}

		if (pS != nullptr)
		{
			i3::rc_string conv;
			i3::extract_filename(pS->GetNameString(), conv);
			SetName(conv);
		}
	}
}

void i3SoundNode::Reset(void)
{
	m_timeAccm		= 0.0f;
	m_PlayedCount	= 0;

	removeStyle( I3SG_SNDNODE_STYLE_POSITIONAL);
}

void i3SoundNode::Drive( i3SceneTracer * pTracer)
{
	i3SoundContext * pSndCtx = pTracer->getSoundContext();

	if( pSndCtx == nullptr)
		return;

	if( m_lastFrameID == pTracer->getFrameID())
		return;

	m_lastFrameID = pTracer->getFrameID();

	if( m_pSndPlayInfo->GetSoundType() & pSndCtx->getState())
	{
		m_timeAccm += pTracer->GetTime();
		if( m_pSndPlayState == nullptr)
		{
			// 아직 재생을 시작하지 않았다.
			if( m_timeAccm > m_timeStart)
			{
				i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

				m_pSndPlayState = pSndCtx->Play( m_pSndPlayInfo, pStack->GetTop());
				if( m_pSndPlayState != nullptr)
				{
					I3_MUST_ADDREF( m_pSndPlayState);

					if( m_pSndPlayInfo->Get3DMode() != I3SND_RT_3DMODE_DISABLE)
					{
						addStyle( I3SG_SNDNODE_STYLE_POSITIONAL);
					}
				}
				else
				{
					DeactivateNode();
				}
			}
		}
		else
		{
			if(m_pSndPlayState->isFree() )
			{
				DeactivateNode();
			}
			else
			{
				// 재생 중이다.
				if( getStyle() & I3SG_SNDNODE_STYLE_POSITIONAL)
				{
					i3MatrixStack * pStack = pTracer->GetModelViewMatrixStack();

					 m_pSndPlayState->UpdatePos( pStack->GetTop());
				}
			}
		}
	}
	else
	{
		if( m_pSndPlayState != nullptr)
		{
			_StopSound();

			if( m_pSndPlayInfo->GetLoopMode() != I3SND_RT_LOOPMODE_LOOPING)
			{
				DeactivateNode();
			}
		}

	}
}

void i3SoundNode::OnChangeTime( REAL32 tm)
{
	if( tm == 0.0f)
	{
		_StopSound(m_pSndPlayInfo->isStopOnReset());
	}

	m_timeAccm = tm;

	//I3TRACE("ONCHANGETIME: %s, %f\n", m_pSndPlayInfo->getSound()->GetName(), tm);

	i3Node::OnChangeTime( tm);
}

void i3SoundNode::OnActivate(void)
{
	//I3TRACE("OnActivate: %s\n", m_pSndPlayInfo->getSound()->GetName());

	i3Node::OnActivate();
}

void i3SoundNode::OnDeactivate(void)
{
	//I3TRACE("ONDEACTIVATE: %s\n", m_pSndPlayInfo->getSound()->GetName());
	_StopSound( true);

	Reset();

	i3Node::OnDeactivate();
}

void i3SoundNode::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pSndPlayInfo != nullptr)
	{
		m_pSndPlayInfo->OnBuildObjectList( List);
	}

	i3Node::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ALIGN8 SOUND_NODE
	{
		INT8		m_ID[4] = { 'S', 'N', 'D', '1' };
		OBJREF		m_pSndPlayInfo = 0;
		REAL32		m_timeStart = 0.0f;
		UINT32		m_Style = 0;
		UINT32		m_LoopCount = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3SoundNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SOUND_NODE data;

	Result = i3Node::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	data.m_pSndPlayInfo = (OBJREF)pResFile->GetObjectPersistID( getSoundPlayInfo());
	data.m_timeStart = getStartTime();
	data.m_Style = getStyle();
	data.m_LoopCount = getLoopCount();

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data.");
		return STREAM_ERR;
	}

	return Result;
}

UINT32 i3SoundNode::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::SOUND_NODE data;

	Result = i3Node::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( data.m_pSndPlayInfo != 0)
	{
		i3SoundPlayInfo * pSoundInfo = (i3SoundPlayInfo *) pResFile->FindObjectByID( data.m_pSndPlayInfo);

		I3ASSERT( pSoundInfo != nullptr);

		setSoundPlayInfo( pSoundInfo, pResFile);
	}

	setStartTime( data.m_timeStart);
	setStyle( data.m_Style);
	setLoopCount( data.m_LoopCount);

	return Result;
}

bool i3SoundNode::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3Node::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->addAttr("SoundPlayInfoID", (OBJREF)pFile->GetObjectPersistID(getSoundPlayInfo()));
	pXML->addAttr("StartTime", getStartTime());
	pXML->addAttr("Style", getStyle());
	pXML->addAttr("LoopCount", getLoopCount());

	return true;
}

bool i3SoundNode::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3Node::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	OBJREF64 temp;
	pXML->getAttr("SoundPlayInfoID", &temp);
	if (temp != 0)
	{
		i3SoundPlayInfo * pSoundInfo = (i3SoundPlayInfo *)pFile->FindObjectByID((OBJREF)temp);

		I3ASSERT(pSoundInfo != nullptr);

		setSoundPlayInfoFromXML(pSoundInfo, pFile);
	}

	REAL32 starttime;
	pXML->getAttr("StartTime", &starttime);
	setStartTime(starttime);

	UINT32 style;
	pXML->getAttr("Style", &style);
	setStyle(style);

	UINT32 loopcount;
	pXML->getAttr("LoopCount", &loopcount);
	setLoopCount(loopcount);

	return true;
}

void i3SoundNode::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Node::CopyTo( pDest, method);

	i3SoundNode * pSnd = (i3SoundNode *) pDest;

	{
		i3SoundPlayInfo * pInfo = i3SoundPlayInfo::new_object_ref();
		getSoundPlayInfo()->CopyTo( pInfo, I3_COPY_INSTANCE);

		pSnd->setSoundPlayInfo( pInfo);
	}

	pSnd->setStartTime( getStartTime());
	pSnd->setStyle( getStyle());
	pSnd->setLoopCount( getLoopCount());
}
