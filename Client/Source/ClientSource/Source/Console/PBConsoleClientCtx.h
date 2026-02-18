

#ifndef _PBCONSOLECLIENTCTX_H_
#define _PBCONSOLECLIENTCTX_H_

#include "i3Framework/i3UI/i3UIConsoleContext.h"

class PBConsoleLog;


class PBConsoleClientCtx : public i3UIConsoleClientContext
{
	I3_CLASS_DEFINE(PBConsoleClientCtx, i3UIConsoleClientContext);

	// i3UIConsoleClientContext
public:	
	virtual	i3UIScene* GetMainScene() override { return m_consoleUIMain; }

	virtual void AddLog(UINT32 flag, const wchar_t* fmt, ...) override;
	virtual void AddLogStr(UINT32 flag, const wchar_t* str) override;

	virtual bool SaveLogToFile(const wchar_t* path=nullptr) override;
	virtual void ClearLog() override;

protected:
	virtual bool OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnShow(bool isShow) override;
	virtual void OnEnableConsoleScenes(bool enable) override;

	virtual bool OnKeyInput(i3InputDeviceManager* inputDevMan) override;

	virtual bool OnExecBefore(const wchar_t* execTxt, const wchar_t** rstReason=nullptr) override;
	virtual void OnExecAfter(const i3UICSCmd* cmd, EI3UIConsoleRst rst, i3UICStrAry* rstStr=nullptr) override;

	virtual i3UIConsoleExecutor* OnCreateExecutor() override;


	// PBConsoleClientCtx
public:
	PBConsoleClientCtx();
	virtual ~PBConsoleClientCtx();

	void		ReDrawConsoleLog();

private:
	bool CreateUIScene();
	void DestroyUIScene();

	EI3UIConsoleRst SubmitConsoleInput();

	void ShowPrologueLog();

private:
	PBConsoleLog*							m_logger;
	DWORD									m_loggerTextRev;

		// console ui scene stuff
	i3UIScene*								m_consoleUIMain;
	i3UIFrameWnd*							m_consoleUIFrame;
	i3UIEditBox*							m_consoleUILog;
	i3UIEditBox*							m_consoleUIInput;

	bool									m_attached;
};

i3ClassMeta* GetPBConsoleClientCtxMeta();


#endif
