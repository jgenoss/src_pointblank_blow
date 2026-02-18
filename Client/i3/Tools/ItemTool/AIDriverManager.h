#pragma once

typedef std::vector<std::string> vStrAIDriveListCon;


class cAIDriverData : public i3ElementBase
{
	I3_CLASS_DEFINE( cAIDriverData, i3ElementBase);

public:
	cAIDriverData(){}
	virtual ~cAIDriverData(){}

	void GetWepaonList(vStrAIDriveListCon& weaponList);
	void GetProcList(vStrAIDriveListCon& procList);

	void SetAIDirverProc(const std::string& strProc);
	void SetAIDriverWeapon(const std::string& strWeapon);
	
	bool FindProc(const std::string& strName);
	bool FindWeapon(const std::string& strName);

	void SetName(const std::string& name) { m_strName = name; }
	std::string GetName() const { return m_strName; }
private:
	typedef std::vector<std::string > vAINameCon;
	vAINameCon m_vProcList;
	vAINameCon m_vWeaponList;
	std::string m_strName;
};

class cAIDriverEnum : public i3ElementBase
{
	I3_CLASS_DEFINE( cAIDriverEnum, i3ElementBase);

public:
	cAIDriverEnum()
		:m_nIndex(0)
	{}
	virtual ~cAIDriverEnum(){}

	int GetIndex() const { return m_nIndex; }
	void SetIndex(int val) { m_nIndex = val; }

	std::string GetAIEnum() const { return m_strAIEnum; }
	void SetAIEnum(const std::string& val) { m_strAIEnum = val; }

	std::string GetAIName() const { return m_strAIName; }
	void SetAIName(const std::string& val) { m_strAIName = val; }

private:
	int			m_nIndex;		//클라이언트 Enum AI 인덱스값
	std::string m_strAIEnum;	//클라이언트 Enum AI 이름
	std::string m_strAIName;	//Enum AI에 해당되는 AI이름
};

typedef std::map<std::string, cAIDriverData* > mAIDriverCon;
typedef std::vector<cAIDriverEnum*> vAIEnumCon;

class cAIDriverManager : public i3ElementBase
{
	I3_CLASS_DEFINE( cAIDriverManager, i3ElementBase);
public:
	cAIDriverManager(void);
	virtual ~cAIDriverManager(void);

private:
	mAIDriverCon	m_mAIDriverCon;
	vAIEnumCon		m_vAIEnum;

public:
	void Init();
	void Clear();
	//void LoadAIDriver();
	//void LoadAIEnum();

	mAIDriverCon*	GetAIDriverCon() { return &m_mAIDriverCon; }

	vAIEnumCon*		GetAIEnum() { return &m_vAIEnum; }

private:
	//WeaponAIDriver.xml
	void SetAIDriverData(TiXmlElement* pElm);
	void AITableList(TiXmlElement* pElm, const std::string& strType);
	void AIWeaponList(TiXmlElement* pElm, const std::string& strType);
	
	//AIDriver이름으로 무기및 테이블을 검색한다.
	bool FindAIDriverToWeapon(const std::string& strType, const std::string& strName);
	bool FindAIDriverToProc(const std::string& strType, const std::string& strName);

	//AIDriverEnum.xml

};
