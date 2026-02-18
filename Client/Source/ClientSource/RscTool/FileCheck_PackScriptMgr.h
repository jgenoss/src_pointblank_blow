#pragma once

// 나중에 기회가 더 될것 같다면 리팩토링하기로 하고..(기존 PackScriptMgr 정비 필요)
// 일단은 파일체킹용으로 매니저를 하나 둔다. (기존 매니저보다는 더 정확한 정보가 필요했음)
  
// 

//#include "PackScriptFileMapElement.h"
//
#include "i3Base/itl/signal/signal.h"

struct PreprocessMessageCallback;

struct PackScriptResult;
struct PackScriptContentsResult;
class PackScriptFile;

struct PackScriptUnitResult
{
	PackScriptContentsResult*	parent;					//
	i3::string					pack_filename;			// file_name만 존재할 것임..
	i3::vector<i3::string>		result_file_list;	
};

typedef i3::shared_ptr<PackScriptUnitResult>		PackScriptUnitResultPtr;

enum PathMode;

struct PackScriptContentsResult
{
	PackScriptResult*					parent;
	PathMode							path_mode;
	i3::vector<PackScriptUnitResultPtr> unit_list;
	PackScriptUnitResultPtr				copy_unit;
};
typedef i3::shared_ptr<PackScriptContentsResult>	PackScriptContentsResultPtr;

#include "i3Base/smart_ptr/enable_shared_from_this.h"

struct PackScriptResult : i3::enable_shared_from_this<PackScriptResult>
{
	i3::wstring							packscript_path;
	PackScriptContentsResultPtr			contents[2];
};

typedef i3::shared_ptr<PackScriptResult>			PackScriptResultPtr;

typedef i3::shared_ptr<struct RSCFileContentsResult>	RSCFileContentsResultPtr;

struct RSCFileResult 
{
	i3::rc_string				rsc_file_path;
	RSCFileContentsResultPtr	contents[2];			// L - R 개념..
};

typedef i3::shared_ptr<RSCFileResult>			RSCFileResultPtr;


struct PackScriptResultMapElement
{
	i3::rc_string						resource_file_path;
	i3::vector<const PackScriptUnitResult*>	unit_result_list_by_path[2];		// L - R 개념..
	i3::vector<size_t>						unit_string_index_list[2];
};

typedef i3::shared_ptr<PackScriptResultMapElement>		PackScriptResultMapElementPtr;

struct RSCFileResultMapElement
{
	i3::vector<const RSCFileContentsResult*>	contents_result_list_by_path[2];		// L-R개념..
	i3::vector<size_t>							resFileListIndex[2];
};

typedef i3::shared_ptr<RSCFileResultMapElement>			RSCFileResultMapElementPtr;

//////////////////////////////////////////////////////////////

void  FindDiffString(const i3::vector<i3::string>& lhs, const i3::vector<i3::string>& rhs, 
					 i3::vector<size_t>& in_out_lhs_only, i3::vector<size_t>& in_out_rhs_only, 
					 i3::vector<size_t>& in_out_found_lhs, i3::vector<size_t>& in_out_found_rhs);


typedef i3::shared_ptr<struct PSUnitResult_MapResFile>		PSUnitResult_MapResFilePtr;

void  FindDiffString(const PackScriptUnitResult* lhs, const PackScriptUnitResult* rhs, 
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_lhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_rhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_lhs_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_rhs_unit);


void  FindDiffString(const i3::vector<PSUnitResult_MapResFilePtr>& unit_lhs, 
					 const i3::vector<PSUnitResult_MapResFilePtr>& unit_rhs, 
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_lhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_rhs_only_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_lhs_unit,
					 i3::vector<PSUnitResult_MapResFilePtr>& in_out_found_rhs_unit);

					


struct DiffResult_PackScriptResult;

bool  FindDiffPackScriptContentsResult(const PackScriptContentsResult* lhs, const PackScriptContentsResult* rhs, 
									  DiffResult_PackScriptResult& in_out_res);

bool  FindDiffPackScriptContentsResult(const RSCFileContentsResult* lhs, const RSCFileContentsResult* rhs, 
									   DiffResult_PackScriptResult& in_out_res);

enum PackMode;

class FileCheck_PackScriptMgr : public i3::shared_ginst<FileCheck_PackScriptMgr>
{
public:
	FileCheck_PackScriptMgr();
	~FileCheck_PackScriptMgr();

	const PackScriptResultMapElement*	FindPackScriptFileMapElement_Upper(const i3::fixed_string& strPath) const;
	const PackScriptResultMapElement*	FindPackScriptFileMapElement(const i3::const_char_range& strPath) const;

	const RSCFileResultMapElement*	FindRSCFileMapElement_Upper(const i3::fixed_string& strPath) const;
	const RSCFileResultMapElement*	FindRSCFileMapElement(const i3::const_char_range& strPath) const;


	const PackScriptResultPtr			FindPackScriptFileResult(const PackScriptFile* f) const;
	
	PackMode							GetCurrPackMode() const { return m_pack_mode; }

	PathMode							GetCurrLeftPathMode() const { return m_l_r_path_mode[0];  }
	PathMode							GetCurrRightPathMode() const { return m_l_r_path_mode[1];  }

	void		BuildPackScriptMap(PackMode m, PreprocessMessageCallback* cb);
	void		ClearPackScriptMap();

	
	void					Update_Progress();
	
	i3::signal<void ()>*	GetProgressSignal() { return &m_progress_cb_signal;  }
	
	const i3::vector<RSCFileResultPtr>&		GetRSCFileResultList() const { return m_rsc_result_list; }

private:
	void				FillFileNameResultMapImpl( const PackScriptUnitResult* unit, int l_r );
	
	void				AddRSCFileResult( const i3::rc_string& rel_rscfile_path );

	RSCFileResultPtr	CreateRSCFileResult( const i3::rc_string& rel_rscfile_path) const;
	
	PackMode															m_pack_mode;
	PathMode															m_l_r_path_mode[2];
	i3::unordered_map<const PackScriptFile*, PackScriptResultPtr>		m_psf_result_map;	// 중복이 없을 것이고, 항상 유효..

	i3::unordered_map<i3::fixed_string, PackScriptResultMapElementPtr>	m_filename_result_map;	// 중복 있을 것임..
	
	i3::unordered_map<i3::fixed_string, RSCFileResultMapElementPtr>		m_filename_rsc_result_map;	// 중복이 있을지 모르겠으나, 일단 단일로 처리.

	i3::vector<RSCFileResultPtr>										m_rsc_result_list;		// 이것도 빌드시에 직접 만든다..
	
	
	i3::signal<void ()>													m_progress_cb_signal;

};

