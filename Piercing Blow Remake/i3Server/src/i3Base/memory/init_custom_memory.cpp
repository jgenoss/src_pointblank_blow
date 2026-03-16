#include "i3CommonType.h"


//#include <stdlib.h>
//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>

#include "memory/delayed_atexit.h"

#include "pool/pool_dbg.h"
#include "pool/static_large_object_cache.h"
#include "pool/static_pool.h"


namespace i3 {	void	init_custom_memory();  }

#pragma warning (disable : 4074)
#pragma init_seg(compiler)

namespace i3 
{
	namespace detail_inst { inst_memory_mgr   g_inst_memory_mgr; }
	namespace detail_tls {	tls_mgr	g_TlsManager; }
}

namespace
{
	class Initializer
	{
	public:
		Initializer()
		{
			i3::init_custom_memory();
		}
	} _;

} //namespace


namespace i3
{
	typedef		i3::vector< void(*)(), i3::allocator<void(*)()> >	vec_exit_fn_type;

	int		delayed_atexit( void (*func)() )
	{
		vec_exit_fn_type& vec = *i3::find_inst_memory<vec_exit_fn_type>(); 
		vec.push_back(func);
		return 0;
	}
	
	namespace detail
	{
		void	cleanup_memory_exit()
		{
			i3::dump_leak();
			
			vec_exit_fn_type& vec = *i3::find_inst_memory<vec_exit_fn_type>();

			while( !vec.empty() )
			{
				void (*fn)() = vec.back();
				(*fn)();
				vec.pop_back();
			}

			i3::delete_inst_memory<vec_exit_fn_type>();
		}

	}
	
	void	init_custom_memory()
	{
		i3::create_inst_memory<vec_exit_fn_type>();

		::atexit(&detail::cleanup_memory_exit);
		
		i3::init_dbg_util();
		static_large_object_cache::i();
		static_pool::i();
	}
		
}

