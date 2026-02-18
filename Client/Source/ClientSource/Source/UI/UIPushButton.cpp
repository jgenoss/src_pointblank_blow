#include "pch.h"
#include "UIPushButton.h"

PushButton_LIST::PushButton_LIST()
{
	m_vbtnlist.clear();
	m_ui8Value = 0xFF;
}

PushButton_LIST::~PushButton_LIST()
{
	i3::cu::for_each_delete_clear(m_vbtnlist);
	m_ui8Value = 0xFF;
}

void	PushButton_LIST::InitList(i3UIScene * pScene )
{
	for each(PushButton* btn in m_vbtnlist)
		btn->pNormalBtn = static_cast<i3UIButton*>(pScene->FindChildByName(btn->str_btn_name.c_str()));

	SetValue(m_vbtnlist[0]->ui8Value);
}

void	PushButton_LIST::PushBtn(i3::string str_btn_name, UINT8 btn_value)
{
	PushButton* push_button = new PushButton();
	push_button->SteBtnInfo(btn_value, nullptr, str_btn_name);
	m_vbtnlist.push_back(push_button);
}

void	PushButton_LIST::SetValue(UINT8 btn_value)
{
	m_ui8Value = btn_value;

	for each(PushButton* btn in m_vbtnlist)
		if (btn->GetPushed() && btn_value == btn->ui8Value) return;

	for each(PushButton* btn in m_vbtnlist)
		if (btn->GetPushed() || btn_value == btn->ui8Value) btn->SetPushed();		
}