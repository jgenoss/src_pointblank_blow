#pragma once

// 스냅샷 저장을 위해서는 크리티컬 섹션이 필요해서 좀더 보강함..
// 테스트 어플리케이션 실행을 해보니, 저장을 행하지 않는다면 크리티컬섹션은 부하가 되지 않음..
// 
//
// i3Thread->i3::thread를 리팩토링후 쓰레드풀을 따로 구성하려는 작업 예정(2013년 하반기) 전까지는..
// 저장루틴을 별도의 쓰레드로 운용하는 작업은 미룹니다..
//

#include "../pool/class_object_pool.h"

namespace i3 { namespace profile { class node; } }
i3_disable_pool_dbg(i3::profile::node)

namespace i3
{
	// class_object_pool은 해당 타입에 대해서만 메모리 풀로 작동한다..
	namespace profile
	{
		class core;

		class node : public class_object_pool<node>
		{
		public:
			~node();

			node*		find_or_create_child(const char* name, bool skip_autosave);
			
			const char* get_name() const { return m_name; }
			int			get_num_call() const { return m_num_call; }
			double		get_total_time() const { return m_total_time; }
			
			int			get_num_elapsed_call() const { return m_num_elapsed_call; }
			double		get_elapsed_time() const { return m_elapsed_time; }

			node*		get_parent() const { return m_parent; }
			node*		get_sibling() const { return m_sibling; }
			node*		get_child() const	{ return m_child; }
			bool		has_child() const { return m_child != nullptr; }
			
		
			void		start();
			bool		end();		// end의 리턴값은 해당단계 블록의 완전 종료를 묻는다..
			
		
		private:
			node(core* owner);
			node(const char* name, node* parent, bool skip_autosave);
			node(const node* rhs, node* new_parent);
		
			int64_t		get_start_tick() const { return m_start_tick; }
			void		reset_child();
			void		reset_elapsed_time();

			double		m_total_time;
			double		m_elapsed_time;		// 최외각함수 시작시마다 리셋..
		
			int64_t		m_start_tick;

			const char*	m_name;		// 고정 리터럴임..
			int			m_num_call;
			int			m_num_elapsed_call;		// 최외각 함수 시작시마다 리셋..
			int			m_recursion_count;	//

			node*		m_parent;
			node*		m_sibling;		// 연결된 형제
			node*		m_child;		//
			
			core*		m_core;
			bool		m_skip_autosave;
			friend class core;
		};

		class core : public i3::shared_tls<core>
		{	
		public:
			core();
			~core();

			void		start(const char* name, bool skip_autosave);
			void		end();

			void		reset_node();
			void		set_degrade_factor(double val); 
			void		set_degrade_threshold(double val); 
		
			const node*	get_root() const { return m_root; }

			void		clone_root(node*& out_root, i3::vector<i3::rc_string>& inout_msg);		// 스냅샷..(모든 tls변수들을 하나의 파일로 묶어야함)
			
			void		on_create_tls();				// CRTP다형성..여기서, 쓰레드아이디들을 모아서 매니저에 등록해두는게 좋다.
														// save/reset_node시에 tls의 독립규칙을 깨서, 모든 객체에 저장시도를 한다...
			void		on_elapsed_time_checking(node* curr_node, double call_time);

		private:
			bool		is_entered_profile() const { return m_entered; }
			void		reserve_process_save(const char* filename, bool snapshot);
			void		reserve_reset() { m_reset_reserved = true; }
			node*		m_root;
			node*		m_cursor;						// 현재 노드..
			
			double					m_degrade_factor;		//	
			double					m_degrade_threshold;	//

			i3::string				m_reserved_filename;	// 
			i3::vector<i3::rc_string>	m_overload_msg_list;	//

			i3::cs		m_cs;							// 최외각의 start-end구간과 저장/리셋호출간에 크리티컬섹션 요구된다..
			bool		m_entered;						//
			bool		m_save_reserved;				//
			bool		m_save_snapshot;				//
			bool		m_reset_reserved;
			friend class manager;
		};

		class manager;
	}
		// 매니저 클래스..
//	template<> struct use_delayed_atexit_ginst<profile::manager> : true_type {};
	
	namespace profile
	{
		class I3_EXPORT_BASE manager : public shared_ginst<manager>
		{
		public:
			manager();
			~manager();
			void	start(const char* name, bool skip_autosave = false) { m_core_ptr->start(name, skip_autosave); }
			void	end()					{ m_core_ptr->end(); }

			void	reset();						// 전체 쓰레드 전파가 필요함...
			void	set_degrade_factor(double val); // 전체 쓰레드 전파가 필요함...
			void	set_degrade_threshold(double val); // 전체 쓰레드 전파가 필요함...
			
			double	get_degrade_factor() const { return m_degrade_factor; }
			double	get_degrade_threshold() const {  return m_degrade_threshold; }
			void	save(const char* file_name, bool snapshot);		// 전체 쓰레드 전파가 필요함...
						
		private:
			void	process_save(const char* file_name, bool snapshot);
			void	process_reset();
			void	add_thread_id(DWORD id);
			
	
			friend class core;
			
			i3::tls_ptr<core>	m_core_ptr;
			i3::vector<DWORD>	m_vec_hthread;			// 크리티컬섹션이 필요함..(자주 호출되지 않게 해야한다..)
			i3::cs				m_cs;					// 쓰레드아이디들을 모을때와 아이디 조회시에 사용한다..
			double					m_degrade_factor;		//	
			double					m_degrade_threshold;	//

		};

		struct scope
		{
			scope( const char* name, bool skip_autosave = false )	{ manager::i()->start(name, skip_autosave); }
			~scope()					{ manager::i()->end(); }
		};

	}
}


