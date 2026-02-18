#include "i3CommonType.h"
#include "class_meta/class_meta_system.h"
#include "class_meta/class_meta.h"
//
#include "string/i3string.h"
#include "i3Debug.h"
#include "itl/stack.h"
//#include "v_pool/v_storage_pool_mt.h"



namespace i3
{

	class_meta_system::class_meta_system() : m_root(nullptr)
	{

	}

	class_meta_system::~class_meta_system() 
	{ 
		//delete m_root; 
		i3::cu::for_each_delete(m_ctn_meta_for_clear);
	}

	class_meta*	class_meta_system::find_class_meta(const key_string& k) const
	{
		container_string::const_iterator it = m_ctn_string.find(k);
		return ( it != m_ctn_string.end() ) ? it->second : nullptr;
	}

	class_meta*	class_meta_system::find_class_meta( key_crc k) const
	{
		container_crc::const_iterator it = m_ctn_crc.find(k);
		return (it != m_ctn_crc.end() ) ? it->second : nullptr;
	}


	void		class_meta_system::register_meta(class_meta_type* meta)
	{
		i3::pair<typename container_string::iterator, bool > pr_string = m_ctn_string.insert(i3::make_pair(meta->class_name(), meta) );
		if (!pr_string.second)
		{
			// FATAL스러운 메세지처리..
			I3TRACE( "NAME COLLISION : %s[%08X] existed.\n", meta->class_name(), meta->class_crc() );
			I3PRINTLOG(I3LOG_FATAL,  "ClassMeta Name Collision!");
			return;
		}

		i3::pair<typename container_crc::iterator, bool> pr_crc = m_ctn_crc.insert( i3::make_pair(meta->class_crc(), meta) );
		if (!pr_crc.second)
		{
			class_meta_type* old_meta = pr_crc.first->second;
			I3TRACE( "CRC COLLISION : %s[%08X] - %s[%08X] existed.\n",	meta->class_name(), meta->class_crc(), 
				old_meta->class_name(), old_meta->class_crc());
			I3PRINTLOG(I3LOG_FATAL, "ClassMeta CRC Collision!");
			// FATAL스러운 메세지처리..
		}

//		메모리 소거처리를 여기서 예약..
		m_ctn_meta_for_clear.push_back(meta);
//
	}


	#pragma push_macro("create_instance_by_key")
	#pragma push_macro("create_ghost_by_key")

	#undef create_instance_by_key
	#undef create_ghost_by_key

#if !defined(_DEBUG)

	class_meta_system::result_type		class_meta_system::create_instance_by_key(const key_string& k ) const
	{    
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_fun() : nullptr;
	}


	class_meta_system::result_type		class_meta_system::create_instance_by_key(key_crc k ) const
	{    
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_fun() : nullptr;
	}

	class_meta_system::result_type	class_meta_system::create_ghost_by_key(const key_string&  k) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_ghost_fun() : nullptr;
	}


	class_meta_system::result_type	class_meta_system::create_ghost_by_key(key_crc  k) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_ghost_fun() : nullptr;
	}

#else

	class_meta_system::result_type	class_meta_system::create_instance_by_key(const key_string& k, const char* file, int line) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_fun(file, line) : nullptr;
	}

		
	class_meta_system::result_type		class_meta_system::create_instance_by_key(key_crc k, const char* file, int line) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_fun(file, line) : nullptr;
	}

	class_meta_system::result_type		class_meta_system::create_ghost_by_key(const key_string&  k, const char* file, int line) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_ghost_fun(file, line) : nullptr;
	}

	class_meta_system::result_type		class_meta_system::create_ghost_by_key(key_crc k, const char* file, int line) const
	{
		class_meta_type* m = find_class_meta(k);
		return (m) ? m->create_instance_ghost_fun(file, line) : nullptr;
	}

	#endif
	
	#pragma pop_macro("create_ghost_by_key")
	#pragma pop_macro("create_instance_by_key")


	void class_meta_system::print_class_meta()
	{
		if (!m_root) return;
		i3::stack<int>	s;
		class_meta_type* p_meta = m_root;
		i3::string indent;
		I3TRACE("%sClass Name %s Size %d\n", indent, p_meta->class_name(), p_meta->class_size());

		if ( p_meta->get_num_child() > 0)
		{
			s.push(0);
			indent += ' ';
			size_t i = 0;
			while ( !s.empty() )
			{
				class_meta_type* meta = p_meta->get_child(i);

				I3TRACE("%sClass Name %s Size %d\n", indent, meta->class_name(), meta->class_size());

				if ( meta->get_num_child() > 0)
				{
					indent += ' ';
					p_meta = meta;		s.push(++i);
					i = 0;
				}
				else
				{
					++i;
					
					while ( !s.empty() && p_meta->get_num_child() == i )
					{
						indent.erase(indent.size() - 1);
						p_meta = p_meta->get_parent();
						i = s.top();
						s.pop();
					}
				}
			}
		}
	}

	void		class_meta_system::get_all_metas(i3::vector<class_meta*>& l)
	{
		if (!m_root) return;
		l.push_back(m_root);
		m_root->get_all_derived_metas(l);
	}

#if defined(_DEBUG)

	namespace
	{

		bool		is_bad_none_pool(const char* pool_name, size_t class_size, 
									long tot_alloc, long max_alloc, long curr_alloc,
				long req_pool_alloc_count, long req_pool_free_percent, long req_pool_memory_size)
		{
			long free_percent = (tot_alloc) ? (tot_alloc - curr_alloc) * 100 / tot_alloc : 0;
			
			if ( max_alloc >= req_pool_alloc_count && 
				free_percent >= req_pool_free_percent && 
				long(class_size) * max_alloc >= req_pool_memory_size )
			{
				if ( i3::generic_is_equal("class i3::v_none_pool", pool_name) )
					return true;
			}
			return false;
		}

		bool		is_bad_compact_pool(const char* pool_name, size_t class_size, 
			long tot_alloc, long max_alloc, long curr_alloc,
			long req_pool_alloc_count, long req_pool_free_percent, long req_pool_memory_size)
		{
			long free_percent = (tot_alloc) ? (tot_alloc - curr_alloc) * 100 / tot_alloc : 0;

			if ( max_alloc > 0 && max_alloc < req_pool_alloc_count && 
				( tot_alloc < req_pool_alloc_count || free_percent < req_pool_free_percent) && 
				long(class_size) * max_alloc < req_pool_memory_size )
			{
				if (i3::generic_is_equal("class i3::v_compact_pool", pool_name) )
					return true;
			}
			return false;			
		}

	}
		

	void		print_class_meta_alloc_infos(long req_pool_alloc_count, long req_pool_free_percent, long req_pool_memory_size)  // 16, 50, pool_block_size
	{

		I3TRACE("=================================================================================\n");
		I3TRACE("                               Class Meta Alloc Info\n");
		I3TRACE("=================================================================================\n");


		if ( req_pool_memory_size < sizeof(mem_block_header) )
			req_pool_memory_size = 0;
		else
			req_pool_memory_size -= sizeof(mem_block_header);

		class_meta* root = i3::class_meta_system::i()->get_root();

		if (!root)
		{
			I3TRACE("empty.\n");
			I3TRACE("=================================================================================\n");
			return;
		}

		i3::stack<int>	s;
		i3::vector<class_meta*> bad_pool_list;
		i3::vector<class_meta*> bad_none_pool_list;

		class_meta* p_meta = root;

		i3::string indent;
		I3TRACE("%sclass: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
			indent, p_meta->class_name(), p_meta->get_total_alloc_count(), p_meta->get_max_alloc_count(), p_meta->get_curr_alloc_count(), 
			p_meta->pool_name(), p_meta->class_size());

	
		if ( p_meta->get_num_child() > 0)
		{
			s.push(0);
			indent += ' ';
			size_t i = 0;
			while ( !s.empty() )
			{
				class_meta* meta = p_meta->get_child(i);
				
				long tot_alloc = meta->get_total_alloc_count();
				long max_alloc = meta->get_max_alloc_count();
				long curr_alloc = meta->get_curr_alloc_count();
			//	long free_percent = (tot_alloc) ? (tot_alloc - curr_alloc) * 100 / tot_alloc : 0;

				I3TRACE("%sclass: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
					indent, meta->class_name(), tot_alloc, max_alloc, curr_alloc, meta->pool_name(), meta->class_size());
				

				if (is_bad_none_pool(meta->pool_name(), meta->class_size(), tot_alloc, max_alloc, curr_alloc, req_pool_alloc_count,
					req_pool_free_percent, req_pool_memory_size) )
				{
					bad_none_pool_list.push_back(meta);
				}

				if (is_bad_compact_pool(meta->pool_name(), meta->class_size(), tot_alloc, max_alloc, curr_alloc, req_pool_alloc_count,
					req_pool_free_percent, req_pool_memory_size) )
				{
					bad_pool_list.push_back(meta);
				}	
				
				if ( meta->get_num_child() > 0)
				{
					indent += ' ';
					p_meta = meta;		s.push(++i);
					i = 0;
				}
				else
				{
					++i;
					while ( !s.empty() && p_meta->get_num_child() == i )
					{
						indent.erase(indent.size() - 1);
						p_meta = p_meta->get_parent();
						i = s.top();
						s.pop();
					}
				}
			}
		}
		else
		{
			I3TRACE("empty.\n");
		}
		
		I3TRACE("=================================================================================\n");
		I3TRACE("                                   Bad Pool Classes\n");
		I3TRACE("=================================================================================\n");

		const size_t num_bad_pool = bad_pool_list.size();

		if (num_bad_pool)
		{
			for (size_t i = 0 ; i < num_bad_pool ; ++i)
			{
				class_meta* meta = bad_pool_list[i];

				long tot_alloc = meta->get_total_alloc_count();
				long max_alloc = meta->get_max_alloc_count();
				long curr_alloc = meta->get_curr_alloc_count();
			
				I3TRACE("class: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
					meta->class_name(), tot_alloc, max_alloc, curr_alloc, 
					meta->pool_name(), meta->class_size());
			}
		}
		else
		{
			I3TRACE("empty.\n");
		}


		I3TRACE("=================================================================================\n");
		I3TRACE("                                   Bad None-Pool Classes\n");
		I3TRACE("=================================================================================\n");
		
		const size_t num_bad_none_pool = bad_none_pool_list.size();
		
		if (num_bad_none_pool)
		{
			for (size_t i = 0 ; i < num_bad_none_pool ; ++i)
			{
				class_meta* meta = bad_none_pool_list[i];

				long tot_alloc = meta->get_total_alloc_count();
				long max_alloc = meta->get_max_alloc_count();
				long curr_alloc = meta->get_curr_alloc_count();
			
				I3TRACE("class: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
					meta->class_name(), tot_alloc, max_alloc, curr_alloc, 
					meta->pool_name(), meta->class_size());
			}
		}
		else
		{
			I3TRACE("empty.\n");
		}
		
		I3TRACE("=================================================================================\n");
	}

	void	 print_class_meta_wrong_pool_infos(long req_pool_alloc_count, long req_pool_free_percent, long req_pool_memory_size)  // 16, 50, pool_block_size
	{

		if ( req_pool_memory_size < sizeof(mem_block_header) )
			req_pool_memory_size = 0;
		else
			req_pool_memory_size -= sizeof(mem_block_header);


		i3::vector<i3::class_meta*> vecMetaList;

		i3::class_meta_system::i()->get_all_metas(vecMetaList);

		i3::vector<class_meta*> bad_pool_list;
		i3::vector<class_meta*> bad_none_pool_list;

		const size_t num = vecMetaList.size();

		for (size_t i = 0 ; i < num ; ++i )
		{
			class_meta* meta = vecMetaList[i];
			
			long tot_alloc = meta->get_total_alloc_count();
			long max_alloc = meta->get_max_alloc_count();
			long curr_alloc = meta->get_curr_alloc_count();		// dealloc count = tot_alloc - curr_alloc //
		//	long free_percent = (tot_alloc) ? (tot_alloc - curr_alloc) * 100 / tot_alloc : 0;
			
			if (is_bad_none_pool(meta->pool_name(), meta->class_size(), tot_alloc, max_alloc, curr_alloc, req_pool_alloc_count,
				req_pool_free_percent, req_pool_memory_size) )
			{
				bad_none_pool_list.push_back(meta);
			}

			if (is_bad_compact_pool(meta->pool_name(), meta->class_size(), tot_alloc, max_alloc, curr_alloc, req_pool_alloc_count,
				req_pool_free_percent, req_pool_memory_size) )
			{
				bad_pool_list.push_back(meta);
			}	
						
		}

		if (!bad_pool_list.empty())
		{
			I3TRACE("=================================================================================\n");
			I3TRACE("                                   Bad Pool Classes\n");
			I3TRACE("=================================================================================\n");

			const size_t num_bad_pool = bad_pool_list.size();
			for (size_t i = 0 ; i < num_bad_pool ; ++i)
			{
				class_meta* meta = bad_pool_list[i];

				long tot_alloc = meta->get_total_alloc_count();
				long max_alloc = meta->get_max_alloc_count();
				long curr_alloc = meta->get_curr_alloc_count();

				I3TRACE("class: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
					meta->class_name(), tot_alloc, max_alloc, curr_alloc, 
					meta->pool_name(), meta->class_size());
			}

		}

		if (!bad_none_pool_list.empty())
		{
						
			I3TRACE("=================================================================================\n");
			I3TRACE("                                   Bad None-Pool Classes\n");
			I3TRACE("=================================================================================\n");

			const size_t num_bad_none_pool = bad_none_pool_list.size();
			for (size_t i = 0 ; i < num_bad_none_pool ; ++i)
			{
				class_meta* meta = bad_none_pool_list[i];

				long tot_alloc = meta->get_total_alloc_count();
				long max_alloc = meta->get_max_alloc_count();
				long curr_alloc = meta->get_curr_alloc_count();

				I3TRACE("class: %s tot_alloc: %d max_alloc: %d curr_alloc: %d pool: %s class_size: %d\n", 
					meta->class_name(), tot_alloc, max_alloc, curr_alloc, 
					meta->pool_name(), meta->class_size());
			}

			I3TRACE("=================================================================================\n");
		}
		else
		if ( !bad_pool_list.empty() )
		{
			I3TRACE("=================================================================================\n");
		}
			
	}


#endif

}