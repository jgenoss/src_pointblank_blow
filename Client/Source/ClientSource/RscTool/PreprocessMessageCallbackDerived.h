#pragma once

#include "PreprocessMessageCallback.h"
#include "PreprocessItemDlg.h"

class PreprocessMessageCallbackDerived : public PreprocessMessageCallback
{
public:
	PreprocessMessageCallbackDerived();
	
	virtual ~PreprocessMessageCallbackDerived();
	
	virtual void OnStart();
	
	virtual void OnTextMessage(const wchar_t* strMsg);
	
	virtual void OnEnd(); 

private:	
	PreprocessItemDlg dlg;
};
