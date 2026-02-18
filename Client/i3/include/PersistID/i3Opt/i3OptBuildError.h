//*******************************************************************************************************
//	에러 및 결과값 정의
//
//	by KOMET 2006.02.10
//*******************************************************************************************************
#if !defined( __I3_OPT_BUILD_ERROR_H)
#define __I3_OPT_BUILD_ERROR_H


enum I3OPT_RESULT
{
	I3OPT_OK					= 0,		//	성공

	I3OPT_FAIL					= -1,		//	실패
	I3OPT_FAIL_NOT_ENOUGH_MEM	= -2,		//	메모리 부족
	I3OPT_FAIL_INVALID_VAL		= -3,		//	잘못된 값
	I3OPT_FAIL_NONE_VERTEX		= -4,		//	VertexArray가 없다.
	I3OPT_FAIL_NONE_INDEX		= -5,		//	IndexArray가 없다.
	I3OPT_FAIL_GEN_VERTEX		= -6,		//	VertexArray 생성 실패
	I3OPT_FAIL_GEN_INDEX		= -7, 		//	IndexArray 생성 실패

	I3OPT_FAIL_NOT_USE			= -100,		//	함수가 처리 되지 않았다.

	I3OPT_FAIL_NOT_SUPPORT		= -1000,	//	현재 함수내에서 지원하지 않음.
};

typedef struct _tagi3OptBuildError
{
	INT32	m_nResult;		//	마지막 결과값
	INT32	m_nFirstError;	//	최초로 저장된 에러값

	char	m_szResultMsg[256];
	char	m_szFirstErrorMsg[256];

	void	Reset( void)			
			{	
				m_nResult = m_nFirstError = I3OPT_OK;	
				memset( m_szResultMsg, 0, sizeof(m_szResultMsg));
				memset( m_szFirstErrorMsg, 0, sizeof(m_szFirstErrorMsg));
			}

	//	마지막 결과값 반환
	INT32	GetResult(void)			{	return m_nResult;						}
	//	최초로 저장된 에러값 반환
	INT32	GetFirstError(void)		{	return m_nFirstError;					}
	//	마지막 결과값 메시지 반환
	char *  GetResultMsg(void)		{	return m_szResultMsg;					}
	//	최초로 저장된 메시지 반환
	char *  GetFirstErrorMsg(void)	{	return m_szFirstErrorMsg;				}

			//	에러 및 결과값 저장
	void	SetResult( I3OPT_RESULT nResult, char * pMsg = NULL)
			{
				m_nResult = nResult;

				if( pMsg)
					i3String::Copy( m_szResultMsg, pMsg);
				else
					i3String::Copy( m_szResultMsg, GetText( nResult));

				if( m_nFirstError == I3OPT_OK)	
				{
					m_nFirstError = nResult;

					if( pMsg)
						i3String::Copy( m_szFirstErrorMsg, pMsg);
					else
						i3String::Copy( m_szFirstErrorMsg, GetText( nResult));
				}
			}

			//	문자열로 반환
	PSTR	GetText( INT32 nResult)	
			{
				switch( (I3OPT_RESULT) nResult)
				{
				case I3OPT_OK:					return "I3OPT_OK";					break;
				case I3OPT_FAIL:				return "I3OPT_FAIL";				break;
				case I3OPT_FAIL_NOT_ENOUGH_MEM:	return "I3OPT_FAIL_NOT_ENOUGH_MEM";	break;
				case I3OPT_FAIL_INVALID_VAL:	return "I3OPT_FAIL_INVALID_VAL";	break;
				case I3OPT_FAIL_NONE_VERTEX:	return "I3OPT_FAIL_NONE_VERTEX";	break;
				case I3OPT_FAIL_NONE_INDEX:		return "I3OPT_FAIL_NONE_INDEX";		break;
				case I3OPT_FAIL_GEN_VERTEX:		return "I3OPT_FAIL_GEN_VERTEX";		break;
				case I3OPT_FAIL_GEN_INDEX:		return "I3OPT_FAIL_GEN_INDEX";		break;				
				case I3OPT_FAIL_NOT_USE:		return "I3OPT_FAIL_NOT_USE";		break;
				case I3OPT_FAIL_NOT_SUPPORT:	return "I3OPT_FAIL_NOT_SUPPORT";	break;
				default: return "Not Defined";
				}
			}


} OPT_ERROR;


#endif

