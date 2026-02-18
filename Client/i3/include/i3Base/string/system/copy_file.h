#pragma once

//
// src 루트 기준 상대경로에 존재하는 파일 1개를 dest루트 상대경로에 복사하는 함수.
// dest 루트 경로에 실제 복사파일 상대경로들이 존재하지 않으면 알아서 생성해줌..
// 

namespace i3
{
	I3_EXPORT_BASE bool copy_file( const i3::wliteral_range& src_root_path, 
		const i3::wliteral_range& src_rel_path_filename, 
		const i3::wliteral_range& dest_root_path, bool overwrite_enable = true);

	I3_EXPORT_BASE bool copy_file( const i3::literal_range& src_root_path, 
		const i3::literal_range& src_rel_path_filename, 
		const i3::literal_range& dest_root_path, bool overwrite_enable = true);

	I3_EXPORT_BASE bool copy_file( const i3::wliteral_range& src_root_path, 
		const i3::wliteral_range& src_rel_path, const i3::wliteral_range& only_filename,
		const i3::wliteral_range& dest_root_path, bool overwrite_enable = true);

	I3_EXPORT_BASE bool copy_file( const i3::literal_range& src_root_path, 
		const i3::literal_range& src_rel_path, const i3::literal_range& only_filename,
		const i3::literal_range& dest_root_path, bool overwrite_enable = true);

}
