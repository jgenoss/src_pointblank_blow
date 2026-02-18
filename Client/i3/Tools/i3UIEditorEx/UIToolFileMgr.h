#pragma once 

class CUIToolFileMgr
{
public:
	CUIToolFileMgr();
	virtual ~CUIToolFileMgr();

public:
	bool MoveToNatioFolder(const char * pszNation);
	void RemoveNationFolderFile(const char * pszNation);
};