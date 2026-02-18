#if !defined( __I3_CHARA_EDITOR_FRAMEWORK_H)
#define __I3_CHARA_EDITOR_FRAMEWORK_H

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	3 chara editor framework. </summary>
///
/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class i3CharaEditorFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3CharaEditorFramework, i3ViewerFramework);
protected:

//	i3ShadowMap *		m_pShadowMap;
	i3Node *			m_pCaster;
	i3Node *			m_pReceiver;
	i3Transform *		m_pFloorRoot;
	i3ReflectMapBindAttr *	m_pReflectMapAttr;

//	i3Sprite2D *		m_pShadowMapView;

public:
	i3CharaEditorFramework(void);
	virtual ~i3CharaEditorFramework(void);

	virtual void		OnCreate(void);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Executes the query attach node action. </summary>
	///
	/// <remarks>	РЬСјБе, 2010-11-22. </remarks>
	///
	/// <param name="pRes">	[in,out] If non-null, the resource. </param>
	///
	/// <returns>	null if it fails, else. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual i3Node *	OnQueryAttachNode( i3GameRes * pRes);
	virtual		void	OnChangeBound(void);
	virtual void		OnKeyDown( UINT32 nKey);
};

#endif
