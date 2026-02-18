
namespace i3
{
	
	__forceinline
	vec3&				aabb::min() { return m_min; }
	__forceinline
	vec3&				aabb::max() { return m_max; }
	__forceinline
	const vec3&			aabb::min() const { return m_min; }
	__forceinline
	const vec3&			aabb::max() const { return m_max; }
	__forceinline
	void				aabb::min(const vec3& v) { m_min = v; }
	__forceinline
	void				aabb::max(const vec3& v) { m_max = v; }

	inline aabb::aabb() :	m_min( FLT_MAX, FLT_MAX, FLT_MAX ), 
		m_max( -FLT_MAX, -FLT_MAX, -FLT_MAX )
	{
	}

	inline
	aabb::aabb(float min_x, float min_y, float min_z, float max_x, float max_y, float max_z) :
	m_min(min_x, min_y, min_z), m_max(max_x, max_y, max_z)
	{
	}

	inline
	aabb::aabb(const vec3& p_min, const vec3& p_max) : m_min(p_min), m_max(p_max)
	{
	}


	inline aabb::aabb(const aabb& a) : m_min(a.m_min), m_max(a.m_max)
	{

	}

	inline aabb::aabb(const vec3& v) : m_min(v), m_max(v)
	{

	}

	inline void	aabb::identity() 
	{ 
		m_min = vec3(-.5f, -.5f, -.5f);	m_max = vec3( .5f, .5f, .5f);
	}

	inline void	aabb::reset()
	{
		m_min = vec3(FLT_MAX, FLT_MAX, FLT_MAX );
		m_max = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	}

	inline void	aabb::reset(const vec3& v)
	{
		m_min = v;	m_max = v;
	}

	inline void	aabb::set_center_extent(const vec3& center, const vec3& extent)
	{
		m_min = center;		m_max = center;		m_min -= extent;	m_max += extent;
	}

	inline void	aabb::calc_center(vec3& outCenter) const
	{ 
		outCenter = m_max; outCenter += m_min; outCenter *= .5f; 
	}

	inline void	aabb::calc_extent(vec3& outExtent) const
	{ 
		outExtent = m_max; outExtent -= m_min; outExtent *= .5f; 
	}

	inline void	aabb::calc_length(vec3& outLength) const
	{
		outLength = m_max; outLength -= m_min;
	}

	inline vec3		aabb::calc_center() const
	{
		return vec3( (m_max.x + m_min.x)* .5f, (m_max.y + m_min.y)* .5f, (m_max.z + m_min.z)* .5f);
	}

	inline vec3		aabb::calc_extent() const
	{
		return vec3( (m_max.x - m_min.x)* .5f, (m_max.y - m_min.y)* .5f, (m_max.z - m_min.z)* .5f);
	}

	inline vec3		aabb::calc_length() const
	{
		return vec3( (m_max.x - m_min.x), (m_max.y - m_min.y), (m_max.z - m_min.z) );
	}

	inline aabb&	aabb::operator=(const aabb& a) 
	{ 
		m_min = a.m_min;	m_max = a.m_max; return *this; 
	}

	inline aabb&	aabb::operator+=( const vec3& rhs) 
	{ 
		m_min += rhs;	m_max += rhs;  return *this; 
	}

	inline aabb&	aabb::operator*=( const vec3& rhs) 
	{		
		m_min.x *= rhs.x;	m_min.y *= rhs.y;	m_min.z *= rhs.z;
		m_max.x *= rhs.x;	m_max.y *= rhs.y;	m_max.z *= rhs.z;
		return *this;
	}

	inline void			aabb::merge(const vec3& v)
	{
		if( m_min.x > v.x ) m_min.x = v.x;
		if( m_min.y > v.y ) m_min.y = v.y;
		if( m_min.z > v.z ) m_min.z = v.z;
		if( m_max.x < v.x ) m_max.x = v.x;
		if( m_max.y < v.y ) m_max.y = v.y;
		if( m_max.z < v.z ) m_max.z = v.z;
	}

	inline void			aabb::merge(const aabb& a)
	{
		if ( m_min.x > a.min().x ) m_min.x = a.min().x;
		if ( m_min.y > a.min().y ) m_min.y = a.min().y;
		if ( m_min.z > a.min().z ) m_min.z = a.min().z;

		if ( m_max.x < a.max().x ) m_max.x = a.max().x;
		if ( m_max.y < a.max().y ) m_max.y = a.max().y;
		if ( m_max.z < a.max().z ) m_max.z = a.max().z;
	}


	inline
		vec3		aabb::calc_vertex(int is_max_x, int is_max_y, int is_max_z ) const
	{
		return vec3( (&m_min)[is_max_x].x, (&m_min)[is_max_y].y, (&m_min)[is_max_z].z );
	}

	inline
		void		aabb::calc_vertex(i3::vec3& out, int is_max_x, int is_max_y, int is_max_z ) const
	{
		out.x = (&m_min)[is_max_x].x;
		out.y = (&m_min)[is_max_y].y;
		out.z =	(&m_min)[is_max_z].z;
	}

	inline
	float		aabb::calc_vertex_x(int is_max) const
	{
		return (&m_min)[is_max].x;
	}

	inline
	float		aabb::calc_vertex_y(int is_max) const
	{
		return (&m_min)[is_max].y;
	}

	inline
	float		aabb::calc_vertex_z(int is_max) const
	{
		return (&m_min)[is_max].z;
	}
	
	inline bool	aabb::is_valid() const { return m_min.x != FLT_MAX; }		// 하나만 비교하도록 처리..

	
}
