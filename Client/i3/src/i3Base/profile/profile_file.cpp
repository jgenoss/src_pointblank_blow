#include "i3CommonType.h"
#include "profile/profile_file.h"
#include "profile/profile_core.h"
#include "string/stack_string.h"

namespace i3
{
	namespace profile
	{

		file_node*	file_node::create_file_node_frameupdate_base(const node* root_node)
		{
			if (root_node->get_child() == nullptr)
				return nullptr;

			file_node* file_root = new file_node(root_tag());
			NetworkDump_Malloc( file_root, sizeof(file_node), __FILE__, __LINE__);
			
			frameupdate_base_tag tag;

			for (const node* child = root_node->get_child(); child != 0;
				child = child->get_sibling() )
			{
				file_node* f_n = new file_node(tag, child, child);
				NetworkDump_Malloc( f_n, sizeof(file_node), __FILE__, __LINE__);
				file_root->m_child_list.push_back(f_n);
				file_node::create_file_node_recursive(tag, child, child, f_n);
			}

			return file_root;
		}
		
		file_node*	file_node::create_file_node_currframe_base(const node* root_node)
		{
			if (root_node->get_child() == nullptr)
				return nullptr;

			file_node* file_root = new file_node(root_tag());
			NetworkDump_Malloc( file_root, sizeof(file_node), __FILE__, __LINE__);
			
			currframe_base_tag	tag;

			for (const node* child = root_node->get_child(); child != 0;
				child = child->get_sibling() )
			{
				file_node* f_n = new file_node(tag, child, child);
				NetworkDump_Malloc( f_n, sizeof(file_node), __FILE__, __LINE__);
				file_root->m_child_list.push_back(f_n);
				file_node::create_file_node_recursive(tag, child, child, f_n);
			}

			return file_root;
		}

		void file_node::create_file_node_recursive(frameupdate_base_tag tag, 
				const node* parent, const node* first_child, file_node* file_parent)
		{
			for (const node* child = parent->get_child(); 
				child != 0; child = child->get_sibling())
			{
				file_node* f_n = new file_node(tag, child, first_child);
				NetworkDump_Malloc( f_n, sizeof(file_node), __FILE__, __LINE__);
				file_parent->m_child_list.push_back(f_n);				
				
				file_node::create_file_node_recursive(tag, child, first_child, f_n);
			}
		}

		void file_node::create_file_node_recursive(currframe_base_tag tag, 
				const node* parent, const node* first_child, file_node* file_parent)
		{
			for (const node* child = parent->get_child(); 
				child != 0; child = child->get_sibling())
			{
				file_node* f_n = new file_node(tag, child, first_child);
				NetworkDump_Malloc( f_n, sizeof(file_node), __FILE__, __LINE__);
				file_parent->m_child_list.push_back(f_n);				
				
				file_node::create_file_node_recursive(tag, child, first_child, f_n);
			}
		}		


		// 루트에는 특별이 기록할 것이 없도록 해야한다..
		file_node::file_node(root_tag) : m_name(nullptr), m_num_call(0), m_total_time(0),
			m_msTime_per_frame(0.0), m_msTime_per_call(0.0), m_percent_per_parent(0.f), m_percent_per_all(0.f),
			m_num_call_per_frame(0.f), m_is_root(true)
		{
			
		}

		file_node::file_node(frameupdate_base_tag, const node* curr_node, const node* first_child) :
			m_name(curr_node->get_name()), m_num_call(curr_node->get_num_call()), 
			m_total_time(curr_node->get_total_time()), m_is_root( curr_node == first_child )
		{
			
			file_node::initialize_common(curr_node->get_parent()->get_total_time(), first_child->get_total_time(),
				first_child->get_num_call());
		}
			
		file_node::file_node(currframe_base_tag, const node* curr_node, const node* first_child) :
			m_name(curr_node->get_name()), m_num_call(curr_node->get_num_elapsed_call()), 
			m_total_time(curr_node->get_elapsed_time()), m_is_root( curr_node == first_child )
		{

			file_node::initialize_common(curr_node->get_parent()->get_elapsed_time(), first_child->get_elapsed_time(),
				first_child->get_num_elapsed_call());
		}

		void	file_node::initialize_common(double parent_total_time, double root_total_time, int num_root_call)
		{
			if (parent_total_time > DBL_MIN)
				m_percent_per_parent = (float)(m_total_time/ parent_total_time * 100.0);
			else
				m_percent_per_parent = 0.f;
			
			if (num_root_call > 0)
			{
				if (root_total_time > DBL_MIN)
					m_percent_per_all = (float)( m_total_time / root_total_time * 100.0);
				else
					m_percent_per_all = 0.f;
				m_msTime_per_frame = m_total_time / (double)(num_root_call) * 1000.0;
				m_num_call_per_frame = (float)(m_num_call) / (float)(num_root_call);
			}
			else
			{
				m_percent_per_all = 0.f;
				m_msTime_per_frame = 0.0;
				m_num_call_per_frame = 0.f;				
			}

			if ( m_num_call > 0)
				m_msTime_per_call = m_total_time / (double)( m_num_call ) * 1000.0;
			else
				m_msTime_per_call = 0.0;		
		}

		file_node::~file_node()
		{
			i3::cu::for_each_delete(m_child_list);	
		}
		
		
		// static함수이다..쓰레드별로 하나씩이다..
		void	file_node::save_file(FILE* fp)
		{
			// 첫 루트에는 정보가 없기 때문에, 자식부터 센다..
			save_recursive(fp, 0); 
		}
		
		void	file_node::save_main_info(FILE* fp, int indent, const char* str_indent)
		{
			if (m_is_root)
			{
				fprintf_s(fp,"%s%-64.64s    %-12s%-12s%-12.5f%-12.5f%-12.5f\n", str_indent, m_name, 
					"N/A", "N/A", m_msTime_per_frame, m_msTime_per_call, m_num_call_per_frame);						
			}
			else
			{
				if (m_num_call)
				{
					fprintf_s(fp,"%s%-64.64s    %-12.3f%-12.3f%-12.5f%-12.5f%-12.5f\n", str_indent, m_name, 
						m_percent_per_parent, m_percent_per_all, m_msTime_per_frame, m_msTime_per_call, m_num_call_per_frame);			
				}
				else
				{
					fprintf_s(fp,"%s%-64.64s    %-12s%-12s%-12s%-12s%-12s\n", str_indent, m_name, 
						"NotRun", "NotRun", "NotRun", "NotRun", "NotRun");								
				}
			}
			
			file_node::save_recursive(fp, ++indent);
		}

		void	file_node::save_recursive(FILE* fp, int indent)
		{
			const size_t num_child = m_child_list.size();
			if (num_child == 0) return;		// 자식이 없으면 표형식부터 제낌..
			
		//	i3::stack_string str_indent(indent, '\t');
			i3::stack_string str_indent(indent*4, ' ');		// 한줄이 너무 길어지는 정보범람의 문제로 인덴트를 스페이스4개로 변경한다.
			const char* psz_indent = str_indent.c_str();
						
			fprintf_s(fp, "\n");
			fprintf_s(fp, "%s==================================================================================================================================\n",psz_indent);
			fprintf_s(fp, "%s%-64.64s    %-12s%-12s%-12s%-12s%-12s\n", psz_indent,"Function","%/Parent", "%/Root","ms/Root","ms/Call","Call/Root" );
			fprintf_s(fp, "%s==================================================================================================================================\n",psz_indent);
			
			
			for (size_t i = 0 ; i < num_child ; ++i)
			{
				m_child_list[i]->save_main_info(fp, indent, psz_indent);
			}
			
			fprintf_s(fp, "%s==================================================================================================================================\n",psz_indent);
			fprintf_s(fp, "\n");
			
		}


	}
}