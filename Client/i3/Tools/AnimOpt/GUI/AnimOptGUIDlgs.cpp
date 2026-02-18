
#include "AnimOptGUIPCH.h"
#include "AnimOptGUIDlgs.h"

#include "resource.h"

#include "i3Math/i3MathType.h"
#include "i3Math/i3AnimationPackFile.h"
#include "i3Math/i3Animation.h"
#include "i3Math/i3Animation2.h"
#include "i3Math/i3AnimationResManager.h"


//////////////////////////////////////..((((((((((((((((((((////////////////////////////////////
// defined at i3AnimationPackFile.cpp
//////////////////////////////////////////////////////////////////////////

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ANIMPACKFILE_HEADER
	{
		UINT8		m_ID[4] = { 'A', 'P', 'F', '2' };

		UINT32		m_AnimCount;
		UINT32		m_TArrayCount;
		UINT32		m_RArrayCount;
		UINT32		m_SArrayCount;

		INT64		m_DataOffset;
		INT64		m_DataSize;

		UINT32		m_T16ArrayCount;
		UINT32		m_R16ArrayCount;
		UINT32		m_S16ArrayCount;

		UINT32		pad[29];
	};

	struct ANIM_INFO
	{
		UINT32		m_Type;		// 0 : i3Animation, 1:i3Animation2, 2:i3Animation2 & 16Bits 3:i3Animation2 TrackTable's 32bits
		UINT32		m_TrackCount;
		REAL32		m_Duration;
		INT64		m_Offset;
		char		m_szName[256];
		UINT32		pad[2];
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif




template<class TMMapIn, class TOper>
static void MMapOperator(TMMapIn& in, TOper& oper)
{
	for(TMMapIn::iterator xIter = in.begin(); xIter != in.end();
		xIter=in.upper_bound((*xIter).first))
	{
		TMMapIn::_Pairii rangeIter = in.equal_range((*xIter).first);

		for(TMMapIn::iterator itemIter = rangeIter.first;
			itemIter != rangeIter.second; itemIter++)
		{
			oper((*itemIter).first, (*itemIter).second);
		}
	}
}


static void SubClassCtl(HWND hWnd, bool subClass, void* prm0=NULL, void* prm1=NULL, void* prm2=NULL)
{
	if(!hWnd)
		return;

	if(subClass)
	{	
		// 0:rst old procedure, 1:new procedure, , 2:instance
		WNDPROC* ppRstOldProc = (WNDPROC*)prm0;
		*ppRstOldProc = (WNDPROC)GetWindowLongPtr(hWnd, GWL_WNDPROC);

		SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)prm1);
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)prm2);
	}
	else
	{
		SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)prm0);  // old procedure
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)NULL);
	}
}


//////////////////////////////////////////////////////////////////////////
// FileDropTargetCtl
//////////////////////////////////////////////////////////////////////////

FileDropTargetCtl::FileDropTargetCtl(HWND hCtl, CallBack* callBack)
{
	m_oldProc = NULL;
	m_hwnd = hCtl;
	m_callBack = callBack;

	DWORD exStyle = GetWindowLong(m_hwnd, GWL_EXSTYLE);
	exStyle |= WS_EX_ACCEPTFILES;
	SetWindowLong(m_hwnd, GWL_EXSTYLE, exStyle);

	SubClassCtl(m_hwnd, true, (void*)&m_oldProc, (void*)&FileDropTargetCtl::WndProc, (void*)this);
}

FileDropTargetCtl::~FileDropTargetCtl()
{
	SubClassCtl(m_hwnd, false, (void*)m_oldProc);
}

void FileDropTargetCtl::OnDropFiles(HDROP hDrop)
{
	if(m_callBack)
	{
		m_callBack->Reset();
		m_callBack->m_dropInfo = hDrop;

		char fileName[MAX_PATH];

		INT32 numFile = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
		for(INT32 i=0; i<numFile; i++)
		{
			DragQueryFile(hDrop, i, fileName, sizeof(fileName) - 1);
			if(!m_callBack->OnIterateDropFiles(numFile, i, fileName))
				break;
		}

		m_callBack->OnIterateAfter(numFile);
	}
}

LRESULT FileDropTargetCtl::WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	FileDropTargetCtl* glue = (FileDropTargetCtl*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	switch(uiMsg)
	{
	case WM_DROPFILES:
		{			
			glue->OnDropFiles((HDROP)wParam);
			DragFinish((HDROP)wParam);			
		}
		break;

	case WM_DESTROY:
		{
			if(glue)
			{
				SubClassCtl(glue->m_hwnd, false, glue->m_oldProc);
				glue->m_hwnd = NULL;
				glue = NULL;
			}
		}
		break;

	default:
		break;
	}

	if(glue)
		return ::CallWindowProc(glue->m_oldProc, hWnd, uiMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);
}


//////////////////////////////////////////////////////////////////////////
// APKFileViewCtl
//////////////////////////////////////////////////////////////////////////

APKFileViewCtl::APKFileViewCtl(AnimOptDlg* owner, HWND listViewWnd)
{
	m_hwnd = listViewWnd;

	m_filterView = false;
	m_filterFlag = 0;

	m_owner = owner;
	m_oldProc = NULL;
	m_imgListStatus = NULL;

	SubClassCtl(m_hwnd, true, (void*)&m_oldProc, (void*)&APKFileViewCtl::WndProc, (void*)this);

	InitView();
}

APKFileViewCtl::~APKFileViewCtl()
{
	if(m_imgListStatus)
	{
		ImageList_Destroy(m_imgListStatus);
		m_imgListStatus = NULL;
	}

	SubClassCtl(m_hwnd, false, (void*)m_oldProc);
}

LRESULT CALLBACK APKFileViewCtl::WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	APKFileViewCtl* glue = (APKFileViewCtl*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	switch(uiMsg)
	{
	case WM_DESTROY:
		{
			if(glue)
			{
				SubClassCtl(glue->m_hwnd, false, glue->m_oldProc);
				glue->m_hwnd = NULL;
				glue = NULL;
			}
		}
		break;

	default:
		break;
	}

	if(glue)
		return ::CallWindowProc(glue->m_oldProc, hWnd, uiMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);
}

LRESULT APKFileViewCtl::OnCustDrawItem(LPARAM param)
{
	LRESULT rst = CDRF_DODEFAULT;
	LPNMHDR hdr = (LPNMHDR)param;
//	LPNMLISTVIEW nlv = (LPNMLISTVIEW)param;

	if(hdr->hwndFrom != m_hwnd)
		return rst;

	if(hdr->code != NM_CUSTOMDRAW)
		return rst;

	LPNMLVCUSTOMDRAW cd = (LPNMLVCUSTOMDRAW)param;

	Row* rowData = (Row*)cd->nmcd.lItemlParam;
	bool badStateItem = false;
	
	if(rowData && rowData->m_flag & 0xff000000)
		badStateItem = true;
	
	switch(cd->nmcd.dwDrawStage)
	{
	case CDDS_PREPAINT: 
		rst = CDRF_NOTIFYITEMDRAW; 
		break;

	case CDDS_ITEMPREPAINT:
		{
			if(badStateItem)
			{
				cd->clrTextBk = RGB(255, 0, 0);
			}
			
			rst = CDRF_NOTIFYSUBITEMDRAW; // or CDRF_NEWFONT
		}
		break;

	case CDDS_SUBITEM:
		{
			if(badStateItem)
			{
				cd->clrTextBk = RGB(255, 0, 0);
			}

			rst = CDRF_NEWFONT;
		}		
		break;
	}

	return rst;
}

void APKFileViewCtl::InitView()
{
	const char*			hdrColStr[]	 = { "No", "Flag", "AnimPak", "i3Animation", "status" };
	const static INT32	hdrColWidth[] = { 50, 100, 200, 200, 200 };
	const static INT32  numHdrColStr = sizeof(hdrColStr) / sizeof(hdrColStr[0]);

	LVCOLUMN lvCol;
	lvCol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvCol.fmt = LVCFMT_LEFT;

	m_imgListStatus = ImageList_LoadBitmap(m_owner->m_moduleInst, 
		MAKEINTRESOURCE(IDB_APKVIEW_STATUS), 16,1, RGB(255,255,255));

	ListView_SetImageList(m_hwnd, m_imgListStatus, LVSIL_SMALL);

	for(INT32 i=0; i<numHdrColStr; i++)
	{
		lvCol.cx = hdrColWidth[i];
		lvCol.pszText = (char*)(hdrColStr[i]);
		lvCol.iSubItem = i;

		ListView_InsertColumn(m_hwnd, i, &lvCol);
	}

	ListView_SetExtendedListViewStyle(m_hwnd, LVS_EX_FULLROWSELECT);
}


void APKFileViewCtl::ResetContent()
{
	struct DeleteRow
	{
		void operator()(const RowMMap::key_type& key, RowMMap::mapped_type& val)
		{
			delete val;
		}
	} deleter;

	MMapOperator(m_row, deleter);
	m_row.clear();

	RefreshRow();
}

bool APKFileViewCtl::AddAnimPack(const char* path, bool append)
{
	if(!path)
		return false;

	const char* p = strrchr(path, '.');

	if(!p)
		return false;

	if(stricmp(p, ".i3AnimPack") != 0)
		return false;

	if(!append)
		ResetContent();

	GenerateAnimPackFileDescRow(path);
	
	return true;
}

bool APKFileViewCtl::GenerateAnimPackFileDescRow(const char* path)
{
	i3FileStream stream;

	if(!stream.Open(path, STREAM_READ | STREAM_SHAREREAD))
		return false;

	INT32 Rc = 0;
	
	i3MemoryBuffer *	pBuff = NULL;
	VEC3D *				pTArray = NULL;
	QUATERNION *		pRArray = NULL;
	VEC3D *				pSArray = NULL;
	REAL16 *			pTArray16Bits = NULL;
	REAL16 *			pRArray16Bits = NULL;
	REAL16 *			pSArray16Bits = NULL;
	bool				b16Bits = false;
	pack::ANIMPACKFILE_HEADER header;

	{
		Rc = stream.Read(&header, sizeof(header));

		if(memcmp( header.m_ID, "APF2", 4) == 0 )
			b16Bits = true;
	}

	if(header.m_DataSize == 0)
	{
		Row* newRow = new Row;
		newRow->m_no = 0;
		newRow->m_flag = 0x08000000;

		if(b16Bits)
			newRow->m_flag |= 0x00000001;

		strcpy_s(newRow->m_animPakFileName, MAX_PATH, path);
		strupr(newRow->m_animPakFileName);

		strcpy_s(newRow->m_i3AName, MAX_PATH, "***empty file***");

		RowMMap::value_type val(newRow->m_animPakFileName, newRow);
		m_row.insert(val);

		return true;
	}

	// Animation Data Load
	pBuff = i3MemoryBuffer::new_object_ref();
	pBuff->Create( (UINT32) header.m_DataSize);

	stream.SetPosition((INT32)header.m_DataOffset, STREAM_BEGIN);
	Rc = stream.Read(pBuff->getBuffer(), (INT32) header.m_DataSize);

	UINT32	iIndexer = 0;

	if(header.m_TArrayCount > 0)
	{
		pTArray = (VEC3D *) pBuff->getBuffer();
		iIndexer += sizeof(VEC3D) * header.m_TArrayCount;
	}

	if( header.m_RArrayCount > 0)
	{
		pRArray = (QUATERNION *) ((char*)pBuff->getBuffer() + iIndexer);
		iIndexer += sizeof(QUATERNION) * header.m_RArrayCount;
	}

	if( header.m_SArrayCount > 0 )
	{
		pSArray = (VEC3D *) ((char*)pBuff->getBuffer() + iIndexer);
		iIndexer += sizeof(VEC3D) * header.m_SArrayCount;
	}

	if( header.m_T16ArrayCount > 0)
	{
		pTArray16Bits = (REAL16*) ((char*)pBuff->getBuffer() + iIndexer);
		iIndexer += sizeof(REAL16) * header.m_T16ArrayCount * 3;
	}

	if( header.m_R16ArrayCount > 0)
	{
		pRArray16Bits = (REAL16*) ((char*)pBuff->getBuffer() + iIndexer);
		iIndexer += sizeof(REAL16) * header.m_R16ArrayCount * 4;
	}

	if( header.m_S16ArrayCount > 0)
	{
		pSArray16Bits = (REAL16*) ((char*)pBuff->getBuffer() + iIndexer);
	}

	stream.SetPosition( sizeof(header), STREAM_BEGIN);

	std::vector<i3Animation2*> i3ani2ptrs;

	typedef std::vector<Row*> RowVec;
	RowVec row;

	// Animation Table
	pack::ANIM_INFO info;
	
	for(INT32 i=0; i<(INT32)header.m_AnimCount; i++)
	{
		i3Animation2 * pAnim = i3Animation2::new_object();

		INT32 sizi3A = 0;

		sizi3A = stream.Read(&info, sizeof(info));

		Row* newRow = new Row;
		row.push_back(newRow);

		newRow->m_no = i;
		newRow->m_flag = 0;

		if(b16Bits)
			newRow->m_flag |= 0x00000001;

		if(Rc == STREAM_ERR)
		{
			I3_SAFE_RELEASE(pAnim);

			newRow->m_no = -1;
			newRow->m_flag |= 0x80000000;
			break;
		}

		newRow->m_type = info.m_Type;
		newRow->m_trackCnt = info.m_TrackCount;
		newRow->m_Offset = info.m_Offset;

		strcpy_s(newRow->m_animPakFileName, MAX_PATH, path);
		strupr(newRow->m_animPakFileName);

		strcpy_s(newRow->m_i3AName, MAX_PATH, info.m_szName);
		strupr(newRow->m_i3AName);

		if( info.m_Type >= 2)
		{			
			if( info.m_Type == 3)
			{
				newRow->m_flag |= 0x00000010;
				pAnim->LoadData16Bits( pBuff, (INT32) info.m_Offset, info.m_TrackCount, info.m_Duration, 1);
			}
			else
			{
				newRow->m_flag |= 0x00000020;
				pAnim->LoadData16Bits( pBuff, (INT32) info.m_Offset, info.m_TrackCount, info.m_Duration);
			}

			pAnim->setArrays( pTArray16Bits, pRArray16Bits, pSArray16Bits);
		}
		else
		{
			newRow->m_flag |= 0x00000040;

			pAnim->LoadData( pBuff, (INT32) info.m_Offset, info.m_TrackCount, info.m_Duration);
			pAnim->setArrays( pTArray, pRArray, pSArray);
		}


		//char outPath[MAX_PATH] = {0};

		//char outPath2[MAX_PATH] = {0};
		//strcpy_s(outPath2, MAX_PATH, newRow->m_i3AName);
		//char* p = strrchr(outPath2, '/');
		//p++;

		//i3OptAnimation opt;
		//opt.OptimizeAnim( pAnim, false, false);

		//sprintf_s(outPath, "d:/test/%s", p);

		//pAnim->SaveToFile(outPath);

		i3ani2ptrs.push_back(pAnim);
	}

	// sort
	struct sorterLess
	{
		bool operator()(const APKFileViewCtl::Row* a, const APKFileViewCtl::Row* b) const
		{
			if(stricmp(a->m_animPakFileName, b->m_animPakFileName) < 0)
				return true;

			return false;
		}
	};
	
	std::sort(row.begin(), row.end(), sorterLess());

	// insert 
	for(INT32 i=0; i<(INT32)row.size(); i++)
	{
		std::string key = "**error**";

		if(strlen(row[i]->m_animPakFileName) > 0)
			key = std::string(row[i]->m_animPakFileName);

		RowMMap::value_type val(key, row[i]);
		m_row.insert(val);
	}

	// clean up
	for(INT32 i=0; i<(INT32)i3ani2ptrs.size(); i++)
		I3_SAFE_RELEASE(i3ani2ptrs[i]);

	stream.Close();
	
	return true;
}

void APKFileViewCtl::RefreshRow()
{
	ListView_DeleteAllItems(m_hwnd);

	LVITEM item;
	memset(&item, 0, sizeof(item));

	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;
	item.stateMask = LVIS_STATEIMAGEMASK | LVIS_FOCUSED | LVIS_SELECTED;
	item.state = 0; //INDEXTOSTATEIMAGEMASK(0);
	item.cchTextMax = MAX_PATH;

	INT32 rowNum = 0;
	char text[MAX_PATH];

	RowMMap::iterator x = m_row.begin();
	for(;x != m_row.end(); x = m_row.upper_bound((*x).first))
	{
		RowMMap::_Pairii rangeIter = m_row.equal_range((*x).first);

		for(RowMMap::iterator itemIter = rangeIter.first; itemIter != rangeIter.second; itemIter++)
		{
			Row* val = (*itemIter).second;

			bool skip = false;

			val->m_filtered = false;

			if(m_filterView)
			{
				skip = true;

				if(m_filterFlag)
				{
					bool badState = false;

					if(val->m_flag & 0xff000000)
						badState = true;

					if(!badState && m_filterFlag & EFIL_I3ANIPATH)
					{
						I3AniPathStore src(val->m_i3AName);
						if(m_filterI3AniPath == src)
							skip = false;
					}

					if(m_filterFlag & EFIL_BADSTATE_ANIMPACK)
					{
						if(badState)
							skip = false;
					}
				}

				val->m_filtered = !skip;
			}

			if(skip)
				continue;

			item.iItem = rowNum;
			item.iSubItem = 0;

			item.pszText = text;
			sprintf_s(text, MAX_PATH, "%03d", rowNum);
			item.lParam = (LPARAM)val;

			item.iImage = 0;

			ListView_InsertItem(m_hwnd, &item);

			sprintf_s(text, MAX_PATH, "%08x", val->m_flag);
			ListView_SetItemText(m_hwnd, rowNum, 1, (LPSTR)text);
			ListView_SetItemText(m_hwnd, rowNum, 2, val->m_animPakFileName);
			ListView_SetItemText(m_hwnd, rowNum, 3, val->m_i3AName);

			rowNum++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// ColorListBox
//////////////////////////////////////////////////////////////////////////

ColorListBox::ColorListBox(HWND hCtl)
{
	m_oldProc = NULL;
	m_hwnd = hCtl;

	DWORD style = GetWindowLong(m_hwnd, GWL_STYLE);

	// if this style modification is not working then
	// goto dialog resource and set OwnerDraw to true
	style |= LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_HASSTRINGS;
	SetWindowLong(m_hwnd, GWL_STYLE, style);

	SubClassCtl(m_hwnd, true, (void*)&m_oldProc, (void*)&ColorListBox::WndProc, (void*)this);
}

ColorListBox::~ColorListBox()
{
	SubClassCtl(m_hwnd, false, (void*)m_oldProc);
}

LRESULT CALLBACK ColorListBox::WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	ColorListBox* glue = (ColorListBox*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	switch(uiMsg)
	{
	case WM_DRAWITEM:
		{
		}
		break;

	case WM_DESTROY:
		{
			if(glue)
			{
				SubClassCtl(glue->m_hwnd, false, glue->m_oldProc);
				glue->m_hwnd = NULL;
				glue = NULL;
			}
		}
		break;

	default:
		break;
	}

	if(glue)
		return ::CallWindowProc(glue->m_oldProc, hWnd, uiMsg, wParam, lParam);
	else
		return ::DefWindowProc(hWnd, uiMsg, wParam, lParam);	
}

void ColorListBox::AddLog(const char* fmt, ...)
{
	char buf[1024];

	va_list args;
	va_start(args, fmt);
	vsprintf(buf, fmt, args);

	SendMessage(m_hwnd, LB_ADDSTRING, 0, (LPARAM)buf);

	INT32 cnt = SendMessage(m_hwnd, LB_GETCOUNT, 0, 0);

	if(cnt != -1 && cnt > 0)
		SendMessage(m_hwnd, LB_SETCURSEL, cnt-1, 0);
}

void ColorListBox::ResetContent()
{
	SendMessage(m_hwnd, LB_RESETCONTENT, 0, 0);
}

LRESULT ColorListBox::OnOwnerDrawItem(LPARAM param)
{
	DRAWITEMSTRUCT* drawInfo = (DRAWITEMSTRUCT*)param;

	if(drawInfo->itemID == -1) // there are no listbox item, skip
		return 0;

	switch(drawInfo->itemAction)
	{
	case ODA_SELECT:
	case ODA_DRAWENTIRE:
		{			
			char buf[1024] = {0};
			TEXTMETRIC tm;
			INT32 yPos;

			SendMessageA(drawInfo->hwndItem, LB_GETTEXT, drawInfo->itemID, (LPARAM)buf); 
			GetTextMetrics(drawInfo->hDC, &tm); 

			yPos = (drawInfo->rcItem.bottom + drawInfo->rcItem.top - tm.tmHeight) / 2;
			INT32 len = strlen(buf);

			// Is the item selected? 
			if(drawInfo->itemState & ODS_SELECTED) 
			{ 
				// Set RECT coordinates to surround only the 
				// Draw a rectangle around bitmap to indicate 
				// the selection.
				FillRect(drawInfo->hDC, &drawInfo->rcItem, (HBRUSH)GetStockObject(DKGRAY_BRUSH));
			}
			else
			{
				FillRect(drawInfo->hDC, &drawInfo->rcItem, (HBRUSH)GetStockObject(WHITE_BRUSH));
			}

			INT32 gab = 6;
			COLORREF oldBkColor = GetTextColor(drawInfo->hDC);
			
			char* p = strchr(buf, ';');

			if(p)
			{
				*p=NULL;

				if(!stricmp(buf, "CLR=RED"))
					SetTextColor(drawInfo->hDC, RGB(255, 0, 0));
				else if(!stricmp(buf, "CLR=GREEN"))
					SetTextColor(drawInfo->hDC, RGB(0, 255, 0));
				else if(!stricmp(buf, "CLR=BLUE"))
					SetTextColor(drawInfo->hDC, RGB(0, 0, 255));
			}
			else
			{
				p = buf;
			}

			TextOut(drawInfo->hDC, gab, yPos, p, len);
			SetTextColor(drawInfo->hDC, oldBkColor);
		}
		break;

	case ODA_FOCUS:
		{
		}
		break;

	default:
		return 0;
	}

	return 1;
}


//////////////////////////////////////////////////////////////////////////
// AnimOpt Dialog
//////////////////////////////////////////////////////////////////////////


AnimOptDlg::AnimOptDlg()
{
	m_moduleInst = GetAnimOptModuleInstance();
	m_hwnd = NULL;

	memset(m_ctl, 0, sizeof(m_ctl));
	m_apkFileView = NULL;

	m_apkFileDropTarget = NULL;
	m_apkFileDropCallBack = NULL;

	m_df = 0;
}

AnimOptDlg::~AnimOptDlg()
{
	OnDestroyDlg();
}

INT32 AnimOptDlg::Do()
{
	if(m_hwnd || !m_moduleInst)
		return IDCANCEL;

	m_hwnd = CreateDialogParam(m_moduleInst, MAKEINTRESOURCE(IDD_ANIMPAKVIEW), 
		NULL, AnimOptDlg::DlgProc, (LPARAM)this);

	if(IsWindow(m_hwnd))
		ShowWindow(m_hwnd, SW_SHOW);
	else
		return IDCANCEL;

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	};

	return IDOK;
}

BOOL CALLBACK AnimOptDlg::DlgProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	AnimOptDlg* glue = (AnimOptDlg*)GetWindowLongPtr(hWnd, DWL_USER);
	
	switch(uiMsg) 
	{
	case WM_DRAWITEM:
		{
			if(glue)
			{
				HWND hCtl = GetDlgItem(hWnd, wParam);
				
				if(glue->m_apkFileAnalyzeLog->GetHwnd() == hCtl)
					glue->m_apkFileAnalyzeLog->OnOwnerDrawItem(lParam);
			}
		}
		break;

	case WM_NOTIFY:
		{
			LPNMHDR hdr = (LPNMHDR)lParam;
//			LPNMLISTVIEW nlv = (LPNMLISTVIEW)lParam;

			LRESULT rst = CDRF_DODEFAULT;

			if(glue && hdr->hwndFrom == glue->m_apkFileView->m_hwnd)
			{
				rst = glue->m_apkFileView->OnCustDrawItem(lParam);				
			}
			else
			{
				break;
			}

			SetWindowLong(hWnd, DWL_MSGRESULT, rst);
			return TRUE;
		}
		break;

	case WM_INITDIALOG:
		{	
			SetWindowLongPtr(hWnd, DWL_USER, (LONG_PTR)lParam);

			glue = (AnimOptDlg*)lParam;
			glue->m_hwnd = hWnd;
			glue->OnInitDialog();
		}
		return TRUE;

	case WM_COMMAND:
		return glue->OnCmd(wParam, lParam);

	case WM_CLOSE:
		{
			if(glue)
				glue->OnDestroyDlg();
			else
				DestroyWindow(hWnd);

			return TRUE;
		}

	case WM_DESTROY:
		PostQuitMessage(0);
		return FALSE;

	default:
		break;		
	}

	return FALSE;
}

void AnimOptDlg::OnDestroyDlg()
{
	if(m_apkFileView)
	{
		delete m_apkFileView;
		m_apkFileView = NULL;
	}

	if(m_apkFileDropTarget)
	{
		delete m_apkFileDropTarget;
		m_apkFileDropTarget = NULL;
	}

	if(m_apkFileDropCallBack)
	{
		delete m_apkFileDropCallBack;
		m_apkFileDropCallBack = NULL;
	}

	if(m_apkFileAnalyzeLog)
	{
		delete m_apkFileAnalyzeLog;
		m_apkFileAnalyzeLog = NULL;
	}

	if(m_hwnd)
	{
		SetWindowLongPtr(m_hwnd, DWL_USER, (LONG_PTR)NULL);
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;		
	}
}

//BOOL AnimOptDlg::OnDrawControlItem(HWND hWnd, DRAWITEMSTRUCT* itemDrawInfo)
//{
//	if(m_apkFileView && m_apkFileView->GetHWnd() == (HWND)hWnd)
//		return m_apkFileView->OnDrawItem(itemDrawInfo);
//
//	return FALSE;
//}

static bool IsBtnChecked(HWND hWnd)
{
	return SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED ? true : false;
}

static char* GetEbTxt( HWND hWnd, char* buf, INT32 bufSiz)
{
	if(!buf)
		return NULL;

	GetWindowText(hWnd, buf, bufSiz);

	if(!strlen(buf))
		return NULL;

	return buf;
}


BOOL AnimOptDlg::OnCmd(WPARAM wParam, LPARAM lParam)
{
	WORD ctl = LOWORD(wParam);
//	WORD evt = HIWORD(wParam);

	switch (ctl) 
	{
	case IDC_APV_BT_EXEC_ANALYZE:
		{
			Analyze();
		}
		break;

	case IDC_APV_VF_BT_FILTER:
		{
			m_apkFileView->m_filterView = true;
			m_apkFileView->m_filterFlag = 0;

			if(IsBtnChecked(m_ctl[VF_CK_BADSTATE_ANIMPACK]))
				m_apkFileView->m_filterFlag |= APKFileViewCtl::EFIL_BADSTATE_ANIMPACK;

			if(IsBtnChecked(m_ctl[VF_CK_I3ANIPATH]))
			{
				m_apkFileView->m_filterFlag |= APKFileViewCtl::EFIL_I3ANIPATH;

				I3AniPathStore& fil = m_apkFileView->m_filterI3AniPath;
				fil.Reset();

				INT32 ctl2[] = {
					VF_CK_0,
					VF_CK_1,	
					VF_CK_2,	
					VF_CK_3,	
					VF_CK_4,
					VF_CK_5
				};

				const static INT32 numCtl = sizeof(ctl2) / sizeof(ctl2[0]);
				char buf[I3AniPathStore::ETOKEN_SIZ] = {0};

				for(INT32 i=0; i<numCtl; i++)
				{
					if(IsBtnChecked(m_ctl[ctl2[i]]))
					{
						fil.SetToken((I3AniPathStore::ETOKEN)i, GetEbTxt(m_ctl[ctl2[i]+1], buf, I3AniPathStore::ETOKEN_SIZ));
						fil.SetCmpFlag((I3AniPathStore::ETOKEN)i);

						if(!IsBtnChecked(m_ctl[ctl2[i]+2]))
							fil.SetCmpExactMatchFlag((I3AniPathStore::ETOKEN)i);
					}
					else
					{

					}
				}
			}

			SetDirty(EDF_VIEWFILTER);
		}
		break;

	case IDC_APV_VF_BT_CLEAR:
		{
			m_apkFileView->m_filterView = false;
			m_apkFileView->m_filterFlag = 0;

			SetDirty(EDF_VIEWFILTER);
		}
		break;

	case IDCANCEL:
	case IDOK:
		PostMessage(m_hwnd, WM_CLOSE, 0, 0);
		return TRUE;

	default:
		break;
	}

	return FALSE;
}

bool AnimOptDlg::OnInitDialog()
{
	m_ctl[LV_APKFILE_LEFT]				= GetDlgItem(m_hwnd, IDC_APV_LV_FILELIST_LEFT);
	m_ctl[EB_ANALYZE_WORKDIR]			= GetDlgItem(m_hwnd, IDC_APV_EB_WORKDIR);
	m_ctl[BT_EXEC_ANALYZE]				= GetDlgItem(m_hwnd, IDC_APV_BT_EXEC_ANALYZE);
	m_ctl[LB_ANALYZE_RST]				= GetDlgItem(m_hwnd, IDC_APV_LB_ANALYZE);

	m_ctl[VF_CK_BADSTATE_ANIMPACK]		= GetDlgItem(m_hwnd, IDC_APV_VF_CK_BADSTATE_ANIMPACK		);
	m_ctl[VF_CK_I3ANIPATH]				= GetDlgItem(m_hwnd, IDC_APV_VF_CK_I3ANIPATH				);

	m_ctl[VF_CK_0] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_0);
	m_ctl[VF_EB_0] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_0);
	m_ctl[VF_CK_1] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_1);
	m_ctl[VF_EB_1] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_1);
	m_ctl[VF_CK_2] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_2);
	m_ctl[VF_EB_2] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_2);
	m_ctl[VF_CK_3] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_3);
	m_ctl[VF_EB_3] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_3);
	m_ctl[VF_CK_4] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_4);
	m_ctl[VF_EB_4] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_4);
	m_ctl[VF_CK_5] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_5);
	m_ctl[VF_EB_5] = GetDlgItem(m_hwnd, IDC_APV_VF_EB_5);

	m_ctl[VF_CK_EXACTMATCH_0] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_0);
	m_ctl[VF_CK_EXACTMATCH_1] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_1);
	m_ctl[VF_CK_EXACTMATCH_2] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_2);
	m_ctl[VF_CK_EXACTMATCH_3] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_3);
	m_ctl[VF_CK_EXACTMATCH_4] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_4);
	m_ctl[VF_CK_EXACTMATCH_5] = GetDlgItem(m_hwnd, IDC_APV_VF_CK_EXACTMATCH_5);


	m_apkFileView = new APKFileViewCtl(this, m_ctl[LV_APKFILE_LEFT]);	 

	m_apkFileDropCallBack = new FileDropCallBack;
	m_apkFileDropCallBack->m_owner = this;

	m_apkFileDropTarget = new FileDropTargetCtl(
		GetDlgItem(m_hwnd, IDC_APV_ST_FILEDROPTARGET), m_apkFileDropCallBack);

	m_apkFileAnalyzeLog = new ColorListBox(m_ctl[LB_ANALYZE_RST]);

	SetWindowText(m_ctl[EB_ANALYZE_WORKDIR],"D:/Work_Local/trunk/Media");

	SetWindowText(m_ctl[VF_EB_0], "CHARA");
	SetWindowText(m_ctl[VF_EB_1], "SWAT_MALE");
	SetWindowText(m_ctl[VF_EB_2], "1PV");
	SetWindowText(m_ctl[VF_EB_3], "AK");
	
	return true;
}

void AnimOptDlg::ClearDirty()
{
	if(!m_df)
		return;

	if(m_df & EDF_APKFILE)
	{
		m_apkFileView->ResetContent();

		for(INT32 i=0; i<m_apkFileDropCallBack->GetNum(); i++)
		{
			m_apkFileView->AddAnimPack(m_apkFileDropCallBack->GetFileName(i), i==0 ? false : true);
		}

		m_df |= EDF_REFRESH_VIEW;	
	}

	if(m_df & EDF_VIEWFILTER)
	{
		m_df |= EDF_REFRESH_VIEW;
	}

	if(m_df & EDF_REFRESH_VIEW)
	{
		m_apkFileView->RefreshRow();
	}

	m_df = 0;
}


template<class TRow>
class CheckI3AFile
{
public:
	CheckI3AFile()
	{
		m_fliteredOnly = false;
	}

	template<class T>
	struct RowData
	{
		T			m_ori;
		char		m_i3aOnlyName[MAX_PATH] = { 0 };
	};

	typedef RowData<typename TRow::mapped_type>	RowDataType;
	typedef std::vector<RowDataType > RowVec;
	typedef std::map<std::string, RowVec > i3ADescMap;

	bool operator()(typename const TRow::key_type key, typename TRow::mapped_type val)
	{	
		char i3APath[MAX_PATH] = {0};
		char i3AName[MAX_PATH] = {0};

		if(m_fliteredOnly && !val->m_filtered)
			return true;

		bool abnormalState = false;

		if(val->m_flag & 0xff000000)
			abnormalState = true;

		if(abnormalState)
		{
			strcpy_s(i3APath, MAX_PATH, val->m_i3AName);
		}
		else
		{
			strcpy_s(i3APath, MAX_PATH, val->m_i3AName);
			char* p = strrchr(i3APath, '/');

			if(p)
			{
				strcpy_s(i3AName, MAX_PATH, p+1);
				*p = NULL;
			}
			else
			{
				I3PRINTLOG(I3LOG_FATAL,"err!-bad path!!!");
				return false;
			}
		}

		i3ADescMap::iterator x = m_desc.find(i3APath);

		RowDataType rowdata;
		rowdata.m_ori = val;
		strcpy_s(rowdata.m_i3aOnlyName, MAX_PATH, i3AName);

		if(x == m_desc.end())
		{
			RowVec vec;
			vec.push_back(rowdata);

			i3ADescMap::value_type val2(i3APath, vec);
			m_desc.insert(val2);
		}
		else
		{
			((*x).second).push_back(rowdata);
		}

		return true;
	}

	void CompareToWorkDir(const char* workDir)
	{
		if(!workDir)
			return;

		char path[MAX_PATH] = {0};

		i3ADescMap::iterator x = m_desc.begin();
		while(x != m_desc.end())
		{
			RowVec& valVec = (*x).second;			
			const char* i3ARelativePath = ((*x).first).c_str();

			if(valVec[0].m_ori->m_flag & 0xff000000)
			{
				Log(1, "ERR! -  %s is empty i3AnimPackFile..", valVec[0].m_ori->m_animPakFileName);
				x++;
				continue;
			}			
			
			sprintf_s(path, MAX_PATH, "%s/%s", workDir, i3ARelativePath);

			char findPath[MAX_PATH] = {0};
			sprintf_s(findPath, MAX_PATH, "%s/*.i3a", path);

			HANDLE findHandle;
			WIN32_FIND_DATA	wfd;

			struct workFileDat
			{
				char				m_path[MAX_PATH] = { 0 };
				DWORD				m_siz = 0;
				DWORD				m_flag = 0;

				char				m_lastCmpAniPakFile[MAX_PATH] = { 0 };
			};


			std::vector<workFileDat> workDirFindI3AFiles;

			findHandle = FindFirstFile(findPath, &wfd);
			if(findHandle != INVALID_HANDLE_VALUE)
			{
				BOOL findCont;

				do 
				{
					if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) &&
						!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
					{
						workFileDat fd;

						sprintf_s(fd.m_path, MAX_PATH, "%s/%s", path, wfd.cFileName);
						fd.m_siz = wfd.nFileSizeLow;
						fd.m_flag = 0;

						workDirFindI3AFiles.push_back(fd);
					}				
					findCont = FindNextFile(findHandle, &wfd);

				} while(findCont);

				FindClose(findHandle);
				findHandle = INVALID_HANDLE_VALUE;
			}
			
			for(INT32 i=0; i<(INT32)valVec.size(); i++)
			{
				char cmpPath[MAX_PATH];
				sprintf_s(cmpPath, MAX_PATH, "%s/%s", path, valVec[i].m_i3aOnlyName);

				const char* animPakFile = valVec[i].m_ori->m_animPakFileName;

				bool found = false;
				for(INT32 k=0; k<(INT32)workDirFindI3AFiles.size(); k++)
				{
					strcpy_s(workDirFindI3AFiles[k].m_lastCmpAniPakFile, MAX_PATH, animPakFile);

					if(found)
						continue;

					if(!stricmp(workDirFindI3AFiles[k].m_path, cmpPath))
					{
						// name is match

						// check size


						workDirFindI3AFiles[k].m_flag |= 0x00000001;						
						found = true;
					}
					else
					{
						// i3a file in i3Anipackfile does not exist in working directory..
					}
				}

				if(!found)
				{
					Log(2, "ERR! -  %s (in animPackFile) is not exist in working directory(%s)..",
						cmpPath, path);
				}
			}

			for(INT32 k=0; k<(INT32)workDirFindI3AFiles.size(); k++)
			{
				if(workDirFindI3AFiles[k].m_flag == 0)
				{
					Log(3, "ERR! - %s is not exist in anipackfile(%s)..", 
					workDirFindI3AFiles[k].m_path, workDirFindI3AFiles[k].m_lastCmpAniPakFile);
				}
			}
			
			x++;
		}
	}

	void Log(DWORD flag, const char* fmt, ...)
	{
		char buf[1024] = {0};
		char prefix[MAX_PATH] = {0};

		if( (flag & 0xf) )
		{	
			DWORD clr = flag & 0xf;

			if(clr == 1)
				strcat_s(prefix, MAX_PATH, "CLR=RED;");
			else if(clr == 2)
				strcat_s(prefix, MAX_PATH, "CLR=GREEN;");
			else if(clr == 3)
				strcat_s(prefix, MAX_PATH, "CLR=BLUE;");
		}

		strcat_s(buf, 1024, prefix);
		char* p = buf;

		while(*p)
			p++;

		va_list args;
		va_start(args, fmt);
		vsprintf(p, fmt, args);
	
		m_log.push_back(std::string(buf));		
	}

	i3ADescMap						m_desc;
	std::list<std::string>			m_log;
	bool							m_fliteredOnly;
};


void AnimOptDlg::Analyze()
{
	CheckI3AFile<APKFileViewCtl::RowMMap> i3AChecker;

	char workDir[MAX_PATH] = {0};
	GetWindowText(m_ctl[EB_ANALYZE_WORKDIR], workDir, MAX_PATH);

	if(strlen(workDir) == 0)
	{
		MessageBox(m_hwnd, "ERR! - WorkDirPath is blank..", "Err", MB_ICONERROR);
		return;
	}

	if(m_apkFileView->m_filterView)
		i3AChecker.m_fliteredOnly = true;
	
	MMapOperator(m_apkFileView->m_row, i3AChecker);
	i3AChecker.CompareToWorkDir(workDir);

	m_apkFileAnalyzeLog->ResetContent();

	std::list<std::string>::iterator x = i3AChecker.m_log.begin();
	while(x != i3AChecker.m_log.end())
	{
		m_apkFileAnalyzeLog->AddLog((*x).c_str());
		x++;
	}
}
