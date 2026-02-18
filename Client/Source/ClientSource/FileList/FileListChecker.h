#ifndef __FILELISTCHECKER_H__
#define	__FILELISTCHECKER_H__


#include "FileList.h"


typedef		std::pair<CFileListFileInfo*, UINT32>		ErrorFileInfo;
typedef		std::list<ErrorFileInfo>						ListErrorFileInfo;

class	CFileListChecker
{
public:
	CFileListChecker();
	~CFileListChecker()		{}

public:
	bool	Begin(CFileList * pDestFileList, ListErrorFileInfo * pErrorList, FileProgressData * pProgData, bool IsFullCheck, bool IsNTFS);
	bool	End();
	bool	Update();	// Begin 후 false를 반환할때 까지 계속 Update를 호출해야함.


private:
	void	Clear();

protected:
	bool					m_IsFullCheck;
	bool					m_IsNTFS;
	CFileList *				m_pDestFileList;
	const ListFileInfo *			m_pFileInfoList;
	ListFileInfo::const_iterator	m_DestFileList_iter;
	ListErrorFileInfo *		m_pErrorList;
	FileProgressData *		m_pProgData;
	FileProgressData		m_tempProgData;
};


#endif

