
#include "i3FrameworkPCH.h"
#include "i3UIManager.h"

#include "i3Framework/i3Framework.h"
#include "i3UIConsoleContext.h"
#include "i3UIConsoleCmd.h"


//
// i3UICSTokenUtil
//

bool i3UICSTokenUtil::operator()(INT32 index, const wchar_t* val) const
{
	if((INT32)m_tokens.size() > index)
	{
		if(!wcsicmp(m_tokens[index].c_str(), val))
			return true;
	}

	return false;
}

bool i3UICSTokenUtil::operator()(INT32 index, INT32 val) const
{
	if((INT32)m_tokens.size() > index)
	{
		INT32 v = _wtoi(m_tokens[index].c_str());

		if(val == v)
			return true;
	}

	return false;
}

bool i3UICSTokenUtil::operator()(INT32 index, float val) const
{
	if((INT32)m_tokens.size() > index)
	{
		float v = (float)_wtof(m_tokens[index].c_str());

		if(val == v)
			return true;
	}

	return false;
}



I3_CLASS_INSTANCE(i3UIConsoleExecutorListener) 

i3UIConsoleExecutorListener::i3UIConsoleExecutorListener()
{
}

i3UIConsoleExecutorListener::~i3UIConsoleExecutorListener()
{
}

//
// i3UIConsoleExecutor
//

I3_CLASS_INSTANCE(i3UIConsoleExecutor)

i3UIConsoleExecutor::i3UIConsoleExecutor()
{
	m_flag = 0;
	m_cmdQ = new i3UICSCmdQ;

	m_lastExecResult = EI3UICSRST_OK;
	m_lastDispCtx = nullptr;
	m_lastPreDispatcher = nullptr;

	m_defaultDispCtx = new i3UICSCmdDispCtx;
	m_isInRepeat = false;
	m_nextRepeatDelta = 0.0f;
}

i3UIConsoleExecutor::~i3UIConsoleExecutor()
{
	if(m_cmdQ)
	{
		delete m_cmdQ;
		m_cmdQ = nullptr;
	}

	if(m_defaultDispCtx)
	{
		delete m_defaultDispCtx;
		m_defaultDispCtx = nullptr;
	}
}

EI3UIConsoleRst i3UIConsoleExecutor::Exec(const wchar_t* execText, i3UICStrAry* rstStr, 
	I3UICSPreDispatchPFN preDispatcher)
{
	if(!execText)
	{
		m_lastDispCtx = nullptr;
		SetLastExecResult(EI3UICSRST_FAIL_EXEC_INVALID_CALL);
		return m_lastExecResult;
	}

	i3UICSCmd::Create(execText, &m_lastExec);

	return _Exec(m_defaultDispCtx, preDispatcher);
}

void i3UIConsoleExecutor::ProcessRepeat(i3UICSCmdDispCtx* execDispCtx)
{
	if(m_defaultDispCtx->IsEnableRepeat())
	{
		INT32 repeatRemain = m_defaultDispCtx->DecRepeatRemain();

		if(repeatRemain > 0)
		{
			SetRepeat(true);

			REAL32 nextDeltaTime = m_defaultDispCtx->GetRepeatNextDeltaTime();

			if(nextDeltaTime == -1)
			{
				// continue
			}
			else
			{
				SetRepeatNextDeltaTime(nextDeltaTime);
			}
		}
		else
			SetRepeat(false);
	}
	else
	{
		SetRepeat(false);
	}
}

EI3UIConsoleRst i3UIConsoleExecutor::_Exec(i3UICSCmdDispCtx* execDispCtx, 
	I3UICSPreDispatchPFN preDispatcher)
{
	bool executed = false;
	EI3UIConsoleRst rst;

	m_lastDispCtx = execDispCtx;
	m_lastPreDispatcher = preDispatcher;

	if(preDispatcher)
	{
		executed = true;
		execDispCtx->InitWithCmd(&m_lastExec);		

		if(!preDispatcher(execDispCtx))
		{
			rst = execDispCtx->GetResult();

			if(EI3UICSRST_FAIL_EXEC_NOGLUE == rst)
				executed = false;
		}
	}

	if(!executed)
		m_lastExec.Exec(execDispCtx);

	rst = execDispCtx->GetResult();
	SetLastExecResult(rst);

	ProcessRepeat(execDispCtx);

	return rst;
}

bool i3UIConsoleExecutor::ParseText(const wchar_t* text, i3UICStrAry* rstTokens,
	DWORD* lastErrPos)
{
	return i3UICSCmd::ParseTextIntoToken(text, rstTokens, lastErrPos);
}

bool i3UIConsoleExecutor::EnqueueCmd(i3UICSCmd& cmd, bool execImmediate)
{
	if(execImmediate)
	{
		bool execRst = cmd.Exec(m_defaultDispCtx);
		
		EI3UIConsoleRst dispRst = m_defaultDispCtx->GetResult();
		SetLastExecResult(dispRst);

		return execRst;
	}

	return m_cmdQ->PostCmd(cmd);
}

EI3UIConsoleDispatchRst i3UIConsoleExecutor::DispatchCmd()
{
	EI3UIConsoleDispatchRst dispRst;
	EI3UIConsoleRst rst = EI3UICSRST_OK;

	dispRst = m_cmdQ->DispatchCmd(&rst);

	return dispRst;
}

void i3UIConsoleExecutor::OnUpdate(REAL32 deltaSec)
{
	if(IsInRepeat())
	{
		if(m_nextRepeatDelta > 0.0f)
			m_nextRepeatDelta -= deltaSec;

		if(m_nextRepeatDelta <= 0.0f)
			_Exec(m_lastDispCtx, m_lastPreDispatcher);			
	}
}

void i3UIConsoleExecutor::SetRepeat(bool repeat)
{
	m_isInRepeat = repeat;

	if(!repeat)
		m_nextRepeatDelta = 0.0f;
}


//
// i3UIConsoleContext
//

I3_CLASS_INSTANCE(i3UIConsoleClientContext)

i3UIConsoleClientContext* i3UIConsoleClientContext::ms_instance = nullptr;
I3UICSPreDispatchPFN i3UIConsoleClientContext::ms_preDispatcher = nullptr;

i3UIConsoleClientContext::i3UIConsoleClientContext()
{
	I3ASSERT(!ms_instance);
	ms_instance = this;

	m_uiManager = nullptr;
	m_executor = nullptr;
	m_show = false;
}

i3UIConsoleClientContext::~i3UIConsoleClientContext()
{
	I3ASSERT(ms_instance);

	m_uiManager = nullptr;
	I3ASSERT(!m_executor); // Detach가 호출되지않았다.

	ms_instance = nullptr;
}

EI3UIConsoleRst i3UIConsoleClientContext::Attach(i3UIManager* uiManager)
{
	if(m_uiManager)
		return EI3UICSRST_FAIL_INIT_ALREADY_CREATED;

	if(!uiManager)
		return EI3UICSRST_FAIL_INIT_BAD_PARAM;

	m_uiManager = uiManager;
	m_show = false;

	m_executor = OnCreateExecutor();

	if(!OnAttach())
		return EI3UICSRST_FAIL_INIT_ATTACH;

	return EI3UICSRST_OK;
}

void i3UIConsoleClientContext::Detach()
{
	OnDetach();


	I3_SAFE_RELEASE(m_executor);
}

bool i3UIConsoleClientContext::Show(bool isShow)
{
	if(isShow != m_show)
	{	
		OnEnableConsoleScenes(isShow);
		OnShow(isShow);

		m_show = isShow;
	}

	return m_show;
}

bool i3UIConsoleClientContext::IsShow()
{
	return m_show;
}

EI3UIConsoleRst i3UIConsoleClientContext::Exec(const wchar_t* execText, i3UICStrAry* rstStr)
{
	if(m_executor->IsInRepeat())
		return EI3UICSRST_FAIL_EXEC_IN_REPEAT;

	if(!execText)
		return EI3UICSRST_FAIL_EXEC_INVALID_CALL;

	if(!OnExecBefore(execText))
		return EI3UICSRST_FAIL_EXEC_ABORT;

	EI3UIConsoleRst rst = m_executor->Exec(execText, rstStr, ms_preDispatcher);

	OnExecAfter(m_executor->GetLastExecCmd(), rst, rstStr);

	return rst;
}

void i3UIConsoleClientContext::OnUpdate(REAL32 rDeltaSeconds)
{
	if(m_executor)
		m_executor->OnUpdate(rDeltaSeconds);
}

i3UIManager* i3UIConsoleClientContext::GetUIManager()
{
	return m_uiManager;
}

bool i3UIConsoleClientContext::OnKeyInput(i3InputDeviceManager* inputDevMan)
{
	if(!i3UIManager::IsUsingConsole())
		return false;

	return true;
}

bool i3UIConsoleClientContext::OnEvent(UINT32 event, i3ElementBase* pObj, 
	UINT32 param2, I3_EVT_CODE code)
{
	if(!i3UIManager::IsUsingConsole())
		return false;

	if(code == I3_EVT_CODE_ACTIVATE)
	{
		if(event == I3_EVT_UI_NOTIFY)
		{
		}
		else if( event == I3_EVT_INPUT)
		{
			if(OnKeyInput((i3InputDeviceManager*)pObj))
				return true;
		}
	}

	return false;
}

