#if !defined( __I3_PACK_FILESYSTEM_H)
#define __I3_PACK_FILESYSTEM_H

#include "i3PackNode.h"
#include "i3PackVolume.h"
#include "i3PackFile.h"

namespace i3Pack
{
	INT32							comparebylatest( void * p1, void *p2);
	I3_EXPORT_BASE	void			Sort(void);

	I3_EXPORT_BASE	bool			Init(void);
	I3_EXPORT_BASE	bool			Finish(void);

	I3_EXPORT_BASE	bool			Preview( const char * pszPath);
	I3_EXPORT_BASE	bool			Bind();
	I3_EXPORT_BASE  void			DumpList();

	I3_EXPORT_BASE	bool			Bind( const char * pszPath);

	I3_EXPORT_BASE	i3PackVolume *	isFile( const char * pszPath);
	I3_EXPORT_BASE	i3PackFile *	Open( const char * pszPath);
	I3_EXPORT_BASE	void			Close( i3PackFile * pFile);

	I3_EXPORT_BASE  i3PackNode*		FindDirectoryPackNode(const char* pszDir);			// 디렉토리였던 패킹노드를 찾는다...디렉토리가 아니면 NULL리턴..(12.02.17.수빈)
};

#endif
