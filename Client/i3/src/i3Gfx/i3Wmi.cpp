#include "i3GfxType.h"
#include "i3Wmi.h"


i3Wmi::i3Wmi()
{
	WmiComInit();
	WmiComSecurity();
	bool bCreate = WmiComcreate();
	bool bConnect = WmiConnect();
	bool bProxy = WmiComProxy();

	m_MachineType = enum_Other;

	if (!bCreate
		|| !bConnect
		|| !bProxy)
	{
		m_bOK = false;
	}
	else
	{
		m_bOK = true;
	}
}


i3Wmi::~i3Wmi()
{
	if (m_pSvc) m_pSvc->Release();
	if (m_pLoc) m_pLoc->Release();
	if (m_pEnum) m_pEnum->Release();

	CoUninitialize();
}
void i3Wmi::WmiComInit()
{
	HRESULT hRes = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hRes))
	{
		I3PRINTLOG(I3LOG_NOTICE, "WMI CoInitializeEx it's already initialized");
	}
}
void i3Wmi::WmiComSecurity()
{
	HRESULT hRes = ::CoInitializeSecurity(
		NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL);

	if (FAILED(hRes))
	{
		I3PRINTLOG(I3LOG_NOTICE, "WMI CoInitializeSecurity it's already initialized");
	}
}
bool  i3Wmi::WmiComcreate()
{
	m_pLoc = nullptr;
	HRESULT hRes = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&m_pLoc);

	if (FAILED(hRes))
	{
		I3PRINTLOG(I3LOG_NOTICE, "WMI CoCreateInstance Err");
		return false;
	}

	return true;
}
bool i3Wmi::WmiConnect()
{
	m_pSvc = nullptr;

	if (m_pLoc != nullptr)
	{
		HRESULT hRes = m_pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &m_pSvc);

		if (FAILED(hRes))
		{
			I3PRINTLOG(I3LOG_NOTICE, "WMI ConnectServer Err");
			return false;
		}

		return true;
	}

	return false;
}

bool i3Wmi::WmiComProxy()
{
	HRESULT hRes = CoSetProxyBlanket(m_pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL, EOAC_NONE);

	if (FAILED(hRes))
	{
		I3PRINTLOG(I3LOG_NOTICE, "WMI CoSetProxyBlanket Err");
		return false;
	}

	return true;
}

int i3Wmi::WmiComExcute(bstr_t query, const wchar_t* object)
{
	if (m_bOK)
	{
		m_pEnum = nullptr;
		HRESULT hRes = m_pSvc->ExecQuery(bstr_t("WQL"), query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL, &m_pEnum);

		if (FAILED(hRes))
		{
			I3PRINTLOG(I3LOG_NOTICE, "WMI ExecQuery err\n");
			return 1;
		}

		IWbemClassObject* pclsObj = nullptr;
		ULONG uRet = 0;
		while (m_pEnum)
		{
			HRESULT hr = m_pEnum->Next(WBEM_INFINITE, 1, &pclsObj, &uRet);
			if (0 == uRet)
				break;

			VARIANT vtProp;

			hr = pclsObj->Get(object, 0, &vtProp, 0, 0);

			if (!FAILED(hr))
			{
				if ((vtProp.vt == VT_NULL) || (vtProp.vt == VT_EMPTY))
				{
					I3PRINTLOG(I3LOG_NOTICE, "WMI ChassisType : nullptr & EMPTY");
				}
				else
				{
					if ((vtProp.vt & VT_ARRAY))
					{

						long lLower, lUpper;
						UINT32 Element = 0;
						SAFEARRAY *pSafeArray = vtProp.parray;
						SafeArrayGetLBound(pSafeArray, 1, &lLower);
						SafeArrayGetUBound(pSafeArray, 1, &lUpper);

						for (long i = lLower; i <= lUpper; i++)
						{
							hRes = SafeArrayGetElement(pSafeArray, &i, &Element);
						}

						m_MachineType = (ChassisTypes)Element;

						SafeArrayDestroy(pSafeArray);
					}
				}

				pclsObj->Release();
				pclsObj = nullptr;
			}
			else
				pclsObj->Release();
		}
	}

	return 0;
}

const i3::rc_string i3Wmi::GetMachineName()
{
	i3::rc_string _type;
	switch (m_MachineType)
	{
	case enum_Other:
		_type = "enum_Other";
		break;
	case enum_Unknown:
		_type = "enum_Unknown";
		break;
	case enum_Desktop:
		_type = "enum_Desktop";
		break;
	case enum_LowProfileDesktop:
		_type = "enum_LowProfileDesktop";
		break;
	case enum_PizzaBox:
		_type = "enum_PizzaBox";
		break;
	case enum_MiniTower:
		_type = "enum_MiniTower";
		break;
	case enum_Tower:
		_type = "enum_Tower";
		break;
	case enum_Portable:
		_type = "enum_Portable";
		break;
	case enum_Laptop:
		_type = "enum_Laptop";
		break;
	case enum_Notebook:
		_type = "enum_Notebook";
		break;
	case enum_Handheld:
		_type = "enum_Handheld";
		break;
	case enum_DockingStation:
		_type = "enum_DockingStation";
		break;
	case enum_AllInOne:
		_type = "enum_AllInOne";
		break;
	case enum_SubNotebook:
		_type = "enum_SubNotebook";
		break;
	case enum_SpaceSaving:
		_type = "enum_SpaceSaving";
		break;
	case enum_LunchBox:
		_type = "enum_LunchBox";
		break;
	case enum_MainSystemChassis:
		_type = "enum_MainSystemChassis";
		break;
	case enum_ExpansionChassis:
		_type = "enum_ExpansionChassis";
		break;
	case enum_SubChassis:
		_type = "enum_SubChassis";
		break;
	case enum_BusExpansionChassis:
		_type = "enum_BusExpansionChassis";
		break;
	case enum_PeripheralChassis:
		_type = "enum_PeripheralChassis";
		break;
	case enum_StorageChassis:
		_type = "enum_StorageChassis";
		break;
	case enum_RackMountChassis:
		_type = "enum_RackMountChassis";
		break;
	case enum_SealedCasePC:
		_type = "enum_SealedCasePC";
		break;
	}

	return _type;
}