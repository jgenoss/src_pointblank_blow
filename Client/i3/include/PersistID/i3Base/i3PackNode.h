#if !defined( __I3_PACK_NODE_H)
#define __I3_PACK_NODE_H

#include "i3TreeNode.h"

#define		I3_PACK_FILE_ATTR_SECURE		0x08000000

class i3PackFile;

#define		MAX_PACK_FILENAME		48

typedef struct _tagi3PackFileInfo
{
	char			m_szName[ MAX_PACK_FILENAME];
	UINT32			m_Attr;
	UINT32			m_codeName;
	UINT32			m_Size;		
	UINT32			m_Offset;

	inline
	void			init(void)
	{
		m_szName[0] = 0;
		m_Attr = 0;
		m_Size = 0;
		m_Offset = 0;
	}

} I3_PACK_FILE_REF;


class I3_EXPORT_BASE i3PackNode : public i3TreeNode
{
	I3_CLASS_DEFINE( i3PackNode);
protected:
	I3_PACK_FILE_REF *		m_pFiles;
	INT32					m_FileCount;

protected:
	UINT32					getNameCode( const char * pszName);
	UINT32					saveFile( i3Stream * pDest, i3Stream * pSrc, I3_PACK_FILE_REF * pFile);

public:
	i3PackNode(void);
	virtual ~i3PackNode(void);

	INT32					getFileCount(void)						{ return m_FileCount; }
	void					SetFileCount( INT32 cnt);
	I3_PACK_FILE_REF *		getFile( INT32 idx)						{ return m_pFiles + idx; }

	INT32					Find( const char * pszName);
	INT32					AddFile( const char * pszName, UINT32 attr, UINT32 Size, UINT32 Offset);
	void					RemoveFile( const char * pszName);

	i3PackNode *			FindPath( char * pszPath, I3_PACK_FILE_REF ** ppFileInfo);

	void					GetFullPath( char * pszBuf);

	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);

	friend class i3PackFile;
};

#endif
