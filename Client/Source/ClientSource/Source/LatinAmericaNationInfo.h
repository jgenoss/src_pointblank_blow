#pragma once

#ifndef _LATINAMERICA_NATIONINFO_H_
#define _LATINAMERICA_NATIONINFO_H_

#define GEOIP2_MAX_COUNTRY_COUNT 256

class LatinAmericaNationInfo : public i3::shared_ginst<LatinAmericaNationInfo>
{
public:
	LatinAmericaNationInfo();
	~LatinAmericaNationInfo();

	UINT8 FindNationShapeIdx(UINT8 nationCode);

private:
	void _Initialize();

private:
	i3::vector<UINT8> m_NationShapeIdxList;
};

#endif

