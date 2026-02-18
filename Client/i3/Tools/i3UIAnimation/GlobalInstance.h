#pragma once

class CGlobalInstance : public CSingleton<CGlobalInstance>
{
	i3::vector<UVITEM*>		m_vecUVItem;

	i3UIAnimationMgr*			m_pUIAnimation;
public:
	CGlobalInstance();
	~CGlobalInstance();

	//UV
	INT32		GetUVCount() { return m_vecUVItem.size(); }
	void		AddUVItem(i3Texture* pTexture, CRect rect);
	UVITEM*		GetUVItem(INT32 nIndex);
	INT32		GetUVItem(i3UIControl* pControl);
	void		UpdateUVItem(i3UIControl* pControl);
	void		RemoveUVItem(UVITEM* pItem);
	void		RemoveUVItems();

	//Aniamtion
	void		AddAnimationItem(char* pchTitle, REAL32 fLifeTime);
	i3UIAnimation2DSprite* GetAnimation(i3::string name);
	void		RemoveAnimation(i3::string name);

	bool		SaveAnimationList();
	bool		OpenAnimationList();
	void		NewAnimationFile();
	i3UIAnimationMgr* GetUIAnimationMgr() { return m_pUIAnimation; }
};

