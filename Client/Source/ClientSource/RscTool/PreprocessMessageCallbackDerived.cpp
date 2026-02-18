#include "stdafx.h"

#include "PreprocessMessageCallbackDerived.h"

PreprocessMessageCallbackDerived::PreprocessMessageCallbackDerived()
{
	dlg.Create(IDD_DIALOG_PREPROCESS_ITEM, NULL);
}

PreprocessMessageCallbackDerived::~PreprocessMessageCallbackDerived()
{

}

void PreprocessMessageCallbackDerived::OnStart() 
{
	dlg.ShowWindow(SW_SHOW);
}

void PreprocessMessageCallbackDerived::OnTextMessage(const wchar_t* strMsg) 
{
	dlg.SetText(strMsg);
}

void PreprocessMessageCallbackDerived::OnEnd() 
{
	dlg.DestroyWindow();
}