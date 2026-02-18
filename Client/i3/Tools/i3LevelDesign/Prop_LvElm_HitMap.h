#pragma once
#include "resource.h"
#include "afxwin.h"

class CHitMapTimeSeqCtrl;
class CProp_LvElm_HitMapBuild : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_HitMapBuild)
	CProgressCtrl m_BuildProgressCtrl;
	CHitMapTimeSeqCtrl	*m_TimeSeqCtrl;

protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	enum { IDD = IDD_PROP_HITMAP_BUILD };

	CProp_LvElm_HitMapBuild(CWnd* pParent = NULL);
	virtual ~CProp_LvElm_HitMapBuild();

	virtual BOOL OnInitDialog() override;

	afx_msg void OnBnClickedButtonBuild();
	void SetTimeCtrl(CHitMapTimeSeqCtrl* ptimeSeq) { m_TimeSeqCtrl = ptimeSeq; }
};

class CProp_LvElm_HitMap : public i3TDKDialogBase
{
	DECLARE_DYNAMIC(CProp_LvElm_HitMap)

	CComboBox m_RoomSelectCtrl;
	CListCtrl m_PlayerListCtrl;
	CHitMapTimeSeqCtrl	*m_TimeSeqCtrl;

	CEdit m_StartTimeEdit;
	CEdit m_EndTimeEdit;
protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	enum {
		IDD = IDD_PROP_HITMAP_COMMON
	};

	CProp_LvElm_HitMap(CWnd* pParent = NULL);
	virtual ~CProp_LvElm_HitMap();

	//virtual void	SetObject(i3ElementBase * pObj);
	virtual BOOL OnInitDialog() override;
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;


	void ResetRoomList();
	void SetTimeCtrl(CHitMapTimeSeqCtrl* ptimeSeq) { m_TimeSeqCtrl = ptimeSeq; }
	void ReLoadKillList(WEAPON_SLOT_TYPE slotType);
	void SetSeqTime(char* chStartTime, char* chEndTime);


	afx_msg void OnCbnSelchangeCbRoomselect();
	afx_msg void OnLvnItemchangedListPlayer(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioEtc();
	afx_msg void OnBnClickedRadioMelee();
	afx_msg void OnBnClickedRadioAll();
	afx_msg void OnBnClickedRadioPrimary();
	afx_msg void OnBnClickedRadioSecondary();
	afx_msg void OnBnClickedRadioThrowing();
	afx_msg void OnEnChangeEditStarttime();
	afx_msg void OnEnChangeEditEndtime();

};

