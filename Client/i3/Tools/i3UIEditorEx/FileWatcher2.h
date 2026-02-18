#pragma once

//File modification을 감시하기 위한 thread
//m_szFile[0] == 0 이면 전체 폴더를 감시한다.
class FileWatcher2 : public i3Thread
{
	I3_CLASS_DEFINE( FileWatcher2, i3Thread);

protected:
	char	m_szFolder[MAX_PATH];	//Folder To Watch
	char	m_szFile[MAX_PATH];		//File To Watch. 
	
	bool	m_bModified;			//이거 없이 바로 Notification 날릴 수 없나? 
	
public:
	FileWatcher2(void);
	virtual ~FileWatcher2(void);

	BOOL		Create( const char* pszFolderToWatch, const char* pszFileToWatch = NULL);

	virtual UINT32 OnRunning( void* pUserData) override;

	bool	isModified( void)				{	return m_bModified;			}
	void	setModified( bool bModified)		{	m_bModified = bModified;	}

};
