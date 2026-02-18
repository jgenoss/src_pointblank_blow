#include "i3SceneDef.h"
#include "i3TimeSequenceInfo.h"
#include "i3Node.h"
#include "i3SceneUtil.h"

I3_CLASS_INSTANCE( i3TimeSequenceInfo);

i3TimeSequenceInfo::~i3TimeSequenceInfo(void)
{
	I3_SAFE_RELEASE( m_pNode);
}

void i3TimeSequenceInfo::SetNode( i3Node * pNode)
{
	I3_REF_CHANGE( m_pNode, pNode);
}

void i3TimeSequenceInfo::Start(void)
{
	if( m_pNode != nullptr)
	{
		m_pNode->OnChangeTime( 0.0f);
		m_pNode->ActivateNode();
	}

	SetState( GetState() | I3SG_TIMESEQ_STATE_ACTIVE);

}


void i3TimeSequenceInfo::Stop(void)
{
	if( m_pNode != nullptr)
	{
		if( !m_pNode->OnQueryDeactivateNode())
			return;
			
		m_pNode->DeactivateNode();
	}

	SetState( GetState() & ~I3SG_TIMESEQ_STATE_ACTIVE);

	m_loopCount =  0;
}

void i3TimeSequenceInfo::SetMaxLoopCount( INT32 cnt)		
{ 
	m_MaxLoopCount = cnt; 

	UINT32 nStyle = m_Style;

    if( m_MaxLoopCount > 1)		nStyle |= I3SG_TIMESEQ_STYLE_REPEAT;
	else						nStyle &= ~I3SG_TIMESEQ_STYLE_REPEAT;

	SetStyle( nStyle);
}

void i3TimeSequenceInfo::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_pNode != nullptr)
	{
		m_pNode->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

void i3TimeSequenceInfo::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	if( i3::same_of<i3TimeSequenceInfo*>(pObj))
	{
		i3TimeSequenceInfo * pDest = (i3TimeSequenceInfo *) pObj;

		pDest->SetStartTime( GetStartTime());
		pDest->SetDuration( GetDuration());
		pDest->SetStyle( GetStyle());
		pDest->setLoopCount( getLoopCount());

		if( method == I3_COPY_REF)
			pDest->SetNode( GetNode());
		else
		{
			if( GetNode() != nullptr)
			{
				i3Node * pNewNode;

				pNewNode = i3Scene::CreateClone( GetNode(), method);

				pDest->SetNode( pNewNode);
			}
		}
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct PACKED TIMESEQUENCEINFO
	{
		REAL32		m_StartTime = 0.0f;
		REAL32		m_Duration = 0.0f;
		UINT32		m_Style = 0;
		OBJREF		m_pNode = 0;
		INT32		m_MaxLoopCount = 0;
		char		m_Reserved[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TimeSequenceInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::TIMESEQUENCEINFO Info;
	
	Info.m_StartTime = GetStartTime();
	Info.m_Duration = GetDuration();
	Info.m_Style = GetStyle();
	Info.m_pNode = (OBJREF) pResFile->GetObjectPersistID( GetNode());
	Info.m_MaxLoopCount = getMaxLoopCount();

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TimeSequenceInfo::OnSave()", "Could not write tm sequence info.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3TimeSequenceInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::TIMESEQUENCEINFO Info;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TimeSequenceInfo::OnLoad()", "Could not read tm sequence info.");
		return STREAM_ERR;
	}
	Result += Rc;

	SetStartTime( Info.m_StartTime);
	SetDuration( Info.m_Duration);
	SetStyle( Info.m_Style);
	SetMaxLoopCount( Info.m_MaxLoopCount);

	if( Info.m_pNode != 0)
	{
		i3Node * pNode = (i3Node *) pResFile->FindObjectByID( Info.m_pNode);
		I3ASSERT( pNode != nullptr);

		SetNode( pNode);
	}

	return Result;
}

bool i3TimeSequenceInfo::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("StartTime", GetStartTime());
	pXML->addAttr("Duration", GetDuration());
	pXML->addAttr("Style", GetStyle());
	pXML->addAttr("MaxLoopCount", getMaxLoopCount());
	pXML->addAttr("NodeID", (OBJREF)pFile->GetObjectPersistID(GetNode()));

	return true;
}

bool i3TimeSequenceInfo::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	REAL32 starttime;
	pXML->getAttr("StartTime", &starttime);
	SetStartTime(starttime);
	
	REAL32 duration;
	pXML->getAttr("Duration", &duration);
	SetDuration(duration);
	
	UINT32 style;
	pXML->getAttr("Style", &style);
	SetStyle(style);
	
	INT32 maxloopcount;
	pXML->getAttr("MaxLoopCount", &maxloopcount);
	SetMaxLoopCount(maxloopcount);

	OBJREF64 temp;
	pXML->getAttr("NodeID", &temp);
	if (temp != 0)
	{
		i3Node * pNode = (i3Node *)pFile->FindObjectByID((OBJREF)temp);
		I3ASSERT(pNode != nullptr);

		SetNode(pNode);
	}
	return true;
}

bool i3TimeSequenceInfo::IsSame( i3TimeSequenceInfo * pDest)
{
	if( pDest->m_StartTime != m_StartTime)
		return false;

	if( pDest->m_Duration != m_Duration)
		return false;

	if( pDest->m_Style != m_Style)
		return false;

	if( pDest->m_MaxLoopCount != m_MaxLoopCount)
		return false;

	I3ASSERT( m_pNode != nullptr && pDest->m_pNode);

	if( pDest->m_pNode->kind_of( m_pNode->static_meta() ))
	{
		if( m_pNode->hasName() && pDest->m_pNode->hasName() &&
			i3::generic_is_iequal( pDest->m_pNode->GetNameString(), m_pNode->GetNameString()) == false)
		{
			return false;
		}
	}

	return true;
}
