#pragma once

namespace i3
{

	struct int3
	{
		int3() { }
		int3(int x, int y, int z) : x(x), y(y), z(z) { }
		const int&	operator[](int i) const { return (&x)[i]; }
		int&		operator[](int i) { return (&x)[i]; }

		int x, y, z;
	};
	
}
