#if !defined( __I3_VLOG_DEFINE_H)
#define __I3_VLOG_DEFINE_H

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

enum I3VLOG_CMD
{
	I3VLOG_CMD_BEGIN	= 'B',
	I3VLOG_CMD_END		= 'E',
	I3VLOG_CMD_COLOR	= 'C',
	I3VLOG_CMD_POINT	= 'P',
	I3VLOG_CMD_LINE		= 'L',
	I3VLOG_CMD_TRI		= 'T',
};

struct ALIGN2 I3VLOG_HEADER
{
	UINT8	tag[4];
	UINT16	size;

	inline 
	void	init( I3VLOG_CMD type)
	{
		tag[0] = 'V';
		tag[1] = 'L';
		tag[2] = 'G';
		tag[3] = (UINT8) type;
	}
} ;

////////////////////////////////////////
// Begin
struct ALIGN2 I3VLOG_PACK_BEGIN
{
	I3VLOG_HEADER	head;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_BEGIN);
		head.size = 0;
	}
	
} ;

////////////////////////////////////////
// End
struct ALIGN2 I3VLOG_PACK_END
{
	I3VLOG_HEADER	head;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_END);
		head.size = 0;
	}
	
} ;

////////////////////////////////////////
// COLOR
struct ALIGN2 I3VLOG_PACK_COLOR
{
	I3VLOG_HEADER	head;
	UINT32			color;
	
	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_COLOR);
		head.size = sizeof(color);
	}
} ;

////////////////////////////////////////
// POINT
struct ALIGN2 I3VLOG_PACK_POINT
{
	I3VLOG_HEADER	head;
	i3::pack::VEC3D	pos;
	REAL32			scale;
	UINT16			point_type;
	
	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_POINT);
		head.size = sizeof(pos);
	}
} ;

////////////////////////////////////////
// LINE
struct ALIGN2 I3VLOG_PACK_LINE
{
	I3VLOG_HEADER	head;
	i3::pack::VEC3D	start;
	i3::pack::VEC3D	end;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_LINE);
		head.size = sizeof( start) + sizeof(end);
	}
} ;

////////////////////////////////////////
// TRI
struct ALIGN2 I3VLOG_PACK_TRI
{
	I3VLOG_HEADER	head;
	i3::pack::VEC3D	p1;
	i3::pack::VEC3D	p2;
	i3::pack::VEC3D	p3;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_TRI);
		head.size = sizeof(p1) + sizeof(p2) + sizeof(p3);
	}
} ;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
