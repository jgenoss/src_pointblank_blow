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

typedef struct ALIGN2 _tagVLogHeader
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
} I3VLOG_HEADER;

////////////////////////////////////////
// Begin
typedef struct ALIGN2 _tagVLogPackBegin
{
	I3VLOG_HEADER	head;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_BEGIN);
		head.size = 0;
	}
	
} I3VLOG_PACK_BEGIN;

////////////////////////////////////////
// End
typedef struct ALIGN2 _tagVLogPackEnd
{
	I3VLOG_HEADER	head;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_END);
		head.size = 0;
	}
	
} I3VLOG_PACK_END;

////////////////////////////////////////
// COLOR
typedef struct ALIGN2 _tagVLogPackColor
{
	I3VLOG_HEADER	head;
	UINT32			color;
	
	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_COLOR);
		head.size = sizeof(color);
	}
} I3VLOG_PACK_COLOR;

////////////////////////////////////////
// POINT
typedef struct ALIGN2 _tagVLogPackPoint
{
	I3VLOG_HEADER	head;
	PERSIST_VEC3D	pos;
	REAL32			scale;
	UINT16			point_type;
	
	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_POINT);
		head.size = sizeof(pos);
	}
} I3VLOG_PACK_POINT;

////////////////////////////////////////
// LINE
typedef struct ALIGN2 _tagVLogPackLine
{
	I3VLOG_HEADER	head;
	PERSIST_VEC3D	start;
	PERSIST_VEC3D	end;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_LINE);
		head.size = sizeof( start) + sizeof(end);
	}
} I3VLOG_PACK_LINE;

////////////////////////////////////////
// TRI
typedef struct ALIGN2 _tagVLogPackTri
{
	I3VLOG_HEADER	head;
	PERSIST_VEC3D	p1;
	PERSIST_VEC3D	p2;
	PERSIST_VEC3D	p3;

	inline
	void	init(void)
	{
		head.init( I3VLOG_CMD_TRI);
		head.size = sizeof(p1) + sizeof(p2) + sizeof(p3);
	}
} I3VLOG_PACK_TRI;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

#endif
