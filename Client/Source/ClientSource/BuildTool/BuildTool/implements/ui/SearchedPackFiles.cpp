#include "stdafx.h"
#include "SearchedPackFiles.h"

#include "ugctrl/UGCtrl.h"
#include "UIDefines.h"
#include "CustomUGCtrl.h"

namespace ui
{
	// Contents View쪽 UI 스타일을 따르기 위해 가져옴
#pragma region struct SearchedPackFilesList_Callback : CustomUGCtrlCallback
	class SearchedPackFiles;
	struct SearchedPackFilesList_Callback : CustomUGCtrlCallback
	{
		SearchedPackFiles& _owner;
		PackedFileSearchBar& _parent;

		SearchedPackFilesList_Callback(SearchedPackFiles& owner, PackedFileSearchBar& parent)
			: _owner(owner), _parent(parent) {}
		virtual ~SearchedPackFilesList_Callback() {}

	protected: // UI쪽은 여기서 멤버변수로 갖는게 나을것 같음
		CPen	_penIvoryBlackLight;
		CPen	_penGray;
		CPen	_penOrange;
		CBrush	_brushIvoryBlack;
		int		_iSelectedRow = -1;

	protected:
		virtual void _SetColumnStyle(COLTYPE eType, LPCTSTR text
			, CUGCell& cell, CUGCtrl*& ctrl, short Alignment = (UG_ALIGNCENTER | UG_ALIGNVCENTER));
		virtual void _SetSelectedRow(CUGCtrl* ctrl, long row);

	public:
		virtual void OnSetup(CUGCtrl* ctrl) override;
		virtual void SetColumnContents(int row, COLTYPE eType, LPCTSTR text
			, CUGCtrl* ctrl, short Alignment = (UG_ALIGNLEFT | UG_ALIGNVCENTER));
		virtual void OnLClickedDown(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed);
		virtual void OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed);
		virtual void OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum);
		virtual void OnMenuCommand(CUGCtrl* ctrl, int col, long row, int section, int item) override;
		virtual int OnMenuStart(CUGCtrl* ctrl, int col, long row, int section) override;
	};

	void SearchedPackFilesList_Callback::_SetColumnStyle(COLTYPE eType, LPCTSTR text, CUGCell& cell, CUGCtrl*& ctrl
		, short Alignment/*= (UG_ALIGNCENTER | UG_ALIGNVCENTER)*/)
	{
		ctrl->GetCell(eType, -1, &cell);
		cell.SetText(text);
		cell.SetAlignment(Alignment);
		cell.SetBackColor(COLOR_BLACK);
		cell.SetTextColor(COLOR_WHITE);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(&_penGray);
		ctrl->SetCell(eType, -1, &cell);
	}

	void SearchedPackFilesList_Callback::OnSetup(CUGCtrl* ctrl) 
	{
		ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
		ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

		ctrl->SetNumberCols(COLTYPE_MAX);
		ctrl->SetNumberRows(0);
		ctrl->EnableMenu(TRUE);
		ctrl->SetDoubleBufferMode(TRUE);
		ctrl->SetUserSizingMode(2);
		ctrl->SetVScrollMode(UG_SCROLLTRACKING);
		ctrl->SetMultiSelectMode(2);
		//ctrl->DragDropTarget(TRUE);

		ctrl->SetDefFont(ctrl->GetFont(1));

		CUGCell cell;
		ctrl->GetHeadingDefault(&cell);
		cell.SetFont(ctrl->GetFont(0));
		ctrl->SetHeadingDefault(&cell);

		ctrl->SetSH_Width(0);	// side heading 안보이도록. 0번 cell 이 side heading 역할을 한다.

		_penIvoryBlackLight.CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
		_penGray.CreatePen(PS_SOLID, 1, COLOR_GRAY);
		_penOrange.CreatePen(PS_SOLID, 1, COLOR_ORANGE);
		_brushIvoryBlack.CreateSolidBrush(GetSysColor(COLOR_IVORY_BLACK));

		// Column Text는 File View쪽 Colume Text를 따름
		// 전부 i3Pack 파일이라 item type은 불필요 
		LPCTSTR strColums[] = { L"Name", L"Full Path" };
		for (int i = 0; i < (int)COLTYPE_MAX; ++i)
		{
			_SetColumnStyle((COLTYPE)i, strColums[i], cell, ctrl);
		}

	}

	void SearchedPackFilesList_Callback::SetColumnContents(int row, COLTYPE eType, LPCTSTR text
		, CUGCtrl* ctrl, short Alignment/*= (UG_ALIGNLEFT | UG_ALIGNVCENTER)*/)
	{	
		CUGCell cell;
		ctrl->GetCell(eType, row, &cell);
		cell.SetText(text);
		cell.SetAlignment(Alignment);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);	
		
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetBorderColor(&_penIvoryBlackLight);
		
		ctrl->SetCell(eType, row, &cell);
	}

	void SearchedPackFilesList_Callback::_SetSelectedRow(CUGCtrl* ctrl, long row)
	{
		// 이미 선택되어 있는 셀
		if (_iSelectedRow == row)
			return;

		CUGCell cell;

		if (_iSelectedRow >= 0)
		{
			for (int i = 0; i < COLTYPE_MAX; ++i)
			{
				ctrl->GetCell(i, _iSelectedRow, &cell);
				cell.SetBackColor(COLOR_IVORY_BLACK);
				cell.SetTextColor(COLOR_SILVER);
				cell.SetBorderColor(&_penIvoryBlackLight);
				ctrl->SetCell(i, _iSelectedRow, &cell);
			}
			ctrl->RedrawRow(_iSelectedRow);
		}
		
		for (int i = 0; i < COLTYPE_MAX; ++i)
		{
			ctrl->GetCell(i, row, &cell);
			cell.SetBackColor(COLOR_ORANGE);
			cell.SetTextColor(COLOR_IVORY_BLACK);
			cell.SetBorderColor(&_penOrange);
			ctrl->SetCell(i, row, &cell);
		}
		ctrl->RedrawRow(row);

		_iSelectedRow = row;
	}

	void SearchedPackFilesList_Callback::OnLClickedDown(CUGCtrl* ctrl, int col, long row, RECT* rect, POINT* pt, int processed)
	{
		if (row == -1)
			return;

		_SetSelectedRow(ctrl, row);
		CString strFileName = _owner.GetFileString(row, COLTYPE_FILEFULLPATH);
		_parent.GetCallback()->OnAddContents(row, strFileName);
	}

	void SearchedPackFilesList_Callback::OnDClicked(CUGCtrl* ctrl, int col, long row, RECT* rt, POINT* pt, BOOL processed)
	{
	}

	void SearchedPackFilesList_Callback::OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum)
	{
	}

	// 우클릭해도 메뉴 뜨게 하자
	void SearchedPackFilesList_Callback::OnMenuCommand(CUGCtrl* ctrl, int col, long row, int section, int item) 
	{
	}

	int SearchedPackFilesList_Callback::OnMenuStart(CUGCtrl* ctrl, int col, long row, int section) 
	{
		return TRUE;
	}	
	
#pragma endregion // end struct SearchedPackFilesList_Callback : CustomUGCtrlCallback

#pragma region // class CustomUGCtrlModify : public CustomUGCtrl
	using SearchedPackFilesList_CallbackPtr = std::unique_ptr<SearchedPackFilesList_Callback>;

	class CustomUGCtrlModify : public CustomUGCtrl
	{
	private:
		SearchedPackFilesList_CallbackPtr m_callback;

	public:
		virtual void SetCallback(SearchedPackFilesList_CallbackPtr cb)	{ m_callback = std::move(cb); }
		virtual SearchedPackFilesList_Callback*	GetCallback() const	{ return m_callback.get(); }

	public:
		virtual void OnSetup()	{ m_callback->OnSetup(this); }
		virtual void OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed) override;
		virtual void OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed) override;
		virtual void OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum);		
		virtual void SetColumnContents(int row, COLTYPE eType, LPCTSTR text
			, short Alignment = (UG_ALIGNLEFT | UG_ALIGNVCENTER));

	};

	void CustomUGCtrlModify::OnLClicked(int col, long row, int updn, RECT* rect, POINT* pt, int processed)
	{
		if (updn > 0)
			m_callback->OnLClickedDown(this, col, row, rect, pt, processed);
	}

	void CustomUGCtrlModify::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
	{
		m_callback->OnDClicked(this, col, row, rect, point, processed);
	}

	void CustomUGCtrlModify::OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum)
	{
		m_callback->OnSelectionChanged(this, startCol, startRow, endCol, endRow, blockNum);
	}

	void CustomUGCtrlModify::SetColumnContents(int row, COLTYPE eType, LPCTSTR text
						, short Alignment /*= (UG_ALIGNLEFT | UG_ALIGNVCENTER)*/)
	{
		m_callback->SetColumnContents(row, eType, text, this, Alignment);
	}

#pragma  endregion // end class CustomUGCtrlModify : public CustomUGCtrl

	SearchedPackFiles::SearchedPackFiles(PackedFileSearchBar& owner)
	{
		if (nullptr == m_pCtrl)
			m_pCtrl = new CustomUGCtrlModify;
		m_pCtrl->SetCallback(std::make_unique<SearchedPackFilesList_Callback>(*this, owner));
		m_pCtrl->CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS
			, CRect(0, 0, 0, 0), &owner, ID_CONTENTSVIEW_LIST);
	}
	
	SearchedPackFiles::~SearchedPackFiles()
	{
		if (nullptr != m_pCtrl)
		{
			delete m_pCtrl;
			m_pCtrl = nullptr;
		}
	}

	CString SearchedPackFiles::GetFileString(int row, COLTYPE eType)
	{
		CString str;
		m_pCtrl->QuickGetText((int)eType, row, &str);

		return str;
	}

	void SearchedPackFiles::AdjustLayout(const RECT& rt)
	{
		m_pCtrl->MoveWindow(&rt);

		int cx = rt.right - rt.left;
		cx -= 17;	// 종 스크롤바 사이즈 만큼 빼줘야 횡 스크롤바가 나타나지 않는다.                  

		int accumSize = 0;

		int size = static_cast<int>(cx * 0.34f);
		accumSize += size;
		m_pCtrl->SetColWidth(COLTYPE_FILENAME, size);

		size = static_cast<int>(cx * 0.66f);	
		accumSize += size;
		m_pCtrl->SetColWidth(COLTYPE_FILEFULLPATH, size);
	}

	void SearchedPackFiles::Add(const std::vector<std::wstring>& NameContainer)
	{
		// 기존 리스트 삭제
		ClearList();

		const long prevTotalNumRows = m_pCtrl->GetNumberRows();
		const size_t addedSize = NameContainer.size();

		if (0 >= addedSize)
			return;

		m_pCtrl->SetNumberRows(prevTotalNumRows + addedSize, FALSE);
		long row = prevTotalNumRows;
				
		size_t findLastOf = -1;
		CString strFileName, strFullFilePath;
		
		size_t idxMax = NameContainer.size();
		for (size_t i = 0; i < idxMax; ++i)
		{		
			
			// 파일명 (이름만 담는다)	
			findLastOf = NameContainer[i].find_last_of(L"\\");
			if (findLastOf >= 0)
				strFileName = NameContainer[i].substr(findLastOf + 1, wcslen(NameContainer[i].c_str())).c_str();
			
			((CustomUGCtrlModify*)m_pCtrl)->SetColumnContents(row, COLTYPE_FILENAME, strFileName);
			
			// 파일사이즈	
			strFullFilePath = NameContainer[i].c_str();
			((CustomUGCtrlModify*)m_pCtrl)->SetColumnContents(row, COLTYPE_FILEFULLPATH, strFullFilePath);
			
			++row;
		}

		m_pCtrl->RedrawAll();	// 속도 때문에 매번 그리는 것보다 한 번에 그리는 편이 훨씬 낫다.
	}

	void SearchedPackFiles::ClearList()
	{
		m_pCtrl->SetNumberRows(0, TRUE);
	}

}
