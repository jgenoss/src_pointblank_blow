// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OptionDialog.h"
#include "resource.h"
#include "i3Export.h"
#include "OptionInfo.h"
#include "SelectBoneDialog.h"

inline void _SetTextCtrl( HWND hwndDialog, UINT id, char * pszText)
{
	HWND hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		::SetWindowText( hwndCtrl, pszText);
	}
}

static char s_szHelp[] =
"\
CCR_            :  콘크리트\r\n\
MET_            :  금속\r\n\
GND_            :  흙\r\n\
WOOD_           :  나무\r\n\
SNOW_           :  눈\r\n\
WTD_            :  물 (깊은)\r\n\
WTS_            :  물 (얕은)\r\n\
CLAY_           :  흙 (진흙)\r\n\
GRASS_          :  잔디\r\n\
MARB_           :  대리석\r\n\
LEAF_           :  낙엽\r\n\
PAP_            :  종이\r\n\
GLASS_          :  유리\r\n\
PLASTIC_        :  플라스틱\r\n\
RUB_            :  고무\r\n\
CLOTH_          :  옷감\r\n\
TH_CCR_         :  콘크리트 (얇은)\r\n\
TH_MET_         :  금속 (얇은)\r\n\
TH_WOOD_        :  나무 (얇은)\r\n\
TH_MARB_        :  대리석 (얇은)\r\n\
TH_GND_         :  흙 (얇은) <관통>\r\n\
TH_SNOW_        :  눈 (얇은) <관통>\r\n\
TH_CLAY_        :  흙 (진흙-얇은) <관통>\r\n\
TH_GRASS_       :  잔디 (얇은) <관통>\r\n\
TH_PAP_         :  종이 (얇은) <관통>\r\n\
TH_LEAF_        :  낙엽 (얇은) <관통>\r\n\
TH_GLASS_       :  유리 (얇은) <관통>\r\n\
TH_PLASTIC_     :  플라스틱 (얇은) <관통>\r\n\
TH_RUB_         :  고무 (얇은) <관통>\r\n\
TH_CLOTH_       :  옷감 (얇은) <관통>\r\n\
TH_BARBEDWIRE_  :  창살 <관통>\r\n\
WR_MET_         :  철사 <관통>\r\n\
WR_WOOD_        :  나무 줄기 <관통>\r\n\
WR_PLASTIC_     :  플라스틱(가는) <관통>\r\n\
BLOOD_          :  피\r\n\
TEMP1_          :  임시1\r\n\
TEMP2_          :  임시2\r\n\
ND_CLOTH_       :  천(데칼없는)\r\
NONE_		    :  아무것도 없다\r\n";

BOOL CALLBACK  _HelpDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;

	switch( msg) 
	{
		case WM_INITDIALOG:
			{				
				_SetTextCtrl( hwnd, IDC_STATIC_HELP, s_szHelp);

				CenterWindow( hwnd, GetParent( hwnd));
			}
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case IDOK:
					case IDCANCEL :
						EndDialog( hwnd, id);
						break;
				}
			}
			break;

		default:
			return false;
	}
	return true;
}
