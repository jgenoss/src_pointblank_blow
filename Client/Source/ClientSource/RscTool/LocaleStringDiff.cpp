#include "stdafx.h"
#include "LocaleStringDiff.h"
#include "i3Base/itl/range/algorithm/find.h"
#include "i3Base/itl/svector.h"
#include "i3Base/itl/bind/bind.h"
#include "i3Base/itl/bind/placeholders.h"

bool LoadLocaleStringForDiff(const i3::wstring& root_path, const i3::const_char_range& rel_path, 
							 LocaleStringSet& out)
{
	out.m_list.clear();
	wchar_t str_curr_dir[MAX_PATH];
	DWORD num_curr_dir_char = ::GetCurrentDirectoryW(MAX_PATH, str_curr_dir);

	::SetCurrentDirectoryW( root_path.c_str() );
	
	i3FileStream file;
	BOOL res = file.Open(&rel_path[0], STREAM_READ|STREAM_SHAREREAD);
	
	if ( res )
	{
		WCHAR16 wszReadLine[1024];		// 이런 코드 싫다..
		i3::rc_wstring strReadLine;
		while( file.GetLastError() != i3Stream::STREAM_EOS)
		{
			INT32 nRead = file.ReadLineW(wszReadLine, sizeof(wszReadLine) );
			if ( nRead == 0 )
				continue;
			
			std::size_t wszReadLineLen = i3::generic_string_size(wszReadLine);
			i3::const_wchar_range rng( wszReadLine, wszReadLine + wszReadLineLen );
		
			if (rng.empty())
				continue;

			i3::const_wchar_range::iterator it = i3::range::find(rng, L'\t');

			if ( it != rng.end() )
			{
				LocaleStringPtr p = i3::make_shared<LocaleString>();
				p->m_key.assign( rng.begin(), it);
				p->m_val.assign( ++it, rng.end());
				out.m_list.push_back(p);
			}
			else
			{
				LocaleStringPtr p = i3::make_shared<LocaleString>();
				p->m_key.assign( rng.begin(), it);
				p->m_val.clear();
				out.m_list.push_back(p);
			}
		}
		
	}
	::SetCurrentDirectoryW( str_curr_dir );
	
	return res != FALSE;
}

void LocaleStringDiff_Accept::move_function(LocaleStringDiff_Accept::fun_t& f)
{
	m_visit_mem_fun.swap(f);
}

void LocaleStringDiff_Accept::visit(LocaleStringDiff_Visitor* v)
{
	m_visit_mem_fun(v);
}

namespace
{
	void CompareAndSetStringDiff(const LocaleString* l, const LocaleString* r,
								 LocaleStringDiffSet& out)
	{
		// 두개의 키는 일단 동일한 것으로 가정..
		if ( l->m_val == r->m_val)		// 값이 같다면 무시한다..
			return;
		
		typedef i3::function<void (LocaleStringDiff_Visitor*)> function_type;

		using namespace i3;

		function_type f( i3::bind( &LocaleStringDiff_Visitor::Modified, _1, l, r ) );

		LocaleStringDiff_AcceptPtr ptr = i3::make_shared<LocaleStringDiff_Accept>();
		ptr->move_function(f);
		out.m_list.push_back(ptr);
	}
	
	void SetStringDiff_Added( const LocaleString* r, LocaleStringDiffSet& out)
	{
		using namespace i3;

		typedef i3::function<void (LocaleStringDiff_Visitor*)> function_type;

		function_type f (i3::bind( &LocaleStringDiff_Visitor::Added, _1, r ) );

		LocaleStringDiff_AcceptPtr ptr = i3::make_shared<LocaleStringDiff_Accept>();
		ptr->move_function(f);
		out.m_list.push_back(ptr);
	}

	void SetStringDiff_Removed( const LocaleString* l, LocaleStringDiffSet& out)
	{
		using namespace i3;

		typedef i3::function<void (LocaleStringDiff_Visitor*)> function_type;

		function_type f (i3::bind( &LocaleStringDiff_Visitor::Removed, _1, l ) );

		LocaleStringDiff_AcceptPtr ptr = i3::make_shared<LocaleStringDiff_Accept>();
		ptr->move_function(f);
		out.m_list.push_back(ptr);
	}	


}

bool FindLocaleStringDiff( const LocaleStringSet& lhs, const LocaleStringSet& rhs, 
						  LocaleStringDiffSet& out)
{
	out.m_list.clear();

	// 데이터가 상당이 많기 때문에 인덱스검색에 신경을 좀 써야한다..
	const size_t num_lhs = lhs.m_list.size();
	const size_t num_rhs = rhs.m_list.size();
	
	i3::svector<bool>	lhs_found_list(num_lhs, false);
	i3::svector<bool>	rhs_found_list(num_rhs, false);

	const size_t num_l_r = (num_lhs < num_rhs) ? num_lhs : num_rhs;

	for (size_t i = 0 ; i < num_l_r ; ++i )
	{
		const LocaleString* rhs_data = rhs.m_list[i].get();
	
		if ( lhs_found_list[i] == false)
		{
			const LocaleString* lhs_data = lhs.m_list[i].get();
			
			if ( rhs_data->m_key == lhs_data->m_key )
			{
				CompareAndSetStringDiff(lhs_data, rhs_data, out);
				lhs_found_list[i] = true;		rhs_found_list[i] = true;
				continue;
			}
		}

		bool	proceed_1 = true;
		bool	proceed_2 = true;	
		bool	found_key = false;

		for (size_t diff_count = 1; proceed_1 || proceed_2 ; ++diff_count)
		{
			if ( i > diff_count )
			{
				if ( lhs_found_list[i - diff_count] == false )
				{
					const LocaleString* lhs_data = lhs.m_list[ i - diff_count ].get();

					if (rhs_data->m_key == lhs_data->m_key)
					{
						CompareAndSetStringDiff(lhs_data, rhs_data, out);
						lhs_found_list[i - diff_count] = true;		
						rhs_found_list[i] = true;
						break;
					}
				}
			}
			else
			{
				proceed_1 = false;
			}
			
			if ( i + diff_count < num_lhs )
			{
				if ( lhs_found_list[ i + diff_count] == false )
				{
					const LocaleString* lhs_data = lhs.m_list[i + diff_count ].get();
					
					if (rhs_data->m_key == lhs_data->m_key)
					{
						CompareAndSetStringDiff(lhs_data, rhs_data, out);
						lhs_found_list[i + diff_count] = true;
						rhs_found_list[i] = true;
						break;
					}

				}
			}
			else
			{
				proceed_2 = false;
			}
		}
		
	}
	
	for (size_t i = num_l_r ; i < num_rhs ; ++i )
	{
		const LocaleString* rhs_data = rhs.m_list[i].get();

		for (size_t j = 0 ; j < num_lhs ; ++j)	// 거꾸로 추적하는게 좋을것이다.
		{
			size_t k = num_lhs - j - 1;

			const LocaleString* lhs_data = lhs.m_list[k].get();

			if ( rhs_data->m_key == lhs_data->m_key )
			{
				CompareAndSetStringDiff(lhs_data, rhs_data, out);
				lhs_found_list[k] = true;		rhs_found_list[i] = true;
				break;
			}
		}
	}

	for (size_t i = 0 ; i < num_rhs ; ++i )
	{
		if ( rhs_found_list[i] == false )
		{
			const LocaleString* rhs_data = rhs.m_list[i].get();
			SetStringDiff_Added(rhs_data, out);
		}
	}
	
	for (size_t i = 0; i < num_lhs ; ++i )
	{
		if (lhs_found_list[i] == false)
		{
			const LocaleString* lhs_data = lhs.m_list[i].get();
			SetStringDiff_Removed(lhs_data, out);
		}
	}
	
	return !out.m_list.empty();
}


void VisitLocaleStringDiff( const LocaleStringDiffSet& in, LocaleStringDiff_Visitor* v)
{
	const size_t num = in.m_list.size();

	for (size_t i = 0 ; i < num ; ++i)
	{
		in.m_list[i]->visit(v);
	}
}


