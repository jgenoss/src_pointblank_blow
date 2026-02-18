#if !defined( __I3_PACK_FILESYSTEM_H)
#define __I3_PACK_FILESYSTEM_H

#include "i3PackNode.h"
#include "i3PackVolume.h"
#include "i3PackFile.h"

namespace i3Pack
{
	I3_EXPORT_BASE	bool			Init(void);
	I3_EXPORT_BASE	bool			Finish(void);

	I3_EXPORT_BASE	bool			Bind( const char * pszPath);

	I3_EXPORT_BASE	i3PackVolume *	isFile( const char * pszPath);
	I3_EXPORT_BASE	i3PackFile *	Open( const char * pszPath);
	I3_EXPORT_BASE	void			Close( i3PackFile * pFile);
};

#endif
