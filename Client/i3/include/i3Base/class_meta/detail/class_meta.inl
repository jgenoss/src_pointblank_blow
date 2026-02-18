
namespace i3
{

	inline unsigned int calc_crc32( const char* str, size_t len )
	{
		return ::CRC32( 0xFFFFFFFF, reinterpret_cast<unsigned char*>(const_cast<char*>(str)), len);
	}

	inline unsigned int calc_crc32( const i3::fixed_string& str)
	{
		return calc_crc32(str.c_str(), str.size());
	}
	
	namespace detail
	{
	
#if !defined(_DEBUG)

		typedef i3::element*	(*create_element_fun_type)();

		template<class T>
		i3::element*	create_element_impl()
		{
			void* raw_data = T::static_meta()->allocate();			//메타안에는 이미 정렬바이트 정보가 포함되있다.
			return ::new (raw_data) T;
		}
		inline i3::element*	create_abstract_element_impl() { return nullptr; }

#else

		typedef i3::element*	(*create_element_fun_type)(const char* file, int line);

#pragma push_macro("new")
#undef new
		template<class T> 
		i3::element*	create_element_impl(const char* file, int line)
		{
			void* raw_data = T::static_meta()->allocate(file, line);		//메타안에는 이미 정렬바이트 정보가 포함되있다.
			T* res = ::new (raw_data) T;
			i3::element_on_create_file_line(res, file, line);
			return res;
		}
		inline i3::element*	create_abstract_element_impl(const char* file, int line) { return nullptr; }
#pragma pop_macro("new")

#endif

		template<class T, class Bool>	
		struct select_create_element_if_abs 
		{	
			static  create_element_fun_type call() { return &create_abstract_element_impl; }
		};

		template<class T>				
		struct select_create_element_if_abs<T, false_type> 
		{	
			static create_element_fun_type call() { return &create_element_impl<T>; }
		};
		
		template<class T>
		struct select_create_element 
		{
			static create_element_fun_type call()
			{
				typedef typename select_create_element_if_abs<T, typename i3::is_abstract<T>::type >	fun_type;
				return fun_type::call();
			}
		};
	
	}

#pragma warning(push )
#pragma warning(disable: 4355)

	template<class T, class B>
	class_meta::class_meta(const char* classname, i3::identity<T> t, i3::identity<B> b) : 
		m_rtti_info(b), m_pool_info(t), m_creation_meta(this), m_ghost_meta(this), 
		m_creation_fun(detail::select_create_element<T>::call() ),
		m_parent(0), m_class_name(classname), m_crc( calc_crc32(m_class_name)), 
		m_cms( (i3::is_abstract<T>::value) ? cms_abstract : cms_normal ),
		m_is_abstract_class( i3::is_abstract<T>::value )
#if defined(_DEBUG)
		, m_curr_alloc_count(0), m_total_alloc_count(0), m_max_alloc_count(0)
#endif
	{
		class_meta::register_meta();
	}
#pragma warning(pop)

}
