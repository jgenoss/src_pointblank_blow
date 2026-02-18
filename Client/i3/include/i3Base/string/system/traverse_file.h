#pragma once

namespace i3
{

	struct I3_EXPORT_BASE traverse_file_callback_wc
	{
		virtual void on_file(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) = 0;
		virtual bool on_dir(const WIN32_FIND_DATAW& fd, const i3::rc_wstring& base_path, const i3::rc_wstring& subdir_path) = 0;

		virtual void on_enter_sub_dir(const i3::rc_wstring& base_path, const i3::rc_wstring& child_path) {}
		virtual void on_leave_sub_dir(const i3::rc_wstring& base_path, const i3::rc_wstring& parent_path) {}

		virtual void on_interruptible(bool& out_interrupt) {}
		virtual ~traverse_file_callback_wc() {}
	};

	struct I3_EXPORT_BASE traverse_file_callback_mb
	{
		virtual void on_file(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path) = 0;
		virtual bool on_dir(const WIN32_FIND_DATAA& fd, const i3::rc_string& base_path, const i3::rc_string& subdir_path) = 0;

		virtual void on_enter_sub_dir(const i3::rc_string& base_path, const i3::rc_string& child_path) {}
		virtual void on_leave_sub_dir(const i3::rc_string& base_path, const i3::rc_string& parent_path) {}

		virtual void on_interruptible(bool& out_interrupt) {}
		virtual ~traverse_file_callback_mb() {}
	};


//
//  기존 i3System의 LookForFiles는 깊이 우선으로 되어있고, 이쪽이 콜백클래스에서 응용하기 좋기 때문에..
//  하위 폴더 검색에 깊이 우선 방식 순회로 처리..
//

	I3_EXPORT_BASE void	traverse_file( const i3::wliteral_range& Path, traverse_file_callback_wc* cb, bool recursive = true);
	I3_EXPORT_BASE void	traverse_file( const i3::literal_range& Path, traverse_file_callback_mb* cb, bool recursive = true);

}

