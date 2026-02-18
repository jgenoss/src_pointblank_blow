
#include "i3FrameworkPCH.h"
#include "i3UIConsoleCmd.h"
#include "i3UIConsoleContext.h"


//
// Parser
//

static INT32 i3UICSValCheckDelimiter(const wchar_t c)
{
	INT32 delimiterIndex = 0;

	switch(c)
	{
	case L' '  : delimiterIndex = 1; break;
	case L'\'' : delimiterIndex = 2; break;
	case L'\"' : delimiterIndex = 3; break;
	default: break;
	}

	return delimiterIndex;
}

static bool i3UICSValParse(const wchar_t*& src, i3UICSTokenAry* rst, 
	const wchar_t** rstErrPos=nullptr)
{
	i3::vector<wchar_t> token;

	bool inQuat = false;
	wchar_t lastQuatCh = 0;

	wchar_t c = 0;
	while((c=*src++))
	{
		INT32 curDelimiter = i3UICSValCheckDelimiter(c);

		if(curDelimiter == 0)
		{
			token.push_back(c);
			continue;
		}
		else
		{
			switch(curDelimiter)
			{
			case 1:	// space
				{
					if(!inQuat) 
					{
						if(!token.empty())
						{
							token.push_back(0); rst->push_back(&token[0]); token.clear();
						}
					}
					else
					{
						token.push_back(c);
					}
				}
				break;

			case 2: case 3:	// ', "
				{
					if(!inQuat)
					{
						inQuat = true;
						lastQuatCh = (wchar_t)curDelimiter;
					}
					else
					{
						if(lastQuatCh != curDelimiter) 
							return false;

						inQuat = false;
						lastQuatCh = 0;
						token.push_back(0); rst->push_back(&token[0]); token.clear();
					}
				}
				break;

			default:
				if(rstErrPos) *rstErrPos = (src-1);
				return false;
			}
		}

	} // end of while

	// null로 끝났지만 ',"를 찾지못했다면 오류
	if(inQuat)
	{
		*rstErrPos = (src-1);
		return false;
	}

	if(!token.empty())
	{
		token.push_back(0);
		rst->push_back(&token[0]);
		token.clear();
	}

	return true;
}


//
// i3UICSCmd
//

bool i3UICSCmd::ms_genParseToken = false;

i3UICSCmd& i3UICSCmd::operator=(const i3UICSCmd& rhs)
{
	if(this == &rhs)
		return *this;

	m_tokens = rhs.m_tokens;
	m_disp = rhs.m_disp;
		
	return *this;
}

bool i3UICSCmd::operator==(const i3UICSCmd& rhs) const
{
	if(m_disp != rhs.m_disp)
		return false;

	for(INT32 i=0; i<(INT32)m_tokens.size(); i++)
	{
		if(wcsicmp(m_tokens[i].c_str(), rhs.m_tokens[i].c_str()))
			return false;
	}

	return true;
}

bool i3UICSCmd::operator!=(const i3UICSCmd& rhs) const
{
	return !this->operator==(rhs);
}

bool i3UICSCmd::Exec(i3UICSCmdDispCtx* dispCtx)
{
	if(!m_disp)
	{
		if(dispCtx) dispCtx->SetResult(EI3UICSRST_FAIL_EXEC_NOGLUE);
		return false;
	}

	static i3UICSCmdDispCtx _dispCtx;
	
	if(!dispCtx)
	{
		dispCtx = &_dispCtx;
		_dispCtx.Reset();
	}

	if( dispCtx != nullptr)
	{
		if(m_disp)
		{
			dispCtx->InitWithCmd(this);
			m_disp(dispCtx);

			if(dispCtx->GetResult() == EI3UICSRST_OK)
				return true;
		}

		dispCtx->SetResult(EI3UICSRST_FAIL);
	}

	return false;
}

bool i3UICSCmd::ParseTextIntoToken(const wchar_t* text, i3UICSTokenAry* rstTokens,
	DWORD* lastErrPos)
{
	if(!text)
		return false;

	const wchar_t* cur = text;
	const wchar_t* rstErrPos = 0;

	bool rst = i3UICSValParse(cur, rstTokens, &rstErrPos);

	if(lastErrPos)
		*lastErrPos = rst ? 0 : (DWORD)(rstErrPos - text);

	return rst;
}

void i3UICSCmd::Reset()
{
	m_tokens.clear();
	m_disp = nullptr;
}


bool i3UICSCmd::Create(const wchar_t* cmdText, i3UICSCmd* rstCmd)
{
	rstCmd->Reset();

	if(!cmdText || !*cmdText)
		return false;

	rstCmd->m_cmdText = cmdText;	

	DWORD lastErrPos = 0;

	if(IsGenerateParseToken())
	{
		if(i3UICSCmd::ParseTextIntoToken(cmdText, &rstCmd->m_tokens, &lastErrPos))
		{
			rstCmd->m_disp = i3UICSCmdQ::LookupCmdDispatch(rstCmd->m_tokens[0].c_str());

			if(rstCmd->m_disp)
				return true;
		}

		return false;
	}

	return true;
}

bool i3UICSCmd::Create(i3UICSTokenAry* cmdTokens, i3UICSCmd* rstCmd)
{
	rstCmd->Reset();

	if(cmdTokens->empty())
		return false;

	rstCmd->m_tokens = *cmdTokens;
	rstCmd->m_disp = i3UICSCmdQ::LookupCmdDispatch((*cmdTokens)[0].c_str());

	if(rstCmd->m_disp)
		return true;		

	return false;
}

const wchar_t* i3UICSCmd::GetToken(INT32 index) const
{
	return m_tokens[index].c_str();
}

INT32 i3UICSCmd::GetNumToken() const
{
	return (INT32)m_tokens.size();
}

i3::wstring i3UICSCmd::GetMergedStr() const
{
	i3::wstring mer;

	for(INT32 i=0; i<(INT32)m_tokens.size(); i++)
	{
		if(i==0)
			mer = m_tokens[0];
		else
			mer = mer + i3::wstring(L" ") + m_tokens[i];
	}
	
	return mer;
}

//
// i3UICSCmdDispCtx
//

i3UICSCmdDispCtx::i3UICSCmdDispCtx()
{
	InitWithCmd(nullptr);
}

i3UICSCmdDispCtx::i3UICSCmdDispCtx(const i3UICSCmd& cmd) : m_repeatParam(nullptr)
	, m_repeatRemain(0), m_repeatNextDeltaTime(0.f)
{
	InitWithCmd(&cmd);
}

void i3UICSCmdDispCtx::ResetParsePos()
{
	m_curParseIdx = 0;
}

bool i3UICSCmdDispCtx::IsAvailableParseToken()
{
	if((INT32)m_cmd->m_tokens.size() < m_curParseIdx)
		return true;

	return false;
}

bool i3UICSCmdDispCtx::ParseCmp(const wchar_t* tokenStr, bool* canParseNext)
{
	const wchar_t* str;
	
	if(!ParseStr(&str)) 
	{ 
		if(canParseNext) *canParseNext = false;
		return false;
	}

	if(canParseNext) *canParseNext = IsAvailableParseToken();

	return wcsicmp(str, tokenStr) == 0 ? true : false;
}

bool i3UICSCmdDispCtx::ParseCmp(INT32 tokenVal, bool* canParseNext)
{
	const wchar_t* str;
	
	if(!ParseStr(&str))
	{ 
		if(canParseNext) *canParseNext = false;
		return false;
	}

	if(canParseNext) *canParseNext = IsAvailableParseToken();

	return _wtoi(str) == tokenVal ? true : false;
}

bool i3UICSCmdDispCtx::ParseCmp(float tokenVal, bool* canParseNext)
{
	const wchar_t* str;
	
	if(!ParseStr(&str))
	{ 
		if(canParseNext) *canParseNext = false;
		return false;
	}

	if(canParseNext) *canParseNext = IsAvailableParseToken();

	return _wtof(str) == tokenVal ? true : false;
}

bool i3UICSCmdDispCtx::ParseStr(const wchar_t** rstStr)
{
	const wchar_t* str;

	if(!GetNextToken(&str)) return false;
	*rstStr = str;

	return true;
}

bool i3UICSCmdDispCtx::ParseFloat(float* val)
{
	const wchar_t* str;

	if(!GetNextToken(&str)) return false;
	*val = (float)_wtof(str);

	return true;
}

bool i3UICSCmdDispCtx::ParseINT(INT32* val)
{
	const wchar_t* str;

	if(!GetNextToken(&str)) return false;
	*val = _wtoi(str);

	return true;
}

bool i3UICSCmdDispCtx::GetNextToken(const wchar_t** str)
{
	if(!m_cmd)
		return false;

	if((INT32)m_cmd->m_tokens.size() > (m_curParseIdx+1))
	{
		m_curParseIdx++;
		*str = m_cmd->m_tokens[m_curParseIdx].c_str();
		
		return true;
	}

	return false;
}

void i3UICSCmdDispCtx::Log(UINT32 flag, const wchar_t* fmt, ...)
{
	wchar_t buf[1024]; 
	buf[0] = 0;
	va_list arg; va_start(arg, fmt);
	vswprintf_s(buf, 1024, fmt, arg); va_end(arg);

	i3UIConsoleClientContext::Get()->AddLogStr(flag, buf);
}

void i3UICSCmdDispCtx::InitWithCmd(const i3UICSCmd* cmd)
{
	m_result = EI3UICSRST_FAIL;
	m_cmd = cmd;
	m_curParseIdx = 0;

	if(!cmd)
	{
		m_repeatRemain = 0;
		m_repeatParam = nullptr;
		m_repeatNextDeltaTime = 0.0f;
	}
}

void i3UICSCmdDispCtx::Reset()
{
	InitWithCmd(nullptr);
}

void i3UICSCmdDispCtx::SetRepeat(INT32 repeatNumber, void* param)
{
	m_repeatRemain = repeatNumber;
	m_repeatParam = nullptr;
	m_repeatNextDeltaTime = 0.0f;
}

INT32 i3UICSCmdDispCtx::DecRepeatRemain()
{
	if(m_repeatRemain > 0)
		m_repeatRemain--;
	
	if(m_repeatRemain < 1)
		SetRepeat(0);

	return m_repeatRemain;
}

//
// i3UICSCmdQ
//

i3UICSCmdDispPFN i3UICSCmdQ::ms_cmdDisp[i3UICSCmdQ::MAX_REGISTER_CMD]		= {0};
const wchar_t* i3UICSCmdQ::ms_cmdDispPrefix[i3UICSCmdQ::MAX_REGISTER_CMD]	= {0};
INT32 i3UICSCmdQ::ms_cmdDispNum = 0;


i3UICSCmdQ::i3UICSCmdQ()
{
	m_transactionNumber = 0;
	m_curTransactionID = 0;
}

i3UICSCmdQ::~i3UICSCmdQ()
{
}

bool i3UICSCmdQ::PostCmd(const i3UICSCmd& cmd)
{
	if(!cmd.m_disp)
		return false;

	CmdData cmdData;
	cmdData.m_type = CmdData::ETYPE_CMD;

	cmdData.m_cmd = cmd;
	m_cmdDataQ.push_back(cmdData);

	return true;
}

bool i3UICSCmdQ::PostCmd(i3UICStrAry& cmdTokens)
{
	CmdData cmdData;
	cmdData.m_type = CmdData::ETYPE_CMD;

	if(!i3UICSCmd::Create(&cmdTokens, &cmdData.m_cmd))
		return false;

	m_cmdDataQ.push_back(cmdData);

	return true;
}

EI3UIConsoleDispatchRst i3UICSCmdQ::DispatchCmd(EI3UIConsoleRst* rst)
{
	if(*rst) *rst = EI3UICSRST_FAIL;

	if(m_cmdDataQ.empty())
		return EI3UICSCMD_DISP_RST_FAIL_NOMORE_CMD;

	CmdData cmd = m_cmdDataQ.front(); 
	m_cmdDataQ.pop_front(); // for support aborting transaction

	m_dispCtx.Reset();

	if(cmd.m_cmd.Exec(&m_dispCtx))
	{
		//if(cur.InTransaction())
		//{
		//	//AbortTransaction();
		//	//	
		//	//const char* transDesc = cur.m_transactionDesc.c_str();
		//	//if(!transDesc || !(*transDesc))
		//	//	transDesc = "NoTransDesc";

		//	//Report("[ERR!] transaction [%s] is aborted..", transDesc);
		//}

		return EI3UICSCMD_DISP_RST_OK;
	}

	return EI3UICSCMD_DISP_RST_FAIL_EXEC_CMD;
}

bool i3UICSCmdQ::Register(const wchar_t* cmd, i3UICSCmdDispPFN pfn)
{
	if(!cmd) 
		return false;

	if(LookupCmdDispatch(cmd))
		return false;

	if(ms_cmdDispNum +1 > MAX_REGISTER_CMD)
		return false;

	ms_cmdDispPrefix[ms_cmdDispNum] = cmd;
	ms_cmdDisp[ms_cmdDispNum] = pfn;
	ms_cmdDispNum++;

	return true;
}

i3UICSCmdDispPFN i3UICSCmdQ::LookupCmdDispatch(const wchar_t* cmdPrefix)
{
	if(!cmdPrefix)
		return nullptr;

	// 100개정도 이상부턴 해시검색으로 코드를 바꿔보자
	for(INT32 i=0; i<ms_cmdDispNum; i++)
	{	
		if(!wcsicmp(cmdPrefix, ms_cmdDispPrefix[i]))
			return ms_cmdDisp[i];
	}

	return nullptr;
}
