#pragma once

struct PackScriptUnitResult;

enum DiffResult_PS_Category
{
	DIFFRESULT_PS_LEFT_ONLY,
	DIFFRESULT_PS_RIGHT_ONLY,
	DIFFRESULT_PS_FOUND_LEFT,
	DIFFRESULT_PS_FOUND_RIGHT,
	NUM_DIFFRESULT_PS_CATEGORY,
};


struct RSCFileContentsResult;

struct PSUnitResult_MapResFile
{
	PSUnitResult_MapResFile(){}
	const PackScriptUnitResult*		unit;
	size_t							string_idx;
	bool							file_exist;
};

typedef i3::shared_ptr<PSUnitResult_MapResFile>		PSUnitResult_MapResFilePtr;

struct DiffResult_PackScriptResult
{
	i3::vector<PSUnitResult_MapResFilePtr>	in_out_alist[ NUM_DIFFRESULT_PS_CATEGORY ];
};

struct RSCFileResult;
typedef i3::shared_ptr<struct PackScriptUnitResult>		PackScriptUnitResultPtr;
typedef i3::shared_ptr<struct PackScriptResult>			PackScriptResultPtr;
enum PathMode;

struct RSCFileContentsResult
{
	RSCFileResult*				parent;
	PathMode					path_mode;

	i3::vector<PackScriptResultPtr>	psr_holder_list;	// 보존..(멀티쓰레드 대비)
	PackScriptUnitResultPtr		non_pack_script_holder;			// 생성해서 ps가 아닌 파일들을 몰아 넣는다..

	i3::vector<PSUnitResult_MapResFilePtr>   resFileList;

};

typedef i3::shared_ptr<RSCFileContentsResult>	RSCFileContentsResultPtr;

struct LeftRightResult_PackScriptResult
{
	i3::vector<PSUnitResult_MapResFilePtr>	in_out_result[2];		// L-R
};

