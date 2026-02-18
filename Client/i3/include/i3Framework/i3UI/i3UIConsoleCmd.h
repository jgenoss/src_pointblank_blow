
#ifndef _I3UICONSOLECMD_H_
#define _I3UICONSOLECMD_H_

class i3UICSCmdQ;
class i3UICSCmdDispCtx;

#include "i3UIConsoleCommon.h"
#include "i3Base/itl/deque.h"


//
// i3UICSCmd
//

// see i3UIConsoleDef.h


//
// i3UICSCmdDispCtx
//

class I3_EXPORT_FRAMEWORK i3UICSCmdDispCtx
{
	friend class i3UICSCmdQ;
	friend class i3UICSCmd;

public:
	i3UICSCmdDispCtx();
	i3UICSCmdDispCtx(const i3UICSCmd& cmd);
	void InitWithCmd(const i3UICSCmd* cmd);

	const wchar_t* GetCmdText() const { return m_cmd ? m_cmd->GetCmdText() : nullptr; }

	void ResetParsePos();
	bool IsAvailableParseToken();
	
	bool ParseCmp(const wchar_t* tokenStr, bool* canParseNext=nullptr);
	bool ParseCmp(INT32 tokenVal, bool* canParseNext=nullptr);
	bool ParseCmp(float tokenVal, bool* canParseNext=nullptr);

	bool ParseStr(const wchar_t** rstStr);
	bool ParseFloat(float* rstVal);
	bool ParseINT(INT32* rstVal);
	
	void SetResult(EI3UIConsoleRst rst=EI3UICSRST_OK) { m_result = rst; }
	EI3UIConsoleRst GetResult() const { return m_result; }

	const wchar_t* GetResultString() { return m_resultStr.c_str(); }
	void SetResultString(const wchar_t* rstStr) 
	{ 
		if(!rstStr) m_resultStr.clear();
		else m_resultStr = rstStr;
	}

	INT32 GetNumToken() const { return (INT32)m_cmd->m_tokens.size(); }
	const i3::wstring& GetToken(INT32 index) const { return (m_cmd->m_tokens)[index]; }
	void Log(UINT32 flag, const wchar_t* fmt, ...);
	void Reset();

		// Repeat
	bool IsEnableRepeat() const { return m_repeatRemain == 0 ? false : true; }
	void SetRepeat(INT32 repeatNumber, void* param=nullptr);
	
	INT32 GetRepeatRemain() const { return m_repeatRemain; }
	INT32 DecRepeatRemain();

	REAL32 GetRepeatNextDeltaTime() const { return m_repeatNextDeltaTime; }

	// deltaTime: -1 for continue
	void SetRepeatNextDeltaTime(REAL32 deltaTime) { m_repeatNextDeltaTime = deltaTime; }

private:
	bool GetNextToken(const wchar_t** str);

private:
	EI3UIConsoleRst						m_result;
	i3::wstring							m_resultStr;

	const i3UICSCmd*					m_cmd;
	INT32								m_curParseIdx;	

	INT32								m_repeatRemain;			// 0: none, -1:eternity
	void*								m_repeatParam;
	REAL32								m_repeatNextDeltaTime;
};


//
// i3UICSCmdQ
//

class I3_EXPORT_FRAMEWORK i3UICSCmdQ
{
public:
	enum { MAX_REGISTER_CMD = 1024 };

	static bool Register(const wchar_t* cmd, i3UICSCmdDispPFN pfn);
	static i3UICSCmdDispPFN LookupCmdDispatch(const wchar_t* cmdPrefix);

public:
	struct CmdData
	{	
		enum ETYPE 
		{ 
			ETYPE_CMD, 
			ETYPE_TRANSACTION_BEGIN, 
			ETYPE_TRANSACTION_END
		};

		bool InTransaction() { return m_transactionID != 0 ? true : false; }

		CmdData() : m_type(ETYPE_CMD), m_transactionID(0)
		{
		}

		CmdData(const CmdData& rhs) : m_type(ETYPE_CMD), m_transactionID(0)
		{
			operator=(rhs);
		}

		CmdData& operator=(const CmdData& rhs)
		{
			if(this == &rhs)
				return *this;

			m_type = rhs.m_type;
			m_transactionID = rhs.m_transactionID;
			m_transactionDesc = rhs.m_transactionDesc;
			m_cmd = rhs.m_cmd;

			return *this;
		}

		ETYPE						m_type;
		UINT						m_transactionID;
		i3::wstring					m_transactionDesc;

		i3UICSCmd					m_cmd;
	};
	
	typedef i3::deque<CmdData>		CmdDataQ;

public:
	i3UICSCmdQ();
	~i3UICSCmdQ();

	bool PostCmd(const i3UICSCmd& cmd); // ECMD cmd
	bool PostCmd(i3UICSTokenAry& cmdTokens);

	EI3UIConsoleDispatchRst DispatchCmd(EI3UIConsoleRst* rst);

private:		
	CmdDataQ						m_cmdDataQ;

	// 나중에 문자열 해시로
	static const wchar_t*			ms_cmdDispPrefix[MAX_REGISTER_CMD];
	static i3UICSCmdDispPFN			ms_cmdDisp[MAX_REGISTER_CMD];
	static INT32					ms_cmdDispNum;

	i3UICSCmdDispCtx				m_dispCtx;

	UINT32							m_transactionNumber;
	UINT32							m_curTransactionID;
	i3::wstring						m_curTransactionDesc;
};


#endif

