#include "i3CommonType.h"
#include "i3TimeEventGen.h"
#include "i3EventDef.h"

I3_CLASS_INSTANCE( i3TimeEventGen, i3NamedElement);

static INT32	_TimeEventCompareProc( i3TimeEventInfo * p1, i3TimeEventInfo * p2)
{
	if( p1->getStartTime() > p2->getStartTime())
		return 1;
	else if( p1->getStartTime() < p2->getStartTime())
		return -1;
	
	return 0;
}

i3TimeEventGen::i3TimeEventGen(void)
{
	m_Style = 0;
	m_timeLength = 0.0f;
	m_pReceiver = NULL;

	m_EventList.SetCompareProc( (COMPAREPROC) _TimeEventCompareProc);

	m_bInfinite = FALSE;

	Reset();
}

i3TimeEventGen::~i3TimeEventGen(void)
{
	RemoveAllEvents();
}

void i3TimeEventGen::RemoveAllEvents(void)
{
	UINT32 i;
	i3TimeEventInfo * pEvent;

	for( i = 0; i < getEventCount(); i++)
	{
		pEvent = getEvent( i);
		pEvent->Release();
	}

	m_EventList.Clear();
}

void i3TimeEventGen::Reset(void)
{
	m_timeLocal = 0.0f;
	m_CurEvent = 0;
}

void i3TimeEventGen::_UpdateDuration(void)
{
	UINT32 i;
	i3TimeEventInfo * pInfo;

	m_timeLength = 0.0f;

	m_bInfinite = FALSE;

	for( i = 0; i < getEventCount(); i++)
	{
		pInfo = (i3TimeEventInfo *) getEvent( i);

		if( pInfo->getDuration() <= 0.0f)
			m_bInfinite = TRUE;

		m_timeLength = MAX( m_timeLength, pInfo->getDuration() + pInfo->getStartTime());
	}
}

INT32 i3TimeEventGen::AddEvent( i3TimeEventInfo * pInfo)
{
	I3ASSERT( pInfo != NULL);

	m_timeLength = MAX( m_timeLength, pInfo->getStartTime() + pInfo->getDuration());

	pInfo->AddRef();

	return m_EventList.Add( pInfo);
}

void i3TimeEventGen::RemoveEvent( INT32 idx)
{
	i3TimeEventInfo * pInfo = getEvent( idx);

	pInfo->Release();
	m_EventList.Delete( idx);

	_UpdateDuration();
}

void i3TimeEventGen::RemoveEvent( i3TimeEventInfo * pInfo)
{
	INT32 idx = GetEventIndex( pInfo);
	I3ASSERT( idx != -1);

	RemoveEvent( idx);
}

void i3TimeEventGen::SortEvents(void)
{
	m_EventList.Sort( (COMPAREPROC) _TimeEventCompareProc);
}

void i3TimeEventGen::Stop(void)
{
	INT32 i;
	bool bOwnProcess;

	for( i = 0; i < m_CurEvent; i++)
	{
		i3TimeEventInfo * pInfo = getEvent( i);

		bOwnProcess = false;

		if( pInfo->getAction() != NULL)
		{
			i3Action * pAction = pInfo->getAction();

			if( pAction->getStyle() & I3_ACTION_STYLE_OWNPROCESS)
			{
				pAction->OnEvent( m_pReceiver, 0.0f, I3_EVT_CODE_DEACTIVATE);
				bOwnProcess = true;
			}
		}

		if( bOwnProcess == false)
		{
			if( m_pReceiver != NULL)
			{
				m_pReceiver->OnEvent( I3_EVT_ACTION, pInfo->getAction(), pInfo->getParam(), I3_EVT_CODE_DEACTIVATE);
			}
		}
	}
}

void i3TimeEventGen::OnUpdate( REAL32 tm)
{
	i3TimeEventInfo * pEvent;
	bool bOwnProcess;

	if( m_timeLocal > m_timeLength)
	{
		// 시간이 만료되었다면 그냥 종료.
		// REPEAT Mode인 경우에는 미리 Local Time을 0으로 초기화하기 때문에
		// 이 경우에 걸리지 않음.
		return;
	}

	m_timeLocal += tm;

	if( m_CurEvent < (INT32) getEventCount())
	{
		pEvent = getEvent( m_CurEvent);

		while((m_CurEvent < (INT32) getEventCount()) && (pEvent->getStartTime() <= m_timeLocal))
		{
			bOwnProcess = false;

			// 시간이 만료된 Event들을 발생시켜준다.
			if( pEvent->getAction() != NULL)
			{
				i3Action * pAction = pEvent->getAction();

				if( pAction->getStyle() & I3_ACTION_STYLE_OWNPROCESS)
				{
					pEvent->getAction()->OnEvent( m_pReceiver, m_timeLocal, I3_EVT_CODE_ACTIVATE);
					bOwnProcess = true;
				}
			}
			
			if( bOwnProcess == false)
			{
				if( m_pReceiver != NULL)
				{
					m_pReceiver->OnEvent( I3_EVT_ACTION, pEvent->getAction(), pEvent->getParam(), I3_EVT_CODE_ACTIVATE);
				}
			}

   			m_CurEvent++;
			pEvent = getEvent( m_CurEvent);
		}
	}

	if( m_timeLocal > m_timeLength)
	{
		if( isStyle( I3_TIMEEVENT_REPEAT))
			Reset();
		else
			m_timeLocal = m_timeLength;
	}
}

void i3TimeEventGen::OnChangeTime( REAL32 tm)
{
	i3TimeEventInfo * pEvent;
	bool bOwnProcess;

	if( m_CurEvent == -1)
		return;

	if( m_timeLocal >= tm)
	{
		// 되돌리는 경우라면 처음부터 다시 Event들을 실행해준다.
		m_CurEvent = 0;

		for( UINT32 i = 0; i < getEventCount(); ++i)
		{
			i3TimeEventInfo * pEvInfo = getEvent( i);

			if( pEvInfo->getStartTime() >= tm)
			{
				i3Action * pAction = pEvInfo->getAction();

				pAction->OnChangeTime( tm);
			}
		}

		return;
	}

	pEvent = getEvent( m_CurEvent);

	m_timeLocal = tm;

	while((m_CurEvent < (INT32) getEventCount()) && (pEvent->getStartTime() <= tm))
	{
		bOwnProcess = false;

		// 시간이 만료된 Event들을 발생시켜준다.
		if( pEvent->getAction() != NULL)
		{
			i3Action * pAction = pEvent->getAction();

			if( pAction->getStyle() & I3_ACTION_STYLE_OWNPROCESS)
			{
				pEvent->getAction()->OnEvent( m_pReceiver, m_timeLocal, I3_EVT_CODE_ACTIVATE);
				bOwnProcess = true;
			}
		}
		
		if( bOwnProcess == false)
		{
			if( m_pReceiver != NULL)
			{
				m_pReceiver->OnEvent( I3_EVT_ACTION, pEvent->getAction(), pEvent->getParam(), I3_EVT_CODE_ACTIVATE);
			}
		}

   		m_CurEvent++;
		pEvent = getEvent( m_CurEvent);
	}
}

void	i3TimeEventGen::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo( pDest, method);

	I3ASSERT( pDest->IsTypeOf( i3TimeEventGen::GetClassMeta()));
	i3TimeEventGen * pGen = (i3TimeEventGen *) pDest;

	pGen->setStyle( getStyle());
	pGen->setDuration( getDuration());
	pGen->setEventReceiver( getEventReceiver());

	UINT32 i;
	for( i = 0; i < getEventCount(); i++)
	{
		i3TimeEventInfo * pSrcInfo = getEvent( i);
		if( pSrcInfo != NULL)
		{
			i3TimeEventInfo * pInfo = i3TimeEventInfo::NewObjectRef();

			pSrcInfo->CopyTo( pInfo, method);

			pGen->AddEvent( pInfo);
		}
	}
}

void	i3TimeEventGen::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	UINT32 i;
	i3TimeEventInfo * pInfo;

	for( i = 0; i < getEventCount(); i++)
	{
		pInfo = getEvent( i);

		pInfo->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct _tagi3PersistTimeEventGen
{
	UINT8		m_ID[4];
	UINT32		m_Style;
	REAL32		m_timeLength;
	UINT32		pad[4];
} I3_PERSIST_TIMEEVENTGEN;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TimeEventGen::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_TIMEEVENTGEN data;

	result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "TEG1", 4);
	data.m_Style = getStyle();
	data.m_timeLength = getDuration();

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	rc = m_EventList.SaveTo( pResFile);
	I3_CHKIO( rc);
	result += rc;
	
	return result;
}

UINT32 i3TimeEventGen::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	I3_PERSIST_TIMEEVENTGEN data;

	result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setStyle( data.m_Style);
	m_timeLength = data.m_timeLength;

	rc = m_EventList.LoadFrom( pResFile, true);
	I3_CHKIO( rc);
	result += rc;

	return result;
}
