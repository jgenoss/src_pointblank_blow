#ifndef __LOGINWORKER_NORTHA_STEAM_H__
#define __LOGINWORKER_NORTHA_STEAM_H__

#include "LoginHTTP.h"

class CLoginWorker_NorthA_Steam : public CLoginHTTP
{
protected:
	BOOL					_WorkProcess( LOGIN_INPUT * pInput, LOGIN_OUTPUT* pOutput );
	BOOL					_ParsingLogInBySteam( char * pstrString, LOGIN_OUTPUT * pReadData );
	BOOL					_ParsingLogInByWeb( char * pstrString, LOGIN_OUTPUT * pReadData );

	i3NetworkHTTP*			m_pHttpSteam;

	void					_ConvertHexKey( char* pstrOut, INT32 i32OutLength, char* pstrIn, INT32 i32InLength );

public:
	CLoginWorker_NorthA_Steam();
	virtual ~CLoginWorker_NorthA_Steam();

	virtual BOOL			OnCreate( INT32 i32Idx, UINT32 ui32BufferMaxCount, UINT8 ui8MethodIdx );
};

#endif	// __LOGINWORKER_NORTHA_H__