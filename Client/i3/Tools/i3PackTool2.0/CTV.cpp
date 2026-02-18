#include "stdafx.h"
#include "CTV.h"

// devpia 어느분의 함수 개조 <-- 이 분의 함수 개조함(tree <-> tree 연동 가능토록. 지우근)
HTREEITEM CTV::DragItem(CTreeCtrl* pTree, HTREEITEM hDragItem, CPoint point)
{
	if(GetCapture() != NULL) return NULL;	// 이미 다른 윈도가 마우스 캡처 중이면,,,즐;;;

	AfxLockTempMaps();    // MFC 어플 프렘웍의 메시지 맵을 잠시 막음
	pTree->SetCapture();	// 마우스 캡처 시작
	pTree->Invalidate(FALSE);	// 컨트롤을 다시 그림

	CImageList* pDragImage = new CImageList();
	pDragImage->Create(16, 16, ILC_MASK | ILC_COLOR32, 0, 0);
	CBitmap bitmap;
	bitmap.LoadBitmap(IDB_BITMAP);		// 리소스에 32비트 16x16 이미지 만들어두어야 합니다.
	pDragImage->Add(&bitmap, RGB(255,255,255));
	if(pDragImage == NULL) {ReleaseCapture(); AfxUnlockTempMaps(FALSE);	return NULL;}

	// 드랙 처리 시작
	CRect rect;
	CWnd* pWnd = pTree->GetCapture();
	pTree->GetItemRect(hDragItem, &rect, TRUE);
	//rect.left -= pTree->GetIndent();

	pDragImage->BeginDrag(0, point - rect.TopLeft());
	pDragImage->DragEnter(pWnd, point);

	pTree->SelectDropTarget(NULL);
	HTREEITEM hDropTargetItem = NULL;

	MSG msg;
	BOOL bExit = false;

	while(GetMessage(&msg, NULL, 0, 0)) {
		if(pWnd != pTree->GetCapture() || bExit) break;

		switch(msg.message) {
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
				bExit = true;
				break;

			case WM_MOUSEMOVE:		// 왜 마우스 좌표가 틀리지?
				point.x = (short)LOWORD(msg.lParam);
				point.y = (short)HIWORD(msg.lParam);
				hDropTargetItem = pTree->HitTest(point);

				// 자신이나 자기 차일드에 드롭 방지
				if((hDropTargetItem == hDragItem) || (hDropTargetItem == pTree->GetParentItem(hDragItem))) hDropTargetItem = NULL; 
				else {  // 드롭
					HTREEITEM hItem = hDropTargetItem;
					while((hItem = pTree->GetParentItem(hItem)) != NULL) {
						if(hItem == hDragItem) {
							hDropTargetItem = NULL;
							break;
						}
					}
				}

				if(hDropTargetItem != pTree->GetDropHilightItem()) {
					SetCursor(AfxGetApp()->LoadStandardCursor((hDropTargetItem == NULL) ? IDC_NO : IDC_ARROW));
					pDragImage->DragShowNolock(FALSE);
					pTree->SelectDropTarget(hDropTargetItem);
					pDragImage->DragShowNolock(TRUE);

					pTree->SetTimer(ID_TIMER_AUTOEXPAND, ID_TIMER_AUTOEXPAND, NULL);                // 오토 익스팬드를 위한 타이머를 설치합니답
				}

				// 오토 스크롤 영역 테스트를 위해서 컨트롤의 위치를 얻습니답
				pTree->GetClientRect(&rect);
				if(((point.x >= rect.left) && (point.x <= rect.right)) && ((point.y < rect.top + 10) || (point.y > rect.bottom)))
				{
					pTree->SetTimer(ID_TIMER_AUTOSCROLL, ID_TIMER_AUTOSCROLL, NULL);                // 오토 스크롤을 위한 타이머를 설치합니답
				}

				pDragImage->DragMove(point);
				break;

			case WM_TIMER:
				if(msg.wParam == ID_TIMER_AUTOEXPAND)   // 오토 익스팬드 처리
				{
					hDropTargetItem = pTree->GetDropHilightItem();

					if(hDropTargetItem != NULL) {
						pDragImage->DragShowNolock(FALSE);
						pTree->Expand(hDropTargetItem, TVE_EXPAND);
						pDragImage->DragShowNolock(TRUE);
					}
					pTree->KillTimer(ID_TIMER_AUTOEXPAND);

				} else if(msg.wParam == ID_TIMER_AUTOSCROLL) {           // 오토 스크롤 처리
					GetCursorPos(&point);
					pTree->ScreenToClient(&point);
					pTree->GetClientRect(&rect);

					BOOL bTestX = (point.x >= rect.left) && (point.x <= rect.right);
					if(bTestX && (point.y < rect.top)) pTree->SendMessage(WM_VSCROLL, SB_LINEUP);
					else if(bTestX && (point.y > rect.bottom)) pTree->SendMessage(WM_VSCROLL, SB_LINEDOWN);
					else pTree->KillTimer(ID_TIMER_AUTOSCROLL);
				}
				break;

			case WM_KEYDOWN:
				if(msg.wParam == VK_ESCAPE) {  // 드랙 취소
					pTree->SelectDropTarget(NULL);
					bExit = true;
				}
				break;

			default:// 원래의 메시지 핸들러에게 메시지를 넘깁니답
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
		}
	}

	pTree->KillTimer(ID_TIMER_AUTOSCROLL);
	pTree->KillTimer(ID_TIMER_AUTOEXPAND);

	pDragImage->DragLeave(pWnd);
	pDragImage->EndDrag();
	delete pDragImage;

	pTree->Invalidate(FALSE);// 화면을 갱신하고
	ReleaseCapture();// 마우스 캡처를 중지하고
	AfxUnlockTempMaps(FALSE);// 잠시 막아놨던 메시지 맵을 풉니답

	hDropTargetItem = pTree->GetDropHilightItem();// 지정된 드롭 타겟 아이템을 백업 해두고
	pTree->SelectDropTarget(NULL);// 초기화 합니답

	return hDropTargetItem;    // 백업해둔 드롭 타겟 아이템을 리턴하면 끝 ^0^//
}

HTREEITEM CTV::CopyItem(CTreeCtrl* pTree, HTREEITEM hSrc, HTREEITEM hDestParent, HTREEITEM hCopyAfter)
{

	TCHAR lpszText[256];
	TVINSERTSTRUCT tvis;

	ZeroMemory(&tvis, sizeof(tvis));
	tvis.hParent = hDestParent;
	tvis.hInsertAfter = hCopyAfter;
	tvis.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_STATE | TVIF_PARAM;
	tvis.item.hItem = hSrc;
	tvis.item.pszText = lpszText;
	tvis.item.cchTextMax = 256;
	tvis.item.stateMask = TVIS_BOLD | TVIS_CUT | TVIS_EXPANDED;
	pTree->GetItem(&tvis.item);

	HTREEITEM hCopyItem = pTree->InsertItem(&tvis);
	if(hCopyItem == NULL) return NULL;
	HTREEITEM hItem = pTree->GetChildItem(hSrc);

	while(hItem != NULL) {
		CopyItem(pTree, hItem, hCopyItem);
		hItem = pTree->GetNextSiblingItem(hItem);
	}

	return hCopyItem;
}

void CTV::DeleteItem(CTreeCtrl* pTree, HTREEITEM hItem)
{
	HTREEITEM hChildItem = pTree->GetChildItem(hItem);

	while(hChildItem != NULL) {	// 차일드부터 삭제.
		DeleteItem(pTree, hChildItem);
		hChildItem = pTree->GetNextSiblingItem(hChildItem);
	}

	pTree->DeleteItem(hItem);	// 차일드부터 삭제.
}