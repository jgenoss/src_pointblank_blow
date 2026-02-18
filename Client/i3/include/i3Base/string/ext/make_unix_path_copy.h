#pragma once

// MakeUnixPath의 일반화..
#include "../../itl/range/literal_begin.h"
#include "../../itl/range/literal_end.h"
#include "make_unix_path.h"

namespace i3
{
	// 이래저래 고민해보았는데, 아웃풋객체를 비운후 요소하나씩 뒤에 삽입하는것보다는..
	// 그냥 통으로 카피한 후, 카피된 객체에서 replace하는것이 더 나은것 같다...
	// i3::copy 혹은 컨테이너의 대입은 거의 memcpy와 성능 등가이기 때문임..
	//
	template<class InOutRange> inline typename i3::disable_if_iterator<InOutRange>::type
	make_unix_path_copy(const InOutRange& input_path, InOutRange& out_path)
	{
		out_path = input_path;
		i3::make_unix_path(out_path);
	}
	
	template<class InputRange, class OutputRange> inline typename i3::disable_if_iterator<OutputRange>::type
	make_unix_path_copy(const InputRange& input_path, OutputRange& out_path)
	{
		OutputRange dest_holder(i3::literal_begin(input_path), i3::literal_end(input_path));
		i3::make_unix_path(dest_holder);
		i3::copy_or_swap(out_path, dest_holder);
	}
	
	template<class InputRange, class OutputLit> inline typename i3::enable_if_iterator<OutputLit>::type
	make_unix_path_copy(const InputRange& path, OutputLit out_path, size_t out_buff_size)
	{
		i3::safe_string_copy(out_path, path, out_buff_size);
		i3::make_unix_path(out_path);
	}
	
	template<class InputRange, class Ch, std::size_t N> inline
	make_unix_path_copy(const InputRange& path, Ch (&out_path)[N])
	{
		make_unix_path_copy(path, out_path, N);
	}


}