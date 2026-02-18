#ifndef __LOGINWORKER_ITALY_H__
#define __LOGINWORKER_ITALY_H__


#include "LoginDB.h"


typedef struct _LogInReturnValueByItaly
{	
	INT32	i32ReturnCode; 
	UINT32	ui32UniqueID;
	UINT8	ui32Gender;
}LOGIN_RETURN_VALUE_BY_ITALY;


class CLoginWorker_Italy : public CLoginDB
{

protected:
	virtual BOOL			_WorkProcess( LOGIN_INPUT* pLoginIn, LOGIN_OUTPUT* pLoginOut );
	void					_ParsingLogInByItaly( LOGIN_RETURN_VALUE_BY_ITALY * ReturnValue );
	
public:
	CLoginWorker_Italy();
	virtual ~CLoginWorker_Italy();
};
 
 

#endif // #ifndef _LOGINWORKER_ITALY_H