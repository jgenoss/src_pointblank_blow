#include "i3CommonType.h"

//#include "string/ext/num_to_str.h"

#include "string/ext/locale_util.h"

namespace i3
{
	namespace detail
	{
		I3_EXPORT_BASE i3::c_locale_type	g_locale;
	
		struct global_locale_raii_
		{
			global_locale_raii_()
			{
				g_locale = ::_get_current_locale();
			}
			~global_locale_raii_()
			{
				::_free_locale(g_locale);
			}
		} _global_locale_raii_;
	}
}


