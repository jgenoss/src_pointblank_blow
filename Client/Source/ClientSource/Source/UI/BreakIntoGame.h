#pragma once

namespace bing	//break into game (게임 난입)
{
	bool is_break_into_game();
	void print_room_info(); //난입 스코어를 출력한다.
	INT32 get_room_info_string(i3::vector< i3::wstring > & list); //게임 정보를 string list로 반환한다.
	
	//get_room_info_string 함수와 같은 기능이다. 단지 score도 반환
	INT32 get_room_info_string(i3::vector< i3::wstring > & list, INT32 & red_score, INT32 & blue_score); 
}