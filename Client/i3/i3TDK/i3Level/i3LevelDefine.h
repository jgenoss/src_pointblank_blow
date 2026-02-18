#if !defined( __I3_LEVEL_DEFINE_H)
#define __I3_LEVEL_DEFINE_H

#define		I3LV_EVENT_MOVE				(I3_EVT_USER + 1)
#define		I3LV_EVENT_ROTATE			(I3_EVT_USER + 2)
#define		I3LV_EVENT_SCALE			(I3_EVT_USER + 3)
#define		I3LV_EVENT_DELETE			(I3_EVT_USER + 4)

struct I3_EXPORT_TDK I3LV_SPACE
{
	enum TYPE
	{
		NA		=	-1,
		WORLD	=	0,
		LOCAL,
		
		SPACE_COUNT
	};

	static INT32		getSpaceCount(void)				{	return SPACE_COUNT; }
	static const char *	getSpaceString( INT32 idx);
	static I3LV_SPACE::TYPE	getSpaceByString( const char * pszString);
};

#endif
