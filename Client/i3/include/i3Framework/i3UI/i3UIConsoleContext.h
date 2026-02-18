
#ifndef _I3UICONSOLECONTEXT_H_
#define _I3UICONSOLECONTEXT_H_

class i3UIManager;
class i3UIScene;
class i3UICSCmdQ;
class i3UICSCmdDispCtx;

#include "i3UIConsoleCommon.h"


typedef bool (*I3UICSPreDispatchPFN)(i3UICSCmdDispCtx* );


//
// i3UICSTokenUtil
//

class I3_EXPORT_FRAMEWORK i3UICSTokenUtil
{
public:
	i3UICSTokenUtil(i3UICStrAry& tokens) : m_tokens(tokens) {}

		// nocase-compare
	bool operator()(INT32 index, const wchar_t* val) const;
	bool operator()(INT32 index, INT32 val) const;
	bool operator()(INT32 index, float val) const;

	const wchar_t* GetTokenStr(INT32 index) const { return m_tokens[index].c_str(); }
	INT32 GetNumTokens() const { return (INT32)m_tokens.size(); }

private:
	const i3UICStrAry&					m_tokens;
};


//
// Console Command Executor
//

class I3_EXPORT_FRAMEWORK i3UIConsoleExecutorListener : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE(i3UIConsoleExecutorListener, i3ElementBase);

public:
	i3UIConsoleExecutorListener();
	virtual ~i3UIConsoleExecutorListener();

	// 이상하게 클래스내에 생성자,소멸자, 순가상함수만 있을경우 오류나서 ㅋ
	virtual EI3UIConsoleRst OnExecutorEvent(EI3UIConsoleExecEvent evt, void* prm0=nullptr, 
		void* prm1=nullptr, void* prm2=nullptr, void* prm3=nullptr) { return EI3UICSRST_OK; }; 
};


class I3_EXPORT_FRAMEWORK i3UIConsoleExecutor : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE(i3UIConsoleExecutor, i3ElementBase);

public:
	enum EFLAG
	{
		EFL_SHOW_PARSE_TOKEN					= 0x00000001,
	};

	EI3UIConsoleRst Exec(const wchar_t* execText, i3UICStrAry* rstStr=nullptr, 
		I3UICSPreDispatchPFN preDispatcher=nullptr);

	bool EnqueueCmd(i3UICSCmd& cmd, bool execImmediate=false);
	EI3UIConsoleDispatchRst DispatchCmd();

	const i3UICSCmd* GetLastExecCmd() const { return &m_lastExec; }
	void SetLastExecResult(EI3UIConsoleRst rst) { m_lastExecResult = rst; }
	EI3UIConsoleRst GetLastExecResult() { return m_lastExecResult; }

	DWORD GetFlag() { return m_flag; }
	void SetFlag(DWORD flag) { m_flag |= flag; }
	void RemoveFlag(DWORD flag) { m_flag &= ~flag; }

	i3UIConsoleExecutor();
	virtual ~i3UIConsoleExecutor();

	virtual void OnUpdate(REAL32 deltaSec);

	static bool CompareStrToToken(i3UICStrAry& cmdTokens, INT32 index, const wchar_t* text);
	static bool ParseText(const wchar_t* text, i3UICStrAry* rstTokens,
		DWORD* lastErrPos=nullptr);

	bool IsInRepeat() const { return m_isInRepeat; }
	void SetRepeat(bool repeat);
	void SetRepeatNextDeltaTime(REAL32 nextRepeatDelta) 
	{ 
		if(m_isInRepeat) 
			m_nextRepeatDelta = nextRepeatDelta; 
		else
			m_nextRepeatDelta = 0.0f;
	}

protected:
	EI3UIConsoleRst _Exec(i3UICSCmdDispCtx* execDispCtx, I3UICSPreDispatchPFN preDispatcher);
	void ProcessRepeat(i3UICSCmdDispCtx* execDispCtx);

protected:
	i3UICSCmd									m_lastExec;
	EI3UIConsoleRst								m_lastExecResult;
	i3UICSCmdDispCtx*							m_lastDispCtx;
	I3UICSPreDispatchPFN						m_lastPreDispatcher;
	

	i3UICSCmdDispCtx*							m_defaultDispCtx;

	bool										m_isInRepeat;
	REAL32										m_nextRepeatDelta;
	
private:
	DWORD										m_flag;
	i3UICSCmdQ*									m_cmdQ;
};


//
// ConsoleClientContext
//

class I3_EXPORT_FRAMEWORK i3UIConsoleClientContext : public i3ElementBase
{
	I3_EXPORT_ABSTRACT_CLASS_DEFINE(i3UIConsoleClientContext, i3ElementBase);

	friend class i3UIManager;

public:
	i3UIConsoleClientContext();
	virtual ~i3UIConsoleClientContext();

	static i3UIConsoleClientContext* Get() { return ms_instance; }

	enum EUIRESOLUTION
	{
		EUIRES_WIDTH = 1024,
		EUIRES_HEIGHT = 768,
	};

	EI3UIConsoleRst Attach(i3UIManager* uiManager);
	void Detach();

	bool Show(bool isShow=true);
	bool IsShow();

	EI3UIConsoleRst Exec(const wchar_t* execText, i3UICStrAry* rstStr=nullptr);

	i3UIManager* GetUIManager();
	virtual i3UIScene* GetMainScene() = 0;

	i3UIConsoleExecutor* GetExecutor() { return m_executor; }

	virtual void AddLog(UINT32 flag, const wchar_t* fmt, ...) {}
	virtual void AddLogStr(UINT32 flag, const wchar_t* str) {}
	
	virtual void OnUpdate(REAL32 rDeltaSeconds);

	static void SetPreDispatcher(I3UICSPreDispatchPFN preDispatcher) {
		ms_preDispatcher = preDispatcher; }

	virtual bool SaveLogToFile(const wchar_t* path=nullptr) { return false; }
	virtual void ClearLog() {}

protected:
	virtual bool OnAttach() = 0;
	virtual void OnDetach() = 0;

	virtual void OnShow(bool isShow) = 0;
	virtual void OnEnableConsoleScenes(bool enable) = 0;

	virtual bool OnKeyInput(i3InputDeviceManager* inputDevMan);

	virtual bool OnExecBefore(const wchar_t* execTxt, const wchar_t** rstReason=nullptr) 
		{ return execTxt ? true : false; }
	virtual void OnExecAfter(const i3UICSCmd* cmd, EI3UIConsoleRst rst, 
		i3UICStrAry* rstStr=nullptr) {}

	virtual i3UIConsoleExecutor* OnCreateExecutor() = 0;

private:
	// 키입력이벤트 I3_EVT_INPUT, i3UIManager::OnConsoleEvent()
	virtual bool OnEvent(UINT32 event, i3ElementBase* pObj, UINT32 param2, 
		I3_EVT_CODE code=I3_EVT_CODE_ACTIVATE);

private:
	static i3UIConsoleClientContext*			ms_instance;
	static I3UICSPreDispatchPFN					ms_preDispatcher;

	i3UIManager*								m_uiManager;
	i3UIConsoleExecutor*						m_executor;

	bool										m_show;

};


#endif
