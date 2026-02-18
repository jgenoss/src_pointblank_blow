
namespace i3
{

	template<class T>  __forceinline
	T*		create_inst_memory()			// 소거는 이 어플 수준에서 끝낼까...(T를 알아야함!)
	{
		return detail_inst::g_inst_memory_mgr.create_inst<T>();
	}

	template<class T>	__forceinline
	T*		find_inst_memory()
	{
		return detail_inst::g_inst_memory_mgr.find_inst<T>();
	}

	template<class T>  __forceinline
	void	delete_inst_memory()			// T를 아는 상태에서 소거에 들어가야함...
	{
		return detail_inst::g_inst_memory_mgr.delete_inst<T>();
	}


}