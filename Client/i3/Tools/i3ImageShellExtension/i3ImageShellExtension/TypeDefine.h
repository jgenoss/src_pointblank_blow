#include <Shlwapi.h>


typedef signed char INT8;
typedef unsigned char UINT8;
typedef unsigned short int UINT16;
typedef unsigned int UINT32;
typedef signed int			INT32;
typedef signed __int64		INT64;

#define			MIN( a, b)								( (a) > (b) ? (b) : (a))
#define			MAX( a, b)								( (a) > (b) ? (a) : (b))
#define			MINMAX( _min_, _v_, _max_ )				( MIN( MAX( (_min_), (_v_) ), (_max_) ) )

#define I3TRACE(fmt, ...)				I3PRINTF(fmt, __VA_ARGS__)