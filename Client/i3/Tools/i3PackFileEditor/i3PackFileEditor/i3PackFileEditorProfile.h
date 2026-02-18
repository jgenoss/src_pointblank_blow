#if !defined( __EDITOR_PROFILE_H)
#define __EDITOR_PROFILE_H

class i3PackFileEditorProfile
{
public:
	char		m_szWorkDir[MAX_PATH];
	char		m_szTexDir[MAX_PATH];
	char		m_szOutDir[MAX_PATH];
	INT32		m_MainWndX, m_MainWndY;
	INT32		m_MainWndCX, m_MainWndCY;

public:
	i3PackFileEditorProfile(void);
	virtual ~i3PackFileEditorProfile(void);

	void	Load(void);
	void	Save(void);

};

#endif