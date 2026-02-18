#pragma once



namespace i3
{
	namespace profile
	{
		class node;

		class file_node
		{
		public:
			struct root_tag{};
			struct frameupdate_base_tag{};
			struct currframe_base_tag{};

			static file_node*	create_file_node_frameupdate_base(const node* root_node);
			static file_node*	create_file_node_currframe_base(const node* root_node);

			void				save_file(FILE* fp);

			~file_node();
		
		private:
			static void create_file_node_recursive(frameupdate_base_tag, const node* parent, const node* first_child, file_node* file_parent);
			static void create_file_node_recursive(currframe_base_tag, const node* parent, const node* first_child, file_node* file_parent);
			
			void				save_recursive(FILE* fp, int indent);
			void				save_main_info(FILE* fp, int indent, const char* str_indent);

			file_node(root_tag);		// 초기화된 값 그대로 사용..
			file_node(frameupdate_base_tag, const node* curr_node, const node* first_child);
			file_node(currframe_base_tag, const node* curr_node, const node* first_child);
			void	initialize_common(double parent_total_time, double root_total_time, int num_root_call);

			const char*		m_name;
			int				m_num_call;

			double			m_total_time;			// 
			double			m_msTime_per_frame;		// 최외각기준 시간..(1단계기준)
			double			m_msTime_per_call;		// 1회당 평균실행시간..

			float			m_percent_per_parent;	// 상위부모기준 시간 비중..
			float			m_percent_per_all;		// 1단계부모 기준 시간 비중
			float			m_num_call_per_frame;	// 프레임당 호출 횟수.
			
			bool			m_is_root;				// 
			i3::vector<file_node*>	m_child_list;	// 자식들..
		};
	}

}