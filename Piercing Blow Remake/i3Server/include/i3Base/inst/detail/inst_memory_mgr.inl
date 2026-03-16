#if !defined(pp_is_iterating)

#include "../../itl/preprocessor/pp_iterate.h"


namespace i3
{


	namespace detail_inst
	{

		template<class T>
		shared_inst shared_inst::create() 
		{
			return shared_inst(new (i3::heap_aligned_malloc(i3::get_process_heap(), sizeof(T), __alignof(T))) T );
		}

		template<class T>
		void		shared_inst::release()
		{
			if ( --m_ref_count == 0)
			{
				T* data = get_data<T>();
				data->~T();
				i3::heap_aligned_free(i3::get_process_heap(), m_data);
				m_data = 0;
			}
		}

		template<class T>	
		T*		inst_memory_mgr::create_inst()			// 소거는 이 어플 수준에서 끝낼까...(T를 알아야함!)
		{
			const std::type_info* typeinfo =&typeid(T);
			shared_inst_map::iterator	iter = m_inst_map.find( typeinfo );

			if ( iter != m_inst_map.end() )
			{
				shared_inst& inst = iter->second;
				inst.add_ref();
				return inst.get_data<T>();
			}

			iter = m_inst_map.insert( shared_inst_map::value_type( typeinfo, shared_inst::create<T>() ) ).first;
			shared_inst& inst = iter->second;
			return inst.get_data<T>();
		}

		//
		// 비얀 스트로스트럽에 따르면, type_info객체의 중복출현이 있을수 있다고 한다..( 아마도 dll이 동적으로 붙는다면, 그럴 것임)
		// 그래서, 상등성 점검은 type_info에 대한 ==operator를 쓰도록 권장된다.....(type_info는 복사방지되 있으므로, 컨테이너에는 포인터를 담는다..)

		// shared_inst는 처음에는 레퍼런스카운팅을 생각했으나, atexit()시점에서 동적dll은 사라지고 없어지는것으로 간주하고..
		// 그렇게 처리하지 않고 발견즉시 삭제하도록 했다...

		template<class T>
		T*				inst_memory_mgr::find_inst()
		{
			const std::type_info* typeinfo =&typeid(T);
			shared_inst_map::iterator	iter = m_inst_map.find( typeinfo );

			if ( iter != m_inst_map.end() )
			{
				shared_inst& inst = iter->second;
				return inst.get_data<T>();
			}
			return NULL;
		}


		template<class T>	
		void			inst_memory_mgr::delete_inst()			// T를 아는 상태에서 소거에 들어가야함...
		{
			const std::type_info* typeinfo =&typeid(T);
			shared_inst_map::iterator	iter = m_inst_map.find( typeinfo );
			if ( iter == m_inst_map.end() )
				return;

			shared_inst& inst = iter->second;
			inst.release<T>();

			if ( inst.get_data<T>() == 0)
				m_inst_map.erase(iter);
		}

	}
}

#define		pp_iteration_params_1	(3, (1, inst_arity_limit, "inst_memory_mgr.inl"))				// 1단계 파일반복..
#include	pp_iterate()


#else		// pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	template<class T, pp_enum_params_1(i_, class P) > __forceinline
	T*		create_inst_memory(pp_enum_params_1_cat_params(i_, const P, &, p) )
	{
		return detail_inst::g_inst_memory_mgr.create_inst<T>(pp_enum_params_1(i_, p) );
	}

	namespace detail_inst 
	{
		template<class T, pp_enum_params_1(i_, class P) >
		shared_inst shared_inst::create(pp_enum_params_1_cat_params(i_, const P, &, p) )
		{
			return shared_inst(new (i3::heap_aligned_malloc(i3::get_process_heap(), sizeof(T), __alignof(T))) T( pp_enum_params_1(i_, p) ) );
		}
		

		template<class T, pp_enum_params_1(i_, class P) >
		T*		inst_memory_mgr::create_inst(pp_enum_params_1_cat_params(i_, const P, &, p) )
		{
			const std::type_info* typeinfo =&typeid(T);
			shared_inst_map::iterator	iter = m_inst_map.find( typeinfo );

			if ( iter != m_inst_map.end() )
			{
				shared_inst& inst = iter->second;
				inst.add_ref();
				return inst.get_data<T>();
			}

			iter = m_inst_map.insert( shared_inst_map::value_type( typeinfo, shared_inst::create<T>( pp_enum_params_1(i_, p) ) ) ).first;
			shared_inst& inst = iter->second;
			return inst.get_data<T>();
		}

	}
}

#undef i_

#endif

