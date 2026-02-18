#if !defined( __I3_OPT_EXTRACT_ANIM_H)
#define __I3_OPT_EXTRACT_ANIM_H

#include "i3SceneOptimizer.h"

class I3_EXPORT_OPT i3OptExtractAnim : public i3SceneOptimizer
{
	I3_CLASS_DEFINE( i3OptExtractAnim);
protected:
	i3List			m_NodeList;
	bool			m_bSkin;
	char			m_szOutputPath[MAX_PATH];

public:
	i3OptExtractAnim(void);
	virtual ~i3OptExtractAnim(void);

	char *		getOutputPath(void)				{ return m_szOutputPath; }
	void		setOutputPath( char * pszStr)	{ i3String::Copy( m_szOutputPath, pszStr); }

	virtual BOOL	OnNode( i3Node * pNode);
	virtual void	Trace( i3Node * pRoot);
	virtual void SetProperty( char * pszFieldName, char * pszValue);
};

#endif
