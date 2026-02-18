#if !defined( __PACK_NODE_H)
#define __PACK_NODE_H

class PackNode : public i3PackNode
{
	I3_CLASS_DEFINE( PackNode);
protected:
	char		m_szPath[MAX_PATH];

public:
	PackNode(void);
	virtual ~PackNode(void);

	char *		GetPath(void)						{ return m_szPath; }
	void		SetPath( const char * pszPath)		{ i3String::Copy( m_szPath, (char *) pszPath); }

	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
