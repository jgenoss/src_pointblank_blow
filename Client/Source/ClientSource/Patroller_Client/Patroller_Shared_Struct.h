#pragma once

#define PIPENAME		L"\\\\.\\Pipe\\Patroller"
#define EVENTNAME		L"PipeEvent"

#define IN_BUFF_SIZE    65536
#define OUT_BUFF_SIZE   65536
#define MAX_DLL_COUNT   150
#define TIME_STAMP_FOR_DLL_LIST 10.0f
#define TIME_STAMP_FOR_LIVE_CHECK 3000

namespace PAT
{
	enum PROTOCOL_LIST
	{
		PROTOCOL_LIVE_CHECK		= 0,
		PROTOCOL_ACCOUNT_INFO	= 1,
		PROTOCOL_DLL_LIST		= 2,
		PROTOCOL_MAX
	};
}

#pragma pack( push, 1)

typedef struct tHeader
{
	UINT32	_nSize;
	UINT8	_nProtocol;
}IPC_HEADER;

typedef struct tData_LiveCheck
{
	IPC_HEADER	_Header;
	UINT32		_nSendTime;

	tData_LiveCheck()
	{
		_Header._nSize		= sizeof(tData_LiveCheck);
		_Header._nProtocol	= PAT::PROTOCOL_LIVE_CHECK;

		_nSendTime = 0;
	}

	UINT32	CopyToBuffer( char * pBuffer )
	{
		tData_LiveCheck * pDest = (tData_LiveCheck *) pBuffer;

		pDest->_Header._nSize = _Header._nSize;
		pDest->_Header._nProtocol = _Header._nProtocol;

		pDest->_nSendTime = _nSendTime;

		return (UINT32) sizeof(tData_LiveCheck);
	}
}IPC_DATA_LIVE_CHECK;

typedef struct tData_AccountInfo
{
	IPC_HEADER	_Header;
	UINT64		_nTUID;

	tData_AccountInfo()
	{
		_Header._nSize		= sizeof(tData_AccountInfo);
		_Header._nProtocol	= PAT::PROTOCOL_ACCOUNT_INFO;

		_nTUID = 0;
	}

	UINT32	CopyToBuffer( char * pBuffer )
	{
		tData_AccountInfo * pDest = (tData_AccountInfo *) pBuffer;

		pDest->_Header._nSize = _Header._nSize;
		pDest->_Header._nProtocol = _Header._nProtocol;

		pDest->_nTUID = _nTUID;

		return (UINT32) sizeof(tData_AccountInfo);
	}
}IPC_DATA_ACCOUNT_INFO;

typedef struct tData_DLLList
{
	IPC_HEADER	_Header;
	char		_szDLLList[MAX_DLL_COUNT][MAX_PATH];

	tData_DLLList()
	{
		_Header._nSize		= sizeof(tData_DLLList);
		_Header._nProtocol	= PAT::PROTOCOL_DLL_LIST;

		memset(_szDLLList,0,sizeof(_szDLLList) );
	}

	UINT32	CopyToBuffer( char * pBuffer )
	{
		tData_DLLList * pDest = (tData_DLLList *) pBuffer;

		pDest->_Header._nSize = _Header._nSize;
		pDest->_Header._nProtocol = _Header._nProtocol;

		memcpy(pDest->_szDLLList,_szDLLList,sizeof(_szDLLList));

		return (UINT32) sizeof(tData_DLLList);
	}
}IPC_DATA_DLL_LIST;

#pragma pack( pop )