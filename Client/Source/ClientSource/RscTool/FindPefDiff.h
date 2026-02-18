#pragma once


// lhs --> old   , rhs --> new ..
#include "i3Base/itl/function/function.h"

struct PefDiffRegData;
struct PefDiffSet;
struct PefDiffRegData_Visitor;
class i3RegData;

namespace Detail 
{
	void FindRecursivePefDiff(i3RegKey* , i3RegKey* , PefDiffSet&); 
}

//
// i3::function / i3::bind의 Closure특성(개별 인수 타입을 제거하고, 공통함수화되는 방식)을 이용...
//

struct PefDiffRegData_Visitor
{
 	virtual void SameType(const PefDiffRegData* a, INT32* val1, INT32* val2) {}
	virtual void SameType(const PefDiffRegData* a, RT_REAL32* val1, RT_REAL32* val2) {}
	virtual void SameType(const PefDiffRegData* a, const i3::rc_wstring& val1, const i3::rc_wstring& val2) {}
	virtual void SameType(const PefDiffRegData* a, RT_VEC2D* val1, RT_VEC2D* val2) {}
	virtual void SameType(const PefDiffRegData* a, RT_VEC3D* val1, RT_VEC3D* val2) {}
	virtual void SameType(const PefDiffRegData* a, RT_VEC4D* val1, RT_VEC4D* val2) {}
	virtual void SameType(const PefDiffRegData* a, I3COLOR* val1, I3COLOR* val2) {}
	virtual void SameType(const PefDiffRegData* a, RT_MATRIX* val1, RT_MATRIX* val2) {}

	virtual void DiffType(const PefDiffRegData* a, INT32* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, RT_REAL32* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, const i3::rc_wstring& val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, RT_VEC2D* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, RT_VEC3D* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, RT_VEC4D* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, I3COLOR* val, INT32 lhs_rhs) {}
	virtual void DiffType(const PefDiffRegData* a, RT_MATRIX* val, INT32 lhs_rhs) {}

	virtual void PartialType(const PefDiffRegData* a, INT32* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, RT_REAL32* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, const i3::rc_wstring& val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, RT_VEC2D* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, RT_VEC3D* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, RT_VEC4D* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, I3COLOR* val, INT32 lhs_rhs) {}
	virtual void PartialType(const PefDiffRegData* a, RT_MATRIX* val, INT32 lhs_rhs) {}

	virtual void PartialKey( i3RegKey* key, INT32 lhs_rhs) {}
//	
	virtual ~PefDiffRegData_Visitor() {}
};

struct PefDiffRegData
{
	i3RegKey*  lhs_key, * rhs_key;	// 
	i3RegData* lhs, * rhs;			// 둘중한쪽이 NULL인 경우, 양쪽이 모두 유효값인 경우..
};


class PefDiffRegData_Accept
{
public:
	PefDiffRegData_Accept(i3RegKey* lhs_key, i3RegKey* rhs_key, i3RegData* lhs_data, i3RegData* rhs_data);
	void visit(PefDiffRegData_Visitor* v);
	~PefDiffRegData_Accept() {}

private:
	typedef i3::function<void (PefDiffRegData_Visitor*, const PefDiffRegData*) >	function_type;
	typedef function_type			fun_t;
	void move_function(function_type& f, INT32 lhs_rhs = 0);
	friend void Detail::FindRecursivePefDiff(i3RegKey* , i3RegKey* , PefDiffSet& );

	PefDiffRegData	m_data;
	function_type	m_visit_mem_fun[2];
};

typedef i3::shared_ptr<PefDiffRegData_Accept>				PefDiffRegData_AcceptPtr;


struct PefDiffRegKey
{
	i3RegKey*	key;				// 
	INT32		lhs_rhs;
};

struct PefDiffSet
{
	i3::vector<PefDiffRegKey>				m_regKeyDiffList;
	i3::vector<PefDiffRegData_AcceptPtr>	m_regDataDiffList;
};


// i3RegKey입력 전 처리 1. Stamp는 제거된 상태로.... 2. 국가인덱스는 지정된 상태로....

bool FindPefDiff( i3RegKey* lhs_root, i3RegKey* rhs_root, PefDiffSet& out);
void VisitPefDiff( const PefDiffSet& in, PefDiffRegData_Visitor* v);


//
void			PushbackPackVolume(i3PackVolume* vol);
i3PackVolume*	LoadPackVolume( const i3::const_char_range& full_path);
void			ClearAllPackVolume();
bool			LoadPefForDiff(const i3::wstring& root_path, const i3::const_char_range& rel_path, 
					int nation_index, int num_nation, i3RegKey*& out_root);

void			ImportResultFilesFromi3PackFile_Acc(const i3::const_char_range& full_path, i3::vector<i3::string>& inout_files);
//
