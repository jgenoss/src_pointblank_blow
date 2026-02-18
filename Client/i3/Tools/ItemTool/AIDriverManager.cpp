#include "StdAfx.h"
#include "AIDriverManager.h"

//class cAIDriverData
I3_CLASS_INSTANCE( cAIDriverData); //, i3ElementBase);

void cAIDriverData::GetWepaonList(std::vector<std::string>& weaponList)
{
	weaponList.resize(m_vWeaponList.size());

	for (size_t ui = 0; ui < m_vWeaponList.size(); ++ui)
	{
		weaponList[ui] = m_vWeaponList[ui];
	}
}

void cAIDriverData::GetProcList(std::vector<std::string>& procList)
{
	procList.resize(m_vProcList.size());

	for (size_t ui = 0; ui < m_vProcList.size(); ++ui)
	{
		procList[ui] = m_vProcList[ui];
	}
}

void cAIDriverData::SetAIDirverProc(const std::string& strProc)
{
	m_vProcList.push_back(strProc);
}

void cAIDriverData::SetAIDriverWeapon(const std::string& strWeapon)
{
	m_vWeaponList.push_back(strWeapon);
}

bool cAIDriverData::FindProc(const std::string& strName)
{
	for each(std::string p in m_vProcList)
	{
		if (p == strName)
			return true;
	}

	return false;
}

bool cAIDriverData::FindWeapon(const std::string& strName)
{
	for each(std::string p in m_vWeaponList)
	{
		if (p == strName)
			return true;
	}

	return false;
}

//class cAIDriverEnum
I3_CLASS_INSTANCE( cAIDriverEnum); //, i3ElementBase);

//class cAIDriverManager
I3_CLASS_INSTANCE( cAIDriverManager); //, i3ElementBase);

cAIDriverManager::cAIDriverManager(void)
{
}

cAIDriverManager::~cAIDriverManager(void)
{
	Clear();
}


void cAIDriverManager::Init()
{
	if (g_strCurrentDir.IsEmpty())
		return;

	//std::string strPath = g_strRootPath + "\\config\\WeaponAIDriver.xml";
	//LoadAIDriver();
	//LoadAIEnum();
	
}

void cAIDriverManager::Clear()
{
	mAIDriverCon::iterator iterAIDriver = m_mAIDriverCon.begin();
	for (; iterAIDriver != m_mAIDriverCon.end(); ++iterAIDriver)
	{
		i3::destroy_instance(iterAIDriver->second);		// I3_SAFE_DELETE(iterAIDriver->second);
	}
	m_mAIDriverCon.clear();

	vAIEnumCon::iterator iterAIEnum = m_vAIEnum.begin();
	for (; iterAIEnum != m_vAIEnum.end(); ++iterAIEnum)
	{
		i3::destroy_instance(*iterAIEnum); 			// I3_SAFE_DELETE(*iterAIEnum);
	}
	m_vAIEnum.clear();
}

//void cAIDriverManager::LoadAIDriver()
//{
//	////std::string strPath = g_strCurrentDir + "\\Data\\WeaponAIDriver.xml";
//	//std::string strPath = g_strRootPath + "\\config\\AIDriverWeapon.xml";
//
//	//TiXmlDocument doc(strPath.c_str());
//
//	//bool rt = doc.LoadFile();
//	//I3ASSERT(rt);
//
//	//TiXmlElement* pRoot = doc.FirstChildElement("AIDriver");
//	//I3ASSERT(pRoot);
//
//	//TiXmlElement* pElm = pRoot->FirstChildElement();
//
//	//while(pElm != NULL)
//	//{
//	//	SetAIDriverData(pElm);
//
//	//	pElm = pElm->NextSiblingElement();
//	//}
//}


void cAIDriverManager::SetAIDriverData(TiXmlElement* pElm)
{
	std::string strAIType = pElm->Value();

	TiXmlElement* pChildElm = pElm->FirstChildElement();
	while(pChildElm != NULL)
	{
		mAIDriverCon::iterator iter = m_mAIDriverCon.find(strAIType);
		
		if (iter == m_mAIDriverCon.end())
		{
			cAIDriverData* pAIDriver = cAIDriverData::new_object();
			m_mAIDriverCon.insert(std::make_pair(strAIType, pAIDriver ));
		}

		std::string strChildType = pChildElm->Value();

		if (strChildType == "Table")
		{
			AITableList(pChildElm, strAIType);
		}
		else if (strChildType == "Weapon")
		{
			AIWeaponList(pChildElm, strAIType);
		}
		else
		{
			ASSERT(0);
		}
		
		pChildElm = pChildElm->NextSiblingElement();
	}
}

void cAIDriverManager::AITableList(TiXmlElement* pElm, const std::string& strType)
{
	TiXmlElement* pChildElm = pElm->FirstChildElement();

	cAIDriverData* pAIDriver = m_mAIDriverCon[strType];
	if(NULL == pAIDriver)
	{
		ASSERT(0);
		return;
	}

	while(pChildElm != NULL)
	{
		std::string strChildType = pChildElm->Value();
		
		if (strChildType == "AIPROC")
		{
			std::string strElmName = pChildElm->Attribute("name");
			pAIDriver->SetAIDirverProc(strElmName);
		}
		else
			ASSERT(0);

		pChildElm = pChildElm->NextSiblingElement();
	}
}

void cAIDriverManager::AIWeaponList(TiXmlElement* pElm, const std::string& strType)
{
	cAIDriverData* pAIDriver = m_mAIDriverCon[strType];
	if(NULL == pAIDriver)
	{
		ASSERT(0);
		return;
	}

	std::string strElmName = pElm->Attribute("name");

	if (false == FindAIDriverToWeapon(strType, strElmName))
		pAIDriver->SetAIDriverWeapon(strElmName);
}

bool cAIDriverManager::FindAIDriverToWeapon(const std::string& strType, const std::string& strName)
{
	cAIDriverData* pAIDriver = m_mAIDriverCon[strType];
	
	if (NULL == pAIDriver)	
		return false;

	return pAIDriver->FindWeapon(strName);
}

bool cAIDriverManager::FindAIDriverToProc(const std::string& strType, const std::string& strName)
{
	cAIDriverData* pAIDriver = m_mAIDriverCon[strType];

	if (NULL == pAIDriver)	
		return false;

	return pAIDriver->FindProc(strName);
}


//void cAIDriverManager::LoadAIEnum()
//{
//	////std::string strPath = g_strCurrentDir + "\\Data\\AIDriverEnum.xml";
//	//std::string strPath = g_strRootPath + "\\config\\AIDriverEnum.xml";
//
//	//TiXmlDocument doc(strPath.c_str());
//
//	//bool rt = doc.LoadFile();
//	//I3ASSERT(rt);
//
//	//TiXmlElement* pRoot = doc.FirstChildElement("AIDriverEnum");
//	//I3ASSERT(pRoot);
//
//	//TiXmlElement* pElm = pRoot->FirstChildElement();
//
//	//while(pElm != NULL)
//	//{
//	//	std::string strElmName = pElm->Attribute("name");
//	//	int nIndex;
//	//	pElm->QueryIntAttribute("index", &nIndex);
//
//	//	cAIDriverEnum* pAIEnum = cAIDriverEnum::new_object();
//	//	pAIEnum->SetIndex(nIndex);
//	//	pAIEnum->SetAIEnum(pElm->Value());
//	//	pAIEnum->SetAIName(strElmName);
//
//	//	m_vAIEnum.push_back(pAIEnum);
//
//	//	pElm = pElm->NextSiblingElement();
//	//}
//}