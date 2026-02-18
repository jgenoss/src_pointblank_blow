#pragma once


struct PackPathUnit
{
	i3::wstring PathName;
	i3::wstring PackScriptPath;
	i3::wstring MediaPath;
	i3::wstring QAPath;
	i3::wstring LivePath;
	i3::wstring Nation;

	PackPathUnit& operator=(const PackPathUnit& rhs);
};

bool operator==(const PackPathUnit& lhs, const PackPathUnit& rhs);
bool operator!=(const PackPathUnit& lhs, const PackPathUnit& rhs);

void ResetPackPathUnit(PackPathUnit& out);

typedef i3::shared_ptr<PackPathUnit>	PackPathUnitPtr;

struct PackPathFile
{
	i3::vector<PackPathUnitPtr>	unit_list;
	i3::uint16_t				ver_major;
	i3::uint16_t				ver_minor;
};

// 텍스트 형식도 생각해봤지만..그냥 바이너리로 편하게 저장한다..
// 툴에서는 중요해도, 텍스트 편집으로는 필요성이 적을 것이다..

bool	LoadPackPathFile(const i3::wstring& path, PackPathFile& res);
bool	SavePackPathFile(const i3::wstring& path, const PackPathFile& ppf);

void	InitPackPathFile_2_0(PackPathFile& ppf);

bool	LoadOrCreatePackPathFile(const i3::wstring& path, PackPathFile& res);

