#pragma once

//
// rc_wstring타입을 전역 정적 생성 후에 크래시 문제가 생겨서...(이건 나중에 고치고)
// wstring타입으로 변경..
// 
#include "PackPath.h"
#include "PackScriptContents.h"

namespace
{
	struct init_i3string
	{
		init_i3string()
		{
			i3::force_initialize_i3string(i3::identity<i3::string>());
		}
	} _init_i3string_;
}


struct PreprocessMessageCallback;


class PathMgr : public i3::shared_ginst<PathMgr>
{
public:	
	PathMgr();
	~PathMgr();
	
	void	Initialize();
	void	SavePathFile();
	void	SaveCurrExceptionFile();

	void				SelectPathName(int idx);
	bool				PreprocessForItemView(PreprocessMessageCallback* cb);
	bool				IsPreprocessNeededForItemView() const { return m_bItemViewPreprocessNeed; }

	const i3::wstring&  GetPathName() const     { return m_unit.PathName; }
	const i3::wstring&	GetMainMediaPath() const { return m_unit.MediaPath; }
	const i3::wstring&	GetPackScriptPath() const { return m_unit.PackScriptPath; }
	const i3::wstring&	GetLivePath() const { return m_unit.LivePath; }
	const i3::wstring&  GetQAPath() const		{ return m_unit.QAPath; }
	const i3::wstring&  GetNation() const		{ return m_unit.Nation; }
	int					GetNationIndex() const  { return m_curr_nation_idx; }
	int					GetNumNationIndex() const { return m_num_nation; }

	bool				IsMediaPathExist() const;
	bool				IsQAPathExist() const;
	bool				IsLivePathExist() const;
	bool				IsPackScriptPathExist() const;
	bool				IsMediaNationFileExist() const;
	
	bool				FindMediaNationFileFullPath(i3::wstring& full_path) const;
	
	PackPathFile*		GetPackPathFile() { return &m_PackPathFile; }

	bool				IsCurrPathName(int idx) const;
	bool				IsEqualToCurrPathUnit(int idx) const;
	
	const PackScriptExceptionString&			GetExceptionString() const { return m_exception_string; }
	const PackScriptExceptionFilterList&	GetExceptionList() const { return m_exception_filterlist; }

	void				SetExceptionString(const PackScriptExceptionString& str);
	const i3::vector<i3::string>&	GetNationFolderFilterList() const { return m_nation_folder_filter_list; }

	const PackPathUnit&	GetCurrPathUnit() const { return m_unit; }
	
	void				SetExportRootPath(const i3::wstring& wstrPath) { m_wstrExportRootPath = wstrPath; }
	const i3::wstring&	GetExportRootPath() const { return m_wstrExportRootPath; }


private:

	PackPathFile	m_PackPathFile;

	PackPathUnit	m_unit;				// 값으로 처리한다..
	PackScriptExceptionBuffer	m_exception_buffer;

	PackScriptExceptionString	m_exception_string;

	PackScriptExceptionFilterList	m_exception_filterlist;
//	i3::vector<i3::string>		m_exception_list;
	
	INT32						m_curr_nation_idx;
	INT32						m_num_nation;
	i3::vector<i3::string>		m_nation_folder_filter_list;
	bool						m_bItemViewPreprocessNeed;
	
	i3::wstring				m_wstrExportRootPath;
};

