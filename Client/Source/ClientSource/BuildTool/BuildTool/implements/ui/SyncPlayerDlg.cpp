#include "stdafx.h"
#include "BuildTool.h"
#include "SyncPlayerDlg.h"
#include "afxdialogex.h"
#include "UIDefines.h"
#include "ui/LoadingProgressMsgCallback.h"
#include "MainFrm.h"
#include "resource/ResourceController.h"
#include "resource/CountrySettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
	void _DuplicatedPackFileCheckMessage(const std::wstring& message)
	{
		if (IDYES == AfxMessageBox(message.c_str(), MB_YESNO))
		{
			GetMainFrame()->GetDuplicatedPackFileReport().SetUseDuplicatedFileCheck(true);
			return;
		}
		
		GetMainFrame()->GetDuplicatedPackFileReport().SetUseDuplicatedFileCheck(false);
		
	}
}

namespace ui
{
	class SyncPefRscProgressMsgCallback : public ILoadingProgressMsgCallback
	{
	public:
		SyncPefRscProgressMsgCallback(SyncProgressBar* progressBar) : m_progressBar(progressBar) {}
		virtual ~SyncPefRscProgressMsgCallback() {}

		virtual void OnTextMessage(const std::wstring& wstrText) override
		{
			if (m_progressBar)
				m_progressBar->SetText(wstrText.c_str());
		}

		virtual void OnGaugeProgress(const int progress) override
		{
			if (m_progressBar)
				m_progressBar->SetGaugePos(progress);
		}

	private:
		SyncProgressBar* m_progressBar = nullptr;
	};

	struct IRequestSyncCallback
	{
		virtual void operator()(SyncPlayerDlg& owner) = 0;
		virtual ~IRequestSyncCallback() {}
	};

	struct RequestSync_None : IRequestSyncCallback
	{
		virtual void operator()(SyncPlayerDlg& owner) {}
		virtual ~RequestSync_None() {}
	} g_req_sync_none;

	struct RequestSync : IRequestSyncCallback
	{
		virtual void operator()(SyncPlayerDlg& owner)
		{
			const long currSelRow = owner.m_contentsListCtrl->GetCurrSelectionRow();
			const long currSyncRow = owner.GetCurrSyncRow();
			if (currSelRow != currSyncRow)
			{
				rsc::ResourceController::i()->OnRequestSync(owner.GetContentsData(currSelRow));
				owner.SetCurrSyncRow(currSelRow);
			}
		}
		virtual ~RequestSync() {}
	} g_req_sync;

	struct RequestSyncSequentially : IRequestSyncCallback
	{
		virtual void operator()(SyncPlayerDlg& owner)
		{
			const long row = owner.GetCurrSyncRow();
			if (-1 <= row && row < long(owner.GetTotalNumOfContents()) - 1)
			{
				const long rowNext = row + 1;
				rsc::ResourceController::i()->OnRequestSync(owner.GetContentsData(rowNext));
				owner.SetCurrSyncRow(rowNext);
			}
		}
		virtual ~RequestSyncSequentially() {}
	} g_req_sync_seq;

	struct RequestSyncRandomly : IRequestSyncCallback
	{
		virtual void operator()(SyncPlayerDlg& owner)
		{
			const long row = owner.GetCurrSyncRow();
			if (0 <= row && row < long(owner.GetTotalNumOfContents()))
			{
				std::vector<long> idxList;
				for (long i = 0; i < long(owner.GetTotalNumOfContents()); i++)
				{
					if (i == row)
						continue;
					idxList.push_back(i);
					std::random_shuffle(idxList.begin(), idxList.end());
				}

				const long randomRow = idxList[0];
				rsc::ResourceController::i()->OnRequestSync(owner.GetContentsData(randomRow));

				owner.SetCurrSyncRow(row);
			}
		}
		virtual ~RequestSyncRandomly() {}
	} g_req_sync_rand;



	struct ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) = 0;
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) = 0;
		virtual void OnSync(SyncPlayerDlg& owner, const float progress) = 0;
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) = 0;
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) = 0;
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) = 0;
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) = 0;
		virtual void OnReset(SyncPlayerDlg& owner) = 0;
		virtual ~ISyncPlayState() {}
	};

	struct SyncState_OnNone : public ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) override {}
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) override {}
		virtual void OnSync(SyncPlayerDlg& owner, const float progress) {}
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) override {}
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) override {}
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) override {}
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) override {}
		virtual void OnReset(SyncPlayerDlg& owner) override {}
		virtual ~SyncState_OnNone() {}
	} g_sync_state_none;

	struct SyncState_OnReadyPlay : public ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) override;
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) override;
		virtual void OnSync(SyncPlayerDlg& owner, const float progress);
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) override;
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) override;
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnReset(SyncPlayerDlg& owner) override;
		virtual ~SyncState_OnReadyPlay() {}
	} g_sync_state_on_ready_play;

	struct SyncState_OnPlay : public ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) override;
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) override;
		virtual void OnSync(SyncPlayerDlg& owner, const float progress);
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) override;
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) override;
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnReset(SyncPlayerDlg& owner) override;
		virtual ~SyncState_OnPlay() {}
	} g_sync_state_on_play;

	struct SyncState_OnReadyPlayAll : public ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) override;
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) override;
		virtual void OnSync(SyncPlayerDlg& owner, const float progress);
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) override;
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) override;
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnReset(SyncPlayerDlg& owner) override;
		virtual ~SyncState_OnReadyPlayAll() {}
	} g_sync_state_on_ready_playall;

	struct SyncState_OnPlayAll : public ISyncPlayState
	{
		virtual void OnEntrance(SyncPlayerDlg& owner) override;
		virtual void OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName) override;
		virtual void OnSync(SyncPlayerDlg& owner, const float progress);
		virtual void OnPostSync(SyncPlayerDlg& owner, const bool success) override;
		virtual void OnPlayButtonClicked(SyncPlayerDlg& owner) override;
		virtual void OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok) override;
		virtual void OnReset(SyncPlayerDlg& owner) override;
		virtual ~SyncState_OnPlayAll() {}

		bool m_isReadyToExportPefRsc = true;
	} g_sync_state_on_playall;

	// ------------------------------------
	// SyncState_OnReadyPlay
	// ------------------------------------
	void SyncState_OnReadyPlay::OnEntrance(SyncPlayerDlg& owner)
	{
		owner.m_playButtonCtrl->SetPause(false);
		owner.SetCurrSyncRow(-1);
	}

	void SyncState_OnReadyPlay::OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName)
	{}

	void SyncState_OnReadyPlay::OnSync(SyncPlayerDlg& owner, const float progress)
	{}

	void SyncState_OnReadyPlay::OnPostSync(SyncPlayerDlg& owner, const bool success)
	{}

	void SyncState_OnReadyPlay::OnPlayButtonClicked(SyncPlayerDlg& owner)
	{
		owner.SetReqSyncCallback(g_req_sync);
		owner.SetSyncPlayState(g_sync_state_on_play);
	}

	void SyncState_OnReadyPlay::OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool isPlayAll)
	{
		if (isPlayAll)
		{
			owner.SetReqSyncCallback(g_req_sync_none);
			owner.SetSyncPlayState(g_sync_state_on_ready_playall);
		}
	}

	void SyncState_OnReadyPlay::OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok)
	{}

	void SyncState_OnReadyPlay::OnReset(SyncPlayerDlg& owner)
	{}

	// ------------------------------------
	// SyncState_OnPlay
	// ------------------------------------
	void SyncState_OnPlay::OnEntrance(SyncPlayerDlg& owner)
	{
		owner.GetReqSyncCallback()(owner);
	}

	void SyncState_OnPlay::OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName)
	{
		owner.m_playButtonCtrl->SetPause(true);

		owner.m_progressBarCtrl->SetGaugePos(0);
		owner.m_progressBarCtrl->SetText(L"");

		owner.ShowSyncContentsInfo(owner.GetCurrSyncRow(), strCountryName);

		if (owner.m_exportPefRscButtonCtrl->IsExportMode())
		{
			const rsc::CountryDataPtr countryData = rsc::CountrySettings::i()->FindData(strCountryName);

			SyncPefRscProgressMsgCallback cbPefRscProgress(owner.m_progressBarCtrl.get());
			Savei3RegXMLToPEF(countryData->GetQAWorkdir(), cbPefRscProgress);
			SaveAllRSGToRSC(countryData->GetQAWorkdir(), cbPefRscProgress);
		}
	}

	void SyncState_OnPlay::OnSync(SyncPlayerDlg& owner, const float progress)
	{
		owner.m_progressBarCtrl->SetGaugePos(int(progress * 100.0f));
	}

	void SyncState_OnPlay::OnPostSync(SyncPlayerDlg& owner, const bool success)
	{
		if (success)
		{
			owner.m_progressBarCtrl->SetGaugePos(100);
		}

		owner.SetReqSyncCallback(g_req_sync_none);
		owner.SetSyncPlayState(g_sync_state_on_ready_play);
	}

	void SyncState_OnPlay::OnPlayButtonClicked(SyncPlayerDlg& owner)
	{
		// 동기화 스레드가 종료되는 시점(OnPostSync 를 받는 시점) 에서 상태가 전환된다.
	}

	void SyncState_OnPlay::OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool isPlayAll)
	{
		if (isPlayAll)
		{
			owner.SetSyncPlayState(g_sync_state_on_playall);
			owner.SetReqSyncCallback(g_req_sync_none);
		}
	}

	void SyncState_OnPlay::OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok)
	{}

	void SyncState_OnPlay::OnReset(SyncPlayerDlg& owner)
	{
		owner.SetReqSyncCallback(g_req_sync_none);
		owner.SetSyncPlayState(g_sync_state_on_ready_play);
	}

	// ------------------------------------
	// SyncState_OnReadyPlayAll
	// ------------------------------------
	void SyncState_OnReadyPlayAll::OnEntrance(SyncPlayerDlg& owner)
	{
		owner.m_playButtonCtrl->SetPause(false);
		owner.SetCurrSyncRow(-1);
	}

	void SyncState_OnReadyPlayAll::OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName)
	{}

	void SyncState_OnReadyPlayAll::OnSync(SyncPlayerDlg& owner, const float progress)
	{}

	void SyncState_OnReadyPlayAll::OnPostSync(SyncPlayerDlg& owner, const bool success)
	{}

	void SyncState_OnReadyPlayAll::OnPlayButtonClicked(SyncPlayerDlg& owner)
	{
		owner.SetReqSyncCallback(g_req_sync_seq);
		owner.SetSyncPlayState(g_sync_state_on_playall);
	}

	void SyncState_OnReadyPlayAll::OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool isPlayAll)
	{
		if (!isPlayAll)
			owner.SetSyncPlayState(g_sync_state_on_ready_play);
	}

	void SyncState_OnReadyPlayAll::OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok)
	{}

	void SyncState_OnReadyPlayAll::OnReset(SyncPlayerDlg& owner)
	{}

	// ------------------------------------
	// SyncState_OnPlayAll
	// ------------------------------------
	void SyncState_OnPlayAll::OnEntrance(SyncPlayerDlg& owner)
	{
		owner.GetReqSyncCallback()(owner);
		m_isReadyToExportPefRsc = true;
	}

	void SyncState_OnPlayAll::OnPreSync(SyncPlayerDlg& owner, const std::string& strCountryName)
	{
		owner.m_playButtonCtrl->SetPause(true);
		owner.m_progressBarCtrl->SetGaugePos(0);
		owner.ShowSyncContentsInfo(owner.GetCurrSyncRow(), strCountryName);

		if (owner.m_exportPefRscButtonCtrl->IsExportMode() && m_isReadyToExportPefRsc)
		{
			const rsc::CountryDataPtr countryData = rsc::CountrySettings::i()->FindData(strCountryName);

			SyncPefRscProgressMsgCallback cbPefRscProgress(owner.m_progressBarCtrl.get());
			Savei3RegXMLToPEF(countryData->GetQAWorkdir(), cbPefRscProgress);
			SaveAllRSGToRSC(countryData->GetQAWorkdir(), cbPefRscProgress);
		}
	}

	void SyncState_OnPlayAll::OnSync(SyncPlayerDlg& owner, const float progress)
	{
		owner.m_progressBarCtrl->SetGaugePos(int(progress * 100.0f));
	}

	void SyncState_OnPlayAll::OnPostSync(SyncPlayerDlg& owner, const bool success)
	{
		if (success)
			owner.m_progressBarCtrl->SetGaugePos(100);

		const long row = owner.GetCurrSyncRow();
		if (0 <= row && row < long(owner.GetTotalNumOfContents()) - 1)
		{
			owner.GetReqSyncCallback()(owner);
		}
		else
		{
			owner.SetReqSyncCallback(g_req_sync_none);
			owner.SetSyncPlayState(g_sync_state_on_ready_playall);


			if (GetMainFrame()->GetDuplicatedPackFileReport().UseDuplicatedFileCheck())
			{
				const rsc::CountryDataPtr countryData = rsc::CountrySettings::i()->FindData(rsc::ResourceController::i()->GetCurrCountryName());
				std::string dstWorkDir = countryData->GetLiveWorkdir().string();
				GetMainFrame()->GetDuplicatedPackFileReport().DuplicatedFileCheck(dstWorkDir);
				GetMainFrame()->GetDuplicatedPackFileReport().SetUseDuplicatedFileCheck(false);
			}

		}

		m_isReadyToExportPefRsc = false;
	}

	void SyncState_OnPlayAll::OnPlayButtonClicked(SyncPlayerDlg& owner)
	{
		owner.SetSyncPlayState(g_sync_state_on_ready_playall);
	}

	void SyncState_OnPlayAll::OnPlayAllModeClicked(SyncPlayerDlg& owner, const bool isPlayAll)
	{
		if (!isPlayAll)
			owner.SetSyncPlayState(g_sync_state_on_play);
	}

	void SyncState_OnPlayAll::OnForceCopyModeClicked(SyncPlayerDlg& owner, const bool ok)
	{}

	void SyncState_OnPlayAll::OnReset(SyncPlayerDlg& owner)
	{
		owner.SetReqSyncCallback(g_req_sync_none);
		owner.SetSyncPlayState(g_sync_state_on_ready_playall);
	}


	rsc::ContentsSyncDataPtr g_nullContentsData(new rsc::ContentsSyncData);

	IMPLEMENT_DYNAMIC(SyncPlayerDlg, CDialogEx)

	SyncPlayerDlg::SyncPlayerDlg(CWnd* pParent /*=NULL*/)
		: CDialogEx(IDD_DIALOG_SYNCPLAYER, pParent)
	{}

	SyncPlayerDlg::~SyncPlayerDlg()
	{}

	void SyncPlayerDlg::AddContents(const std::vector<rsc::ContentsSyncDataPtr>& contentsDataList)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		std::copy(contentsDataList.begin(), contentsDataList.end(), std::back_inserter(m_contentsDataAddQ));

		PostMessage(WMU_INSERT_CONTENTS_SYNCLIST);
	}

	void SyncPlayerDlg::DelContents(const std::vector<long>& rows)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		for (auto &row : rows)
		{
			if (0 <= row && row < long(m_contentsDataList.size()))
				m_contentsDataDelQ.push_back(m_contentsDataList[row]);
		}

		PostMessage(WMU_DELETE_CONTENTS_SYNCLIST);
	}

	void SyncPlayerDlg::ClearContents()
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		PostMessage(WMU_CLEAR_CONTENTS_SYNCLIST);
	}

	void SyncPlayerDlg::Reset()
	{
		m_textContentsName.SetText(L"");
		m_textCountryName.SetText(L"");
		m_progressBarCtrl->SetGaugePos(0);
		m_progressBarCtrl->SetText(L"");
		m_contentsListCtrl->ClearContents();
		m_currStateCallback->OnReset(*this);
		m_currSyncRow = -1;
		m_contentsDataAddQ.clear();
		m_contentsDataDelQ.clear();
		m_contentsDataList.clear();
	}

	void SyncPlayerDlg::SetSyncPlayState(ISyncPlayState& callback)
	{
		m_currStateCallback = &callback;
		callback.OnEntrance(*this);
	}

	void SyncPlayerDlg::SetCurrSyncRow(const long row)
	{
		if (-1 <= row && row < long(m_contentsDataList.size()))
		{
			m_contentsListCtrl->SetSyncHighlight(m_currSyncRow, false);
			m_contentsListCtrl->SetSyncHighlight(row, true);
			m_currSyncRow = row;
		}
	}

	const rsc::ContentsSyncDataPtr SyncPlayerDlg::GetContentsData(const long row) const
	{
		if (row < 0 || long(m_contentsDataList.size()) <= row)
			return g_nullContentsData;

		return m_contentsDataList.at(size_t(row));
	}

	size_t SyncPlayerDlg::GetTotalNumOfContents() const
	{
		return m_contentsDataList.size();
	}

	void SyncPlayerDlg::SetReqSyncCallback(IRequestSyncCallback& cb) 
	{
		m_reqSyncCallback = &cb; 
	}

	IRequestSyncCallback& SyncPlayerDlg::GetReqSyncCallback() const 
	{ 
		return *m_reqSyncCallback; 
	}

	void SyncPlayerDlg::ShowSyncContentsInfo(const long row, const std::string& strCountryName)
	{
		const rsc::ContentsSyncDataPtr& contentsData = GetContentsData(row);

		if (0 <= row && row < long(m_contentsDataList.size()))
		{
			std::wstring wstrContentsName;	
			i3::mb_to_utf16(GetContentsData(row)->contents->strContentsName, wstrContentsName);
			m_textContentsName.SetText(wstrContentsName.c_str());
		}

		std::wstring wstrCountryName;	
		i3::mb_to_utf16(strCountryName, wstrCountryName);
		m_textCountryName.SetText(wstrCountryName.c_str());
	}

	void SyncPlayerDlg::OnPreSync(const std::string& strCountryName)
	{
		m_currStateCallback->OnPreSync(*this, strCountryName);
	}

	void SyncPlayerDlg::OnSync(const float progress)
	{
		m_currStateCallback->OnSync(*this, progress);
	}

	void SyncPlayerDlg::OnPostSync(const bool success)
	{
		m_currStateCallback->OnPostSync(*this, success);
	}

	void SyncPlayerDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialogEx::DoDataExchange(pDX);
	}

	BOOL SyncPlayerDlg::OnInitDialog()
	{
		CDialogEx::OnInitDialog();

		m_textContentsName.SubclassDlgItem(IDC_STATIC_CURR_SYNC_CONTENTS_NAME, this);
		LOGFONT lf;	GetFont()->GetLogFont(&lf);
		lf.lfWeight = FW_SEMIBOLD;
		lf.lfHeight = 18;
		m_fontContentsName.CreateFontIndirect(&lf);
		m_textContentsName.SetFont(&m_fontContentsName, TRUE);
		m_textContentsName.SetText(L"");

		m_textCountryName.SubclassDlgItem(IDC_STATIC_CURR_SYNC_COUNTRY_NAME, this);
		lf.lfWeight = FW_MEDIUM;
		lf.lfHeight = 14;
		m_fontCountryName.CreateFontIndirect(&lf);
		m_textCountryName.SetFont(&m_fontCountryName, TRUE);
		m_textCountryName.SetText(L"");

		m_bkGndBottom.SubclassDlgItem(IDC_STATIC_BKGND_BOTTOM, this);
		m_bkGndBottom.SetText(L"");

		m_contentsListCtrl = std::make_unique<SyncContentsList>(*this);
		m_forceCopyModeButtonCtrl = std::make_unique<SyncForceCopyModeButton>(*this);
		m_playAllModeButtonCtrl = std::make_unique<SyncPlayAllModeButton>(*this);
		m_exportPefRscButtonCtrl = std::make_unique<SyncExportPefRscButton>(*this);
		m_playButtonCtrl = std::make_unique<SyncPlayButton>(*this);
		m_progressBarCtrl = std::make_unique<SyncProgressBar>(*this);

		if (m_playAllModeButtonCtrl->IsPlayAllMode())
			SetSyncPlayState(g_sync_state_on_ready_playall);
		else
			SetSyncPlayState(g_sync_state_on_ready_play);

		SetReqSyncCallback(g_req_sync_none);

		return TRUE;
	}


	BEGIN_MESSAGE_MAP(SyncPlayerDlg, CDialogEx)
		ON_WM_CREATE()
		ON_WM_SIZE()
		ON_WM_ERASEBKGND()
		ON_WM_DRAWITEM()
		ON_WM_CTLCOLOR()
		ON_BN_CLICKED(IDOK, &SyncPlayerDlg::OnBnClickedOK)
		ON_BN_CLICKED(IDCANCEL, &SyncPlayerDlg::OnBnClickedCancel)
		ON_MESSAGE(WMU_INSERT_CONTENTS_SYNCLIST, &SyncPlayerDlg::OnAddContents)
		ON_MESSAGE(WMU_DELETE_CONTENTS_SYNCLIST, &SyncPlayerDlg::OnDelContents)
		ON_MESSAGE(WMU_CLEAR_CONTENTS_SYNCLIST, &SyncPlayerDlg::OnClearContents)
	END_MESSAGE_MAP()


	// SyncPlayerDlg 메시지 처리기입니다.
	int SyncPlayerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		if (CDialogEx::OnCreate(lpCreateStruct) == -1)
			return -1;

		m_bkGndBrush.CreateSolidBrush(COLOR_IVORY_BLACK);
		m_bkGndBottomBrush.CreateSolidBrush(COLOR_BLACK);

		return 0;
	}

	void SyncPlayerDlg::OnSize(UINT nType, int cx, int cy)
	{
		CDialogEx::OnSize(nType, cx, cy);
		
		RECT rt;	GetClientRect(&rt);

		RECT rtTextContentsName = rt;	
		rtTextContentsName.bottom = 35;
		rtTextContentsName.left = 20;

		if (m_textContentsName.GetSafeHwnd())
			m_textContentsName.MoveWindow(&rtTextContentsName);

		RECT rtTextCountryName;
		rtTextCountryName.top = rtTextContentsName.bottom;
		rtTextCountryName.bottom = rtTextCountryName.top + 15;
		rtTextCountryName.left = 20;
		rtTextCountryName.right = cx;

		if (m_textCountryName.GetSafeHwnd())
			m_textCountryName.MoveWindow(&rtTextCountryName);

		const LONG cx_devided = static_cast<LONG>(cx * 0.33f);

		RECT rtBtnForceCopyMode;
		rtBtnForceCopyMode.top = rtTextCountryName.bottom + 5;
		rtBtnForceCopyMode.bottom = rtBtnForceCopyMode.top + 40;
		rtBtnForceCopyMode.left = 0;
		rtBtnForceCopyMode.right = cx_devided;

		if (m_forceCopyModeButtonCtrl.get())
			m_forceCopyModeButtonCtrl->MoveWindow(rtBtnForceCopyMode);

		RECT rtBtnPlayAllMode = rtBtnForceCopyMode;
		rtBtnPlayAllMode.left = rtBtnForceCopyMode.right + 2;
		rtBtnPlayAllMode.right = rtBtnForceCopyMode.right + cx_devided;

		if (m_playAllModeButtonCtrl.get())
			m_playAllModeButtonCtrl->MoveWindow(rtBtnPlayAllMode);

		RECT rtBtnExportPefRsc = rtBtnPlayAllMode;
		rtBtnExportPefRsc.left = rtBtnPlayAllMode.right + 2;
		rtBtnExportPefRsc.right = cx;

		if (m_exportPefRscButtonCtrl.get())
			m_exportPefRscButtonCtrl->MoveWindow(rtBtnExportPefRsc);

		RECT rtPlayButton;
		const LONG cy_playBtn = 55;
		rtPlayButton.bottom = cy - 10;	
		rtPlayButton.top = rtPlayButton.bottom - cy_playBtn;

		const LONG cx_playBtn = static_cast<LONG>(cx * 0.5f);
		rtPlayButton.left = static_cast<LONG>((cx - cx_playBtn) * 0.5f);
		rtPlayButton.right = rtPlayButton.left + cx_playBtn;

		if (m_playButtonCtrl.get())
			m_playButtonCtrl->MoveWindow(rtPlayButton);

		RECT rtProgress;
		rtProgress.left = 0;	
		rtProgress.right = cx;
		rtProgress.bottom = rtPlayButton.top - 10;
		rtProgress.top = rtProgress.bottom - 30;
		if (m_progressBarCtrl.get())
			m_progressBarCtrl->MoveWindow(rtProgress);

		RECT rtContentsList;
		rtContentsList.top = rtBtnForceCopyMode.bottom + 5;
		rtContentsList.bottom = rtProgress.top - 5;
		rtContentsList.left = 0;
		rtContentsList.right = cx;

		if (m_contentsListCtrl.get())
			m_contentsListCtrl->AdjustLayout(rtContentsList);

		RECT rtBkGndBottom;
		rtBkGndBottom.top = rtProgress.bottom;
		rtBkGndBottom.bottom = cy;
		rtBkGndBottom.left = 0;
		rtBkGndBottom.right = cx;

		if (m_bkGndBottom.GetSafeHwnd())
			m_bkGndBottom.MoveWindow(&rtBkGndBottom);

		// 컨트롤 Draw 순서는 BuildTool.rc 파일의 선언 순서와 동일함에 유의.
	}

	BOOL SyncPlayerDlg::OnEraseBkgnd(CDC* pDC)
	{
		CBrush* pOldBrush = pDC->SelectObject(&m_bkGndBrush);

		CRect rect;
		pDC->GetClipBox(&rect);
		pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);

		return TRUE;
	}

	HBRUSH SyncPlayerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
	{
		HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

		switch (nCtlColor)
		{
		case CTLCOLOR_STATIC:
			if (pWnd->GetDlgCtrlID() == IDC_STATIC_CURR_SYNC_CONTENTS_NAME)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_WHITE);

				hbr = m_bkGndBrush;
			}
			else if (pWnd->GetDlgCtrlID() == IDC_STATIC_CURR_SYNC_COUNTRY_NAME)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_SILVER);

				hbr = m_bkGndBrush;
			}
			else if (pWnd->GetDlgCtrlID() == IDC_STATIC_BKGND_BOTTOM)
			{
				pDC->SetBkColor(COLOR_BLACK);
				pDC->SetTextColor(COLOR_BLACK);

				hbr = m_bkGndBottomBrush;

				m_playButtonCtrl->SetPause(false); // 드로잉 순서로 인해 재생 버튼이 배경이미지에 가려지는 관계로 한 번 더 그려줌. 일시정지와는 상관 없음.
			}
			else if (pWnd->GetDlgCtrlID() == IDC_PROGRESS_SYNC_TEXT)
			{
				pDC->SetBkColor(COLOR_IVORY_BLACK);
				pDC->SetTextColor(COLOR_TAN3_PERU);

				hbr = m_bkGndBrush;
			}
			break;
		}

		return hbr;
	}

	void SyncPlayerDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
	{
		switch (nIDCtl)
		{
		case IDC_BUTTON_FORCE_COPY_MODE:
			m_forceCopyModeButtonCtrl->DrawButton(lpDrawItemStruct);
			break;

		case IDC_BUTTON_PLAY_ALL_MODE:
			m_playAllModeButtonCtrl->DrawButton(lpDrawItemStruct);
			break;

		case IDC_BUTTON_PLAY:
			m_playButtonCtrl->DrawButton(lpDrawItemStruct);
			break;

		case IDC_BUTTON_EXPORT_PEF_RSC:
			m_exportPefRscButtonCtrl->DrawButton(lpDrawItemStruct);
			break;
		}
	}

	void SyncPlayerDlg::OnBnClickedForceCopyMode()
	{
		//m_forceCopyModeCtrlPtr->ToggleMode();

		m_currStateCallback->OnForceCopyModeClicked(*this, m_forceCopyModeButtonCtrl->IsForceCopyMode());
	}

	void SyncPlayerDlg::OnBnClickedPlayAllMode()
	{
		m_playAllModeButtonCtrl->ToggleMode();
		
		if (!m_playAllModeButtonCtrl->IsPlayAllMode())
			m_exportPefRscButtonCtrl->DrawMode(false);	// 단일 컨텐츠 동기화에서는 PEF/RSC 추출을 사용자 스스로 선택할 수 있게.

		m_currStateCallback->OnPlayAllModeClicked(*this, m_playAllModeButtonCtrl->IsPlayAllMode());
	}

	void SyncPlayerDlg::OnBnClickedExportPefRsc()
	{
		m_exportPefRscButtonCtrl->ToggleMode();
	}

	void SyncPlayerDlg::OnBnClickedPlay()
	{
		if (!GetMainFrame()->GetDuplicatedPackFileReport().UseDuplicatedFileCheck())
			_DuplicatedPackFileCheckMessage(_T("팩 빌드 후 중복 파일 체크를 진행하겠습니까?"));

		m_currStateCallback->OnPlayButtonClicked(*this);
	}

	LRESULT SyncPlayerDlg::OnAddContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		std::copy(m_contentsDataAddQ.begin(), m_contentsDataAddQ.end(), std::back_inserter(m_contentsDataList));

		m_contentsListCtrl->AddContents(m_contentsDataAddQ);

		m_contentsDataAddQ.clear();

		return TRUE;
	}

	LRESULT SyncPlayerDlg::OnDelContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		struct fn : std::binary_function<rsc::ContentsSyncDataPtr, rsc::ContentsSyncDataPtr, bool>
		{
			bool operator()(const rsc::ContentsSyncDataPtr& lhs, const rsc::ContentsSyncDataPtr& rhs) const
			{
				return lhs.get() == rhs.get();
			}
		};

		for (auto &currContentsData : m_contentsDataDelQ)
		{
			auto it = std::find_if(m_contentsDataList.begin(), m_contentsDataList.end(), std::bind2nd(fn(), currContentsData));
			if (it == m_contentsDataList.end())
				return TRUE;

			const size_t row = std::distance(m_contentsDataList.begin(), it);

			if (GetCurrSyncRow() == row)	// 동기화 진행중인 컨텐츠는 삭제 불가.
				continue;

			m_contentsDataList.erase(m_contentsDataList.begin() + row);

			m_contentsListCtrl->DelContents(row);
		}

		m_contentsDataDelQ.clear();

		return TRUE;
	}

	LRESULT SyncPlayerDlg::OnClearContents(WPARAM wparam, LPARAM lparam)
	{
		std::unique_lock<std::mutex> lock(m_mtx);

		m_contentsListCtrl->ClearContents();

		return TRUE;
	}
}
