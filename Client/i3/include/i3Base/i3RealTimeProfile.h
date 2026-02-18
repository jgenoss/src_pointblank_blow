#ifndef __I3_REAL_TIME_PROFILE_H__
#define __I3_REAL_TIME_PROFILE_H__

#include "i3ElementBase.h"
#include "itl/vector.h"
#include "i3StreamFile.h"
//	Real-Time Hierarchical Profiling


class i3RealTimeProfileNode;
class i3RealTimeProfileSample;

#define REAL_PROFILE_NODE		i3RealTimeProfileNode
#define	REAL_PROFILE_HISTORY	i3RealProfileHistory

//	결과 반환 콜백 함수 원형
typedef void (* REAL_PROFILE_CALLBACK) ( REAL_PROFILE_NODE * pRoot);

////////////////////////////////////////////////////////////////////////////////////////////
//	This is Profile system's macro
//	It's activating the profile system then I3_NO_PROFILE flag is Off. Flag On is not activating
//#if !defined( I3_NO_PROFILE)
#if 0

#define REAL_PROFILE_DESTROY()				RealTimeProfile_Destroy()

//	프로파일 Root 시작 (받드시 한번 호출해야 한다.)
#define REAL_PROFILE_START_ROOT()			RealTimeProfile_StartRoot()

//	프로파일 Root 종료 (받드시 한번 호출해야 한다.)
#define REAL_PROFILE_STOP_ROOT()			RealTimeProfile_StopRoot()

//	프로파일을 초기화 한다. 반드시할 필요 없음.
#define REAL_PROFILE_RESET()				RealTimeProfile_Reset()

//	콜백함수를 지정할 경우 매프레임마다 처리된 프로파일 Root Node를 반환한다.
#define REAL_PROFILE_SET_REPORT_CB( func)	RealTimeProfile_SetReportCB( func)

//	지역 프로파일링 (지역내 또는 함수내에서 작동하며 지역을 벗어날 경우 자동으로 닫는다. STOP을 호출해서는 안된다.)
#define REAL_PROFILE( name)					i3RealTimeProfileSample		__real_profile;	__real_profile.Start( name)									

//	프로파일링 시작 (STOP을 하지 않고 다시 호출할 경우 정상적으로 작동하지 않으며 에러 메시지를 출력한다.)
#define REAL_PROFILE_START( name)			RealTimeProfile_Start( name)

//	프로파일링 종료 ( 현재 열린 프로파일링을 닫는다)
#define REAL_PROFILE_STOP()					RealTimeProfile_Stop()	

#define REAL_PROFILE_SET_FPS( fps)			RealTimeProfile_SetFPS( fps)
#define	REAL_PROFILE_GET_FPS()				RealTimeProfile_GetFPS()
#define REAL_PROFILE_GET_ROOT()				RealTimeProfile_GetRoot()
#define REAL_PROFILE_GET_NODE_COUNT()		RealTImeProfile_GetTotalNodeCount()
#define REAL_PROFILE_TRACE()				RealTimeProfile_OutputTrace()
#define REAL_PROFILE_SAVE_FILE( filename)	RealTImeProfile_OutputFile( filename)

#else

	#if defined( I3_COMPILER_VC)
		#if defined( I3_COMPILER_EVC )
			#define REAL_PROFILE_DESTROY()				((void) 0)
			#define REAL_PROFILE_START_ROOT()			((void) 0)
			#define REAL_PROFILE_STOP_ROOT()			((void) 0)
			#define REAL_PROFILE_RESET()				((void) 0)
			#define REAL_PROFILE_SET_REPORT_CB( func)	((void) 0)
			#define REAL_PROFILE( name)					((void) 0)
			#define REAL_PROFILE_START( name)			((void) 0)
			#define REAL_PROFILE_STOP()					((void) 0)
			#define REAL_PROFILE_SET_FPS( fps)			((void) 0)
			#define REAL_PROFILE_GET_FPS()				-1
			#define REAL_PROFILE_GET_ROOT()				nullptr
			#define REAL_PROFILE_GET_NODE_COUNT()		0
			#define REAL_PROFILE_TRACE()				((void) 0)
			#define REAL_PROFILE_SAVE_FILE( filename)	0			
		#else
			#define REAL_PROFILE_DESTROY()				__noop
			#define REAL_PROFILE_START_ROOT()			__noop
			#define REAL_PROFILE_STOP_ROOT()			__noop
			#define REAL_PROFILE_RESET()				__noop
			#define REAL_PROFILE_SET_REPORT_CB( func)	__noop
			#define REAL_PROFILE( name)					__noop
			#define REAL_PROFILE_START( name)			__noop
			#define REAL_PROFILE_STOP()					__noop	
			#define REAL_PROFILE_SET_FPS( fps)			__noop
			#define REAL_PROFILE_GET_FPS()				-1
			#define REAL_PROFILE_GET_ROOT()				0
			#define REAL_PROFILE_GET_NODE_COUNT()		0
			#define REAL_PROFILE_TRACE()				__noop
			#define REAL_PROFILE_SAVE_FILE( filename)	0			
		#endif
	#elif defined( I3_COMPILER_GCC)// || defined( I3_COMPILER_EVC)
		#define REAL_PROFILE_DESTROY()				((void) 0)
		#define REAL_PROFILE_START_ROOT()			((void) 0)
		#define REAL_PROFILE_STOP_ROOT()			((void) 0)
		#define REAL_PROFILE_RESET()				((void) 0)
		#define REAL_PROFILE_SET_REPORT_CB( func)	((void) 0)
		#define REAL_PROFILE( name)					((void) 0)
		#define REAL_PROFILE_START( name)			((void) 0)
		#define REAL_PROFILE_STOP()					((void) 0)
		#define REAL_PROFILE_SET_FPS( fps)			((void) 0)
		#define REAL_PROFILE_GET_FPS()				-1
		#define REAL_PROFILE_GET_ROOT()				nullptr
		#define REAL_PROFILE_GET_NODE_COUNT()		0
		#define REAL_PROFILE_TRACE()				((void) 0)
		#define REAL_PROFILE_SAVE_FILE( filename)	0
	#endif

#endif

#if !defined( I3_NO_PROFILE)
////////////////////////////////////////////////////////////////////////////////////////////

I3_EXPORT_BASE void		RealTimeProfile_Destroy( void);
I3_EXPORT_BASE void		RealTimeProfile_Reset( void);
I3_EXPORT_BASE void		RealTimeProfile_StartRoot( void);
I3_EXPORT_BASE void		RealTimeProfile_StopRoot( void);
I3_EXPORT_BASE void		RealTimeProfile_Start( const char * pszName);
I3_EXPORT_BASE void		RealTimeProfile_Stop( void);
I3_EXPORT_BASE void		RealTimeProfile_SetReportCB( REAL_PROFILE_CALLBACK pFunc);
I3_EXPORT_BASE void		RealTimeProfile_SetFPS( INT32 fps);
I3_EXPORT_BASE INT32	RealTimeProfile_GetFPS( void);
I3_EXPORT_BASE INT32	RealTImeProfile_GetTotalNodeCount( void);
I3_EXPORT_BASE void		RealTimeProfile_OutputTrace( void);
I3_EXPORT_BASE bool		RealTImeProfile_OutputFile( const char * pszFile);
I3_EXPORT_BASE REAL_PROFILE_NODE * RealTimeProfile_GetRoot( void);


////////////////////////////////////////////////////////////////////////////////////////////
//	A node in the Profile Hierarchy Tree
//

#define		I3_REAL_PROFILE_USER_FLAG_MASK	0xFFFF0000
#define		I3_REAL_PROFILE_USER_FLAG		0x00010000

struct i3RealProfileHistory
{
	const char* m_pszName = nullptr; 
	INT32		m_TotalCalls = 0;			//	ProfileNode가 호출된 횟수
	REAL32		m_fUsageRate = 0.0f;			//	부모를 자원을 100을 기준으로 사용량(%)
	REAL32		m_fTotalUsageRate = 0.0f;		//	Root를 기준으로 한 사용률(%)	
	REAL32		m_fProfileTime = 0.0f;

} ;

class I3_EXPORT_BASE i3RealTimeProfileNode : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3RealTimeProfileNode, i3ElementBase);
protected:
	const char* m_pszName = nullptr;

	i3RealTimeProfileNode *	m_pParent = nullptr;
	i3RealProfileHistory	m_sHistory;

	UINT32		m_Style = 0;

private:
	i3::vector<i3RealTimeProfileNode*>	m_ChildList;

	INT64		m_StartTime = 0;
	INT64		m_TotalTime = 0;
	INT64		m_fStartProfileTime = 0;
	INT64		m_fTotalProfileTime = 0;

	INT32		m_RecursionCounter = 0;
	INT32		m_calcTotalCalls = 0;
	REAL32		m_fcalcUsageRate = 0.0f;
	REAL32		m_fcalcTotalUsageRate = 0.0f;
	

public:
	i3RealTimeProfileNode( void);
	~i3RealTimeProfileNode( void);

	INT32					GetChildCount( void) const		{ return INT32(m_ChildList.size());		}
	i3RealTimeProfileNode *	GetChild( const char * pszName);
	i3RealTimeProfileNode * GetChild( INT32 index);
	i3RealTimeProfileNode * GetParent( void)			{ return m_pParent;						}
	i3RealTimeProfileNode * GetFirstChild( void);
	i3RealTimeProfileNode * GetLastChild( void);
	i3RealTimeProfileNode *	AddChild( const char * pszName, INT32 * count = nullptr);
	void		ReleaseChild( void);

	UINT32		GetStyle( void)				{ return m_Style;				}
	void		AddStyle( UINT32 style)		{ m_Style |= style;				}
	void		RemoveStyle( UINT32 style)	{ m_Style &= ~style;			}
	
	void		Set( const char * pszName, i3RealTimeProfileNode * pParent);
	void		Reset( void);
	void		Call( void);
	bool		Return( void);	
	void		Trace( void);
	bool		Trace( i3FileStream * file);

	const char* GetName( void)				{ return m_pszName;					}
	INT64		GetTotalTime( void)			{ return m_TotalTime;				}
	i3RealProfileHistory * GetHistory( void)		{ return &m_sHistory;				}
	

	void		_CalcUsageRate( void);
	void		_SetHistory( void);
};

////////////////////////////////////////////////////////////////////////////////////////////
//	i3RealTimeProfileSample is a simple way to profile a function's scope
//	Use the REAL_PROFILE macro at the start of scope to time
//
class I3_EXPORT_BASE i3RealTimeProfileSample : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3RealTimeProfileSample, i3ElementBase);

public:
	~i3RealTimeProfileSample( void)
	{
		REAL_PROFILE_STOP();
	}

	void	Start( const char * pszName)		{ REAL_PROFILE_START( pszName);		}
};
////////////////////////////////////////////////////////////////////////////////////////////
#endif

#endif // __I3_REAL_TIME_PROFILE_H__
