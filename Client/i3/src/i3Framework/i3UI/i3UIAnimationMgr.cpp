#include "i3FrameworkPCH.h"
#include "i3UIAnimationMgr.h"
#include "i3UIAnimation2DSprite.h"
#include "i3Base/string/compare/generic_compare.h"

I3_CLASS_INSTANCE(i3UIAnimationMgr);

i3UIAnimationMgr::~i3UIAnimationMgr()
{
	for (size_t i = 0; i < m_vecAnimation.size(); i++)
	{
		I3_SAFE_RELEASE(m_vecAnimation[i]);
	}

	m_vecAnimation.clear();
}

void i3UIAnimationMgr::RemoveAnimation(i3::string name)
{
	for (size_t i = 0; i < m_vecAnimation.size(); i++)
	{
		if(i3::generic_compare(m_vecAnimation[i]->GetName(), name) == 0)
		{
			I3_SAFE_RELEASE(m_vecAnimation[i]);
			i3::vu::erase_index(m_vecAnimation, i);
			break;
		}
	}
}

i3UIAnimation2DSprite* i3UIAnimationMgr::GetAnimation(i3::string name)
{
	for (size_t i = 0; i < m_vecAnimation.size(); i++)
	{
		if (i3::generic_compare(m_vecAnimation[i]->GetName(), name) == 0)
		{
			return m_vecAnimation[i];
		}
	}

	return nullptr;
}

void i3UIAnimationMgr::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3NamedElement::OnBuildObjectList(List);

	i3::map<i3::string, i3UIAnimation2DSprite*>::iterator it;
	for (size_t i = 0; i < m_vecAnimation.size(); i++)
	{
		m_vecAnimation[i]->OnBuildObjectList(List);
	}
}

void i3UIAnimationMgr::AddAnimation(i3UIAnimation2DSprite* pInfo)
{
	m_vecAnimation.push_back(pInfo);
}

bool i3UIAnimationMgr::IsAnimation(i3::string name)
{
	i3UIAnimation2DSprite* pInfo = GetAnimation(name);
	if (pInfo)
		return true;
	else
		return false;
}

bool i3UIAnimationMgr::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	pXML->addAttr("AnimationVer", "0.0.1");
	SaveFromListToXML(m_vecAnimation, "Animation2DNode", pFile, pXML, true);

	return true;
}

bool i3UIAnimationMgr::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;


	m_Version = pXML->getAttr("AnimationVer");

	LoadFromXMLToList(m_vecAnimation, "Animation2DNode", pFile, pXML, true);

	return true;
}