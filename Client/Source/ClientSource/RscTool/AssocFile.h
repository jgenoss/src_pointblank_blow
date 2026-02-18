#pragma once

class PackScriptFile;
struct RscFileInfo;

struct AssocFile
{
	AssocFile(const i3::rc_string& reg_name, 
		const i3::rc_string& reg_value) : reg_name(reg_name), reg_value(reg_value) {}
	
	AssocFile(const i3::rc_string& reg_name, 
			const i3::rc_string& reg_value, 
			const i3::rc_string& file_path) : reg_name(reg_name), reg_value(reg_value), file_path(file_path)
			 {}

	AssocFile(const i3::rc_string& reg_name, 
		const i3::rc_string& reg_value, 
		const i3::rc_string& file_path, 
		const i3::rc_string& midpack_path) : reg_name(reg_name), reg_value(reg_value), file_path(file_path),
											 midpack_path(midpack_path) {} 
	
	i3::rc_string reg_name;
	i3::rc_string reg_value;
	i3::rc_string file_path;
	i3::rc_string midpack_path;
	
	i3::vector<size_t> vec_result_file_index;	// i3::vector<AssocResultFile> 인덱스로 ..
												// 인덱스변경이 없어야한다..

	// 더 추가될수 있음..일단 여기까지..	
};

// 아래 결과물 파일들은 실제 컨텐츠가 존재해야 성립되도록 구성된다..
// 최종 작업 파일을 모아둔다.

struct AssocFileSet;

struct AssocResultFileBase
{
	virtual ~AssocResultFileBase() {}
	AssocResultFileBase() : is_packscript(false), is_midpack(false), is_normal_file(false),
							is_rsc_file(false) {}

	bool				is_packscript, is_midpack, is_normal_file, is_rsc_file;
	i3::vector<size_t>	m_assoc_fileset_index_list;		// 연동용인덱스로 
	
	// assoc_file_list 원소목록이 변경되면 안된다.		
};


struct AssocResultFile_RscFile : AssocResultFileBase
{
	AssocResultFile_RscFile() : packscript_file(NULL) { is_rsc_file = true; }

	i3::rc_string				rsc_file_path;			 //
	const PackScriptFile*		packscript_file;		 // rsc_file에 대한 팩스크립트..
	i3::vector<RscFileInfo*>	rscnode_rel_path_list;	 // 중복제거가 별도 요구됨..
};

// 실제 파일이 존재하지 않으면 생성될 여지가 없다...
struct AssocResultFile_PackScript : AssocResultFileBase
{
	AssocResultFile_PackScript() { is_packscript = true; }
	const PackScriptFile*		packscript_file;
	i3::vector<i3::rc_string>	file_path_list;		// 추가.. 중복제거 요구된다..
};

// AnimPack파일의 경우, 실제 파일 존재 여부를 확인한다..
struct AssocResultFile_AnimPack	: AssocResultFileBase		
{
	AssocResultFile_AnimPack() { is_midpack = true; }
	i3::rc_string		 midpack_path;
};

// Normal File의 경우, 실제 파일 존재 여부를 확인한다.....
struct AssocResultFile_NormalFile : AssocResultFileBase		
{
	AssocResultFile_NormalFile() { is_normal_file = true; }
	i3::rc_string		 file_path;
};

typedef i3::shared_ptr<AssocFile> AssocFilePtr;
typedef i3::shared_ptr<AssocResultFileBase>			AssocResultFileBasePtr;

typedef i3::shared_ptr<AssocResultFile_RscFile>		AssocResultFile_RscFilePtr;
typedef i3::shared_ptr<AssocResultFile_PackScript>	AssocResultFile_PackScriptPtr;
typedef i3::shared_ptr<AssocResultFile_AnimPack>	AssocResultFile_AnimPackPtr;
typedef i3::shared_ptr<AssocResultFile_NormalFile>	AssocResultFile_NormalFilePtr;

struct AssocFileSet
{
	i3::rc_string						item_name;
	i3::vector<AssocFilePtr>			assoc_file_list;
	i3::vector<AssocResultFileBasePtr>	result_file_list;
};

void  MergeAssocFileSetIntoAssocFileSetElement(
	AssocFileSet* owner, AssocFileSet* element, const i3::rc_string& reg_name,
	const i3::rc_string& reg_value);
