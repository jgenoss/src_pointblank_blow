#pragma once

namespace i3
{
	namespace detail
	{
		template<class T>
		i3::class_meta*		create_class_meta(const char* name)
		{
			if (T::ms_class_meta) return T::ms_class_meta; 
			typedef typename T::base_type			B;
			B::create_meta_explicit(); 
			T::ms_class_meta = new i3::class_meta( name, i3::identity<T>(), i3::identity<B>());
			NetworkDump_Malloc( T::ms_class_meta, sizeof( i3::class_meta), __FILE__, __LINE__);
			B::static_meta()->add_child(T::ms_class_meta); 
			T::ms_class_meta->set_class_meta_style(T::init_meta_style); 
			return T::ms_class_meta; 
		}

		template<class T>
		i3::class_meta*		create_root_meta(const char* name)
		{
			if (T::ms_class_meta) return T::ms_class_meta; 
			i3::class_meta_system::create_explicit(); 
			T::ms_class_meta = new i3::class_meta( name, i3::identity<T>(), i3::identity<i3::null_type>()); 
			NetworkDump_Malloc( T::ms_class_meta, sizeof( i3::class_meta), __FILE__, __LINE__);
			i3::class_meta_system::i()->set_root(T::ms_class_meta); 
			T::ms_class_meta->set_class_meta_style(T::init_meta_style); 
			return T::ms_class_meta; 
		}
	}
	
	namespace detail
	{

#if !defined(_DEBUG)
		template<class T> __forceinline 
		T*	create_instance_for_new_object()
		{
			return (T*)T::static_meta()->create_instance_fun();
		}
#else
		template<class T> __forceinline typename i3::enable_if< is_abstract<T>,
		T*>::type	create_instance_for_new_object(const char* file, int line)
		{
			if (T::static_meta()->has_concrete_creator() == false)
			{
				I3PRINTLOG(I3LOG_FATAL, "%s class is abstract...", typeid(T).name());
				return nullptr;
			}
			return (T*)T::static_meta()->create_instance_fun(file, line);
		}

		template<class T> __forceinline typename i3::disable_if< is_abstract<T>,
		T*>::type	create_instance_for_new_object(const char* file, int line)
		{
			return (T*)T::static_meta()->create_instance_fun(file, line);
		}
#endif		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define I3_CLASS_INSTANCE_CREATE_META_IMPL(T) \
	template i3::class_meta* i3::detail::create_class_meta<T>(const char* ); \
	i3::class_meta*		T::ms_class_meta = i3::detail::create_class_meta<T>(#T); \
	void				T::create_meta_explicit() { i3::detail::create_class_meta<T>(#T); } \
	void				T::destroy_instance() { void* p = this; T::~T(); ms_class_meta->deallocate(p); }


#define I3_ROOT_CLASS_INSTANCE_CREATE_META_IMPL(T) \
	template i3::class_meta* i3::detail::create_root_meta<T>(const char* ); \
	i3::class_meta*		T::ms_class_meta = i3::detail::create_root_meta<T>(#T); \
	void				T::create_meta_explicit() { i3::detail::create_root_meta<T>(#T); } \
	void				T::destroy_instance() { void* p = this; T::~T(); ms_class_meta->deallocate(p); }

#if !defined(_DEBUG)

#define I3_CLASS_INSTANCE_CREATE_IMPL(T) \
	T*		T::create_instance_fun() const { return static_cast<T*>(meta()->create_instance_fun()); } \
	T*		T::new_object_fun() { T* p = i3::detail::create_instance_for_new_object<T>(); i3::element_add_ref(p); return p; } \
	T*		T::new_object_ref_fun() {  return i3::detail::create_instance_for_new_object<T>(); }

//	T*		T::new_object_fun() { T* p = (T*)ms_class_meta->create_instance_fun(); i3::element_add_ref(p); return p; } \
//	T*		T::new_object_ref_fun() {  return (T*)ms_class_meta->create_instance_fun(); }

#else

#define I3_CLASS_INSTANCE_CREATE_IMPL(T) \
	T*		T::create_instance_fun(const char* file, int line) const { return static_cast<T*>(meta()->create_instance_fun(file, line)); }	\
	T*		T::new_object_fun(const char* file, int line){ T* p = i3::detail::create_instance_for_new_object<T>(file, line); i3::element_add_ref(p); return p; } \
	T*		T::new_object_ref_fun(const char* file, int line){  return i3::detail::create_instance_for_new_object<T>(file, line); }

//	T*		T::new_object_fun(const char* file, int line){ T* p = (T*)ms_class_meta->create_instance_fun(file, line); i3::element_add_ref(p); return p; } \
//	T*		T::new_object_ref_fun(const char* file, int line){  return (T*)ms_class_meta->create_instance_fun(file, line); }	
#endif
