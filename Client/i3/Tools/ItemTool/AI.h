#if !defined( __C_AI_H)
#define __C_AI_H

#include "i3Base.h"
#include "GICDiagramAI.h"

class CChara;

class AI : public i3ResourceObject
{
	I3_CLASS_DEFINE( AI, i3ResourceObject);
protected:
	// Persistent members
	GICDiagramAI *	m_pDiagram;
	i3AIContext *	m_pCtx;
	char			m_szDriverName[128];

	// Volatile members

	friend CChara;

protected:
	bool			ReplaceSymbols( char * pszDest, INT32 len, const char * pszSrc);

public:
	AI(void);
	virtual ~AI(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets a i. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else a i. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	i3AI *			getAI(void)			{ return m_pCtx->getAI(); }
	void			SetAI( i3AI * pAI);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets a i context. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else a i context. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	i3AIContext *	getAIContext(void)	{ return m_pCtx; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the diagram. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else the diagram. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	GICDiagramAI *	getDiagram(void)	{ return m_pDiagram; }

	void			GetFullAIPath( char * pszBuf);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the driver name. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else the driver name. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	char *			getDriverName(void)			{ return m_szDriverName; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets a driver name. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <param name="pszStr">	[in,out] If non-null, the psz string. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void			setDriverName( char * pszStr)
	{
		i3::safe_string_copy( m_szDriverName, pszStr, 128 );
	}

	void			OnSelect( bool bFlag);

	void			ExportC( const char * pszOnlyPath);

	void			DumpTXT( const char * pszPath);

	void			ConvertToAnimTemplate( bool bSetAnimToNull = false);

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
