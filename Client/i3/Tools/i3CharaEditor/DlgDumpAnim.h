#pragma once
#include "afxwin.h"
#include <map>

// CDlgDumpAnim 대화 상자입니다.

class CDlgDumpAnim : public CDialog
{
	DECLARE_DYNAMIC(CDlgDumpAnim)

public:
	CDlgDumpAnim(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgDumpAnim();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_DUMP_ANIMATIONLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edPath;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();

	struct CHRInfo
	{
		std::string					_Path;
		std::vector<UINT128>		_AnimFileIndexList;
	};
	std::map< UINT128, CHRInfo>		m_CharaPathList;

	struct AnimInfo
	{
		std::string					_Path;
		std::vector<UINT128>		_ChrFileIndexList;

		AnimInfo( std::string &str, UINT128 fileIdx)	{ _Path = str; _ChrFileIndexList.push_back( fileIdx); }
	};
	std::map< UINT128, AnimInfo>		m_AnimationPathList;

	void	LoadAnimFromI3CHR( void);
	void	SearchAnimFromAICtx( std::map< UINT128, CHRInfo>::iterator &chrIt, const std::vector<i3AIContext*> &AIList);

	void	DumpFile( const char * pszPath);
};
