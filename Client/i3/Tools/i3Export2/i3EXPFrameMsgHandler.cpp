#include "stdafx.h"
#include "i3EXPFrameMsgHandler.h"
#include "i3Export.h"

i3EXPFrameMsgHandler::i3EXPFrameMsgHandler( i3Export * pExport)
{
	m_pExport = pExport;
}

int i3EXPFrameMsgHandler::ProcessMessage( UINT message,  WPARAM wParam,  LPARAM lParam)
{
	switch( message) 
	{
		case WM_SIZING:
			{
				//
				// Lock the size of the toolbar
				//
				RECT *rc = (RECT*)lParam;
				GetWindowRect( m_pExport->getHWND(), rc );
			}
			return TRUE;

		case WM_SIZE: 
			{
				ICUIFrame *iCUIFrame = GetICUIFrame( m_pExport->getHWND());
				DWORD pos = iCUIFrame->GetCurPosition();
				ReleaseICUIFrame(iCUIFrame);

				// Make sure the floating window is the correct height
				if( pos == CUI_FLOATING ) 
				{
					RECT rect;
					GetWindowRect( m_pExport->getHWND(), &rect);
					int iHeight = rect.bottom-rect.top;

					SetWindowPos( m_pExport->getHWND(), NULL, 0, 0, rect.right-rect.left, 
							iHeight, SWP_NOMOVE|SWP_NOZORDER);
				}
			}
			break;

		case CUI_POSDATA_MSG: 
			{
				CUIPosData **cpd = (CUIPosData **)lParam;
				cpd[0] = m_pExport->getPosData();
			}
			return TRUE;
			
		default:
			break;
	}

	return FALSE;
}

