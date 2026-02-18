#pragma once

#include "typedef_element.h"
#include "../string/fixed_string_fwd.h"


namespace i3
{
	class class_meta;

	class I3_EXPORT_BASE class_meta_system 
	{
	public:
		typedef i3::class_meta									class_meta_type;
		typedef i3::element										result_class_type;
		typedef i3::element*									result_type;

		typedef i3::fixed_string								key_string;
		typedef unsigned int									key_crc;

		typedef i3::unordered_map<key_string, class_meta_type*>	container_string;
		typedef i3::unordered_map<key_crc, class_meta_type*>	container_crc;
		typedef	i3::vector<class_meta_type*>					vector_meta;

		class_meta_system();
		~class_meta_system();
		
		static class_meta_system*	i() { return ginst<class_meta_system>(); }
		static void				create_explicit() {  ginst<class_meta_system>::create_explicit(); }
		class_meta_type*	find_class_meta(const key_string& k) const;	
		class_meta_type*	find_class_meta(key_crc     k) const;
#if !defined(_DEBUG)
		result_type		create_instance_by_key(const key_string& k ) const;
		result_type		create_instance_by_key(key_crc k) const;

		result_type		create_ghost_by_key(const key_string&  k) const;
		result_type		create_ghost_by_key(key_crc k) const;
#else
		result_type		create_instance_by_key(const key_string& k, const char* file, int line) const;
		result_type		create_instance_by_key(key_crc k, const char* file, int line) const;

		result_type		create_ghost_by_key(const key_string&  k, const char* file, int line) const;
		result_type		create_ghost_by_key(key_crc  k, const char* file, int line) const;
#endif
	
		class_meta_type*	get_root() const { return m_root; }	
		void				print_class_meta();
		void				get_all_metas(i3::vector<class_meta_type*>& l);
		void				set_root(class_meta_type* meta) { m_root = meta; }

	private:
		void				register_meta(class_meta_type* meta);		// 중복 불가로 처리..반드시 insert성공해야함..
	
		friend class class_meta;

		container_string		m_ctn_string;			
		container_crc			m_ctn_crc;
		vector_meta				m_ctn_meta_for_clear;
		class_meta_type*		m_root;			
	};
	
#if defined(_DEBUG)
	void	I3_EXPORT_BASE	print_class_meta_alloc_infos(long req_pool_alloc_count = 16, 
														 long req_pool_free_percent = 50, 
														 long req_pool_memory_size = i3::pool_block_size);		// 디버그용으로만..
	void	I3_EXPORT_BASE  print_class_meta_wrong_pool_infos(  long req_pool_alloc_count = 16, 
																long req_pool_free_percent = 50, 
																long req_pool_memory_size = i3::pool_block_size);	// 디버그용으로만..
#endif
}

namespace i3
{
#define create_instace_by_key_no_dbg(k)		create_instance_by_key(k)	
#define	create_ghost_by_key_no_dbg(k)		create_ghost_by_key(k)	

#if defined(_DEBUG)
#define create_instance_by_key(k)			create_instance_by_key(k, __FILE__, __LINE__)
#define create_ghost_by_key(k)				create_ghost_by_key(k, __FILE__, __LINE__)
#endif
}
