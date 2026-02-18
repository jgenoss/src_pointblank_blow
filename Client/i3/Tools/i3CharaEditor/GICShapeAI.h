#if !defined( __GIC_SHAPE_AI_H)
#define __GIC_SHAPE_AI_H

class GICShapeAI : public i3GICShapeRect
{
	I3_CLASS_DEFINE( GICShapeAI, i3GICShapeRect);
protected:
	i3AIState *				m_pAIState;

	i3::vector<i3::rc_string>	m_SrcText;
	i3::vector<i3::rc_string>	m_CommentText;

	HBRUSH					m_hbrBlendFill;
	COLORREF				m_blendFillColor;

	bool					m_bTemplateAI;
	bool					m_bError;

protected:
	void					_CreateGDIBlendFillObject();
	void					_DeleteGDIBlendFillObject();

public:
	GICShapeAI(void);
	virtual ~GICShapeAI(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets a i state. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else a i state. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	i3AIState *				getAIState(void)				{ return m_pAIState; }
	void					setAIState( i3AIState * pState);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the error state. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	true if it succeeds, false if it fails. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	bool					getErrorState(void)				{ return m_bError; }
	void					setErrorState( bool bState);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the source text. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else the source text. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	i3::vector<i3::rc_string>&			getSourceText(void)				{ return m_SrcText; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the comment text. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <returns>	null if it fails, else the comment text. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	i3::vector<i3::rc_string>&			getCommentText(void)			{ return m_CommentText; }

	virtual void			OnDetach( i3GICDiagram * pDiagram);
	virtual	bool			OnPreSize( REAL32 cx, REAL32 cy);
	virtual	void			OnDraw( i3GICDiagramContext * pCtx);//( i3GICDeviceContext * pGDC);

	virtual	void			OnBuildObjectList( i3::vector<i3PersistantElement*>& List);
	virtual UINT32			OnSave( i3ResourceFile * pResFile);
	virtual UINT32			OnLoad( i3ResourceFile * pResFile);
	virtual bool			OnSaveXML(i3XMLFile * pFile, i3XMLElement * pXML);
	virtual bool			OnLoadXML(i3XMLFile * pFile, i3XMLElement * pXML);

	virtual void			OnBindDiagram( bool bBind);
};

#endif
