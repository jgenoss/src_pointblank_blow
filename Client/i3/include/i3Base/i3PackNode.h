#if !defined( __I3_PACK_NODE_H)
#define __I3_PACK_NODE_H

#include "i3TreeNode.h"

#define		I3_PACK_FILE_ATTR_SECURE		0x08000000

class i3PackFile;

#define		MAX_PACK_FILENAME		48
#define		MAX_PACK_FILENAME_NEW	64

struct I3_PACK_FILE_REF
{
	char			m_szName[ MAX_PACK_FILENAME_NEW];
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

};


class I3_EXPORT_BASE i3PackNode : public i3TreeNode
{
	I3_EXPORT_CLASS_DEFINE( i3PackNode, i3TreeNode);

	friend class i3PackFile;

protected:
	I3_PACK_FILE_REF *		m_pFiles = nullptr;
	INT32					m_FileCount = 0;

protected:
	UINT32					getNameCode( const char * pszName);
	UINT32					saveFile( i3Stream * pDest, i3Stream * pSrc, I3_PACK_FILE_REF * pFile);

public:
	virtual ~i3PackNode(void);

	INT32					getFileCount(void)						{ return m_FileCount; }
	void					SetFileCount( INT32 cnt);
	I3_PACK_FILE_REF *		getFile( INT32 idx)						{ return m_pFiles + idx; }

	INT32					Find( const char * pszName);
	INT32					AddFile( const char * pszName, UINT32 attr, UINT32 Size, UINT32 Offset);
	void					RemoveFile( const char * pszName);

	i3PackNode *			FindPath( const char * pszPath, I3_PACK_FILE_REF ** ppFileInfo);

	void					GetFullPath( char * pszBuf);

	virtual UINT32			OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32			OnLoad( i3ResourceFile * pResFile) override;

	void					DumpPack();
};

#endif
