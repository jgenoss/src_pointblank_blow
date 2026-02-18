#include "i3CommonType.h"
#include "i3Timer.h"
#include "i3String.h"
#include "i3Stream.h"
#include "i3RealTimeProfile.h"

#if !defined( I3_NO_PROFILE)

I3_CLASS_INSTANCE( i3RealTimeProfileNode, i3ElementBase);
I3_CLASS_INSTANCE( i3RealTimeProfileSample, i3ElementBase);

inline void ProfileTime_GetTicks( INT64 * ticks)
{
#if defined( I3_WINDOWS)
	//__asm
	//{
	//	push edx;
	//	push ecx;
	//	mov ecx,ticks;
	//	_emit 0Fh
	//	_emit 31h
	//	mov [ecx],eax;
	//	mov [ecx+4],edx;
	//	pop ecx;
	//	pop edx;
	//}
#else
	*ticks = 0;
#endif
}

inline REAL32 ProfileTime_GetTickRate(void)
{
	static REAL32 _CPUFrequency = -1.0f;

#if defined( I3_WINDOWS)
	if (_CPUFrequency == -1.0f) {
		INT64 curr_rate = 0;
		::QueryPerformanceFrequency ((LARGE_INTEGER *)&curr_rate);
		_CPUFrequency = (REAL32)curr_rate;
	} 
#endif

	return _CPUFrequency;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class i3RealTimeProfileManager
{
public:
	i3RealTimeProfileManager( void);
	~i3RealTimeProfileManager( void);
	
	void		StartProfile( const char * pszName);
	void		StopProfile( void);

	void		Destroy( void);
	void		Reset( void);
	void		Drive( void);
	void		IncrementFrameCounter( void)		{ m_nFrameCounter++;			}

	//INT32		GetFrameCountSinceReset( void)		{ return m_nFrameCounter;		}
	//REAL32		GetTimeSinceReset( void);

	i3RealTimeProfileNode * GetRootNode( void)			{ return m_pRootNode;			}
	i3RealTimeProfileNode * GetCurrentNode( void)		{ return m_pCurrentNode;		}
	void					SetFPS( INT32 fps)			{ m_nFPS = fps;					}
	INT32					GetFPS( void)				{ return m_nFPS;				}
	INT32					GetTotalNodeCount( void)	{ return m_nNodeCounter;		}

	void		StartRoot( void);
	void		StopRoot( void);

	void		OutputTrace( void);
	BOOL		OutputFile( const char * pszFile);

	//	매 프레임 프로파일링 종료후 결과를 콜백한다.
	void		SetReportCallback( REAL_PROFILE_CALLBACK pFunc)		{ m_pReportCallback = pFunc;			}
	
private:
	i3RealTimeProfileNode *		m_pRootNode;
	i3RealTimeProfileNode *		m_pCurrentNode;

	INT32						m_nFPS;
	INT32						m_nFrameCounter;
	INT32						m_nNodeCounter;

	INT64						m_ResetTime;	

	BOOL						m_bRootOpend;

	REAL_PROFILE_CALLBACK		m_pReportCallback;		//	처리 결과를 반환할 콜백함수

	void			_CalcUsageRate( void);
};

static i3RealTimeProfileManager 		g_RealTimeProfileManager;

I3_EXPORT_BASE
void RealTimeProfile_Destroy()
{
	g_RealTimeProfileManager.Destroy();
}

I3_EXPORT_BASE
void RealTimeProfile_Reset( void)
{
	g_RealTimeProfileManager.Reset();
}

I3_EXPORT_BASE
void RealTimeProfile_StartRoot( void)
{		
	g_RealTimeProfileManager.StartRoot();
}

I3_EXPORT_BASE
void RealTimeProfile_StopRoot( void)
{
	g_RealTimeProfileManager.StopRoot();
}

I3_EXPORT_BASE
void RealTimeProfile_Start( const char * pszName)
{
	g_RealTimeProfileManager.StartProfile( pszName);
}

I3_EXPORT_BASE
void RealTimeProfile_Stop( void)
{
	g_RealTimeProfileManager.StopProfile();
}

I3_EXPORT_BASE
REAL_PROFILE_NODE * RealTimeProfile_GetRoot( void)
{
	return g_RealTimeProfileManager.GetRootNode();
}

I3_EXPORT_BASE
void RealTimeProfile_SetFPS( INT32 fps)
{
	g_RealTimeProfileManager.SetFPS( fps);
}

I3_EXPORT_BASE
INT32 RealTimeProfile_GetFPS( void)
{
	return g_RealTimeProfileManager.GetFPS();
}

I3_EXPORT_BASE
INT32 RealTImeProfile_GetTotalNodeCount( void)
{
	return g_RealTimeProfileManager.GetTotalNodeCount();
}

I3_EXPORT_BASE
void RealTimeProfile_SetReportCB( REAL_PROFILE_CALLBACK pFunc)
{
	g_RealTimeProfileManager.SetReportCallback( pFunc);
}

I3_EXPORT_BASE 
void RealTimeProfile_OutputTrace( void)
{
	g_RealTimeProfileManager.OutputTrace();
}

I3_EXPORT_BASE
BOOL RealTImeProfile_OutputFile( const char * pszFile)
{
	return g_RealTimeProfileManager.OutputFile( pszFile);
}

////////////////////////////////////////////////////////////////////////////////////////////

i3RealTimeProfileNode::i3RealTimeProfileNode( void)
{
	m_pszName = NULL;
	m_calcTotalCalls = 0;
	m_StartTime = 0;
	m_TotalTime = 0;
	m_fcalcUsageRate = m_fcalcTotalUsageRate = 0.0f;
	m_RecursionCounter = 0;

	m_Style = 0;
	
	m_pParent = NULL;

	m_fStartProfileTime = 0;
	m_fTotalProfileTime = 0;

	i3mem::FillZero( &m_sHistory, sizeof(m_sHistory));
}

i3RealTimeProfileNode::~i3RealTimeProfileNode( void)
{	
	ReleaseChild();
}

void i3RealTimeProfileNode::Set( const char * pszName, i3RealTimeProfileNode * pParent)
{
	m_pszName = pszName;
	m_pParent = pParent;

	Reset();
}

void i3RealTimeProfileNode::ReleaseChild( void)
{
	//	등록되어 있는 노드들은 모두 제거한다.
	INT32 count = m_ChildList.GetCount();
	for( INT32 i = 0; i < count; i++)
	{
		i3RealTimeProfileNode * pNode = (i3RealTimeProfileNode *) m_ChildList.GetItem( i);
		I3_SAFE_RELEASE( pNode);
	}

	m_ChildList.Clear();
}

void i3RealTimeProfileNode::_SetHistory( void)
{
	m_sHistory.m_pszName = m_pszName;
	m_sHistory.m_TotalCalls = m_calcTotalCalls;
	m_sHistory.m_fUsageRate = m_fcalcUsageRate;
	m_sHistory.m_fTotalUsageRate = m_fcalcTotalUsageRate;
	m_sHistory.m_fProfileTime = (REAL32) (1.0e-6 * m_fTotalProfileTime);
	//m_sHistory.m_fProfileTime = m_fTotalProfileTime;
}

void i3RealTimeProfileNode::Trace( void)
{
	i3RealTimeProfileNode * pParent = GetParent();

	char szOutput[256] = "";
	char szTemp[256] = "";
	while( pParent != NULL)
	{
		i3String::Concat( szOutput, " ");

		pParent = pParent->GetParent();
	}

	if( GetParent() != NULL)
	{	
		i3String::Concat( szOutput, "- ");
	}

	sprintf( szTemp, "[ %s ] parent(%.2f%%), total(%.2f%%), time(%.4fms), call(%d)\n", m_sHistory.m_pszName, m_sHistory.m_fUsageRate, m_sHistory.m_fTotalUsageRate, m_sHistory.m_fProfileTime, m_sHistory.m_TotalCalls);
	i3String::Concat( szOutput, szTemp);

	I3TRACE( szOutput);

	i3RealTimeProfileNode * pChild = NULL;
	for( INT32 i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);
		pChild->Trace();
	}
}

BOOL i3RealTimeProfileNode::Trace( i3FileStream * file)
{
	BOOL ret = TRUE;
	i3RealTimeProfileNode * pParent = GetParent();

	char szOutput[256] = "";
	char szTemp[256] = "";
	while( pParent != NULL)
	{
		i3String::Concat( szOutput, " ");

		pParent = pParent->GetParent();
	}

	if( GetParent() != NULL)
	{	
		i3String::Concat( szOutput, "- ");
	}

	sprintf( szTemp, "[ %s ] parent(%.2f%%), total(%.2f%%), time(%.4fms), call(%d)\n", m_sHistory.m_pszName, m_sHistory.m_fUsageRate, m_sHistory.m_fTotalUsageRate, m_sHistory.m_fProfileTime, m_sHistory.m_TotalCalls);
	i3String::Concat( szOutput, szTemp);

	if( file->WriteLine( szOutput) <= 0)
		ret = FALSE;

	i3RealTimeProfileNode * pChild = NULL;
	for( INT32 i = 0; i < GetChildCount(); i++)
	{
		pChild = GetChild( i);
		if( pChild->Trace( file) <= 0)
			ret = FALSE;
	}

	return ret;
}

void i3RealTimeProfileNode::Reset( void)
{
	_SetHistory();

	m_calcTotalCalls = 0;
	m_StartTime = 0;
	m_TotalTime = 0;
	m_fStartProfileTime = 0;
	m_fTotalProfileTime = 0;
	m_fcalcUsageRate = m_fcalcTotalUsageRate = 0.0f;

	for( INT32 i = 0; i < m_ChildList.GetCount(); i++)
	{	//	등록되어 있는 노드는 모두 초기화한다.
		i3RealTimeProfileNode * pNode = (i3RealTimeProfileNode *) m_ChildList.GetItem( i);

		pNode->Reset();
	}
}

i3RealTimeProfileNode *	i3RealTimeProfileNode::AddChild( const char * pszName, INT32 * count)
{
	//	이미 등록되어 있는 노드가 있는지 검색한다.
	i3RealTimeProfileNode * pNewNode = GetChild( pszName);

	if( pNewNode != NULL)		
		return pNewNode;

	//	등록되어 있지 않는 노드라면 새로운 노드를 만들어서 추가한다.
	pNewNode = i3RealTimeProfileNode::NewObject();
	pNewNode->Set( pszName, this);

	m_ChildList.Add( (void *) pNewNode);

	//	노드가 생성됐으면 카운트 증가
	if( count != NULL)		*count = *count + 1;

	return pNewNode;
}

i3RealTimeProfileNode *	i3RealTimeProfileNode::GetChild( const char * pszName)
{
	for( INT32 i = 0; i < m_ChildList.GetCount(); i++)
	{
		i3RealTimeProfileNode * pNode = (i3RealTimeProfileNode *) m_ChildList.GetItem( i);

		if( pNode->GetName() == pszName)
			return pNode;
	}

	return NULL;
}

i3RealTimeProfileNode * i3RealTimeProfileNode::GetChild( INT32 index)
{
	i3RealTimeProfileNode * pNode = NULL;

	if( m_ChildList.GetCount() > index)
		pNode = (i3RealTimeProfileNode *) m_ChildList.GetItem( index);

	return pNode;
}

i3RealTimeProfileNode * i3RealTimeProfileNode::GetFirstChild( void)
{
	if( m_ChildList.GetCount() > 0)
	{
		return GetChild(0);
	}
	
	return NULL;
}

i3RealTimeProfileNode * i3RealTimeProfileNode::GetLastChild( void)
{
	INT32 count = m_ChildList.GetCount();
	if( count > 0)
	{
		return GetChild( count - 1);
	}
	
	return NULL;
}

void i3RealTimeProfileNode::Call( void)
{
	m_calcTotalCalls++;

	//	재귀 호출이 아닐때 테스트 시작
	if( m_RecursionCounter == 0)
	{
		m_StartTime = i3Timer::GetPerformanceTime();

		m_fStartProfileTime = i3Timer::GetProfileTime();

		m_RecursionCounter++;
	}
}

BOOL i3RealTimeProfileNode::Return( void)
{
	m_RecursionCounter--;

	// 재귀 호출이 아닐때 테스트 종료
	if( m_RecursionCounter == 0 && m_calcTotalCalls != 0)
	{
		m_TotalTime += i3Timer::GetPerformanceTime() - m_StartTime;

		{
			INT64 time;
			ProfileTime_GetTicks( &time);

			m_fTotalProfileTime += i3Timer::GetProfileTime() - m_fStartProfileTime;
		}
	}

	return ( m_RecursionCounter == 0);
}

void i3RealTimeProfileNode::_CalcUsageRate( void)
{
	if( m_RecursionCounter > 0)
	{
		I3TRACE("[i3RealTimeProfileNode] waring! (%s) profile node is yet opened! Need to close.\n", GetName());
		return;
	}

	if( m_RecursionCounter < 0)
	{
		I3TRACE("[i3RealTimeProfileNode] waring! (%s) profile node is already closed! more over close.\n", GetName());
		return;
	}

	//	부모에 비례한 자신의 사용량
	if( GetParent() != NULL)
	{
		if( GetParent()->GetTotalTime() == 0)
			m_fcalcUsageRate = 0.0f;
		else
		{
			m_fcalcUsageRate = ( (REAL32) m_TotalTime / (REAL32) GetParent()->GetTotalTime()) * 100.0f;

			if( m_fcalcUsageRate < 0)		m_fcalcUsageRate = 0;
		}
	}
	else
	{
		m_fcalcUsageRate = 100.0f;		//	100(%)을 기준으로
	}

	//	Root에 비례한 자신의 사용량
	i3RealTimeProfileNode * pRoot = RealTimeProfile_GetRoot();
	if( pRoot != NULL)
	{
		if( pRoot->GetTotalTime() == 0)
			m_fcalcTotalUsageRate = 0.0f;
		else
		{
			m_fcalcTotalUsageRate = ( (REAL32) m_TotalTime / (REAL32) pRoot->GetTotalTime()) * 100.0f;

			if( m_fcalcTotalUsageRate < 0)		m_fcalcTotalUsageRate = 0;
		}
	}
	else
	{
		m_fcalcTotalUsageRate = 0.0f;
	}

	//	자신을 먼저 계산하고 자식들은 나중에 계산해야 한다.
	for( INT32 i = 0; i < m_ChildList.GetCount(); i++)
	{
		i3RealTimeProfileNode * pChild = (i3RealTimeProfileNode *) m_ChildList.GetItem( i);

		pChild->_CalcUsageRate();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////
i3RealTimeProfileManager::i3RealTimeProfileManager( void)
{
	//m_RootNode.Set( "Root", NULL);
	//m_pCurrentNode = &m_RootNode;
	m_pRootNode = NULL;
	m_pCurrentNode = NULL;

	m_nFPS = -1;
	m_nFrameCounter = 0;
	m_nNodeCounter = 0;
	m_ResetTime = 0;

	m_pReportCallback = NULL;
	m_bRootOpend = FALSE;
}

i3RealTimeProfileManager::~i3RealTimeProfileManager( void)
{
	Destroy();
}

void i3RealTimeProfileManager::Destroy( void)
{
	I3_SAFE_RELEASE( m_pRootNode);
}

void i3RealTimeProfileManager::Reset( void)
{
	if( m_pRootNode == NULL)
	{
		I3TRACE( "[i3RealTimeProfileManager] Not set root node.\n");
		return;
	}

	m_pRootNode->Reset();

	m_nFrameCounter = 0;
	m_ResetTime = i3Timer::GetProfileTime();
}

void i3RealTimeProfileManager::StartRoot( void)
{
	if( m_bRootOpend)
	{
		//I3TRACE( "[i3RealTimeProfileManager] warning! Already root node opened. \n\tRoot open is must unique on this current frame.\n");
		return;
	}

	m_bRootOpend = TRUE;

	if( m_pRootNode == NULL)
	{
		m_pRootNode = i3RealTimeProfileNode::NewObject();
		m_pRootNode->Set( "Root", NULL);

		m_nNodeCounter++;
	}

	m_pCurrentNode = m_pRootNode;
	m_pRootNode->Call();
}

void i3RealTimeProfileManager::StopRoot( void)
{
	if( !m_bRootOpend || m_pRootNode == NULL)
	{
		//I3TRACE( "[i3RealTimeProfileManager] Not set root node.\n");
		return;
	}
	
	m_pRootNode->Return();

	Drive();

	m_bRootOpend = FALSE;
}

void i3RealTimeProfileManager::StartProfile( const char * pszName)
{
	if( m_pRootNode == NULL)
	{
		//I3TRACE( "[i3RealTimeProfileManager] Not set root node.\n");
		return;
	}

	if( pszName != m_pCurrentNode->GetName())
	{
		m_pCurrentNode = m_pCurrentNode->AddChild( pszName, &m_nNodeCounter);
	}

	m_pCurrentNode->Call();
}

void i3RealTimeProfileManager::StopProfile( void)
{
	if( m_pCurrentNode == NULL)
	{
		//I3TRACE( "[i3RealTimeProfileManager] Invalid return node or over closed.\n");
		return;
	}

	if( m_pCurrentNode->Return() == TRUE)
	{
		m_pCurrentNode = m_pCurrentNode->GetParent();
	}
}

void i3RealTimeProfileManager::Drive( void)
{
	//IncrementFrameCounter();	

	_CalcUsageRate();
	
	//	프레임마다 결과 값을 초기화한다
	Reset(); 
		
	//	매 프레임 프로파일링 종료후 결과를 콜백함수에 전달한다.
	if( m_pReportCallback != NULL)	
		m_pReportCallback( m_pRootNode);	
}

void i3RealTimeProfileManager::_CalcUsageRate( void)
{
	i3RealTimeProfileNode * pNode = GetRootNode();

	pNode->_CalcUsageRate();
}

void i3RealTimeProfileManager::OutputTrace( void)
{
	i3RealTimeProfileNode * pNode = GetRootNode();

	I3TRACE( "=========================================================\n");

	//	print to debug output
	pNode->Trace();

	I3TRACE( "=========================================================\n");
}

BOOL i3RealTimeProfileManager::OutputFile( const char * pszFile)
{
	i3FileStream file;
	
	if( !file.Create( pszFile, STREAM_WRITE))
	{
		I3TRACE( "[REAL_PROFILE] Warning! Cannot create file (%s)\n", pszFile);
		return FALSE;
	}

	if( !file.Open( pszFile, STREAM_WRITE))
	{
		I3TRACE( "[REAL_PROFILE] Warning! Cannot open file (%s)\n", pszFile);
		return FALSE;
	}

	i3RealTimeProfileNode * pNode = GetRootNode();

	//	Write a text in node to opend file
	if( !pNode->Trace( &file))
	{
		I3TRACE( "[REAL_PROFILE] Warning! Cannot write file (%s)\n", pszFile);
		file.Close();
		return FALSE;
	}
	
	if( !file.Close())
	{
		I3TRACE( "[REAL_PROFILE] Warning! Cannot close file (%s)\n", pszFile);
		return FALSE;
	}

	I3TRACE("[REAL_PROFILE] File save OK. (%s)\n", pszFile);

	return TRUE;
}

/*
REAL32 i3RealTimeProfileManager::GetTimeSinceReset( void)
{
	INT64 time;
	//time = i3Timer::GetProfileTime();
	_profileGetTicks( &time);
	time -= m_ResetTime;

	//return (REAL32) time;
	return (REAL32) time / _profileGetTickRate();
}
*/

////////////////////////////////////////////////////////////////////////////////////////////
#endif


