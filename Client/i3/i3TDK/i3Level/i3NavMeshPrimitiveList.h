#pragma once

class I3_EXPORT_TDK i3NavMeshPrimitive : public i3ElementBase
{
	I3_CLASS_DEFINE( i3NavMeshPrimitive );

public:
	i3NavMeshPrimitive();
	virtual ~i3NavMeshPrimitive();

public:
	const VEC3D&		GetPosition() const			{ return m_pos; }
	const REAL32&		GetScale() const			{ return m_scale; }
	const COLORREAL&	GetColor() const			{ return m_cr; }

public:
	void	SetPosition( const VEC3D& pos )		{ m_pos.x = pos.x, m_pos.y = pos.y, m_pos.z = pos.z; }
	void	SetScale( REAL32 sc )				{ m_scale = sc; }
	void	SetColor( const COLORREAL& cr )		{ m_cr = cr; }

protected:
	RT_VEC3D		m_pos;
	REAL32			m_scale;

	COLORREAL		m_cr;
};

class I3_EXPORT_TDK i3NavMeshPrimitiveList : public i3GeometryAttr
{
	I3_CLASS_DEFINE( i3NavMeshPrimitiveList );

public:
	i3NavMeshPrimitiveList();
	~i3NavMeshPrimitiveList();

protected:
	void		_BuildGeometry_Sphere( UINT32 hdiv = 20, UINT32 vdiv = 10 );
	void		_BuildGeometry_SelBox();

public:
	virtual bool		IsInFrustum( i3RenderContext* ctx, i3NavMeshPrimitive* ptr );

	virtual void		BuildGeometry();
	virtual void		Apply( i3RenderContext* ctx );

private:
	i3::vector< i3NavMeshPrimitive* >	m_primList;
};