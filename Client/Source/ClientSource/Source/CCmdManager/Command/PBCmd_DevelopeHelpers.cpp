#include "pch.h"
#include "PBCmd_DevelopeHelpers.h"
#include "Cheatkey_LimitPred.h"

#include "TBoundChk.h"
#include "../UI/UIDefine.h"
#include "../UI/UIMainFrame.h"
#include "../UI/UIUtil.h"
#include "../../UI/Minimap.h"
#include "../../BattleHUD.h"

namespace Cheatkey
{
	//--------------------------------------------------
	// 팝업 on/off
	// ex) "/popup open 1"
	//	   "/popup close 1"
	//--------------------------------------------------
	bool Popup::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.size() <= 1) return false;

		if (::strcmp("OPEN", params[0].c_str()) == 0)
			m_open = true;
		else if (::strcmp("CLOSE", params[0].c_str()) == 0)
			m_open = false;
		else
			m_open = false;

		m_popupIdx = ::atoi(params[1].c_str());

		if (!TBoundChk<INT32>(m_popupIdx, 0, UPW_MAX-1)) return false;

		return true;
	}

	bool Popup::_Execute()
	{
		if (m_open)
			g_pFramework->GetUIMainframe()->OpenPopup(static_cast<UIPOPUPWIN>(m_popupIdx));
		else
			g_pFramework->GetUIMainframe()->ClosePopup(static_cast<UIPOPUPWIN>(m_popupIdx));

		return true;
	}

	ICommand* Popup::Creator()
	{
		return new Popup(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}

	//--------------------------------------------------
	// 메시지박스 on/off
	// ex) "/messagebox open 박스타입 메시지 캡션 폰트사이즈"
	//	   "/messagebox close"
	//--------------------------------------------------
	bool MessageBox::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.empty()) return false;

		if (::strcmp("OPEN", params[0].c_str()) == 0)
			m_open = true;
		else if (::strcmp("CLOSE", params[0].c_str()) == 0)
			m_open = false;
		else
			return false;

		if (m_open)
		{
			if (params.size() == 2)
			{
				m_msg.messageType = ::atoi(params[1].c_str());	// enum MSG_TYPE
			}
			else if (params.size() == 3)
			{
				m_msg.messageType = ::atoi(params[1].c_str());	// enum MSG_TYPE
				m_msg.message.assign(params[2].begin(), params[2].end());
			}
			else if (params.size() == 4)
			{
				m_msg.messageType = ::atoi(params[1].c_str());	// enum MSG_TYPE
				m_msg.message.assign(params[2].begin(), params[2].end());
				m_msg.caption.assign(params[3].begin(), params[3].end());
			}
			else if (params.size() == 5)
			{
				m_msg.messageType = ::atoi(params[1].c_str());	// enum MSG_TYPE
				m_msg.message.assign(params[2].begin(), params[2].end());
				m_msg.caption.assign(params[3].begin(), params[3].end());
				m_msg.fontSize = ::atoi(params[4].c_str());
			}

			if (!TBoundChk<INT32>(m_msg.messageType, 0, MSG_TYPE_MAX-1)) return false;
		}

		return true;
	}

	bool MessageBox::_Execute()
	{
		if (m_open)
			g_pFramework->GetMessageBoxManager()->OpenMessageBox(m_msg.message, m_msg.caption, m_msg.messageType, nullptr, nullptr, m_msg.fontSize, 1, 1);
		else
			g_pFramework->GetMessageBoxManager()->CloseAllMessageBox();

		return true;
	}

	ICommand* MessageBox::Creator()
	{
		return new MessageBox(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}

	//--------------------------------------------------
	// 탭미니맵 속성값 출력
	// ex) "/tabshow on" 
	//	   "/tabshow off"
	//--------------------------------------------------
	bool ShowTabminimapProps::_ParseParams(const i3::vector<i3::string>& params)
	{
		if (params.empty()) return false;
		
		if (params[0] == "ON")
			m_show = true;
		else if (params[0] == "OFF")
			m_show = false;

		return true;
	}

	bool ShowTabminimapProps::_Execute()
	{
		if (m_show)
		{
			i3::string text("Tabminimap properties : \n");

			const minimap_new::TextureInfo& texInfo = minimap_new::Minimap::GetInstance()->GetTexInfo();

			i3::stack_string strval;

			text += "["; text += "WIDTH"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.w); 
			text += strval; strval.clear();

			text += "["; text += "HEIGHT"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.h); 
			text += strval; strval.clear();

			text += "["; text += "WIDTH-RATIO"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.bakedRatioW); 
			text += strval; strval.clear();

			text += "["; text += "HEIGHT-RATIO"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.bakedRatioH); 
			text += strval; strval.clear();

			text += "["; text += "WIDTH-ADJUST"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.ratio.AdjustWidth); 
			text += strval; strval.clear();

			text += "["; text += "HEIGHT-ADJUST"; text += "]  ";
			i3::sprintf(strval, "%f\n", texInfo.mapImageInfo.ratio.AdjustHeight); 
			text += strval; strval.clear();

			i3::wstring wText(text.begin(), text.end());

			CHud::i()->ShowTabminimapProps(wText);
		}
		else
			CHud::i()->HideTabminimapProps();

		return true;
	}

	ICommand* ShowTabminimapProps::Creator()
	{
		return new ShowTabminimapProps(new LPred_PermittedAccountOnly(new LPred_IngameOnly));
	}

	//--------------------------------------------------
	// 프로파일 출력
	// ex) "/fps"
	//--------------------------------------------------
	bool ShowProfile::_Execute()
	{
#if !defined( I3_NO_PROFILE)
		if (g_pViewer->getProfileMode() == i3Viewer::PROFILE_RENDER)
			g_pViewer->setProfileMode(i3Viewer::PROFILE_NONE);
		else
			g_pViewer->setProfileMode(i3Viewer::PROFILE_RENDER);
#else
		if (!CHud::i()) 
			return false;

		CHud::i()->ToggleProfile();
#endif
		return true;
	}

	ICommand* ShowProfile::Creator()
	{
		return new ShowProfile(new LPred_PermittedAccountOnly(new LPred_Anywhere));
	}
}