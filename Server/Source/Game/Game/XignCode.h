#ifndef __XIGNCODE_H__
#define __XIGNCODE_H__

#include "XignCode\\xtypes.h"
#include "XignCode\\zwave_sdk_errors.h"
#include "XignCode\\zwave_sdk_helper.h"



class CXignCode
{
	IXigncodeServer2*		m_pXignServer;

public:
	
	CXignCode();
	virtual ~CXignCode(); 


	BOOL					OnCreate(char* strPath);	
	void					OnDestroy(void);
	void					Release(void);
	void					Connect(T_UID TUID, INT32 i32SessionIdx, UINT32 ui32IpAddress, char* strID);	
	void					DisConnect(T_UID TUID);	
	void					Recive(T_UID TUID, const char* pData, INT16 i16DataSize );	

};

#endif