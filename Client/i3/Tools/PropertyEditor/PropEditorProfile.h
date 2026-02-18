#pragma once

class CPropEditorProfile
{
public:
	CString m_WorkDirectoryPath;
	CString m_CurrentFilePath;

	CRect m_MainWnd;

	CRect m_SimpleTree, m_SimpleList;
	CRect m_GapRect;

	//절대 위치, 절대 위치
	CRect m_Tree1, m_Tree2;
	CRect m_List1, m_List2;
	CRect m_Image;

	CRect m_GapRect1, m_GapRect2, m_GapRect3, m_GapRect5;

	int m_GapSize;
	int m_SplitFactor;
	bool m_bComplexUse;

public:
	CPropEditorProfile(void);
	virtual ~CPropEditorProfile(void);

public:
	void	Load( void);
	void	Save( void);

public:
	void DefaultRect();
	void UpdateGapRect();

	int MousePointOnSizeBar( POINT pt, int & BarIndex );
	void SetComplexUseType(bool ComplexUse)	{	m_bComplexUse = ComplexUse;	}
};
