#include "stdafx.h"
#include "BuildTool.h"
#include "ShaderSyncDlg.h"
#include "afxdialogex.h"
#include "UIDefines.h"
#include <boost/filesystem.hpp>
#include "resource/SyncShaders.h"
#include <limits>
#include "resource/SyncShaders.h"
#include <boost/format.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace ui
{
	struct i3EnginePathBrowseCallback : CustomUGEditBrowseCallback
	{
		using FuncType = std::function<void(const std::string)>;

		FuncType fnFileShader = nullptr;
		FuncType fnMemoryShader = nullptr;

		i3EnginePathBrowseCallback(const FuncType& f_file, const FuncType& f_memory) 
			: fnFileShader(f_file), fnMemoryShader(f_memory)
		{}

		virtual void OnSelectOK(const std::wstring& wstri3EngineAbsPath) override
		{
			if (wstri3EngineAbsPath.empty())
				return;

			std::string stri3EngineAbsPath;
			i3::utf16_to_mb(wstri3EngineAbsPath, stri3EngineAbsPath);
			
			fnFileShader(stri3EngineAbsPath);
			fnMemoryShader(stri3EngineAbsPath);
		}
	};

	void SetupShaderListGridCtrl(CUGCtrl* ctrl, CPen* penIvoryBlack, CPen* penIvoryBlackLight, CPen* penOrange)
	{
		ctrl->AddFont(L"Arial", -12, FW_BOLD);	// 0, top heading side heading.
		ctrl->AddFont(L"Arial", -12, 500);		// 1, cell

		ctrl->SetNumberCols(1);
		ctrl->SetNumberRows(0);
		ctrl->SetSH_Width(0);
		ctrl->SetTH_Height(0);

		ctrl->SetDoubleBufferMode(TRUE);
		ctrl->SetUserSizingMode(2);
		ctrl->SetVScrollMode(UG_SCROLLTRACKING);
		ctrl->SetMultiSelectMode(2);

		ctrl->SetDefFont(ctrl->GetFont(1));

		penIvoryBlack->CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK);
		penIvoryBlackLight->CreatePen(PS_SOLID, 1, COLOR_IVORY_BLACK_LIGHT);
		penOrange->CreatePen(PS_SOLID, 1, COLOR_ORANGE);

		CUGCell cell;
		ctrl->GetHeadingDefault(&cell);
		cell.SetFont(ctrl->GetFont(0));
		cell.SetAlignment(UG_ALIGNCENTER | UG_ALIGNVCENTER);
		cell.SetBackColor(COLOR_BLACK);
		cell.SetTextColor(COLOR_TAN3_PERU);
		cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
		cell.SetBorderColor(penIvoryBlack);
		ctrl->SetHeadingDefault(&cell);
	}

	void Selection(CUGCtrl* ctrl, CPen* penIvoryBlack, CPen* penIvoryBlackLight, CPen* penOrange, const long currRow, long& prevRow)
	{
		CUGCell cell;

		if (prevRow != -1)
		{
			ctrl->GetCell(0, prevRow, &cell);
			cell.SetBackColor(COLOR_IVORY_BLACK);
			cell.SetTextColor(COLOR_SILVER);
			cell.SetBorderColor(penIvoryBlackLight);
			ctrl->SetCell(0, prevRow, &cell);
			ctrl->RedrawRow(prevRow);
		}

		ctrl->GetCell(0, currRow, &cell);
		cell.SetBackColor(COLOR_IVORY_BLACK);
		cell.SetTextColor(COLOR_SILVER);
		cell.SetBorderColor(penOrange);
		ctrl->SetCell(0, currRow, &cell);
		ctrl->RedrawRow(currRow);

		prevRow = currRow;
	}

	struct FileShaderListCallback : CustomUGCtrlCallback, IShaderListPrintable
	{
		ShaderList& owner;
		ShaderSyncDlg* dlg = nullptr;

		using EditSetTextFunc = std::function<void(const std::string&)>;
		EditSetTextFunc fnSetHLSLText;

		FileShaderListCallback(ShaderList& o, ShaderSyncDlg* dlg_, const EditSetTextFunc& f) 
			: owner(o), dlg(dlg_), fnSetHLSLText(f)
		{}

		virtual ~FileShaderListCallback() 
		{}

		virtual void	OnSetup(CUGCtrl* ctrl) override
		{
			SetupShaderListGridCtrl(ctrl, &owner.m_penIvoryBlack, &owner.m_penIvoryBlackLight, &owner.m_penOrange);
		}

		virtual void	OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) override
		{
			if (blockNum == 1)
			{
				Selection(ctrl, &owner.m_penIvoryBlack, &owner.m_penIvoryBlackLight, &owner.m_penOrange, endRow, owner.m_currSelectionRow);

				LPCTSTR pwszHLSLFileName = ctrl->QuickGetText(0, endRow);
				if (pwszHLSLFileName == nullptr)
					return;

				Path HLSLFileName(pwszHLSLFileName);
				if (HLSLFileName.extension().empty())
					return;

				// shader file edit view 에 HLSL 파일 코드 출력.
				std::wstring wstrHLSLAbsPath(dlg->Geti3EnginePath());
				wstrHLSLAbsPath += L"\\Shader\\";	wstrHLSLAbsPath += HLSLFileName.wstring();

				if (fs::exists(wstrHLSLAbsPath))
				{
					std::string strHLSLAbsPath;
					i3::utf16_to_mb(wstrHLSLAbsPath, strHLSLAbsPath);

					fnSetHLSLText(strHLSLAbsPath);
				}
			}
		}

		virtual void	OnPrintShaderList(CUGCtrl* ctrl, const std::string& stri3EngineAbsPath) override
		{
			ctrl->SetNumberRows(0);	// clear.

			std::string stri3EngineShaderFolderAbsPath(stri3EngineAbsPath);
			stri3EngineShaderFolderAbsPath += "\\Shader";

			if (fs::exists(stri3EngineShaderFolderAbsPath))
			{
				std::vector<Path> HLSLAbsPathList;
				rsc::GatherHLSLAbsPaths(stri3EngineShaderFolderAbsPath, HLSLAbsPathList);

				if (HLSLAbsPathList.empty())
					return;

				ctrl->SetNumberRows(HLSLAbsPathList.size());
				CUGCell cell;
				for (size_t i = 0; i < HLSLAbsPathList.size(); i++)
				{
					ctrl->GetCell(0, i, &cell);
					cell.SetBackColor(COLOR_IVORY_BLACK);
					cell.SetTextColor(COLOR_SILVER);
					cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
					cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
					cell.SetBorderColor(&owner.m_penIvoryBlackLight);
					cell.SetText(HLSLAbsPathList[i].filename().wstring().c_str());
					ctrl->SetCell(0, i, &cell);
				}

				ctrl->RedrawAll();
			}
		}
	};

	struct MemoryShaderListCallback : CustomUGCtrlCallback, IShaderListPrintable
	{
		ShaderList& owner;
		ShaderSyncDlg* dlg = nullptr;

		using EditSetTextFunc = std::function<void(const std::string&, const std::string&)>;
		EditSetTextFunc fnSetMemShaderCodeText;

		MemoryShaderListCallback(ShaderList& o, ShaderSyncDlg* dlg_, const EditSetTextFunc& f) 
			: owner(o), dlg(dlg_), fnSetMemShaderCodeText(f) 
		{}

		virtual ~MemoryShaderListCallback() {}

		virtual void	OnSetup(CUGCtrl* ctrl)
		{
			SetupShaderListGridCtrl(ctrl, &owner.m_penIvoryBlack, &owner.m_penIvoryBlackLight, &owner.m_penOrange);
		}

		virtual void	OnSelectionChanged(CUGCtrl* ctrl, int startCol, long startRow, int endCol, long endRow, int blockNum) override
		{
			if (blockNum == 1)
			{
				Selection(ctrl, &owner.m_penIvoryBlack, &owner.m_penIvoryBlackLight, &owner.m_penOrange, endRow, owner.m_currSelectionRow);

				LPCTSTR pwszMemShaderName = ctrl->QuickGetText(0, endRow);
				if (pwszMemShaderName == nullptr)
					return;

				// memory file edit view 에 HLSL 파일 코드 출력.
				std::wstring wstri3ShaderMemoryCodeCppAbsPath(dlg->Geti3EnginePath());
				wstri3ShaderMemoryCodeCppAbsPath += L"\\src\\i3Gfx\\i3ShaderMemoryCode.cpp";

				if (fs::exists(wstri3ShaderMemoryCodeCppAbsPath))
				{
					std::string stri3ShaderMemoryCodeCppAbsPath;
					i3::utf16_to_mb(wstri3ShaderMemoryCodeCppAbsPath, stri3ShaderMemoryCodeCppAbsPath);

					std::string strMemShaderName;
					i3::utf16_to_mb(pwszMemShaderName, strMemShaderName);
					fnSetMemShaderCodeText(stri3ShaderMemoryCodeCppAbsPath, strMemShaderName);
				}
			}
		}

		virtual void	OnPrintShaderList(CUGCtrl* ctrl, const std::string& stri3EngineAbsPath) override
		{
			ctrl->SetNumberRows(0);	// clear.

			std::string stri3ShaderMemoryCodeCppAbsPath(stri3EngineAbsPath);
			stri3ShaderMemoryCodeCppAbsPath += "\\src\\i3Gfx\\i3ShaderMemoryCode.cpp";

			if (fs::exists(stri3ShaderMemoryCodeCppAbsPath))
			{
				std::vector<std::wstring> globalVarsOfEncShaderCode;
				rsc::GatherGlobalVarsOfEncShaderCode(stri3ShaderMemoryCodeCppAbsPath, globalVarsOfEncShaderCode);

				if (globalVarsOfEncShaderCode.empty())
					return;

				ctrl->SetNumberRows(globalVarsOfEncShaderCode.size());

				CUGCell cell;
				for (size_t i = 0; i < globalVarsOfEncShaderCode.size(); i++)
				{
					ctrl->GetCell(0, i, &cell);
					cell.SetBackColor(COLOR_IVORY_BLACK);
					cell.SetTextColor(COLOR_SILVER);
					cell.SetAlignment(UG_ALIGNLEFT | UG_ALIGNVCENTER);
					cell.SetBorder(UG_BDR_LTHIN | UG_BDR_TTHIN | UG_BDR_RTHIN | UG_BDR_BTHIN);
					cell.SetBorderColor(&owner.m_penIvoryBlackLight);
					cell.SetText(globalVarsOfEncShaderCode[i].c_str());
					ctrl->SetCell(0, i, &cell);
				}

				ctrl->RedrawAll();
			}
		}
	};

	struct FileShaderEditViewRadioCallback : CustomButtonCallback
	{
		std::function<void(const FileShaderCodeConvFunc&)> m_mem_f;
		FileShaderCodeConvFunc m_fConvCode;

		FileShaderEditViewRadioCallback(const std::function<void(const FileShaderCodeConvFunc&)>& mem_f, FileShaderCodeConvFunc fConv)
			: m_mem_f(mem_f), m_fConvCode(fConv)
		{}

		virtual void OnClick(CustomButton* ctrl)
		{
			m_mem_f(m_fConvCode);
		}
	};

	struct MemoryShaderEditViewRadioCallback : CustomButtonCallback
	{
		std::function<void(const MemoryShaderCodeConvFunc&)> m_mem_f;
		MemoryShaderCodeConvFunc m_fConvCode;

		MemoryShaderEditViewRadioCallback(const std::function<void(const MemoryShaderCodeConvFunc&)>& mem_f, MemoryShaderCodeConvFunc fConv)
			: m_mem_f(mem_f), m_fConvCode(fConv)
		{}

		virtual void OnClick(CustomButton* ctrl)
		{
			m_mem_f(m_fConvCode);
		}
	};

	struct SyncShaderButtonCallback : CustomButtonCallback
	{
		ShaderSyncDlg* owner = nullptr;
		SyncShaderButtonCallback(ShaderSyncDlg* o) : owner(o) {}
		virtual void OnClick(CustomButton* ctrl)
		{
			if (owner->Geti3EnginePath() == nullptr)
				return;

			Path i3EngineAbsPath(owner->Geti3EnginePath());

			Path i3EngineShaderFolderAbsPath(i3EngineAbsPath);
			i3EngineShaderFolderAbsPath /= "Shader";

			if (rsc::SyncShaders_NonFormatted(i3EngineShaderFolderAbsPath, i3EngineAbsPath))
			{
				const char* fmt = R"(shader memory code has been exported to below source files.
%s/include/i3Gfx/i3ShaderMemoryCode.h
%s/src/i3Gfx/i3ShaderMemoryCode.cpp)";

				const std::string strMessage = boost::io::str(boost::format(fmt) % i3EngineAbsPath % i3EngineAbsPath);

				std::wstring wstrMessage;
				i3::mb_to_utf16(strMessage, wstrMessage);
				::MessageBox(NULL, wstrMessage.c_str(), L"Shader Sync Succeeded", MB_OK);
			}
			else
			{
				::MessageBox(NULL, L"shader sync failed. please check paths of this version by version setting dialog.", L"Shader Sync Failed", MB_OK);
			}
		}
	};


	IMPLEMENT_DYNAMIC(ShaderSyncDlg, CDialogEx)

	ShaderSyncDlg::ShaderSyncDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_SHADER_SYNC, pParent)
	{
	}

	ShaderSyncDlg::~ShaderSyncDlg()
	{
	}

	void ShaderSyncDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);

		DDX_Radio(pDX, IDC_RADIO_FILE_SHADER_ORIGIN_MODE, (int&)m_radioIdxFileShaderView);
		DDX_Radio(pDX, IDC_RADIO_MEMORY_SHADER_ORIGIN_MODE, (int&)m_radioIdxMemShaderView);
	}

#ifdef max
#undef max
#endif

	BOOL ShaderSyncDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		RECT rt;

		m_editFileShaderCtrl.SubclassDlgItem(IDC_EDIT_FILE_SHADER, this);
		m_editFileShaderCtrl.SetLimitText(std::numeric_limits<unsigned int>::max());
		m_editFileShaderCtrl.SetColor(COLOR_IVORY_BLACK, COLOR_SILVER);
		m_editFileShaderCode->SetCustomEditCtrl(&m_editFileShaderCtrl);

		m_editMemoryShaderCtrl.SubclassDlgItem(IDC_EDIT_MEMORY_SHADER, this);
		m_editMemoryShaderCtrl.SetLimitText(std::numeric_limits<unsigned int>::max());
		m_editMemoryShaderCtrl.SetColor(COLOR_IVORY_BLACK, COLOR_SILVER);
		m_editMemoryShaderCode->SetCustomEditCtrl(&m_editMemoryShaderCtrl);

		m_grpboxFileShaderView.SubclassDlgItem(IDC_GRPBOX_FILE_SHADER_VIEW, this);
		m_grpboxFileShaderView.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.top += 40;

		m_grpboxMemoryShaderView.SubclassDlgItem(IDC_GRPBOX_MEMORY_SHADER_VIEW, this);
		m_grpboxMemoryShaderView.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.top += 40;

		m_grpboxFileShaderList.SubclassDlgItem(IDC_GRPBOX_FILE_SHADER_LIST, this);
		m_grpboxFileShaderList.GetWindowRect(&rt);
		ScreenToClient(&rt);

		RECT rtBrowse = rt;
		rtBrowse.top -= 35;
		rtBrowse.bottom = rtBrowse.top + 20;
		m_browsei3EnginePath.AdjustLayout(rtBrowse);

		rt.top += 20;
		m_fileShaderListCtrl->AdjustLayout(rt);

		m_grpboxMemoryShaderList.SubclassDlgItem(IDC_GRPBOX_MEMORY_SHADER_LIST, this);
		m_grpboxMemoryShaderList.GetWindowRect(&rt);
		ScreenToClient(&rt);
		rt.top += 20;
		m_memoryShaderListCtrl->AdjustLayout(rt);

		m_btnSyncShaders.SubclassDlgItem(IDC_BUTTON_SYNC_SHADERS, this);
		m_btnSyncShaders.SetCallback(std::make_unique<SyncShaderButtonCallback>(this));

		// File Shader View Radio Buttons.
		{
			m_radioFileShaderOrigMode.SubclassDlgItem(IDC_RADIO_FILE_SHADER_ORIGIN_MODE, this);
			m_radioFileShaderOrigMode.SetColor(COLOR_BLACK, COLOR_WHITE);

			auto callback = std::make_unique<FileShaderEditViewRadioCallback>(
				std::bind(&FileShaderCodeEditBox::SetCodeConvFunc, m_editFileShaderCode.get(), std::placeholders::_1),
				std::bind(&ConvNull, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			m_radioFileShaderOrigMode.SetCallback(std::move(callback));
		}

		{
			m_radioFileShaderCombineMode.SubclassDlgItem(IDC_RADIO_FILE_SHADER_COMBINE_MODE, this);

			auto callback = std::make_unique<FileShaderEditViewRadioCallback>(
				std::bind(&FileShaderCodeEditBox::SetCodeConvFunc, m_editFileShaderCode.get(), std::placeholders::_1),
				std::bind(&ConvCombinedShaderCode, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

			m_radioFileShaderCombineMode.SetCallback(std::move(callback));
		}

		// Memory Shader View Radio Buttons.
		{
			m_radioMemShaderOrigMode.SubclassDlgItem(IDC_RADIO_MEMORY_SHADER_ORIGIN_MODE, this);

			auto callback = std::make_unique<MemoryShaderEditViewRadioCallback>(
				std::bind(&MemoryShaderCodeEditBox::SetCodeConvFunc, m_editMemoryShaderCode.get(), std::placeholders::_1),
				std::bind(&ConvMemoryCodeNull, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

			m_radioMemShaderOrigMode.SetCallback(std::move(callback));
		}

		{
			m_radioMemShaderDecryptMode.SubclassDlgItem(IDC_RADIO_MEMORY_SHADER_DECRYPT_MODE, this);

			auto callback = std::make_unique<MemoryShaderEditViewRadioCallback>(
				std::bind(&MemoryShaderCodeEditBox::SetCodeConvFunc, m_editMemoryShaderCode.get(), std::placeholders::_1),
				std::bind(&rsc::DecryptHLSLMemoryCodes, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

			m_radioMemShaderDecryptMode.SetCallback(std::move(callback));
		}

		{
			m_radioMemShaderCombineMode.SubclassDlgItem(IDC_RADIO_MEMORY_SHADER_DECRYPT_COMBINED_MODE, this);

			auto callback = std::make_unique<MemoryShaderEditViewRadioCallback>(
				std::bind(&MemoryShaderCodeEditBox::SetCodeConvFunc, m_editMemoryShaderCode.get(), std::placeholders::_1),
				std::bind(&rsc::GetCombinedHLSLMemoryCodes, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));

			m_radioMemShaderCombineMode.SetCallback(std::move(callback));
		}

		return TRUE;
	}

	void ShaderSyncDlg::Creator()
	{
		ShaderSyncDlg dlg;
		dlg.DoModal();
	}

	LPCTSTR ShaderSyncDlg::Geti3EnginePath()
	{
		return m_browsei3EnginePath.GetText();
	}

	BEGIN_MESSAGE_MAP(ShaderSyncDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
		ON_WM_CTLCOLOR()
	END_MESSAGE_MAP()

	// ShaderSyncDlg 메시지 처리기입니다.
	int ShaderSyncDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1) return -1;

		m_editFileShaderCode = std::make_unique<FileShaderCodeEditBox>();
		m_editMemoryShaderCode = std::make_unique<MemoryShaderCodeEditBox>();

		// shader list ctrls.
		{
			m_fileShaderListCtrl = std::make_unique<ShaderList>();

			auto callback = std::make_unique<FileShaderListCallback>(*m_fileShaderListCtrl, this,
				std::bind(&FileShaderCodeEditBox::SetShaderCode, m_editFileShaderCode.get(), std::placeholders::_1));

			m_fileShaderListCtrl->CreateGridCtrl(this, ID_FILE_SHADER_LIST_CTRL, std::move(callback));
		}

		{
			m_memoryShaderListCtrl = std::make_unique<ShaderList>();

			auto callback = std::make_unique<MemoryShaderListCallback>(*m_memoryShaderListCtrl, this,
				std::bind(&MemoryShaderCodeEditBox::SetShaderCode, m_editMemoryShaderCode.get(), std::placeholders::_1, std::placeholders::_2));

			m_memoryShaderListCtrl->CreateGridCtrl(this, ID_MEMORY_SHADER_LIST_CTRL, std::move(callback));
		}
		
		// browse for i3Engine path.
		m_browsei3EnginePath.SetCallback(std::make_unique<i3EnginePathBrowseCallback>(
			std::bind(&ShaderList::OnShowShaderList, m_fileShaderListCtrl.get(), std::placeholders::_1),
			std::bind(&ShaderList::OnShowShaderList, m_memoryShaderListCtrl.get(), std::placeholders::_1)));

		m_browsei3EnginePath.SetBackColor(COLOR_BLACK);
		m_browsei3EnginePath.SetTextColor(COLOR_TAN3_PERU);
		m_browsei3EnginePath.CreateGrid(WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, ID_BROWSE_SHADER_PATH);

		return 0;
	}

	void ShaderSyncDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);
	}

	BOOL ShaderSyncDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush brush(COLOR_BLACK);

		CBrush* pOldBrush = pDC->SelectObject(&brush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	void ShaderSyncDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		if (nIDCtl == IDOK || nIDCtl == IDCANCEL)
		{
			/*CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

			CRect rt(lpDrawItemStruct->rcItem);

			CString caption;
			GetDlgItemText(nIDCtl, caption);

			pDC->SetBkMode(TRANSPARENT);

			pDC->FillSolidRect(rt, COLOR_BLACK);
			pDC->Draw3dRect(rt, COLOR_TAN3_PERU, COLOR_TAN3_PERU);
			pDC->SetTextColor(COLOR_TAN3_PERU);
			pDC->DrawText(caption, rt, DT_CENTER | DT_VCENTER | DT_SINGLELINE);*/
		}
	}

	void ShaderSyncDlg::OnSync()
	{}

	void ShaderSyncDlg::OnCancel()
	{
		CDialogEx::OnCancel();
	}

	HBRUSH ShaderSyncDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		return hbr;
	}
}