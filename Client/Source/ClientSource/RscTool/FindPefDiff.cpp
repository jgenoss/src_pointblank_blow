#include "stdafx.h"
#include "FindPefDiff.h"
#include "i3Scene/i3RegKey.h"
#include "i3Scene/i3RegData.h"
#include "i3Scene/i3RegINT32.h"
#include "i3Scene/i3RegReal32.h"
#include "i3Scene/i3RegString.h"
#include "i3Scene/i3RegVEC2D.h"
#include "i3Scene/i3RegVEC3D.h"
#include "i3Scene/i3RegVEC4D.h"
#include "i3Math/i3MathType.h"

#include "i3Scene/i3RegCOLOR.h"
#include "i3Scene/i3RegMATRIX.h"
#include "i3Scene/i3RegArray.h"
#include "i3Scene/i3Registry.h"

#include "i3Base/i3NamedElement.h"
#include "i3Base/itl/list.h"

#include "i3Base/itl/range/algorithm/find_if.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

//
/*  현재 get_pointer의 포인터 인수가 포함된 헤더가 없는 문제가 있음...(이 부분은 엔진에서 파일 추가가 있어야됨..)
namespace i3
{
	template<class T> inline 
	T* get_pointer(T* p) { return p; }
}
*/

PefDiffRegData_Accept::PefDiffRegData_Accept(i3RegKey* lhs_key, i3RegKey* rhs_key, i3RegData* lhs_data, i3RegData* rhs_data)
{
	m_data.lhs_key = lhs_key;	m_data.rhs_key = rhs_key;
	m_data.lhs = lhs_data;		m_data.rhs = rhs_data;
}

void PefDiffRegData_Accept::visit(PefDiffRegData_Visitor* v)
{
	m_visit_mem_fun[0](v, &m_data);	m_visit_mem_fun[1](v, &m_data);
}

void PefDiffRegData_Accept::move_function(function_type& f, INT32 lhs_rhs)
{
	i3::swap(f, m_visit_mem_fun[lhs_rhs]);
}

namespace Detail
{
	typedef i3::function<void (PefDiffRegData_Visitor*, const PefDiffRegData*) > pef_diff_reg_data_fun_t;


	bool GetPartialTypeFunction( i3RegData* reg_data, INT32 lhs_or_rhs, pef_diff_reg_data_fun_t& out_fun)
	{
		bool res = false;
		switch( reg_data->getDataType() )
		{
		case I3REG_TYPE_INT32:
			{
				INT32* pval =	static_cast<i3RegINT32*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, INT32*, INT32);
				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);
				res = true;
			}
			break;

		case I3REG_TYPE_REAL32:
			{
				REAL32* pval = static_cast<i3RegREAL32*>(reg_data)->getValue();
				using namespace i3;
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, REAL32*, INT32);
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);
				res = true;
			}
			break;

		case I3REG_TYPE_STRING:
			{
				const i3::rc_wstring& val = static_cast<i3RegString*>(reg_data)->getValue();
				using namespace i3;
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, const i3::rc_wstring&, INT32);
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, val, lhs_or_rhs);			
				res = true;
			}
			break;

		case I3REG_TYPE_VEC2D:
			{
				using namespace i3;
				RT_VEC2D*	pval = static_cast<i3RegVEC2D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC2D*, INT32);
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);
				res = true;
			}
			break;

		case I3REG_TYPE_VEC3D:
			{
				RT_VEC3D*	pval = static_cast<i3RegVEC3D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC3D*, INT32);

				using namespace i3;

				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);	
				res = true;
			}
			break;

		case I3REG_TYPE_VEC4D:
			{
				RT_VEC4D*	pval = static_cast<i3RegVEC4D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC4D*, INT32);

				using namespace i3;

				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);	
				res = true;
			}
			break;

		case I3REG_TYPE_COLOR:
			{
				I3COLOR*	pval = static_cast<i3RegCOLOR*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, I3COLOR*, INT32);

				using namespace i3;

				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);				
				res = true;
			}
			break;
		case I3REG_TYPE_MATRIX:
			{
				RT_MATRIX*	pval = static_cast<i3RegMATRIX*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_MATRIX*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::PartialType, _1,_2, pval, lhs_or_rhs);									
				res = true;
			}
			break;

		case I3REG_TYPE_ARRAY:		// 1개만 따올것이다..
			{
				i3RegArray* r_a =	static_cast<i3RegArray*>(reg_data);
				if (r_a->getSize() > 0)
				{
					i3RegData* target_data = (*r_a)[0];
					res = GetPartialTypeFunction(target_data, lhs_or_rhs, out_fun);
				}
			}
			break;
		default:
			break;
		}		
				
		return res;
	}

	bool GetDiffTypeFunction( i3RegData* reg_data, INT32 lhs_or_rhs, pef_diff_reg_data_fun_t& out_fun)
	{
		bool res = false;
		switch( reg_data->getDataType() )
		{
		case I3REG_TYPE_INT32:
			{
				INT32* pval =	static_cast<i3RegINT32*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, INT32*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);
				res = true;
			}
			break;

		case I3REG_TYPE_REAL32:
			{
				REAL32* pval = static_cast<i3RegREAL32*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, REAL32*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);
				res = true;
			}
			break;

		case I3REG_TYPE_STRING:
			{
				const i3::rc_wstring& val = static_cast<i3RegString*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, const i3::rc_wstring&, INT32);
				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, val, lhs_or_rhs);			
				res = true;
			}
			break;

		case I3REG_TYPE_VEC2D:
			{
				RT_VEC2D*	pval = static_cast<i3RegVEC2D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC2D*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);		
				res = true;
			}
			break;

		case I3REG_TYPE_VEC3D:
			{
				RT_VEC3D*	pval = static_cast<i3RegVEC3D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC3D*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);	
				res = true;
			}
			break;

		case I3REG_TYPE_VEC4D:
			{
				RT_VEC4D*	pval = static_cast<i3RegVEC4D*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC4D*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);	
				res = true;
			}
			break;

		case I3REG_TYPE_COLOR:
			{
				I3COLOR*	pval = static_cast<i3RegCOLOR*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, I3COLOR*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);				
				res = true;
			}
			break;
		case I3REG_TYPE_MATRIX:
			{
				RT_MATRIX*	pval = static_cast<i3RegMATRIX*>(reg_data)->getValue();
				typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_MATRIX*, INT32);

				using namespace i3;
				out_fun = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::DiffType, _1,_2, pval, lhs_or_rhs);									
				res = true;
			}
			break;

		case I3REG_TYPE_ARRAY:		// 1개만 따올것이다..
			{
				i3RegArray* r_a =	static_cast<i3RegArray*>(reg_data);
				if (r_a->getSize() > 0)
				{
					i3RegData* target_data = (*r_a)[0];
					res = GetDiffTypeFunction(target_data, lhs_or_rhs, out_fun);
				}
			}
			break;
		default:
			break;
		}		
		return res;
	}

	bool GetSameTypeFunction(	i3RegData* lhs_data, i3RegData* rhs_data, pef_diff_reg_data_fun_t& out_fun1, 
								pef_diff_reg_data_fun_t& out_fun2)
	{
		bool res = false;
		switch( lhs_data->getDataType() )
		{
		case I3REG_TYPE_INT32:
			{
				INT32* lval = static_cast<i3RegINT32*>(lhs_data)->getValue();
				INT32* rval = static_cast<i3RegINT32*>(rhs_data)->getValue(); 
				
				if (*lval != *rval)		// 여기서 실제적 비교 수행..
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, INT32*, INT32*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_REAL32:
			{
				REAL32* lval = static_cast<i3RegREAL32*>(lhs_data)->getValue();
				REAL32* rval = static_cast<i3RegREAL32*>(rhs_data)->getValue();

				if ( fabs(*rval-*lval) > std::numeric_limits<float>::epsilon() )
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, REAL32*, REAL32*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_STRING:
			{
				const i3::rc_wstring& lval = static_cast<i3RegString*>(lhs_data)->getValue();
				const i3::rc_wstring& rval = static_cast<i3RegString*>(rhs_data)->getValue();

				if ( lval != rval )
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, const i3::rc_wstring&, const i3::rc_wstring&);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);		
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_VEC2D:
			{
				RT_VEC2D*	lval = static_cast<i3RegVEC2D*>(lhs_data)->getValue();
				RT_VEC2D*	rval = static_cast<i3RegVEC2D*>(rhs_data)->getValue();

				if ( *lval != *rval)
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC2D*, RT_VEC2D*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);	
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_VEC3D:
			{
				RT_VEC3D*	lval = static_cast<i3RegVEC3D*>(lhs_data)->getValue();
				RT_VEC3D*	rval = static_cast<i3RegVEC3D*>(rhs_data)->getValue();
				
				if (*lval != *rval)
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC3D*, RT_VEC3D*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_VEC4D:
			{
				RT_VEC4D*	lval = static_cast<i3RegVEC4D*>(lhs_data)->getValue();
				RT_VEC4D*	rval = static_cast<i3RegVEC4D*>(rhs_data)->getValue();

				if (*lval != *rval)
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_VEC4D*, RT_VEC4D*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);	
					out_fun2.clear();
					res = true;
				}
			}
			break;

		case I3REG_TYPE_COLOR:
			{
				I3COLOR*	lval = static_cast<i3RegCOLOR*>(lhs_data)->getValue();
				I3COLOR*	rval = static_cast<i3RegCOLOR*>(rhs_data)->getValue();

				if ( reinterpret_cast<UINT32&>(*lval) != reinterpret_cast<UINT32&>(*rval) )
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, I3COLOR*, I3COLOR*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);		
					out_fun2.clear();
					res = true;
				}
			}
			break;
		case I3REG_TYPE_MATRIX:
			{
				RT_MATRIX*	lval = static_cast<i3RegMATRIX*>(lhs_data)->getValue();
				RT_MATRIX*	rval = static_cast<i3RegMATRIX*>(rhs_data)->getValue();

				if ( lval->vec[0] != rval->vec[0] ||
					lval->vec[1] != rval->vec[1] ||
					lval->vec[2] != rval->vec[2] ||
					lval->vec[3] != rval->vec[3] )
				{
					typedef void ( PefDiffRegData_Visitor::*mem_fun_type)(const PefDiffRegData*, RT_MATRIX*, RT_MATRIX*);

					using namespace i3;
					out_fun1 = i3::bind((mem_fun_type)&PefDiffRegData_Visitor::SameType, _1,_2, lval, rval);								
					out_fun2.clear();
					res = true;
				}
			}
			break;

			// ARRAY의 경우 예외처리가 반드시 필요하다..
		case I3REG_TYPE_ARRAY:		// 1개만 따올것이다..
			{
				i3RegArray* r_a_l =	static_cast<i3RegArray*>(lhs_data);
				i3RegArray* r_a_r =	static_cast<i3RegArray*>(rhs_data);
				if (r_a_l->getSize() > 0 && r_a_r->getSize() > 0)
				{
					i3RegData* ldata = (*r_a_l)[0];
					i3RegData* rdata = (*r_a_r)[0];
					
					if (r_a_l->getElementType() == r_a_r->getElementType() )
						res = GetSameTypeFunction(ldata, rdata, out_fun1, out_fun2);
					else
					{
						res  = GetDiffTypeFunction(ldata, 0, out_fun1);
						res |= GetDiffTypeFunction(rdata, 1, out_fun2);
					}
				}
				else
				{
					if (r_a_l->getSize() > 0)
					{
						i3RegData* ldata = (*r_a_l)[0];
						res = GetPartialTypeFunction(ldata, 0, out_fun1);
						out_fun2.clear();
					}
					else
					if (r_a_r->getSize() > 0)
					{
						i3RegData* rdata = (*r_a_r)[0];
						res = GetPartialTypeFunction(rdata, 1, out_fun2);
						out_fun1.clear();
					}
				}
			}
			break;
		default:
			break;
		}		
		
		return res;
	}


	bool ProcessRootDiff( i3RegKey* lhs_root, i3RegKey* rhs_root, 
								PefDiffSet& out, bool& return_val)
	{
		if (lhs_root == NULL || rhs_root == NULL)
		{
			if (lhs_root == rhs_root)		// 둘다 널값인 경우만 본것... 이후에는 이런 검사가 없음..
			{
				return_val = false;
				return true;
			}

			PefDiffRegKey diff;
			if (lhs_root != NULL)
			{
				diff.key = lhs_root;	diff.lhs_rhs = 0;
			}
			else
			{
				diff.key = rhs_root;	diff.lhs_rhs = 1;
			}

			out.m_regKeyDiffList.push_back(diff);
			return_val = true;
			return true;
		}

		if ( lhs_root->GetNameString() != rhs_root->GetNameString() )
		{
			PefDiffRegKey diff;
			diff.key = rhs_root;	diff.lhs_rhs = 1;
			out.m_regKeyDiffList.push_back(diff);
			
			diff.key = lhs_root;	diff.lhs_rhs = 0;
			out.m_regKeyDiffList.push_back(diff);
		
			return_val = true;
			return true;
		}

		return false;
	}
	
	
	void FindRecursivePefDiff(i3RegKey* lhs, i3RegKey* rhs, PefDiffSet& out)
	{
		// lhs 와 rhs의 키는 함수전에 이미 같은 것으로 간주..
		// 자식들의 키 이름 비교처리..
		// 혹은 데이터리스트의 이름 비교처리..
		{

			const INT32 lhs_data_count = lhs->GetDataCount();
			const INT32 rhs_data_count = rhs->GetDataCount();

			i3::list<i3RegData*>		lhs_data_list;			// 한쪽은 링크드리스트로 넣고 양쪽 자식목록 비교..중복제거용도..
			for (INT32 i = 0 ; i < lhs_data_count ; ++i )
				lhs_data_list.push_back(lhs->GetData(i));
			
			struct FindDataNameFunctor 
			{
				FindDataNameFunctor(i3RegData* rhs) : rhs(rhs) {}	i3RegData* rhs;
				bool operator()(i3RegData* lhs) const
				{
					return lhs->GetNameString() == rhs->GetNameString();
				}
			};

			PefDiffRegData_Accept::fun_t	f1;
			PefDiffRegData_Accept::fun_t	f2;

			for (INT32 i = 0 ; i < rhs_data_count ; ++i )
			{
				i3RegData* rhs_data = rhs->GetData(i);
				
				FindDataNameFunctor comp(rhs_data);

				i3::list<i3RegData*>::iterator it1 = i3::range::find_if( lhs_data_list, comp);

				if (it1 == lhs_data_list.end() )		// 이름을 못찾음..새로 추가된 i3RegData..
				{
					if ( Detail::GetPartialTypeFunction(rhs_data, 1, f2) )
					{
						PefDiffRegData_AcceptPtr new_ptr = i3::make_shared<PefDiffRegData_Accept>(lhs, rhs, (i3RegData*)NULL, rhs_data);
						new_ptr->move_function(f2, 1);
						out.m_regDataDiffList.push_back(new_ptr);
					}
					continue;
				}
				
				i3RegData* lhs_data = *it1;		// 비교대상 추출 후 연결리스트에서 제거..
				lhs_data_list.erase(it1);
				
				if ( lhs_data->getDataType() == rhs_data->getDataType() )
				{
					if ( Detail::GetSameTypeFunction(lhs_data, rhs_data, f1, f2) )
					{
						PefDiffRegData_AcceptPtr new_ptr = i3::make_shared<PefDiffRegData_Accept>(lhs, rhs, lhs_data, rhs_data);
						new_ptr->move_function(f1, 0);		new_ptr->move_function(f2, 1);
						out.m_regDataDiffList.push_back(new_ptr);
					}
				}
				else
				{
					bool res1 = Detail::GetDiffTypeFunction(lhs_data, 0, f1);
					bool res2 = Detail::GetDiffTypeFunction(rhs_data, 1, f2);
					
					if (res1 && res2)
					{
						PefDiffRegData_AcceptPtr new_ptr = i3::make_shared<PefDiffRegData_Accept>(lhs, rhs, lhs_data, rhs_data);
						new_ptr->move_function(f1, 0);		new_ptr->move_function(f2, 1);
						out.m_regDataDiffList.push_back(new_ptr);
					}
				}
			}
			
			i3::list<i3RegData*>::iterator it_end =	lhs_data_list.end();		
			for (i3::list<i3RegData*>::iterator it = lhs_data_list.begin(); it != it_end ; ++it )
			{
				i3RegData* lhs_data = *it;		
				
				if ( Detail::GetPartialTypeFunction(lhs_data, 0, f1) )		// 지워진 데이터..
				{
					PefDiffRegData_AcceptPtr new_ptr = i3::make_shared<PefDiffRegData_Accept>(lhs, rhs, lhs_data, (i3RegData*)NULL);
					new_ptr->move_function(f1, 0);
					out.m_regDataDiffList.push_back(new_ptr);
				}		

			}

		}
		
		// 자식리스트 순회 ( 여기서는 같은 이름은 재귀순회처리하고...같지 않은 이름들은 모조리 out값에 반영하고 즉시 종료)
		{

			const INT32 lhs_child_count = lhs->getChildCount();
			const INT32 rhs_child_count = rhs->getChildCount();

			i3::list<i3RegKey*>		lhs_child_list;			// 한쪽은 링크드리스트로 넣고 양쪽 자식목록 비교..중복제거용도..
			for (INT32 i = 0 ; i < lhs_child_count ; ++i )
				lhs_child_list.push_back(static_cast<i3RegKey*>(lhs->getChild(i)));
			

			struct FindChildNameFunctor 
			{
				FindChildNameFunctor(i3RegKey* rhs) : rhs(rhs) {}	i3RegKey* rhs;
				bool operator()(i3RegKey* lhs) const
				{
					return lhs->GetNameString() == rhs->GetNameString();
				}
			};


			for (INT32 i = 0 ; i < rhs_child_count ; ++i )
			{	
				i3RegKey* rhs_child = static_cast<i3RegKey*>(rhs->getChild(i));

				FindChildNameFunctor comp(rhs_child);

				i3::list<i3RegKey*>::iterator it1 = i3::range::find_if( lhs_child_list, comp);

				if ( it1 == lhs_child_list.end())		// 새로 들어온 키..
				{	
					PefDiffRegKey diff;
					diff.key = rhs_child;	diff.lhs_rhs = 1;
					out.m_regKeyDiffList.push_back(diff);
					continue;
				}
				
				i3RegKey* lhs_child = *it1;
				lhs_child_list.erase(it1);
				
				Detail::FindRecursivePefDiff(lhs_child, rhs_child, out);		// 자식 재귀..
			}


			i3::list<i3RegKey*>::iterator it_end =	lhs_child_list.end();		
			for (i3::list<i3RegKey*>::iterator it = lhs_child_list.begin(); it != it_end ; ++it )
			{
				i3RegKey* lhs_child = *it;		

				 // 지워진 키..
				PefDiffRegKey diff;				
				diff.key = lhs_child;	diff.lhs_rhs = 0;
				out.m_regKeyDiffList.push_back(diff);
			}

		}
		
	}
			
}

bool FindPefDiff( i3RegKey* lhs_root, i3RegKey* rhs_root, PefDiffSet& out)
{
	out.m_regDataDiffList.clear();
	out.m_regKeyDiffList.clear();

	bool res = false;
	
	if (Detail::ProcessRootDiff(lhs_root, rhs_root, out, res))
		return res;
		
	Detail::FindRecursivePefDiff(lhs_root, rhs_root, out);
		
	return !out.m_regDataDiffList.empty() || !out.m_regKeyDiffList.empty();
}

void VisitPefDiff( const PefDiffSet& in, PefDiffRegData_Visitor* v)
{
	const size_t num_key = in.m_regKeyDiffList.size();	

	for (size_t i = 0 ; i < num_key ; ++i)
	{
		const PefDiffRegKey& k = in.m_regKeyDiffList[i];
		v->PartialKey(k.key, k.lhs_rhs);
	}
	
	const size_t num_data = in.m_regDataDiffList.size();
	for (size_t i = 0 ; i < num_data; ++i)
	{
		PefDiffRegData_Accept* p = in.m_regDataDiffList[i].get();
		p->visit(v);
	}
}

//
//  상대경로와 관련되어 약간 거지 같은 부분이 있어서 GetCurrentDirectory/SetCurrentDirectory를 써야한다..
//

bool LoadPefForDiff( const i3::wstring& root_path, const i3::const_char_range& rel_path,
					int nation_index, int num_nation, i3RegKey*& out_root)
{
	out_root = NULL;

	i3Registry::Set_LoadStamp(false);	// STAMP 읽으면 안됨...
	i3Registry::Set_LocationIndex( nation_index, num_nation);

	i3RegistrySet	RegFile;

	wchar_t str_curr_dir[MAX_PATH];
	DWORD num_curr_dir_char = ::GetCurrentDirectoryW(MAX_PATH, str_curr_dir);
	
	::SetCurrentDirectoryW( root_path.c_str() );

	INT nRead = RegFile.LoadRegistrySet(&rel_path[0], REGISTRY_FILE_BINARY);
	
	::SetCurrentDirectoryW( str_curr_dir );

	if (nRead == STREAM_ERR)
		return false;
	
	out_root = RegFile.getRoot();
	
	if (!out_root)
		return false;
	
	out_root->AddRef();
	return true;
}

void PushbackPackVolume(i3PackVolume* vol)
{
	i3Pack::BindVolume(vol);
}

i3PackVolume* LoadPackVolume( const i3::const_char_range& full_path)
{
	i3PackVolume* new_vol = i3PackVolume::new_object();

	if (new_vol->LoadFromFile(&full_path[0]) == false)
	{
		new_vol->Release();
		return NULL;
	}

	return new_vol;
}

void ClearAllPackVolume()
{
	i3Pack::Finish();	
}

namespace
{
	void GatherInPackFileNode(i3PackNode* n, const i3::string& curr_dir_path, i3::vector<i3::string>& inout_list)
	{
		
		const INT32 num_file = n->getFileCount();

		for (INT32 i = 0 ; i < num_file ; ++i)
		{
			I3_PACK_FILE_REF* info = n->getFile(i);
		
			inout_list.push_back(i3::string());
			i3::string& rel_path = inout_list.back();
			rel_path = curr_dir_path;
			rel_path += '/';
			rel_path += info->m_szName;
		}

		const INT32 num_child = n->getChildCount();

		for (INT32 i = 0 ; i < num_child ; ++i)
		{
			i3PackNode* child = static_cast<i3PackNode*>(n->getChild(i));
			i3::string child_dir_path;

			if (!curr_dir_path.empty() )
			{
				child_dir_path = curr_dir_path;
				child_dir_path += '/';
			}

			const i3::rc_string& child_name = child->GetNameString();
			child_dir_path.append( child_name.begin(), child_name.end() );

			GatherInPackFileNode(child, child_dir_path, inout_list);
		}
	}


}


void	ImportResultFilesFromi3PackFile_Acc(const i3::const_char_range& full_path, i3::vector<i3::string>& inout_files)
{
	i3PackVolume* vol = LoadPackVolume(full_path);
	if (!vol) return;
	
	i3PackNode* root = vol->getRoot();

	GatherInPackFileNode(root, i3::string(), inout_files);

	vol->Release();
	
}
