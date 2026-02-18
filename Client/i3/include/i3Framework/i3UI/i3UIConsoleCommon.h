
#ifndef _I3UICONSOLEDEF_H_
#define _I3UICONSOLEDEF_H_


class i3UICSCmdDispCtx;

typedef i3::vector<i3::wstring>			i3UICSTokenAry;
typedef i3::vector<i3::wstring>			i3UICStrAry;


enum EI3UIConsoleRst
{	
	EI3UICSRST_OK								= 0,

	EI3UICSRST_FAIL								= -1,

	EI3UICSRST_FAIL_INIT_ATTACH					= -1001,
	EI3UICSRST_FAIL_INIT_ALREADY_CREATED		= -1002,
	EI3UICSRST_FAIL_INIT_BAD_PARAM				= -1003,

	EI3UICSRST_FAIL_EXEC_INVALID_CALL			= -2001,
	EI3UICSRST_FAIL_EXEC_PARSE_ERR				= -2002,
	EI3UICSRST_FAIL_EXEC_INVALID_PARAM			= -2003,
	EI3UICSRST_FAIL_EXEC_PROCESS				= -2004,
	EI3UICSRST_FAIL_EXEC_NOGLUE					= -2005,
	EI3UICSRST_FAIL_EXEC_ABORT					= -2006,	
	EI3UICSRST_FAIL_EXEC_IN_REPEAT				= -2007,

	EI3UICSRST_FAIL_CMDQ_EXECUTION				= -3002,

	EI3UICSRST_FAIL_USER						= -9000,
};


enum EI3UIConsoleDispatchRst
{
	EI3UICSCMD_DISP_RST_MORE					= 1000,		// Dispatch할 명령이 좀더 남아있어요
	EI3UICSCMD_DISP_RST_OK						= 0,

	EI3UICSCMD_DISP_RST_FAIL					= -1,

	EI3UICSCMD_DISP_RST_FAIL_EXEC_CMD			= -1000,
	EI3UICSCMD_DISP_RST_FAIL_NOMORE_CMD			= -2000,
};


enum EI3UIConsoleExecEvent
{
	EI3UICSEXECEVT_DISPATCH_PRE,
	EI3UICSEXECEVT_DISPATCH_RST,
};

typedef void (*i3UICSCmdDispPFN)(i3UICSCmdDispCtx*);


//
// i3UICSCmd
//

class I3_EXPORT_FRAMEWORK i3UICSCmd
{
	friend class i3UICSCmdQ;
	friend class i3UICSCmdDispCtx;

public:
	i3UICSCmd() {}
	i3UICSCmd(const i3UICSCmd& rhs) { this->operator=(rhs); }

	i3UICSCmd& operator =(const i3UICSCmd& rhs);

	bool operator ==(const i3UICSCmd& rhs) const;
	bool operator !=(const i3UICSCmd& rhs) const;

	bool Exec(i3UICSCmdDispCtx* dispCtx=nullptr);
	const wchar_t* GetCmdText() const { return m_cmdText.c_str();} 

	i3::wstring GetMergedStr() const;
	
	const wchar_t* GetToken(INT32 index) const;
	INT32 GetNumToken() const;

	bool IsInvalidDisp() const { return !m_disp ? true : false; }

	static bool Create(const wchar_t* cmdText, i3UICSCmd* rstCmd);
	static bool Create(i3UICSTokenAry* cmdTokens, i3UICSCmd* rstCmd);

	static void SetGenerateParseToken(bool genToken) { ms_genParseToken = genToken; }
	static bool IsGenerateParseToken() { return ms_genParseToken; }

	static bool ParseTextIntoToken(const wchar_t* text, i3UICSTokenAry* rstTokens, 
		DWORD* lastErrPos=nullptr);

private:
	void Reset();

private:
	i3::wstring								m_cmdText;
	i3UICSTokenAry							m_tokens;
	i3UICSCmdDispPFN						m_disp = nullptr;

	static bool								ms_genParseToken;
};


#endif
