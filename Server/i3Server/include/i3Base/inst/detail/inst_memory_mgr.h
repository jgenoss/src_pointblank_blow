#pragma once

#define inst_arity_limit			5

#include "../../itl/unordered_map_basic.h"
#include "../../memory/heap_allocator.h"

#include "../../itl/preprocessor/ext/pp_enum_params_1.h"
#include "../../itl/preprocessor/ext/pp_enum_params_1_cat_params.h"
#include "../../itl/preprocessor/pp_repeat_from_to.h"

namespace i3
{
	namespace detail_inst
	{

		class shared_inst 
		{
		public:
			shared_inst( void* data) : m_data(data), m_ref_count(1) {}	
			shared_inst(const shared_inst& rhs) : m_data(rhs.m_data), m_ref_count(1) {}
			shared_inst& operator=(const shared_inst& rhs) { m_data = rhs.m_data; m_ref_count = rhs.m_ref_count; return *this; }

			template<class T>
			static shared_inst create();
			
#define		repeat_shared_inst_create(z, n, d) \
			template<class T, pp_enum_params_1(n, class P)> \
			static shared_inst create( pp_enum_params_1_cat_params(n, const P, &, p) );
			pp_repeat_from_to(1, pp_inc(inst_arity_limit), repeat_shared_inst_create, ~)
#undef		repeat_shared_inst_create

			void			add_ref() { ++m_ref_count;  }

			template<class T>
			void			release();
			
			template<class T> T* get_data() const { return reinterpret_cast<T*>(m_data); }

		private:
			void*			m_data;			// 싱글턴 데이터 홀더..
			long			m_ref_count;	// 할당필요없음.
		};

		class inst_memory_mgr
		{
		public:
			typedef i3::unordered_map<const std::type_info*, shared_inst, hash<const std::type_info*>, 
				equal_to<const std::type_info*>, heap_allocator<i3::pair<const std::type_info*, shared_inst> > >	shared_inst_map;	// 문자열검색외에는 방법없어보임..


			inst_memory_mgr();
			~inst_memory_mgr();
	
			template<class T>
			T*					create_inst();			// 소거는 이 어플 수준에서 끝낼까...(T를 알아야함!)
			
#define		repeat_create_inst(z, n, d) \
			template<class T, pp_enum_params_1(n, class P)> \
			T*					create_inst( pp_enum_params_1_cat_params(n, const P, &, p) );
			pp_repeat_from_to(1, pp_inc(inst_arity_limit), repeat_create_inst, ~)
#undef		repeat_create_inst

			template<class T>
			T*					find_inst();
			template<class T>
			void				delete_inst();			// T를 아는 상태에서 소거에 들어가야함...
						
		private:
			shared_inst_map		m_inst_map;			//  
		};

		extern I3_EXPORT_BASE inst_memory_mgr g_inst_memory_mgr;

	}
}

#include "inst_memory_mgr.inl"
