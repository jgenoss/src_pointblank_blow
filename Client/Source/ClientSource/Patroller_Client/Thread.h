/************************************************************************/
/*							Basic Thread Module							*/
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// Thread Class
//////////////////////////////////////////////////////////////////////////

#pragma once

class Thread
{
public:

private:
	HANDLE m_Handle;

public:
	Thread();
	virtual ~Thread();

	HANDLE					GetHandle(void)	const	{ return m_Handle; }

	virtual bool			CreateModule( void );  
	virtual UINT32			OnRunning(void)			{ return 0; }

private:

};