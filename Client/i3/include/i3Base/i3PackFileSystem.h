#if !defined( __I3_PACK_FILESYSTEM_H)
#define __I3_PACK_FILESYSTEM_H

#include "i3PackNode.h"
#include "i3PackVolume.h"

class i3PackFile;
class i3Stream;

typedef void (*CALLBACK_UPDATEFUNC)(void);

namespace i3Pack
{

	I3_EXPORT_BASE	void			Sort(void);

	I3_EXPORT_BASE	bool			Init(void);
	I3_EXPORT_BASE	bool			Finish(void);

	I3_EXPORT_BASE	bool			Preview( const char * pszPath);
	I3_EXPORT_BASE	bool			Bind();
	I3_EXPORT_BASE  void			DumpList();

	I3_EXPORT_BASE	bool			Bind( const char * pszPath);
	
	I3_EXPORT_BASE  void			BindVolume( i3PackVolume* vol);


	I3_EXPORT_BASE	i3PackVolume *	isFile( const i3::literal_range& strResPath);
	I3_EXPORT_BASE	i3PackFile *	Open( const i3::literal_range& strResPath);
	I3_EXPORT_BASE	void			Close( i3PackFile * pFile);

	I3_EXPORT_BASE  i3PackNode*		FindDirectoryPackNode(const char* pszDir);			// 디렉토리였던 패킹노드를 찾는다...디렉토리가 아니면 nullptr리턴..(12.02.17.수빈)

	I3_EXPORT_BASE void				BindCallbackUpdateFunc( CALLBACK_UPDATEFUNC func);
};

#endif
