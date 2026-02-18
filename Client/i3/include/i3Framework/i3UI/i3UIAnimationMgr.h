#pragma once
class i3UIAnimation2DSprite;
class I3_EXPORT_FRAMEWORK i3UIAnimationMgr : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE(i3UIAnimationMgr, i3NamedElement);

	i3::vector<i3UIAnimation2DSprite*>	m_vecAnimation;
	i3::string						m_Version;
public:
	virtual ~i3UIAnimationMgr();

	INT32					GetAnimationCount() {return m_vecAnimation.size();}
	i3UIAnimation2DSprite*		GetAnimation(INT32 i) { return m_vecAnimation[i]; }
	void					RemoveAnimation(i3::string name);
	i3UIAnimation2DSprite*		GetAnimation(i3::string name);
	void					AddAnimation(i3UIAnimation2DSprite* pInfo);
	bool					IsAnimation(i3::string name);

	virtual void		OnBuildObjectList(i3::vector<i3PersistantElement*>& List) override;
	virtual bool		OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool		OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML) override;

};

