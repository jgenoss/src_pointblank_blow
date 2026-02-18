
namespace i3
{

	inline frustum_plane::frustum_plane(const i3::mat4& mat_view_proj)
	{
		frustum_plane::apply(mat_view_proj);
	}


	inline frustum_plane::frustum_plane(const frustum_plane& rhs)
	{
		::memcpy(m_plane, rhs.m_plane, sizeof(plane) * NUM_PLANE);
		::memcpy(m_lookup, rhs.m_lookup, sizeof(DWORD) * NUM_PLANE);
	}

	inline frustum_plane&	frustum_plane::operator=(const frustum_plane& rhs)
	{
		::memcpy(m_plane, rhs.m_plane, sizeof(plane) * NUM_PLANE);
		::memcpy(m_lookup, rhs.m_lookup, sizeof(DWORD) * NUM_PLANE);
		return *this;
	}

}
