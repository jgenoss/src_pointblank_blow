#pragma once
#include <comdef.h>
#include <WbemIdl.h>
#pragma comment(lib, "wbemuuid.lib")

enum ChassisTypes
{
	enum_Other = 1,
	enum_Unknown,
	enum_Desktop,
	enum_LowProfileDesktop,
	enum_PizzaBox,
	enum_MiniTower,
	enum_Tower,
	enum_Portable,
	enum_Laptop,
	enum_Notebook,
	enum_Handheld,
	enum_DockingStation,
	enum_AllInOne,
	enum_SubNotebook,
	enum_SpaceSaving,
	enum_LunchBox,
	enum_MainSystemChassis,
	enum_ExpansionChassis,
	enum_SubChassis,
	enum_BusExpansionChassis,
	enum_PeripheralChassis,
	enum_StorageChassis,
	enum_RackMountChassis,
	enum_SealedCasePC
};

class i3Wmi
{
private:

	IWbemLocator *m_pLoc = nullptr;
	IWbemServices *m_pSvc = nullptr;
	IEnumWbemClassObject *m_pEnum = nullptr;

	void WmiComInit();
	void WmiComSecurity();
	bool WmiComcreate();
	bool WmiConnect();
	bool WmiComProxy();

	ChassisTypes m_MachineType;
	bool		 m_bOK;
public:
	i3Wmi();
	~i3Wmi();
	int WmiComExcute(bstr_t query, const wchar_t* object);
	const i3::rc_string GetMachineName();
};

