#include "pch.h"
#include "LatinAmericaNationInfo.h"


LatinAmericaNationInfo::LatinAmericaNationInfo()
{
	_Initialize();
}


LatinAmericaNationInfo::~LatinAmericaNationInfo()
{
}

void LatinAmericaNationInfo::_Initialize()
{
	m_NationShapeIdxList.resize(GEOIP2_MAX_COUNTRY_COUNT);

	for (int i = 0; i < GEOIP2_MAX_COUNTRY_COUNT; ++i)
	{
		m_NationShapeIdxList[i] = LATINAMERICA_DEFAULT;
	}

	m_NationShapeIdxList[13] = LATINAMERICA_ARGENTINA;
	m_NationShapeIdxList[30] = LATINAMERICA_BOLIVIA;
	m_NationShapeIdxList[32] = LATINAMERICA_BAHAMAS;
	m_NationShapeIdxList[46] = LATINAMERICA_CHILE;
	m_NationShapeIdxList[49] = LATINAMERICA_COLOMBIA;
	m_NationShapeIdxList[50] = LATINAMERICA_COSTARICA;	
	m_NationShapeIdxList[60] = LATINAMERICA_DOMINICANREPUBLIC;
	m_NationShapeIdxList[62] = LATINAMERICA_ECUADOR;
	m_NationShapeIdxList[80] = LATINAMERICA_FRENCHGUIANA;
	m_NationShapeIdxList[90] = LATINAMERICA_GUATEMALA;
	m_NationShapeIdxList[93] = LATINAMERICA_GUYANA;
	m_NationShapeIdxList[96] = LATINAMERICA_HONDURAS;
	m_NationShapeIdxList[98] = LATINAMERICA_HAITI;
	m_NationShapeIdxList[109] = LATINAMERICA_JAMAICA;
	m_NationShapeIdxList[121] = LATINAMERICA_CAYMANISLANDS;
	m_NationShapeIdxList[152] = LATINAMERICA_MEXICO;
	m_NationShapeIdxList[160] = LATINAMERICA_NICARAGUA;
	m_NationShapeIdxList[168] = LATINAMERICA_PANAMA;
	m_NationShapeIdxList[169] = LATINAMERICA_PERU;
	m_NationShapeIdxList[177] = LATINAMERICA_PUERTORICO;
	m_NationShapeIdxList[181] = LATINAMERICA_PARAGUAY;
	m_NationShapeIdxList[201] = LATINAMERICA_SURINAME;
	m_NationShapeIdxList[218] = LATINAMERICA_TRINIDADTOBAGO;
	m_NationShapeIdxList[226] = LATINAMERICA_URUGUAY;
	m_NationShapeIdxList[230] = LATINAMERICA_VENEZUELA;
}


UINT8 LatinAmericaNationInfo::FindNationShapeIdx(UINT8 nationCode)
{
	if (m_NationShapeIdxList.empty())
		return 0;	// default.

	if (m_NationShapeIdxList.size() <= nationCode)
		return 0;

	return m_NationShapeIdxList[nationCode];
}