#pragma once 

#include "intrusive_ptr_add_ref.h"

namespace i3
{
	template<class T> __forceinline
	void null_ref_ptr_add_ref(T* p) {   intrusive_ptr_add_ref(p); }

	template <class T>
	class null_ref_ptr
	{
	public:
		__forceinline operator T* ()  { return ms_pnull_obj; }
		__forceinline operator const T* () const { return ms_pnull_obj; }
		
		__forceinline static T*		get() { return ms_pnull_obj; }
		__forceinline static T*		get_add_ref() { null_ref_ptr_add_ref(ms_pnull_obj); return ms_pnull_obj; }
		__forceinline static T*		safe() {   return create();   }
		__forceinline static T*		safe_add_ref() { null_ref_ptr_add_ref( safe() ); return ms_pnull_obj; }
	private:
		static		T*				create() 
		{   
			if ( !ms_pnull_obj )
			{
				ms_pnull_obj = i3::create_inst_memory<T>();
				null_ref_ptr_add_ref(ms_pnull_obj);
				::atexit(&null_ref_ptr::destroy);	
			}
			return ms_pnull_obj;
		}

		static		void			destroy()	{	i3::delete_inst_memory<T>();				//delete ms_pnull_obj;	
													ms_pnull_obj = nullptr;	}
		
		static		T*				ms_pnull_obj;
	};
	
	template <class T> 
	T* null_ref_ptr<T>::ms_pnull_obj = null_ref_ptr<T>::create();

}
