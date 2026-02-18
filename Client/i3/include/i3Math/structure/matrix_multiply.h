#pragma once

//#if 0



namespace i3
{
	struct mat4;
	struct mat4a;

	I3_EXPORT_MATH
	void matrix_multiply(const mat4a& lhs, const mat4a& rhs, mat4& out);

	I3_EXPORT_MATH
	void matrix_multiply(const mat4& lhs, const mat4& rhs, mat4& out);
	
	
}

//#endif