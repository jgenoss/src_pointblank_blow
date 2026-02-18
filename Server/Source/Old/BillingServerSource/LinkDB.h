#ifndef __LINK_DB_H__
#define __LINK_DB_H__

#include "LinkBase.h"

class CADODatabase; 

class CLinkDB : public CLinkBase
{
	I3_CLASS_DEFINE( CLinkDB );

	char					m_strUdlFilePath[SERVER_STRING_COUNT];
	CADODatabase*			m_pDB;

public:
	CLinkDB();
	virtual ~CLinkDB();

	virtual BOOL			OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut, char* strFilePath );
	void					OnDestroy();

	void					ReconnectDB( char* strQuery );

	void					ConvertString(char * pOutStr, const char * pInStr)
	{
		INT32 InCount  = 0; 
		INT32 OutCount = 0; 
		while(pInStr[InCount] != 0)
		{
			if( pInStr[InCount] == 39 )//"'"
			{
				pOutStr[OutCount] = pInStr[InCount];
				OutCount++; 
			}
			pOutStr[OutCount] = pInStr[InCount];

			InCount++;
			OutCount++;

			if(CONVERT_STRING_COUNT <= OutCount)break; //이런일은 없지만 예외 처리입니다. 
		}
		pOutStr[OutCount] = 0; 
		return; 
	}
};

#endif