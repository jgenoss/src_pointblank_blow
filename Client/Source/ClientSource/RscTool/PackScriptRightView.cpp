// PackScriptRightView.cpp : implementation file
//

#include "stdafx.h"
#include "RscTool.h"
#include "PackScriptRightView.h"
#include "PackScriptTree.h"

#include "MainFrm.h"
#include "PathMgr.h"


#include "ExecutePackFrameWnd.h"
#include "ExecuteTSVN.h"

#include "i3MFCFolderDialog.h"
#include "FindPefDiff.h"

#include "i3Base/itl/algorithm/replace.h"
#include "PreprocessMessageCallbackDerived.h"

// PackScriptRightView
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(PackScriptRightView, CView)

namespace
{
	enum 
	{
		COMMAND_ADD_PACKSCRIPT_FILE = 1,
		COMMAND_DELETE_PACKSCRIPT_FILE = 2,
		COMMAND_MEDIA_TO_QA_COPY,
		COMMAND_QA_TO_LIVE_PACK,
		COMMAND_MEDIA_TO_QA_TO_LIVE,

		COMMAND_TSVN_UPDATE_MEDIA,
		COMMAND_TSVN_UPDATE_QA,
		COMMAND_TSVN_UPDATE_LIVE,
		COMMAND_TSVN_UPDATE_ALL,

		COMMAND_EXPORT_PACKSCRIPT_FILE,
	};

	struct ListCallback : i3MFCListCtrlCallback
	{
		PackScriptRightView* m_owner;

		ListCallback(PackScriptRightView* owner) : m_owner(owner) {}

		virtual void OnDblClk(i3MFCListCtrl* Ctrl, INT32 idx) 
		{
			///
			if (idx == -1)
				return;
			///

			const i3::wstring& wstrMedia = PathMgr::i()->GetMainMediaPath();

			if (GetFileAttributesW(wstrMedia.c_str()) == INVALID_FILE_ATTRIBUTES )
			{
				// 미디어 폴더가 없으면 수정작업을 진행하지 않는다.
				//
				return;
			}
			///

			PackScriptFile* f = reinterpret_cast<PackScriptFile*>( Ctrl->GetItemData(idx) );	

			GetMainFrame()->CreatePackScriptFileFrame(f);
		}	
	
		virtual void OnEndLabelEdit(i3MFCListCtrl* Ctrl, int index, 
			int sub_index, const wchar_t* strText, bool& apply) 
		{
			apply = false;
			if (sub_index > 0 ) return;	
			if (strText == NULL || strText[0] == 0)
			{
				PackScriptFile* f = (PackScriptFile*)Ctrl->GetItemData(index);			
				if (f == NULL)		// 이런건 그냥 삭제해준다...깔끔하게..
					Ctrl->DeleteItem(index);
				return;	
			}

			PackScriptFile* f = (PackScriptFile*)Ctrl->GetItemData(index);			
			// 실제 파일 이름도 변경되어야 한다..
			
			if ( f != NULL)
			{
				i3::wstring absPath;
				f->MakeAbsPath( PathMgr::i()->GetPackScriptPath(), absPath );
				absPath += L".i3PackScript";
			
				i3::wstring absNewPath;
				PackScriptFolder* folder = f->GetParent();
				folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), absNewPath);
				absNewPath += L'/';
				absNewPath += strText;
				absNewPath += L".i3PackScript";

				BOOL bRes = MoveFileW(absPath.c_str(), absNewPath.c_str());
				if (bRes)
					apply = true;
			}
			else		// 새 생성..
			{
				PackScriptFolder* folder = m_owner->GetCurrPackScriptFolder();
				i3::wstring str_path;
				folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), str_path);
				str_path += L'/';
				str_path += strText;
				str_path += L".i3PackScript";
				
				DWORD dwFileAccess = GENERIC_WRITE;
				DWORD dwShareMode = 0;
				HANDLE hFile = ::CreateFileW(str_path.c_str(), dwFileAccess, dwShareMode, 
					NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

				if (hFile == INVALID_HANDLE_VALUE)
				{
					Ctrl->DeleteItem(index);
				}
				else
				{

					f = new PackScriptFile( i3::as_literal(strText) );
					folder->AddFile(f);
					Ctrl->SetItemData(index, DWORD_PTR(f));

					apply = true;
					::CloseHandle(hFile);
				}
			}
			
		}

		virtual void OnContextMenu(i3MFCListCtrl* Ctrl, const CPoint& pt) 
		{
		
			CPoint ptClient = pt;
			Ctrl->ScreenToClient(&ptClient);
			int target_idx = Ctrl->HitTest(ptClient);
			
			CMenu menu;
			menu.CreatePopupMenu();
			
			bool bMediaPath = PathMgr::i()->IsMediaPathExist();
			bool bQAPath = PathMgr::i()->IsQAPathExist();
			bool bLivePath = PathMgr::i()->IsLivePathExist(); 
			
			bool bSeparationNeed = false;

			PackScriptFolder* curr_folder = m_owner->GetCurrPackScriptFolder(); 
				
			if (curr_folder)
			{
				if (bMediaPath && bQAPath)
				{
					menu.AppendMenu(MF_STRING, COMMAND_MEDIA_TO_QA_COPY, L"Copy : Media -> QA");
					bSeparationNeed = true;
				}

				if ( bQAPath && bLivePath )
				{
					menu.AppendMenu(MF_STRING, COMMAND_QA_TO_LIVE_PACK, L"Pack : QA -> Live");
					bSeparationNeed = true;
				}
				if ( bMediaPath && bQAPath && bLivePath)
				{
					menu.AppendMenu(MF_STRING, COMMAND_MEDIA_TO_QA_TO_LIVE, L"Copy/Pack : Media -> QA -> Live");
					bSeparationNeed = true;
				}
			}
			
			if (bLivePath)
			{
				if (bSeparationNeed)
					menu.AppendMenu(MF_SEPARATOR);

				menu.AppendMenu(MF_STRING, COMMAND_EXPORT_PACKSCRIPT_FILE, L"UnPack Live i3Pack File");
				bSeparationNeed = true;
			}


			if (bSeparationNeed && (bMediaPath || bQAPath || bLivePath ))
			{
				menu.AppendMenu(MF_SEPARATOR);
			}

			if (bMediaPath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_MEDIA, L"TortoiseSVN Update : Media Path");
				bSeparationNeed = true;
			}
			if (bQAPath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_QA, L"TortoiseSVN Update : QA Path");
				bSeparationNeed = true;
			}
			if (bLivePath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_LIVE, L"TortoiseSVN Update : Live Path");
				bSeparationNeed = true;
			}

			if (bMediaPath && bQAPath && bLivePath)
			{
				menu.AppendMenu(MF_STRING, COMMAND_TSVN_UPDATE_ALL, L"TortoiseSVN Update : ALL 3 Paths");
				bSeparationNeed = true;
			}

			if (curr_folder)
			{
				if (bSeparationNeed)
				{
					menu.AppendMenu(MF_SEPARATOR);
				}
			
				menu.AppendMenu(MF_STRING, COMMAND_ADD_PACKSCRIPT_FILE, L"New PackScriptFile");
				menu.AppendMenu(MF_STRING, COMMAND_DELETE_PACKSCRIPT_FILE, L"Delete PackScriptFile");
			}
			
			long result_cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON|TPM_RETURNCMD, pt.x, pt.y, Ctrl);
			menu.DestroyMenu();
			
			if (Ctrl->GetFirstSelectedItem() == -1)
			{
				if (target_idx != -1)
					Ctrl->SetItemState(target_idx, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			}

			switch( result_cmd )
			{
			case COMMAND_ADD_PACKSCRIPT_FILE:
				{
					// 아이템 추가외엔 특별한 일은 하지 않도록처리.
					// EditLabel 결과에 의해서 PackScriptFile생성처리할것..
					PackScriptFolder* folder = m_owner->GetCurrPackScriptFolder();
					
					if (folder == NULL)
						return;

					if (target_idx == -1)
						target_idx = Ctrl->GetItemCount();

					target_idx = Ctrl->InsertItem(target_idx, L"", 0);
					Ctrl->EnsureVisible(target_idx, FALSE);
					Ctrl->EditLabel(target_idx);
				}
			break;

			// 셀렉션된게 많다면 일괄 처리해야함..
			case COMMAND_DELETE_PACKSCRIPT_FILE:
				{
					i3::vector<int>		vec_idx;
					Ctrl->GetAllSelectedItems(vec_idx);
					
					if ( vec_idx.empty() )
						vec_idx.push_back(target_idx);

					// 끝에서부터 차례로 삭제...유도...
					
					PackScriptFolder* folder = m_owner->GetCurrPackScriptFolder();
					
					i3::wstring str_dir;
					folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), str_dir);
					str_dir += L'/';
					
					i3::wstring str_file;

					while(!vec_idx.empty() )
					{
						target_idx = vec_idx.back();
						str_file = str_dir;
						PackScriptFile* pf = (PackScriptFile*)Ctrl->GetItemData(target_idx);

						str_file += pf->GetFileTitle();
						str_file += L".i3PackScript";

						folder->DeleteFile(pf);

						::DeleteFileW( str_file.c_str() );
						Ctrl->DeleteItem(target_idx);
						vec_idx.pop_back();
					}
				}
			break;

			case COMMAND_MEDIA_TO_QA_COPY:
				{
					m_owner->DoPackScriptJobWithSelectedItems(PACKMODE_MEDIA_TO_QA_COPY);
				}
			break;
			case COMMAND_QA_TO_LIVE_PACK:
				{
					m_owner->DoPackScriptJobWithSelectedItems(PACKMODE_QA_TO_LIVE_PACK);
				}
			break;
			case COMMAND_MEDIA_TO_QA_TO_LIVE:
				{
					m_owner->DoPackScriptJobWithSelectedItems(PACKMODE_MEDIA_TO_QA_COPY);
					m_owner->DoPackScriptJobWithSelectedItems(PACKMODE_QA_TO_LIVE_PACK);
				}
			break;
			
			case COMMAND_TSVN_UPDATE_MEDIA:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()));
			break;

			case COMMAND_TSVN_UPDATE_QA:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()));
			break;

			case COMMAND_TSVN_UPDATE_LIVE:
				tsvn::ExecuteSync(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()));
			break;

			case COMMAND_TSVN_UPDATE_ALL:
				{
					HANDLE ah[3];
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetMainMediaPath()), ah[0]);
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetQAPath()), ah[1]);
					tsvn::Execute(tsvn::CMD_UPDATE, i3::as_literal(PathMgr::i()->GetLivePath()), ah[2]);
					tsvn::WaitExecuteEnd(ah, 3);
				}
			break;
			
			case COMMAND_EXPORT_PACKSCRIPT_FILE:
				{
					// 1. 먼저 저장 폴더 설정하고 설정된 문자열 갖고옴 (PathMgr에 임시보관 병행)
					// 2. 팩볼륨 1개 이상 열고 (복수개일수 있다..)
					// 3. 연 팩볼륨마다 익스포트함수들 수행...
					// 4. 완료되면 저장폴더쪽의 탐색기를 열어준다...
					
					CWinApp* App = AfxGetApp();
					CString strRegPathName = App->GetProfileString(L"Path", L"ExportRootPath");


					i3MFCFolderDialog dlg;

					i3::wstring wstrPath(LPCWSTR(strRegPathName), strRegPathName.GetLength());
					dlg.SelectFolder( wstrPath );

				
					if ( dlg.DoModal() == IDOK )
					{
				
						PreprocessMessageCallbackDerived cb; 

						const i3::wstring& result_path = dlg.GetResultPath();

						App->WriteProfileString(L"Path", L"ExportRootPath", result_path.c_str());

						i3::string cresult_path;
						i3::utf16_to_mb(result_path, cresult_path);

						PathMgr::i()->SetExportRootPath(result_path);

						i3::vector<int>		vec_idx;
						Ctrl->GetAllSelectedItems(vec_idx);

						if ( vec_idx.empty() && target_idx != -1 )
							vec_idx.push_back(target_idx);

						
						PackScriptFolder* folder = m_owner->GetCurrPackScriptFolder();

						i3::wstring str_dir;
						folder->MakeAbsPath(PathMgr::i()->GetPackScriptPath(), str_dir);
						str_dir += L'/';
						
						i3::string strLivePath; i3::utf16_to_mb(PathMgr::i()->GetLivePath(), strLivePath);

						i3::wstring str_file;
					
						const size_t num_idx = vec_idx.size();
						bool success = false;
						
						cb.OnStart();

						for (size_t i = 0 ; i < num_idx ; ++i)
						{
							target_idx = vec_idx[i];

							str_file = str_dir;
							PackScriptFile* pf = (PackScriptFile*)Ctrl->GetItemData(target_idx);
							str_file += pf->GetFileTitle();
							str_file += L".i3PackScript";
							
							PackScriptContents contents;
							ImportPackScriptFile(str_file, contents);

							const size_t num_unit = contents.unit_list.size();

							for (size_t j = 0 ; j < num_unit ; ++j )
							{
								const PackScriptUnit* unit = contents.unit_list[j].get();

								i3::stack_string full_path = strLivePath;
								full_path += "/Pack/";
								full_path += unit->pack_filename;

								i3PackVolume* vol =	LoadPackVolume(full_path);
							
								if (vol == NULL)
								{
									i3::stack_wstring str;
									i3::sprintf(str, L"%s File does not exist.", unit->pack_filename);
									m_owner->MessageBox(str.c_str(), L"Missing File");
									continue;
								}
								else
								{
									i3::stack_wstring str;
									i3::sprintf(str, L"%s File UnPacking....", unit->pack_filename);
									cb.OnTextMessage( str.c_str() );

									vol->getRoot()->SetName("/");		// 기 저장되었을때 절대 경로로 지정된 것을 '/'로 바꾼다..
								}
								success |= vol->Export( cresult_path.c_str() );
								vol->Release();
							}
						}
						
						cb.OnEnd();

						// 해당 폴더에 대한 탐색기 창 열기..
						{
							
							i3::stack_wstring explorer_path = result_path;
							i3::replace(explorer_path.begin(), explorer_path.end(), L'/', L'\\');
							::ShellExecuteW(NULL, L"open", L"explorer.exe",  explorer_path.c_str(), NULL, SW_SHOW); 
						}

					}
				}
			break;

			default:
			break;
			}
		}


	};


}

PackScriptRightView::PackScriptRightView() : m_folder(NULL)
{
	m_listCtrl.SetCallback( new ListCallback(this) );
}

PackScriptRightView::~PackScriptRightView()
{
	delete m_listCtrl.GetCallback();
}

BEGIN_MESSAGE_MAP(PackScriptRightView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// PackScriptRightView drawing

void PackScriptRightView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// PackScriptRightView diagnostics

#ifdef _DEBUG
void PackScriptRightView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void PackScriptRightView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// PackScriptRightView message handlers

int PackScriptRightView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	
	m_listCtrl.CreateEx(this, 1, LVS_EDITLABELS);

	m_listCtrl.InsertColumn(0, L"");
	
	m_listCtrl.HideHeader();

//	m_listCtrl.EnableS
//	m_listCtrl.Hid
	return 0;
}

void PackScriptRightView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_listCtrl.SetSize(cx, cy);

	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..
	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		cx -= GetSystemMetrics(SM_CXVSCROLL);
	}
	
	m_listCtrl.SetColumnWidth(0, cx);
	
	// TODO: Add your message handler code here
}

void	PackScriptRightView::UpdateListCtrl(PackScriptFolder* pData)
{
	m_listCtrl.DeleteAllItems();		// 일단 깨끄시 지우고 처리..

	m_folder = pData;

	if (pData == NULL)
		return;

	const i3::vector<PackScriptFile*>& file_list = pData->GetFileList();

	const std::size_t num_file = file_list.size();

	for (std::size_t i = 0 ; i < num_file ; ++i )
	{
		PackScriptFile* f = file_list[i];
		
		m_listCtrl.AddItem(f->GetFileTitle().c_str(), f);
	}


	SCROLLINFO info;
	m_listCtrl.GetScrollInfo(SB_VERT, &info, SIF_RANGE | SIF_PAGE );
	// Range와 Page 관계를 통해 스크롤바의 출력 여부를 확인했다.. ( 리스트 콘트롤 기준이며, 그외는 사용자임의 기준이다.)
	// Max + 1 > Page 인 경우 스크롤바가 출력된다..

	CRect rc;	GetClientRect(&rc);

	if ( UINT(info.nMax) + 1 > info.nPage )
	{
		// horz크기를 조정해야한다. (종 스크롤바 너비만큼을 빼야한다.)
		rc.right -= GetSystemMetrics(SM_CXVSCROLL);
	}

	m_listCtrl.SetColumnWidth(0, rc.right);
	
}


BOOL PackScriptRightView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

//	return CView::OnEraseBkgnd(pDC);
	return TRUE;
}

void	PackScriptRightView::Update_OnRename(PackScriptFile* file)
{
	PackScriptFolder* folder = file->GetParent();
	if (folder != m_folder) return;
	
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LPARAM)file;
	int item_idx = m_listCtrl.FindItem(&info);

	if (item_idx != -1)
	{
		m_listCtrl.SetItemText(item_idx, 0, file->GetFileTitle().c_str() );
	}
}

void	PackScriptRightView::Update_BeforeDelete(PackScriptFile* file)
{
	PackScriptFolder* folder = file->GetParent();
	if (folder != m_folder) return;
	
	LVFINDINFO info;
	info.flags = LVFI_PARAM;
	info.lParam = (LPARAM)file;
	int item_idx = m_listCtrl.FindItem(&info);

	if (item_idx != -1)
	{
		m_listCtrl.DeleteItem(item_idx);		
	}
}

void	PackScriptRightView::Update_OnAddFile(PackScriptFile* file )
{
	PackScriptFolder* folder = file->GetParent();
	if (folder != m_folder) return;
	
	LVFINDINFO info;
	info.flags = LVFI_STRING;
	info.psz  = file->GetFileTitle().c_str();
	int item_idx = m_listCtrl.FindItem(&info);

	if (item_idx == -1)
	{
		m_listCtrl.AddItem(file->GetFileTitle().c_str(), file);
	}
	else
	{
		m_listCtrl.SetItemData(item_idx, (DWORD_PTR)file);
	}
		
}

void	PackScriptRightView::DoPackScriptJobWithSelectedItems(PackMode mode)
{
	i3::vector<int>		vec_idx;
	m_listCtrl.GetAllSelectedItems(vec_idx);


	ExecutePackFrameWnd* packWnd = GetMainFrame()->GetExecutePackFrameWnd();
	
	PackScriptFolder* folder = this->GetCurrPackScriptFolder();
	i3::wstring wstr_dir;
	folder->MakeRelPath(wstr_dir);
	wstr_dir += L'/';

	i3::wstring wstr_file;
	int target_idx = -1;
	const size_t num = vec_idx.size();

	for (size_t i = 0 ; i < num ; ++i )
	{
		target_idx = vec_idx[i];
		wstr_file = wstr_dir;
		PackScriptFile* pf = (PackScriptFile*)m_listCtrl.GetItemData(target_idx);

		wstr_file += pf->GetFileTitle();
		wstr_file += L".i3PackScript";

		packWnd->AddPackScriptFile(wstr_file, mode);
	}
}

