#if !defined( __I3_THREAD_H)
#define __I3_THREAD_H

#include "i3CommonType.h"
#include "i3ElementBase.h"
#include "i3MetaTemplate.h"

#if defined( I3_PSP)
#include <thread.h>
#endif

#define  THREAD_RUNNING			0x0001
#define  THREAD_SUSPENDED		0x0002
#define  THREAD_STOPPED			0x0003
#define  THREAD_WAIT			0x0004

#define PRIORITY_HIGHEST		1
#define PRIORITY_HIGH			2
#define PRIORITY_NORMAL			3
#define PRIORITY_LOW			4
#define PRIORITY_LOWEST			5
#define PRIORITY_TIME_CRITICAL	6

#if defined( I3_PSP)
	#define		I3_THREAD_ATTR_VFPU		SCE_KERNEL_TH_USE_VFPU
	
	class i3Thread;
	typedef struct
	{
		i3Thread *p;
	} I3_PSP_THREAD_USERDATA;
#endif

class I3_EXPORT_BASE i3Thread : public i3ElementBase
{
	I3_CLASS_DEFINE( i3Thread);

protected:
#if defined( I3_WINDOWS) || defined( I3_WINCE )

	HANDLE m_Handle;
	UINT32 m_ThreadID;

#elif defined( I3_PSP)

	SceUID	m_Handle;
	I3_PSP_THREAD_USERDATA m_UserData;

#endif

	static i3Thread * s_pThreadHead;
	i3Thread *	m_pNext;			///  내부 관리용 멤버 (사용하지마!!!)

	char	m_szName[64];
	UINT32 m_State;
	UINT32	m_StackSize;
	UINT32	m_Attribute;
	INT32  m_Priority;
	void * m_Parameter;

public:
	UINT32  m_ReturnCode;

	i3Thread(void);
	virtual ~i3Thread(void);

	virtual BOOL Create( char * pszName, UINT32 Attr = 0, UINT32 StackSize = 4096, void * pUserData = NULL, INT32 priority = PRIORITY_NORMAL);  

	virtual BOOL Suspend(void);
	virtual BOOL Resume(void);
	virtual BOOL Terminate( bool bWait = false);
	virtual UINT32 WaitForFinish(void);

	virtual BOOL SetPriority(INT32 priority);

	void *	GetParam(void)			{	return m_Parameter; }
	
	UINT32	GetState(void)			{	return m_State; }
	UINT32	GetReturnCode(void)		{	return m_ReturnCode; }
	INT32	GetPriority(void)		{	return m_Priority; }

	virtual void  BeforeRunning( void * pUserData);
	virtual UINT32 OnRunning( void * /*pUserData*/)			{ return 0; }
	virtual void  AfterRunning( void * pUserData);

	static i3Thread *	GetFirstThread(void)				{ return s_pThreadHead; }

	inline UINT32		GetThreadID()						{ return m_ThreadID;	}
};

#endif
