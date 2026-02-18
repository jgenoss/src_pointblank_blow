#if !defined( __I3_ERROR_HANDLER_H)
#define __I3_ERROR_HANDLER_H

#define	I3ERR_CHN_STDOUT		0x00000001			// 표준출력(stdout)으로 Error Message를 출력한다.
#define	I3ERR_CHN_STDERR		0x00000002
#define	I3ERR_CHN_DEBUG			0x00000004
#define I3ERR_CHN_CALLBACK		0x00000008
#define I3ERR_CHN_FILE			0x00000010
#define I3ERR_CHN_NETWORK		0x00000020
#define I3ERR_CHN_PIPE			0x00000040

#define I3MINDUMP_NORMAL							0x00000000
#define I3MINDUMP_WITHDATASEGS						0x00000001
#define I3MINDUMP_WITHFULLMEMORY					0x00000002
#define I3MINDUMP_WITHHANDLEDATA					0x00000004
#define I3MINDUMP_FILTERMEMORY						0x00000008
#define I3MINDUMP_SCANMEMORY						0x00000010
#define I3MINDUMP_WITHUNLOADEDMODULES				0x00000020
#define I3MINDUMP_WITHINDIRECTLYREFERENCEDMEMORY	0x00000040
#define I3MINDUMP_FILTERMODULEPATHS					0x00000080
#define I3MINDUMP_WITHPROCESSTHREADDATA				0x00000100
#define I3MINDUMP_WITHPRIVATEREADWRITEMEMORY		0x00000200
#define I3MINDUMP_WITHOUTOPTIONALDATA 				0x00000400
#define I3MINDUMP_WITHFULLMEMORYINFO 				0x00000800
#define I3MINDUMP_WITHTHREADINFO 					0x00001000
#define I3MINDUMP_WITHCODESEGS						0x00002000

enum I3LOG_TYPE
{
	I3LOG_NOTICE = 0,
	I3LOG_WARN,
	I3LOG_FATAL,
	I3LOG_TYPE_MAX
};

#define I3LOG_UDP_PORT			5497

typedef struct ALIGN4 _tagi3ErrorInfo
{
	UINT16		m_Version;
	UINT16		m_Type;
	UINT16		m_WhereLength;
	UINT16		m_MsgLength;
} I3_ERROR_INFO;

enum I3ERROR_HANDLE
{
	I3ERROR_HANDLE_BREAK,
	I3ERROR_HANDLE_CONTINUE
};

typedef I3ERROR_HANDLE (*I3LOGCALLBACK)( I3LOG_TYPE type, char * pszWhere, char * pszMsg, void * pUserData);
typedef I3ERROR_HANDLE (*I3EXPECTIONCALLBACK)( char * pszBuffer );

class I3ExceptionHandler
{
	UINT16			m_uDumpType;
	char			m_szDmpFileName[ MAX_PATH ];

public:
	I3ExceptionHandler();
	virtual ~I3ExceptionHandler();

	virtual void	ExceptionInformation( PEXCEPTION_POINTERS pExceptionInfo );

	void			SetPath( char* pszDmpFileName );
	void			SetExceptionHandlerOption( INT32 nOption );
	void			SetCreateMiniDump( PEXCEPTION_POINTERS pExceptionInfo );
	void			GetSystemInfo( char * pszTemp);
};

#define	I3_ERROR_MASK_IO			0x80000000
#define I3_ERROR_MASK_BUG			0x40000000

namespace i3Error
{

	enum CODE
	{
		_NOERROR						=	(0x00000000),
		_OPENFAIL						=	(0x00000001 | I3_ERROR_MASK_IO),
		_READFAIL						=	(0x00000002 | I3_ERROR_MASK_IO),
		_WRITEFAIL						=	(0x00000003 | I3_ERROR_MASK_IO),
		_TIMEOUT						=	(0x00000004 | I3_ERROR_MASK_IO),

		_OUTOFMEMORY					=	(0x00000010 | I3_ERROR_MASK_BUG),
		_OUTOFVIDEOMEMORY				=	(0x00000011),
		_UNSUPPORTED_GPU				=	(0x00000012),
		
		_ENCRIPT_VIOLATION				=	(0x00000100),

		_INVALIDFORMAT					=	(0x00000200),
		_OLDVERSION						=	(0x00000201),
		_UNKNOWNCLASSMETA				=	(0x00000202 | I3_ERROR_MASK_BUG),
	};

    I3_EXPORT_BASE void Init(void);
	I3_EXPORT_BASE void	SetEnv( char * pszFile, INT32 line, char * pszFunc, I3LOG_TYPE lv);
	I3_EXPORT_BASE void	Log( char * pszMsg, ...);
	I3_EXPORT_BASE void	SetChannel( I3LOG_TYPE type, UINT32 chn);
	I3_EXPORT_BASE void	SetCallback( I3LOG_TYPE type, I3LOGCALLBACK pUserProc, void * pUserData);
	I3_EXPORT_BASE void	SetChannelName( I3LOG_TYPE type, const PSTR pszName);
	I3_EXPORT_BASE void SetExceptionHandler( char* pszDmpFileName = NULL );
	I3_EXPORT_BASE void	SetExceptionHandlerOption( INT32 nOption );

	I3_EXPORT_BASE const char *	getMsg( CODE code);

	I3_EXPORT_BASE char*		GetEnvErrorFile();
	I3_EXPORT_BASE INT32		GetEnvErrorLine();
	I3_EXPORT_BASE char*		GetEnvErrorFunc();
	I3_EXPORT_BASE I3LOG_TYPE	GetEnvLogType();

	I3_EXPORT_BASE void	SetDefaultCallback( I3LOG_TYPE type, void * pUserData );
}

#if !defined( __FUNCSIG__)
#define __FUNCSIG__		"Unknown function"
#endif

#define I3NOTICE \
	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_NOTICE);\
	i3Error::Log

#define I3WARN \
	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_WARN);\
	i3Error::Log

#define I3FATAL \
	i3Error::SetEnv( __FILE__, __LINE__, __FUNCSIG__, I3LOG_FATAL);\
	i3Error::Log

#define			I3ERRLOG( code)			( I3PRINTF( "%s(%d) : %s\n", __FILE__, __LINE__, i3Error::getMsg( (code))))

#endif
