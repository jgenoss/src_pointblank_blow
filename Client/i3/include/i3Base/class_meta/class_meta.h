#pragma once

#include "../rtti/rtti_info.h"
#include "../v_pool/v_pool_info.h"
#include "i3CRC32.h"
#include "typedef_element.h"
#include "../string/fixed_string.h"
#include "../pool/class_common_pool.h"

namespace i3
{
	enum class_meta_style;

	class I3_EXPORT_BASE class_meta : public i3::class_common_pool<class_meta>
	{
	public:
		typedef i3::element													element_type;
		typedef class_meta													class_meta_type;
#if !defined(_DEBUG)
		typedef i3::element*												(*create_element_fun_t)();
#else
		typedef i3::element*												(*create_element_fun_t)(const char*, int);
#endif

		bool same_of( const class_meta_type* m) const {  return	this == m; }
		bool kind_of( const class_meta_type* m) const {  return m_rtti_info.kind_of(&m->m_rtti_info); }

		template<class T, class B>
		class_meta(const char* classname, i3::identity<T> t, i3::identity<B> b);
		~class_meta();

#if !defined(_DEBUG)
		element_type*			create_instance_fun() { return (*m_creation_meta->m_creation_fun)();  }
		element_type*			create_instance_ghost_fun() { return (*m_ghost_meta->m_creation_fun)(); }
#else
		element_type*			create_instance_fun(const char* file, int line) { return (*m_creation_meta->m_creation_fun)(file, line); }
		element_type*			create_instance_ghost_fun(const char* file, int line) { return (*m_ghost_meta->m_creation_fun)(file, line); }
#endif

		void			set_pool(const v_pool_ptr& p) { m_pool_info.set_pool(p);  }
#if !defined(_DEBUG)
		void*			allocate() { return m_pool_info.allocate(); }
#else
		void*			allocate(const char* file, int line) 
		{ 
			++m_curr_alloc_count;	++m_total_alloc_count;
			if (m_curr_alloc_count > m_max_alloc_count) m_max_alloc_count = m_curr_alloc_count;

			return m_pool_info.allocate(file, line); 
		}
#endif
		void			deallocate(void* data) 
		{ 
#if defined(_DEBUG)
			--m_curr_alloc_count;
#endif
			m_pool_info.deallocate(data); 
		}

		class_meta_type*		get_parent() const { return m_parent; }
		std::size_t				get_num_child() const { return m_childs.size(); }
		class_meta_type*		get_child(std::size_t idx) const { return m_childs[idx]; }
		bool					add_child(class_meta_type* child);
				
		std::size_t					class_size() const { return m_pool_info.get_class_size(); }
		const i3::fixed_string&		class_name() const { return m_class_name; }
		unsigned int				class_crc()  const { return m_crc; }
		const char*					pool_name() const { return m_pool_info.get_pool_name(); }

		const i3::fixed_string&		persist_class_name() const; 
		
		bool						has_concrete_creator() const { return m_creation_meta != this; }
		bool						has_ghost_creator() const {	return  m_ghost_meta != this ; }
		void						set_class_meta_style(class_meta_style t);
		void						set_class_meta_style_concrete(class_meta_type* parent_meta);
		void						set_class_meta_style_ghost( class_meta_type* parent_meta);

		class_meta_style			get_class_meta_style() { return m_cms; }
		
		void						get_all_derived_metas(i3::vector<class_meta_type*>& l, bool recursive = true);
	
#if defined(_DEBUG)
		long						get_curr_alloc_count() const { return m_curr_alloc_count; }
		long						get_max_alloc_count() const  { return m_max_alloc_count;  }
		long						get_total_alloc_count() const { return m_total_alloc_count; }
#endif

	private:
		void						register_meta();
		class_meta_type*			find_concrete_parent_meta() const;
		class_meta_type*			find_ghost_parent_meta() const;
		bool						check_parents( class_meta_type* may_be_parent_meta) const;

		rtti_info					m_rtti_info;
		v_pool_info					m_pool_info;

		class_meta_type*			m_creation_meta;		// 이 2개의 메타는 널값없이 항상 유효하게끔..
		class_meta_type*			m_ghost_meta;			// 일반적인 메타유형은 this포인터와 같게 처리..
		const create_element_fun_t	m_creation_fun;
				
		const i3::fixed_string		m_class_name;
		const unsigned int			m_crc;			// 선언 위치가 m_class_name 뒤에 있어야함..

		class_meta_type*				m_parent;
		i3::vector<class_meta_type*>	m_childs;
		class_meta_style				m_cms;
		
		const bool						m_is_abstract_class;		// 변경불가..

#if defined(_DEBUG)
		long							m_curr_alloc_count;			// 디버그용도로만..
		long							m_total_alloc_count;
		long							m_max_alloc_count;			// 디버그용도로만..
#endif
		friend class rtti_info;
	};
	

}

#include "detail/class_meta.inl"



