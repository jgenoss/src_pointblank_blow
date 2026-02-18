#pragma once

namespace uiMath{

//최소보다 작으면 최소값 반환, 최대보다 크면 최대값 반환
template <typename T>
T limited(T val, T min_val, T max_val) 
{
    return std::max<T>(min_val, std::min<T>(max_val, val));
}

//최소보다 작으면 최대값 반환, 최대보다 크면 최소값 반환
template <typename T>
T roundlimited(T val, T min_val, T max_val) 
{
	if( val > max_val )
		return min_val;
	else if( val < min_val )
		return max_val;

	return std::max<T>(min_val, std::min<T>(max_val, val));
}

//최소, 최대 범위안에서 dir 방향(감소(false), 증가(true))으로 step 이동한다.
template <typename T>
T move_number(T val, T min_val, T max_val, bool dir, T step_val = 1, bool round = true) 
{
	if( max_val == min_val ) return min_val;

	if( round == true )
	{
		T tmp;
		if( dir == false ) tmp = val - step_val;
		else tmp = val + step_val;

		T result = limited(tmp, min_val, max_val);
		if( tmp == result ) return result;

		T ab;
		if( min_val > tmp )
		{
			ab = abs( min_val - tmp ) - 1;
			return max_val - ab;
		}

		ab = abs( tmp - max_val ) - 1;
		return min_val + ab;
	}

	if( dir == false ) return limited(val - step_val, min_val, max_val);
	return limited(val + step_val, min_val, max_val);
}

 
//값을 반환 한다. (0~1사이 값, 최소, 최대) (0.5f, 10.0f, 20.0f) -> 15.0f
template <typename T>
T interpolate(float t, T min_val, T max_val) 
{
	return (T)(min_val*(1.-t) + max_val*t);
}
 
//0~1 값으로 반환 (값, 최소, 최대) (15.0f, 10.0f, 20.0f) -> 0.5f
template <typename T>
float inverse_interpolate(T val, T min_val, T max_val) 
{
    return 1.0f - ( (val-max_val) / (float)(min_val - max_val));
}

//특정 위치의 값으로 변환(새로운 최소값, 새로운 최대값, 현재 값, 현재 최소값, 현재 최대값
//(20.0f, 40.0f, 15.0f, 10.0f, 20.0f) -> 30.0f
template <typename T>
T remap_ranges(T new_min_val, T new_max_val, T old_middle_val, T old_min_val, T old_max_val) 
{
    float t = inverse_interpolate(old_middle_val, old_min_val, old_max_val);
    return  interpolate(t, new_min_val, new_max_val);
}

}