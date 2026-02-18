#include "i3CommonType.h"
#include "profile/profile_core.h"
#include "profile/profile_file.h"

#include "string/ext/num_to_str.h"

#include <strsafe.h>			//

//
#pragma warning(disable : 4355)
//

namespace
{
	inline double	calculate_tick_rate()
	{
		__int64 curr_rate = 0;
		::QueryPerformanceFrequency((LARGE_INTEGER*)&curr_rate);
		return (double)curr_rate;
	}
	
	inline void		calculate_tick(__int64* ticks)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)ticks);
	}
}

namespace i3
{
	namespace profile
	{
		double tick_rate = calculate_tick_rate();		// 전역변수로 고정..
		
		node::node(core* owner) : m_name(0), 
			m_num_call(0), m_total_time(0), m_start_tick(0), m_recursion_count(0),
			m_parent(0), m_child(0), m_sibling(0), m_elapsed_time(0.0), m_num_elapsed_call(0),
			m_core(owner), m_skip_autosave(false)
		{
			
		}

		node::node(const char* name, node* parent, bool skip_autosave) : m_name(name), 
			m_num_call(0), m_total_time(0), m_start_tick(0), m_recursion_count(0),
			m_parent(parent), m_child(0), m_sibling(0), m_elapsed_time(0.0), m_num_elapsed_call(0),
			m_core(parent->m_core), m_skip_autosave(skip_autosave)
		{
			
		}
		
		node::node(const node* rhs, node* new_parent) : 
			m_name(rhs->m_name), m_num_call(rhs->m_num_call),
			m_total_time(rhs->m_total_time), m_start_tick(rhs->m_start_tick),
			m_recursion_count(rhs->m_recursion_count), m_parent(new_parent),
			m_child(0), m_sibling(0), m_elapsed_time(rhs->m_elapsed_time), 
			m_num_elapsed_call(rhs->m_num_elapsed_call), m_core(nullptr), m_skip_autosave(rhs->m_skip_autosave) // 이 생성자의 core는 활용하지 않는다.
		{
			node* rhs_child = rhs->m_child;
			while( rhs_child )
			{
				node* new_n = new node(rhs_child, this);
				NetworkDump_Malloc( new_n, sizeof(node), __FILE__, __LINE__);
				new_n->m_sibling = m_child;
				m_child = new_n;
				rhs_child = rhs_child->m_sibling;
			}
		}
		
		node::~node()
		{
			NetworkDump_Free( m_child, __FILE__, __LINE__);
			NetworkDump_Free( m_sibling, __FILE__, __LINE__);
			delete m_child;
			delete m_sibling;
		}

		node* node::find_or_create_child(const char* name, bool skip_autosave)
		{
			node* child = m_child;
			while (child)
			{
				if ( child->m_name == name )
				{
					if (!m_parent)							// 현재 루트노드임..
						child->reset_elapsed_time();		//
					return child;
				}
				
				child = child->m_sibling;
			}
			// 못찾으면 생성하고 연결하고 리턴한다.
			node*  n = new node(name, this, skip_autosave);
			NetworkDump_Malloc( n, sizeof(node), __FILE__, __LINE__);
			n->m_sibling = m_child;
			m_child = n;
			return n;
		}

		void  node::reset_child()		// 루트에서만 사용될것임..
		{
			NetworkDump_Free( m_child, __FILE__, __LINE__);
			NetworkDump_Free( m_sibling, __FILE__, __LINE__);
			delete m_child;		m_child = nullptr;
			delete m_sibling;	m_sibling = nullptr;
		}
		
		void	node::reset_elapsed_time()		// 외부에서는 루트에서만 사용..재귀적으로 처리..
		{
			m_elapsed_time = 0.0;
			m_num_elapsed_call = 0;
			
			node* child = m_child;
			while (child)
			{
				child->reset_elapsed_time();
				child = child->m_sibling;
			}
		}

		void node::start()
		{
			++m_num_call;	++m_num_elapsed_call;	
			
			if ( m_recursion_count++ == 0)
				calculate_tick(&m_start_tick);
		}
		
		// end()함수부 처리에서의 time값에 커스터마이징 여지가 있다..
		bool node::end()
		{
			if (--m_recursion_count != 0)
				return false;

			int64_t time;
			calculate_tick(&time);

			double  call_time = (double)(time - m_start_tick) / tick_rate;
			m_total_time += call_time;
			m_elapsed_time += call_time;
		
			if (!m_skip_autosave)
				m_core->on_elapsed_time_checking(this, call_time);

			return true;
		}

		core::core() : m_root( new node(this) ), m_cursor(m_root), m_save_reserved(false), m_entered(false), m_save_snapshot(false),
			m_degrade_factor(100.0), m_degrade_threshold(0.01), m_reset_reserved(false)	//
		{
				
		}
		
		core::~core()
		{
			NetworkDump_Free( m_root, __FILE__, __LINE__);
			delete m_root;
		}

		// CRTP다형성..매니저에 아이디값을 전달한다..전달시 임계보호필요하다..
		void	core::on_create_tls()
		{
			DWORD my_thread_id = this->get_thread_id();
			manager* mng = manager::i();

			mng->add_thread_id(my_thread_id);

			i3::scoped_cs l(m_cs);			// 커버리티 이슈 처리 (14.11.04 - 10827번)
			m_degrade_factor    = mng->get_degrade_factor();
			m_degrade_threshold = mng->get_degrade_threshold();
		}

		void	core::reset_node()
		{
			// 멀티쓰레드 환경에서는 start_end블록안에 작업중일수도 있기 때문에
			// 보호할 필요가 있다..
			i3::scoped_cs l(m_cs);	
			m_root->reset_child();
		}

		void	core::set_degrade_factor(double val)
		{
			i3::scoped_cs l(m_cs);	
			m_degrade_factor = val;
		}

		void	core::set_degrade_threshold(double val)
		{
			i3::scoped_cs l(m_cs);		
			m_degrade_threshold = val;
		}
				
		void	core::clone_root(node*& out_root, i3::vector<i3::rc_string>& inout_msg)		// 스냅샷..(모든 tls변수들을 하나의 파일로 묶어야함)
		{
			// 멀티쓰레드 환경에서는 start_end블록안에 작업중일수도 있기 때문에
			// 보호할 필요가 있다..
			i3::scoped_cs l(m_cs);	

			out_root = new node(m_root, nullptr);
			NetworkDump_Malloc( out_root, sizeof(node), __FILE__, __LINE__);
			inout_msg.insert(inout_msg.end(), m_overload_msg_list.begin(), m_overload_msg_list.end());
			m_overload_msg_list.clear();

		}
		
		
		void core::start(const char* name, bool skip_autosave)
		{
			if (name != m_cursor->get_name() )
			{
				if (m_cursor == m_root)
				{
					m_cs.lock();
					m_entered = true;
				}

				m_cursor = m_cursor->find_or_create_child(name, skip_autosave);
			}
			m_cursor->start();
		}
		
		void core::end()
		{
			if (m_cursor->end())
				m_cursor = m_cursor->get_parent();

			if (m_cursor == m_root)
			{
				m_entered = false;
				m_cs.unlock();			// 최상단에서 임계보호해제..
				
				if (m_save_reserved)
				{
					manager::i()->process_save(m_reserved_filename.c_str(), m_save_snapshot);
					m_save_reserved = false;
				}
				
				if (m_reset_reserved)
				{
					manager::i()->process_reset();
					m_reset_reserved = false;
				}
			}
		}
		
		void	 core::reserve_process_save(const char* filename, bool snapshot)
		{
			m_reserved_filename = filename;
			m_save_snapshot = snapshot;
			m_save_reserved = true;
		}
		
		void	core::on_elapsed_time_checking(node* curr_node, double call_time)
		{

			//
			//	call_time			// 해당 함수 처리 시간은 매개변수로 받고..
			//  curr_node->get_total_time() / curr_node->get_num_call() // 평균 함수 처리 시간
			//
			//  1. 이 2개의 차이가 급격하게 벌어지면, 프레임이 튄다고 가정할수 있다..(기본 가정은 100배로 한다..)
			//  2. 문턱치 millsecond를 두는게 좋다.. (1번과 2번을 동시에 만족할때 저장하도록 조정)
			//  3. 로딩의 경우, 프레임이 튀는것이 당연하기 때문에 이 경우 스킵하는 것도 필요하다... ( 이건 나중에 ) 
			//  자동저장기능임을 파일에 표시하고, 원인 함수를 밝혀서 문자열로 보관하고, 스냅샷을 남길때 원인함수를 출력해준다..
			//  (I3TRACE를 통해 아웃풋창에 파일 저장을 알려주도록 한다)
			
			if (call_time < m_degrade_threshold )
				return;

			double avg_call_time =  curr_node->get_total_time() / curr_node->get_num_call();
			
			if ( call_time > avg_call_time * m_degrade_factor )
			{
				i3::string	tmp("The following function has been suspected as sudden performance degrader by "); 
				i3::string	str_percent;		i3::string str_ms;

				i3::str_conv_option::set_precision(2);
				i3::str_conv_option::set_precision_mode(precision_mode::underpoint);
				i3::real_to_str(call_time / avg_call_time, str_percent);

				i3::str_conv_option::set_precision(5);
				i3::real_to_str(call_time * 1000.0 , str_ms);

				tmp += str_percent;
				tmp += " times!!\n";
				tmp += curr_node->get_name();
				tmp += " : ";
				tmp += str_ms;
				tmp += "ms\n";
				
				m_overload_msg_list.push_back(tmp);
				
				I3TRACE(tmp);
				
				SYSTEMTIME		SystemTime;
				//Cal Local Time
				::GetLocalTime( &SystemTime );	
				INT32 Year		=	SystemTime.wYear;
				INT32 Month		=	SystemTime.wMonth; 
				INT32 Day		=	SystemTime.wDay; 	
				INT32 Hour		=	SystemTime.wHour; 
				INT32 Minute	=	SystemTime.wMinute; 		
				INT32 Second	=	SystemTime.wSecond;
				INT32 iDate	= (Year * 10000) + (Month) * 100 + Day;
				
				char tempDate[MAX_PATH];
				::StringCchPrintfA(tempDate, _countof(tempDate), "_%d_%02d_%02d_%02d_", iDate, Hour, Minute, Second);
				
				i3::string tmp_filename( "profile_sudden_degrade");
				tmp_filename += tempDate;
				tmp_filename += ".txt";

				core::reserve_process_save(tmp_filename.c_str(), true);
			}
		}
	
		manager::manager() : m_degrade_factor(100.0), m_degrade_threshold(0.01)	//
		{

		}

		manager::~manager()
		{

		}

		void	manager::add_thread_id(DWORD id)
		{
			i3::scoped_cs l(m_cs);
			m_vec_hthread.push_back(id);
		}

		void	manager::process_reset()
		{
			i3::vector<DWORD> clone_threadid_list;
			m_cs.lock();
			clone_threadid_list = m_vec_hthread;
			m_cs.unlock();

			const size_t num_thread = clone_threadid_list.size();
			DWORD thread_id = 0;
			for (size_t i = 0 ; i < num_thread ; ++i )
			{
				thread_id = clone_threadid_list[i];
				core* c = m_core_ptr.get_raw_by_thread_id_unsafe(thread_id);
				c->reset_node();		//내부에 추가 임계보호됨..
			}
		}

		void	manager::reset()
		{
			core* curr_thread_ptr =	m_core_ptr.get();
			if ( curr_thread_ptr->is_entered_profile() )
				curr_thread_ptr->reserve_reset();
			else
				process_reset();
		}

		void	manager::set_degrade_factor(double val) // 전체 쓰레드 전파가 필요함...
		{
			m_degrade_factor = val;
			i3::vector<DWORD> clone_threadid_list;
			m_cs.lock();
			clone_threadid_list = m_vec_hthread;
			m_cs.unlock();

			const size_t num_thread = clone_threadid_list.size();
			DWORD thread_id = 0;
			for (size_t i = 0 ; i < num_thread ; ++i )
			{
				thread_id = clone_threadid_list[i];
				core* c = m_core_ptr.get_raw_by_thread_id_unsafe(thread_id);
				c->set_degrade_factor(val);		//내부에 추가 임계보호됨..
			}

		}

		void	manager::set_degrade_threshold(double val) // 전체 쓰레드 전파가 필요함...
		{
			m_degrade_threshold = val;
			i3::vector<DWORD> clone_threadid_list;
			m_cs.lock();
			clone_threadid_list = m_vec_hthread;
			m_cs.unlock();

			const size_t num_thread = clone_threadid_list.size();
			DWORD thread_id = 0;
			for (size_t i = 0 ; i < num_thread ; ++i )
			{
				thread_id = clone_threadid_list[i];
				core* c = m_core_ptr.get_raw_by_thread_id_unsafe(thread_id);
				c->set_degrade_threshold(val);		//내부에 추가 임계보호됨..
			}

		}

		void	manager::save(const char* file_name, bool snapshot)
		{
			core* curr_thread_ptr =	m_core_ptr.get();
			if ( curr_thread_ptr->is_entered_profile() )
				curr_thread_ptr->reserve_process_save(file_name, snapshot);
			else
				process_save(file_name, snapshot);
		}

		void	manager::process_save(const char* file_name, bool snapshot)
		{
			i3::vector<DWORD>	clone_threadid_list;
			i3::vector<node*>	clone_list;
			i3::vector<i3::rc_string>	overload_msg_list;

			m_cs.lock();
			clone_threadid_list = m_vec_hthread;
			m_cs.unlock();

			const size_t num_thread = clone_threadid_list.size();			
			DWORD thread_id = 0;
			
			for (size_t i = 0 ; i <num_thread; ++i)
			{
				thread_id = clone_threadid_list[i];
				core* c = m_core_ptr.get_raw_by_thread_id_unsafe(thread_id);
				node* clone_n;
				c->clone_root(clone_n, overload_msg_list);
				clone_list.push_back(clone_n);
			}
			
			file_node*	(*afn_create_file_node[2])(const node*) =	{ 
																		&file_node::create_file_node_frameupdate_base, 
																		&file_node::create_file_node_currframe_base,
																	};

			file_node*	(*fn_create_file_node)(const node*) = afn_create_file_node[snapshot];
		
			// 이제 클론된 놈을 기준으로 파일 작성하면 되고, 마지막에 
			// 노드 소거 잊지 말것..
			i3::vector<file_node*>	file_node_list;
			for (size_t i = 0 ; i <num_thread; ++i)
			{
				file_node* file_root =	(*fn_create_file_node)(clone_list[i]);
				file_node_list.push_back(file_root);
			}
			i3::cu::for_each_delete_clear(clone_list);
			//
			
			FILE* fp = nullptr;
			errno_t err = ::fopen_s(&fp, file_name, "wt");
			if (err != 0) 
				return;
			
			const char*	apsz_profiler_category[2] = { "  - frame update based mode -\n" , " - frame snapshot based mode -\n" };
			const char* psz_category = apsz_profiler_category[snapshot];

			fprintf_s(fp,"=================================\n");
			fprintf_s(fp,"   i3::profiler version 1.0\n");
			fprintf_s(fp, psz_category);
			fprintf_s(fp,"=================================\n");
				
			if ( !overload_msg_list.empty())
			{
				fprintf_s(fp,"\n");
				const size_t num = overload_msg_list.size();

				for (size_t i = 0 ; i < num ; ++i )
				{
					const i3::rc_string& str_msg = overload_msg_list[i];
					fprintf_s(fp, str_msg.c_str());
				}
			}

			for (size_t i = 0 ; i < num_thread; ++i)
			{
				DWORD thread_id = clone_threadid_list[i];
				file_node* file_root = file_node_list[i];
				if (file_root != nullptr)
				{
					fprintf_s(fp, "\n");
					fprintf_s(fp, "current thread id = %d \n", thread_id);
					file_root->save_file(fp);
				}
			}

			fprintf_s(fp,"================================\n");
			fprintf_s(fp,"         end of profile\n");
			fprintf_s(fp,"================================\n");

			fclose(fp);
			i3::cu::for_each_delete_clear(file_node_list);
		}
	}
	

	
	
}