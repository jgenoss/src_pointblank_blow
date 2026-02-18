#include "i3FrameworkPCH.h"
#include "i3UIAnimation2DSprite.h"
#include "i3UI.h"

I3_CLASS_INSTANCE(i3UIAnimation2DSprite);


i3UIAnimation2DSprite::~i3UIAnimation2DSprite()
{
	I3_SAFE_DELETE_ARRAY(m_pUVInfo);
}

void i3UIAnimation2DSprite::CopyTo(i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3NamedElement::CopyTo(pDest, method);

	i3UIAnimation2DSprite* pAnimation = (i3UIAnimation2DSprite*)pDest;

	pAnimation->SetName(GetName());
	pAnimation->SetLifeTime(GetLifeTime());
	pAnimation->SetSpriteCount(GetSpriteCount());
	pAnimation->CreateSprite(GetSpriteCount());

	for (INT32 i = 0; i < GetSpriteCount(); i++)
	{
		pAnimation->SetUVInfo(i, GetUVRes(i), GetUVPos(i));
	}
}
i3XMLElement* i3UIAnimation2DSprite::newXMLElement(const char* pszName, i3XMLElement * pXML)
{
	i3XMLElement* pChild = new i3XMLElement(pszName);
	pChild = (i3XMLElement*)pXML->LinkEndChild(pChild);
	I3ASSERT(pChild != nullptr);

	return pChild;
}

i3XMLElement* i3UIAnimation2DSprite::GetFromXML(const char * pszName, i3XMLElement * pXML)
{
	i3XMLElement * pChild;

	pChild = (i3XMLElement *)pXML->FirstChild(pszName);
	if (pChild == nullptr)
	{
		I3ERRLOG(i3Error::_INVALIDFORMAT);
	}

	return pChild;
}
bool i3UIAnimation2DSprite::OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnSaveXML(pFile, pXML);
	if (bRv == false)
		return false;

	i3XMLElement * pChild = newXMLElement("node_Info", pXML);
	pChild->addAttr("LifeTime", m_fLifeTime);
	pChild->addAttr("SpriteCount", m_nSpriteCount);

	i3::stack_string conv;
	for (INT32 i = 0; i < m_nSpriteCount; i++)
	{
		i3::sprintf(conv, "sprite_%d", i);
		pChild = newXMLElement(conv.c_str(), pXML);

		pChild->addAttr("x", m_pUVInfo[i]._Rect.left);
		pChild->addAttr("y", m_pUVInfo[i]._Rect.top);
		pChild->addAttr("width", m_pUVInfo[i]._Rect.right);
		pChild->addAttr("height", m_pUVInfo[i]._Rect.bottom);
		pChild->addAttr("resName", m_pUVInfo[i]._ResName);
	}

	return true;
}

bool i3UIAnimation2DSprite::OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML(pFile, pXML);
	if (bRv == false)
		return false;

	i3XMLElement * pChild = GetFromXML("node_Info", pXML);
	pChild->getAttr("LifeTime", &m_fLifeTime);
	pChild->getAttr("SpriteCount", &m_nSpriteCount);

	m_pUVInfo = new I3_UVINFO[m_nSpriteCount];

	INT32 temp;
	i3::string res;
	i3::stack_string conv;
	for (INT32 i = 0; i < m_nSpriteCount; i++)
	{
		i3::sprintf(conv, "sprite_%d", i);
		pChild = GetFromXML(conv.c_str(), pXML);

		pChild->getAttr("x", &temp);
		m_pUVInfo[i]._Rect.left = temp;

		pChild->getAttr("y", &temp);
		m_pUVInfo[i]._Rect.top = temp;

		pChild->getAttr("width", &temp);
		m_pUVInfo[i]._Rect.right = temp;

		pChild->getAttr("height", &temp);
		m_pUVInfo[i]._Rect.bottom = temp;

		res = pChild->getAttr("resName");
		i3::sprintf(m_pUVInfo[i]._ResName, "%s", res.c_str());
	}

	return true;
}

void i3UIAnimation2DSprite::CreateSprite(INT32 nCount)
{
	if (m_pUVInfo == nullptr)
		m_pUVInfo = new I3_UVINFO[nCount];
}

void i3UIAnimation2DSprite::SetUVInfo(INT32 nIndex, i3::string resName, RECT rt)
{ 
	sprintf_s(m_pUVInfo[nIndex]._ResName, "%s", resName.c_str()); 
	m_pUVInfo[nIndex]._Rect = rt;
}

void i3UIAnimation2DSprite::Init(i3UIShape* pShape, i3UIAnimation2DSprite* p2dNode)
{
	SetRoof(pShape->IsAnimationRoof());
	p2dNode->CopyTo(this, I3_COPY_INSTANCE);

	I3_UVINFO pinfo = m_pUVInfo[0];
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	I3VT_TEXINFO *pTexInfo = pVTex->FindRefTexture(pinfo._ResName);

	if (pTexInfo != nullptr)
	{
		m_VTexOffset.x = pTexInfo->m_X;
		m_VTexOffset.y = pTexInfo->m_Y;

		m_TexCoord.left = (REAL32)pinfo._Rect.left;
		m_TexCoord.top = (REAL32)pinfo._Rect.top;
		m_TexCoord.right = (REAL32)pinfo._Rect.right;
		m_TexCoord.bottom = (REAL32)pinfo._Rect.bottom;
	}
}

void i3UIAnimation2DSprite::Drive(REAL32 rDeltaSeconds)
{
	REAL32 fOneFrameDelta = m_fLifeTime / m_nSpriteCount;
	if (m_fTimeDelta >= fOneFrameDelta && !m_bDisable)
	{
		m_fTimeDelta = 0;

		if (m_nCurSpriteIndex >= m_nSpriteCount)
		{
			m_nCurSpriteIndex = 0;

			if (m_bRoof == false)
				m_bDisable = true;
		}

		i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

		I3_UVINFO pinfo = m_pUVInfo[m_nCurSpriteIndex++];
		I3VT_TEXINFO *pTexInfo = pVTex->FindRefTexture(pinfo._ResName);

		if (pTexInfo != nullptr)
		{
			m_VTexOffset.x = pTexInfo->m_X;
			m_VTexOffset.y =	pTexInfo->m_Y;

			m_TexCoord.left = (REAL32)pinfo._Rect.left;
			m_TexCoord.top = (REAL32)pinfo._Rect.top;
			m_TexCoord.right = (REAL32)pinfo._Rect.right;
			m_TexCoord.bottom = (REAL32)pinfo._Rect.bottom;
		}
	}
	m_fTimeDelta += rDeltaSeconds;
}

void i3UIAnimation2DSprite::Reset()
{
	m_fTimeDelta = 0.0f;
	m_nCurSpriteIndex = 0;
}
