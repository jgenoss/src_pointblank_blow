#include "stdafx.h"
#include "PackPath.h"
#include "PackScriptContents.h"

namespace
{
	//
	template<class T>
	void PushScalarToBuff(T val, i3::vector<char>& buff)
	{
		size_t idx = buff.size();
		buff.resize(idx + sizeof(T));
		::memcpy( &buff[idx], &val, sizeof(T) );
	}
		
	void PushStringToBuff(const i3::const_wchar_range& str, i3::vector<char>& buff)
	{
		const i3::int32_t idx = i3::int32_t(buff.size());
		const i3::int32_t str_size =  str.size();
		buff.resize( idx + sizeof(i3::int32_t) + sizeof(wchar_t) * str_size);
	
		::memcpy( &buff[idx], &str_size, sizeof(i3::int32_t) );
		::memcpy( &buff[idx + sizeof(i3::int32_t)], &str[0], sizeof(wchar_t) * str_size );
	}
	
	template<class T>
	T PopScalarFromBuff(i3::identity<T>, char*& buff)
	{
		char* curr = buff;		buff += sizeof(T);
		return T(*reinterpret_cast<T*>(curr));
	}
	
	template<class Ctn>
	void PopStringFromBuff( char*& buff, Ctn& ctn)
	{
		i3::int32_t size = *reinterpret_cast<i3::int32_t*>(buff);	buff += sizeof(i3::int32_t);
		ctn.assign( reinterpret_cast<wchar_t*>(buff), size);
		buff += size * sizeof(wchar_t);
	}
	
}

PackPathUnit& PackPathUnit::operator=(const PackPathUnit& rhs) 
{
	PathName = rhs.PathName;
	PackScriptPath = rhs.PackScriptPath;
	MediaPath = rhs.MediaPath;
	QAPath = rhs.QAPath;
	LivePath = rhs.LivePath;
	Nation   = rhs.Nation;		// 2.0이하의 경우 빈문자열로 될것..
	return *this;
}

void ResetPackPathUnit(PackPathUnit& out)
{
	out.PathName.clear();
	out.PackScriptPath.clear();
	out.MediaPath.clear();
	out.QAPath.clear();
	out.LivePath.clear();
	out.Nation.clear();
}

bool operator==(const PackPathUnit& lhs, const PackPathUnit& rhs)
{
	return lhs.PathName == rhs.PathName &&
		   lhs.PackScriptPath == rhs.PackScriptPath &&
		   lhs.MediaPath == rhs.MediaPath &&
		   lhs.QAPath == rhs.QAPath &&
		   lhs.LivePath == rhs.LivePath && 
		   lhs.Nation == rhs.Nation;
}

bool operator!=(const PackPathUnit& lhs, const PackPathUnit& rhs)
{
	return !(lhs == rhs);
}


bool	LoadPackPathFile(const i3::wstring& path, PackPathFile& res)
{
	res.unit_list.clear();

	i3::vector<char> buff;
	
	if ( !LoadFileIntoBuffer(path, buff) ) 
		return false;
	
	char* pos = &buff[0];
	char* end = pos + buff.size();
	
	i3::wstring file_format;
	PopStringFromBuff(pos, file_format);
	if ( file_format != L"PackPathFile" )
		return false;
	
	res.ver_major = PopScalarFromBuff(i3::identity<i3::uint16_t>(), pos);
	res.ver_minor = PopScalarFromBuff(i3::identity<i3::uint16_t>(), pos);
	
	const i3::int32_t num_unit = PopScalarFromBuff(i3::identity<i3::int32_t>(), pos);
	
	bool bNationInclude = ( res.ver_major > 1 );

	for (i3::int32_t i = 0 ; i < num_unit ; ++i )
	{
		PackPathUnitPtr new_unit = i3::make_shared<PackPathUnit>();
		res.unit_list.push_back(new_unit);

		PopStringFromBuff(pos, new_unit->PathName);
		PopStringFromBuff(pos, new_unit->PackScriptPath);
		PopStringFromBuff(pos, new_unit->MediaPath);
		PopStringFromBuff(pos, new_unit->QAPath);
		PopStringFromBuff(pos, new_unit->LivePath);
		
		if (bNationInclude)
			PopStringFromBuff(pos, new_unit->Nation);
	}
		
	return true;
}

bool	SavePackPathFile(const i3::wstring& path, const PackPathFile& ppf)
{
	i3::vector<char> buff;
	
	const i3::int32_t num_unit = ppf.unit_list.size();
	
	PushStringToBuff(i3::as_literal(L"PackPathFile"), buff);

	i3::uint16_t major = 2;	i3::uint16_t minor = 0;

	PushScalarToBuff( major, buff);
	PushScalarToBuff( minor, buff);

	PushScalarToBuff( num_unit, buff);
	
	for (i3::int32_t i = 0; i < num_unit ; ++i )
	{
		const PackPathUnit* pUnit = ppf.unit_list[i].get();
		PushStringToBuff(i3::as_literal(pUnit->PathName), buff);
		PushStringToBuff(i3::as_literal(pUnit->PackScriptPath), buff);
		PushStringToBuff(i3::as_literal(pUnit->MediaPath), buff);
		PushStringToBuff(i3::as_literal(pUnit->QAPath), buff);
		PushStringToBuff(i3::as_literal(pUnit->LivePath), buff);
		
		PushStringToBuff(i3::as_literal(pUnit->Nation), buff);	// 2.0
	}
		
	if (!SaveFileFromBuffer(buff, path))
		return false;
	return true;
}

void	InitPackPathFile_2_0(PackPathFile& ppf)
{
	ppf.unit_list.clear();
	ppf.ver_major = 2;	ppf.ver_minor = 0;
}

bool	LoadOrCreatePackPathFile(const i3::wstring& path, PackPathFile& res)
{
	if ( LoadPackPathFile(path, res) )
		return true;
	
	InitPackPathFile_2_0(res);

	return SavePackPathFile(path, res);
}

