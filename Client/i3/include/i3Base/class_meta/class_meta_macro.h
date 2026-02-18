#pragma once

#include "detail/class_meta_macro_impl.h"

namespace i3
{
	// 선언만 하고, 구현을 비운다..
	void element_add_ref(i3::element* p);
	void element_on_create_file_line(i3::element* p, const char* file, int line);

	template<class T> __forceinline
	void destroy_instance(T* p) {	if (!p) return;		p->destroy_instance();	}
	template<class T> __forceinline
	void safe_destroy_instance(T*& p) { if (!p) return;		p->destroy_instance();	p = nullptr; }
}


// new/delete를 private 미구현으로 처리해서, 사용을 금지시킴..

#define I3_CLASS_DEFINE_ROOT_IMPL(T) \
	public:		bool same_of( const i3::class_meta* m) const {  return	meta() == m; } \
				bool kind_of( const i3::class_meta* m) const {  return  meta()->kind_of(m); } \
	private:	void*	operator	new(std::size_t); \
				void*	operator	new[] (std::size_t ); \
				void*	operator	new(std::size_t , void* ); \
				void*	operator	new[] ( std::size_t, void * ); \
				void*	operator	new (std::size_t ,const char *,int ); \
				void*	operator	new[] (std::size_t ,const char *,int ); \
				void	operator	delete(void*, void* ); \
				void	operator	delete[] ( void * , void * ); \
				void	operator	delete(void*, const char*,int ); \
				void	operator	delete[] (void *,const char* ,int ); \
				template<class T> friend i3::class_meta* i3::detail::create_root_meta(const char* ); public:


#define I3_CLASS_DEFINE_COMMON_IMPL(T , B, meta_style) \
	public:		typedef		B				base_type; \
	static  i3::class_meta*					static_meta() { return ms_class_meta; } \
	virtual i3::class_meta*					meta() const { return ms_class_meta; } \
	virtual void							destroy_instance(); \
	static void								create_meta_explicit(); \
	private: static	const i3::class_meta_style	init_meta_style = meta_style; \
	static i3::class_meta*					ms_class_meta; \
	template<class T> friend i3::class_meta* i3::detail::create_class_meta(const char* );


#if !defined(_DEBUG)
#define I3_CLASS_DEFINE_CREATE_IMPL(T) \
	public: 	T*	create_instance_fun() const; \
	static T*		new_object_fun() ; \
	static T*		new_object_ref_fun();
#else
#define I3_CLASS_DEFINE_CREATE_IMPL(T) \
	public:		T*	create_instance_fun(const char* file, int line) const;	\
	static T*		new_object_fun(const char* file, int line); \
	static T*		new_object_ref_fun(const char* file, int line);
#endif


#define I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, meta_style) I3_CLASS_DEFINE_COMMON_IMPL(T, B, meta_style) I3_CLASS_DEFINE_CREATE_IMPL(T)
		

#define I3_PREVENT_DEAD_CODE(T)		__pragma(comment(linker, "/INCLUDE:?ms_class_meta@" #T "@@0PAVclass_meta@i3@@A"))
//#define I3_PREVENT_DEAD_CODE(T)		__pragma(comment(linker, "/INCLUDE:?static_meta@" #T "@@SAPAVclass_meta@i3@@XZ"))
// 릴리즈버전에서 static_meta의 기호강제 포함이 되지 않고 링크에러가 나는데,..이것은 static_meta함수가 인라인되어 obj에서 폐기된것이 이유인듯하다.

#define I3_CLASS_DEFINE_BY_TYPE(T, B, meta_style) I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, meta_style) I3_PREVENT_DEAD_CODE(T)


#define I3_CLASS_DEFINE( T, B )					I3_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_normal) 
#define I3_ABSTRACT_CLASS_DEFINE( T, B )		I3_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_abstract) 
#define I3_CONCRETE_CLASS_DEFINE( T, B )		I3_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_concrete) 
#define I3_GHOST_CLASS_DEFINE(T, B )			I3_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_ghost) 
#define I3_ROOT_CLASS_DEFINE(T)					I3_CLASS_DEFINE_BY_TYPE(T, i3::null_type, i3::cms_abstract) I3_CLASS_DEFINE_ROOT_IMPL(T)


#define I3_EXPORT_CLASS_DEFINE( T, B )				I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_normal) 
#define I3_EXPORT_ABSTRACT_CLASS_DEFINE( T, B )		I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_abstract) 
#define I3_EXPORT_CONCRETE_CLASS_DEFINE( T, B )		I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_concrete) 
#define I3_EXPORT_GHOST_CLASS_DEFINE(T, B )			I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, B, i3::cms_ghost) 
#define I3_EXPORT_ROOT_CLASS_DEFINE(T)				I3_EXPORT_CLASS_DEFINE_BY_TYPE(T, i3::null_type, i3::cms_abstract) I3_CLASS_DEFINE_ROOT_IMPL(T)


#define I3_CLASS_INSTANCE(T)	I3_CLASS_INSTANCE_CREATE_META_IMPL(T) I3_CLASS_INSTANCE_CREATE_IMPL(T)
#define I3_ROOT_CLASS_INSTANCE(T)	I3_ROOT_CLASS_INSTANCE_CREATE_META_IMPL(T) I3_CLASS_INSTANCE_CREATE_IMPL(T)


#if defined(_DEBUG)

#define	create_instance()				create_instance_fun(__FILE__, __LINE__)
#define	create_instance_ghost()			create_instance_ghost_fun(__FILE__, __LINE__)

#define	new_object()					new_object_fun(__FILE__, __LINE__)
#define	new_object_ref()				new_object_ref_fun(__FILE__, __LINE__)

#else

#define	create_instance()				create_instance_fun()
#define	create_instance_ghost()			create_instance_ghost_fun()

#define	new_object()					new_object_fun()
#define	new_object_ref()				new_object_ref_fun()

#endif
