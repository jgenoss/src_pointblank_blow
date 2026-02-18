
#ifndef _DLGNOGADAAISTATE_H_
#define _DLGNOGADAAISTATE_H_

#ifdef _USE_TOOLAM_

#include "AIStateEditor_Def.h"
#include "AIStateEditor_Ctl.h"


namespace AIStateEdit
{

	class DlgNogadaAIState : public CDialog
	{
		DECLARE_DYNAMIC(DlgNogadaAIState)

	public:
		DlgNogadaAIState(CWnd* pParent = NULL, AIStateEditCallBack* callback=NULL);
		virtual ~DlgNogadaAIState();

		enum { IDD = IDD_NOGADA_AISTATE };

		enum ECTL
		{
			BT_FIND_UPPER_WEAPON_NAME,
			EB_FIND_UPPER_WEAPON_NAME,
			
			CB_UPPER_WEAPON_CLASS,
			CB_UPPER_WEAPON_NAME,
			BT_RESCAN_UPPER_WEAPON_CLASSANDNAME,
			BT_QUERY_UPPER_WEAPON_NAME,

			ST_AREA_UPPER_AISTATE,

			ECTL_MAX,
		};

	protected:
		void RescanUpperWeaponCalssAndName();

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);
		virtual BOOL OnInitDialog();
		virtual void OnCancel();
		virtual void OnOK();

		virtual void PostNcDestroy();

	private:
		void InitCtl();

	private:
		INT32								m_ctl[ECTL_MAX];
		AIStateEditListCtrl					m_lv;
		AIStateEditCallBack*				m_callback;

		WeaponClassScanData					m_upperWeaponClassScan;

	protected:
		DECLARE_MESSAGE_MAP()

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg void OnBtn_FindUpperWeaponName();
		afx_msg void OnBtn_RescanUpperWeaponClassAndName();
		afx_msg void OnBtn_QueryUpperWeaponName();
	};

}

#endif // end of _USE_TOOLAM_

#endif
