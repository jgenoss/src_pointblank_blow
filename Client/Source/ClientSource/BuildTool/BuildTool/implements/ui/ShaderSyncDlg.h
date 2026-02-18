#pragma once

#include "ShaderList.h"
#include "FileShaderCodeEditBox.h"
#include "MemoryShaderCodeEditBox.h"
#include "CustomButton.h"
#include "CustomUGEditBrowse.h"
#include "CustomEditBox.h"

namespace ui
{
	class ShaderSyncDlg : public CDialogEx
	{
		DECLARE_DYNAMIC(ShaderSyncDlg)

	public:
		ShaderSyncDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
		virtual ~ShaderSyncDlg();

		static void Creator();

		LPCTSTR Geti3EnginePath();

	private:
		// editors shader sources.
		CustomEditBox m_editFileShaderCtrl;
		std::unique_ptr<FileShaderCodeEditBox> m_editFileShaderCode;

		CustomEditBox m_editMemoryShaderCtrl;
		std::unique_ptr<MemoryShaderCodeEditBox> m_editMemoryShaderCode;

		// shader list ctrls.
		std::unique_ptr<ShaderList> m_fileShaderListCtrl;
		std::unique_ptr<ShaderList> m_memoryShaderListCtrl;

		// group boxes.
		CStatic m_grpboxFileShaderList;
		CStatic m_grpboxMemoryShaderList;
		CStatic m_grpboxFileShaderView;
		CStatic m_grpboxMemoryShaderView;

		// radio buttons.
		CustomButton m_radioFileShaderOrigMode;
		CustomButton m_radioFileShaderCombineMode;
		UINT m_radioIdxFileShaderView = 0;

		CustomButton m_radioMemShaderOrigMode;
		CustomButton m_radioMemShaderDecryptMode;
		CustomButton m_radioMemShaderCombineMode;
		UINT m_radioIdxMemShaderView = 0;

		// buttons.
		CustomButton m_btnSyncShaders;

		// browser for i3Engine path.
		CustomUGEditBrowseCtrl m_browsei3EnginePath;

	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
		virtual BOOL OnInitDialog();

		DECLARE_MESSAGE_MAP()
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
		afx_msg void OnSync();
		afx_msg void OnCancel();
		afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	};
}
