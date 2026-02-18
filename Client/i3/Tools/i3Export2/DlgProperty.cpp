#include "stdafx.h"
#include "resource.h"
#include "PickTarget.h"
#include "Util.h"
#include "i3Base/string/ext/ftoa.h"
#include "i3Base/string/ext/atoi.h"

static HWND		s_hwndDlg;
static HWND		s_hwndReverb;
static HWND		s_hwndForceExport;
static HWND		s_hwndThickness;		// Terrain thickness 추가 by dfly79 20071226
static HWND		s_hwndPhysicType;
static HWND		s_hwndProxy;

//cj 13.09.05 외부에서 이용을위해
void _SetDefaultPhysicsProperties(INode * pNode)
{
	pNode->SetUserPropFloat(TSTR("Mass"), 1.0f);
	pNode->SetUserPropFloat(TSTR("Ellasticity"), 0.3f);
	pNode->SetUserPropFloat(TSTR("Density"), 1.0f);
	pNode->SetUserPropFloat(TSTR("Friction"), 1.0f);
}

namespace
{
	static void _NotifyReverb(NotifyInfo * info)
	{
		INT32 i, cnt;
		Interface * ip = GetCOREInterface();
		INT32 reverb = -1, idxReverb = -1;
		INT32 nForceExport = -1, temp;
		bool bCheckReverb = true;
		bool bCheckForceExport = true;

		cnt = ip->GetSelNodeCount();

		// Reverb
		if (cnt == 0)
		{
			SendMessage(s_hwndReverb, CB_SETCURSEL, idxReverb, 0);
			SendMessage(s_hwndReverb, WM_ENABLE, FALSE, 0);

			SendMessage(s_hwndForceExport, BM_SETCHECK, BST_UNCHECKED, 0);
			SendMessage(s_hwndForceExport, WM_ENABLE, FALSE, 0);
		}
		else
		{
			for (i = 0; i < cnt; i++)
			{
				INode * pNode = ip->GetSelNode(i);

				if (bCheckReverb)
				{
					if (pNode->GetUserPropInt(TSTR("Reverb"), reverb) == false)
					{
						idxReverb = -1;
						bCheckReverb = false;
					}
					else
					{
						if (i == 0)
							idxReverb = reverb;
						else
						{
							if (idxReverb != reverb)
							{
								idxReverb = -1;
								bCheckReverb = false;
							}
						}
					}
				}

				if (bCheckForceExport)
				{
					temp = -1;

					if (pNode->GetUserPropInt(TSTR("ForceExport"), temp) == FALSE)
					{
						nForceExport = -1;
						bCheckForceExport = false;
					}
					else
					{
						if (i == 0)
							nForceExport = temp;
						else
						{
							if (nForceExport != temp)
							{
								nForceExport = -1;
								bCheckForceExport = false;
							}
						}
					}
				}
			}

			SendMessage(s_hwndReverb, WM_ENABLE, TRUE, 0);
			SendMessage(s_hwndReverb, CB_SETCURSEL, idxReverb, 0);

			SendMessage(s_hwndForceExport, WM_ENABLE, TRUE, 0);

			switch (nForceExport)
			{
				//case -1 :	SendMessage( s_hwndForceExport, BM_SETCHECK, BST_INDETERMINATE, 0);	break;
			case 0:	SendMessage(s_hwndForceExport, BM_SETCHECK, BST_UNCHECKED, 0);	break;
			case 1:	SendMessage(s_hwndForceExport, BM_SETCHECK, BST_CHECKED, 0);	break;
			}
		}
	}

	static void _NotifyThickness(NotifyInfo * info)
	{
		INT32 i, cnt;
		INT32 thickness = 0, temp = 0;
		Interface * ip = GetCOREInterface();
		bool bCheckThickness = true;

		cnt = ip->GetSelNodeCount();

		char szTemp[16];

		// Thickness
		if (cnt == 0)
		{
			sprintf(szTemp, "-1");
			SendMessage(s_hwndThickness, WM_SETTEXT, 0, (LPARAM)szTemp);
			SendMessage(s_hwndThickness, WM_ENABLE, FALSE, 0);
		}
		else
		{
			for (i = 0; i < cnt; i++)
			{
				INode * pNode = ip->GetSelNode(i);

				if (bCheckThickness)
				{
					if (pNode->GetUserPropInt(TSTR("Thickness"), temp) == false)
					{
						thickness = -1;
						bCheckThickness = false;
					}
					else
					{
						if (i == 0)
							thickness = temp;
						else
						{
							if (thickness != temp)
							{
								thickness = -1;
								bCheckThickness = false;
							}
						}
					}
				}
			}

			SendMessage(s_hwndThickness, WM_ENABLE, TRUE, 0);
			i3::ftoa((REAL32)thickness, szTemp);
			SendMessage(s_hwndThickness, WM_SETTEXT, 0, (LPARAM)szTemp);//thickness, 0);
		}
	}

	static void _NotifyPortal(NotifyInfo * info)
	{
		INT32 i, cnt;
		INT32 portal = -1, temp = 0;
		Interface * ip = GetCOREInterface();
		HWND hwndCtrl;

		cnt = ip->GetSelNodeCount();

		char szTemp[16];

		hwndCtrl = ::GetDlgItem(s_hwndDlg, IDC_EDIT_PORTAL);

		// Thickness
		if (cnt == 0)
		{
			sprintf(szTemp, "");
			SendMessage(hwndCtrl, WM_SETTEXT, 0, (LPARAM)szTemp);
			SendMessage(hwndCtrl, WM_ENABLE, FALSE, 0);
		}
		else
		{
			for (i = 0; i < cnt; i++)
			{
				INode * pNode = ip->GetSelNode(i);

				if (pNode->GetUserPropInt(TSTR("Portal"), temp) == false)
				{
					break;
				}
				else
				{
					if (i == 0)
						portal = temp;
					else
					{
						if (portal != temp)
						{
							portal = -1;
							break;
						}
					}
				}
			}

			SendMessage(hwndCtrl, WM_ENABLE, TRUE, 0);

			if (portal == -1)
			{
				// 선택된 여러 개의 Node들이 서로 다른 Portal 값을 가지고 있다.
				strcpy(szTemp, "");
			}
			else
			{
				sprintf(szTemp, "%d", portal);
			}

			SendMessage(hwndCtrl, WM_SETTEXT, 0, (LPARAM)szTemp);
		}
	}

	static void _NotifyPhysicalType(NotifyInfo * info)
	{
		INT32 i, cnt;
		INT32 physictype = -1, temp = -1;
		Interface * ip = GetCOREInterface();
		bool bEditable = true;

		cnt = ip->GetSelNodeCount();

		// Physic Type
		if (cnt == 0)
		{
			SendMessage(s_hwndPhysicType, CB_SETCURSEL, -1, 0);
			SendMessage(s_hwndPhysicType, WM_ENABLE, FALSE, 0);
		}
		else
		{
			for (i = 0; i < cnt; i++)
			{
				INode * pNode = ip->GetSelNode(i);

				if (bEditable)
				{
					if (pNode->GetUserPropInt(TSTR("PhysicsType"), temp) == false)
					{
						physictype = -1;
						bEditable = false;
						break;
					}
					else
					{
						if (i == 0)
							physictype = temp;
						else
						{
							if (physictype != temp)
							{
								physictype = -1;
								bEditable = false;
								break;
							}
						}
					}
				}
			}

			SendMessage(s_hwndPhysicType, WM_ENABLE, TRUE, 0);

			////////////////////////
			temp = -1;

			if (bEditable)
			{
				switch (physictype)
				{
				case 1:	//	dynamic
					temp = 0;
					break;

				case 2:	//	kinematic
					temp = 1;
					break;

				case 3:	//	static
					temp = 2;
					break;

				default:
					temp = 3;
					break;
				}
			}

			SendMessage(s_hwndPhysicType, CB_SETCURSEL, temp, 0);
		}
	}

	static void _NotifyProxy(NotifyInfo * info)
	{
		INT32 i, cnt;
		Interface * ip = GetCOREInterface();
		bool bEditable = true;
		TSTR temp, proxy = "";
		ICustButton *pBtn = GetICustButton(s_hwndProxy);

		cnt = ip->GetSelNodeCount();

		// Physic Type
		if (cnt == 0)
		{
			pBtn->SetText("(no-selection)");
			pBtn->Enable(FALSE);
		}
		else
		{
			for (i = 0; i < cnt; i++)
			{
				INode * pNode = ip->GetSelNode(i);

				if (bEditable)
				{
					bool bValidProxy = false;

					if (pNode->GetUserPropString(TSTR("Proxy_Geometry"), proxy))
					{
						if (proxy.data()[0] != 0)
							bValidProxy = true;
					}

					if (bValidProxy == false)
					{
						proxy = temp;
						bEditable = false;
						break;
					}
					else
					{
						if (i > 0)
						{
							if (proxy != temp)
							{
								proxy = "";
								bEditable = false;
								break;
							}
						}
					}
				}
			}

			pBtn->Enable(TRUE);

			pBtn->SetText((char*)proxy);
		}

		ReleaseICustButton(pBtn);
	}

	static void _SelectNotifyCallBack(void *param, NotifyInfo *info)
	{
		if (!info || !param) return;

		_NotifyReverb(info);
		_NotifyThickness(info);
		_NotifyPortal(info);
		_NotifyPhysicalType(info);
		_NotifyProxy(info);
	}

	static void _SetReverbType(INT32 reverb)
	{
		INT32 i;
		Interface * ip = GetCOREInterface();

		if (reverb < 0)
			reverb = 0;

		// Reverb
		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);

			pNode->SetUserPropInt(TSTR("Reverb"), reverb);
		}
	}

	static void _SetForceExport(bool bFlag)
	{
		INT32 i;
		Interface * ip = GetCOREInterface();

		// Reverb
		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);

			pNode->SetUserPropInt(TSTR("ForceExport"), bFlag);
		}
	}

	static void _SetThickness(INT32 thickness)
	{
		INT32 i;
		Interface * ip = GetCOREInterface();

		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);
			pNode->SetUserPropInt(TSTR("Thickness"), thickness);
		}
	}

	static void _SetPortal(INT32 portal)
	{
		INT32 i;
		Interface * ip = GetCOREInterface();

		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);

			pNode->SetUserPropInt(TSTR("Portal"), portal);
		}
	}

	

	static void _SetPhysicType(INT32 type)
	{
		INT32 i;
		Interface * ip = GetCOREInterface();

		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);

			pNode->SetUserPropInt(TSTR("PhysicsType"), type);

			if ((type == 1) || (type == 2) || (type == 3))
			{
				_SetDefaultPhysicsProperties(pNode);
			}
		}
	}

	static PickTarget s_pick;

	static void _SetProxy(void)
	{
		Interface * ip = GetCOREInterface();
		INT32 i;

		s_pick.Reset();

		for (i = 0; i < ip->GetSelNodeCount(); i++)
		{
			INode * pNode = ip->GetSelNode(i);

			s_pick.m_SelNodeList.push_back(pNode);
		}

		s_pick.m_hwndDlg = s_hwndDlg;
		s_pick.m_hwndBtn = s_hwndProxy;

		ip->SetPickMode(&s_pick);
	}

	static void	_OnInitDialog(HWND hwnd)
	{
		INT32 i;
		HWND hwndCtrl;
		static char s_ReverbNameTable[I3SND_FX_REVERB_COUNT][32] =
		{
			"(없음)",
			"일반",
			"방음실",
			"방",
			"욕실",
			"거실",
			"석실",
			"방청석",
			"공연장",
			"동굴",
			"아레나",
			"카펫깔린 복도",
			"복도",
			"숲",
			"도시",
			"산속",
			"물속"
		};

		s_hwndDlg = hwnd;

		// Reverb
		{
			hwndCtrl = ::GetDlgItem(hwnd, IDC_CB_REVERBTYPE);
			I3ASSERT(hwndCtrl != NULL);

			s_hwndReverb = hwndCtrl;

			for (i = 0; i < I3SND_FX_REVERB_COUNT; i++)
			{
				SendMessage(hwndCtrl, CB_ADDSTRING, 0, (LPARAM)s_ReverbNameTable[i]);
			}
		}

		// Force Export
		{
			s_hwndForceExport = ::GetDlgItem(hwnd, IDC_CHK_FORCEEXPORT);
			I3ASSERT(s_hwndForceExport != NULL);
		}

		// Thickness
		{
			s_hwndThickness = ::GetDlgItem(hwnd, IDC_EDIT_THICKNESS);
			I3ASSERT(s_hwndThickness != NULL);
		}

		// Physic Type
		{
			s_hwndPhysicType = ::GetDlgItem(hwnd, IDC_CB_PHYSICSTYPE);

			SendMessage(s_hwndPhysicType, CB_ADDSTRING, 0, (LPARAM) "Dynamic");
			SendMessage(s_hwndPhysicType, CB_ADDSTRING, 0, (LPARAM) "Kinematic");
			SendMessage(s_hwndPhysicType, CB_ADDSTRING, 0, (LPARAM) "Static");
			SendMessage(s_hwndPhysicType, CB_ADDSTRING, 0, (LPARAM) "(None)");
		}

		// Proxy
		{
			s_hwndProxy = ::GetDlgItem(hwnd, IDC_BTN_PROXY);

			ICustButton * pBtn = GetICustButton(s_hwndProxy);

			pBtn->SetHighlightColor(RGB(249, 150, 60));
			pBtn->SetCheckHighlight(TRUE);
			pBtn->SetType(CBT_CHECK);
		}

		///
		// Register Notification Callback
		RegisterNotification(_SelectNotifyCallBack, hwnd, NOTIFY_SELECTIONSET_CHANGED);
	}

	static void _OnCommand(HWND hwnd, UINT code, UINT id, HWND hwndCtrl)
	{
		// Edit Box의 Focus 입력을 위한 처리
		switch (code)
		{
		case EN_SETFOCUS:		DisableAccelerators();		break;
		case EN_KILLFOCUS:		EnableAccelerators();		break;
		}

		switch (id)
		{
		case IDC_CB_REVERBTYPE:
			if (code == CBN_SELCHANGE)
			{
				INT32 idx = SendMessage(hwndCtrl, CB_GETCURSEL, 0, 0);

				_SetReverbType(idx);
			}
			break;

		case IDC_CHK_FORCEEXPORT:
			if (code == BN_CLICKED)
			{
				bool bFlag = (SendMessage(hwndCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED);

				_SetForceExport(bFlag);
			}
			break;

		case IDC_EDIT_THICKNESS:
			if (code == EN_CHANGE)
			{
				char text[64];
				memset(&text, 0, 64);
				//SendMessage( hwndCtrl, EM_GETLINE, 0, (LPARAM)text);
				SendMessage(hwndCtrl, WM_GETTEXT, 64, (LPARAM)text);
				//
				INT32 nTickness = i3::atoi((const char*)text);

				if (nTickness < 0)
					nTickness = 0;

				_SetThickness(nTickness);
			}
			break;

		case IDC_EDIT_PORTAL:
			switch (code)
			{
			case EN_CHANGE:
			{
				char conv[256];

				SendMessage(hwndCtrl, WM_GETTEXT, sizeof(conv) - 1, (LPARAM)conv);

				_SetPortal(atoi(conv));
			}
			break;
			}
			break;

		case IDC_CB_PHYSICSTYPE:
			if (code == CBN_SELCHANGE)
			{
				INT32 idx = SendMessage(hwndCtrl, CB_GETCURSEL, 0, 0);
				INT32 PhysicType = 4;		// non-physical

				switch (idx)
				{
				case 0:		PhysicType = 1;	break;	// Dynamic
				case 1:		PhysicType = 2;	break;	// Kinematic
				case 2:		PhysicType = 3;	break;	// Static
				}

				_SetPhysicType(PhysicType);
			}
			break;

		case IDC_BTN_PROXY:
			if (code == BN_CLICKED)
			{
				_SetProxy();
			}
			break;
		}
	}
}

BOOL CALLBACK _DlgPropertiesProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch( msg)
	{	
		case WM_INITDIALOG :			_OnInitDialog( hwnd);	break;
		case WM_COMMAND :				_OnCommand( hwnd, HIWORD( wParam), LOWORD( wParam), (HWND) lParam);	break;
	}

	return FALSE;
}
