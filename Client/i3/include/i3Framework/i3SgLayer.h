#if !defined( __I3_SG_LAYER_H)
#define __I3_SG_LAYER_H

#define		I3_SG_LAYER_STYLE_2D				0x00010000
#define		I3_SG_LAYER_STYLE_TRANSPARENT		0x00000001

class I3_EXPORT_FRAMEWORK i3SgLayer : public i3NamedElement
{
	I3_EXPORT_CLASS_DEFINE( i3SgLayer, i3NamedElement);
protected:
	// Persistant members
	UINT32		m_Style = 0;

	// Volatile Member
	i3LayerRootNode *	m_pRoot = i3LayerRootNode::new_object();

public:
	virtual ~i3SgLayer(void);

	UINT32		getStyle(void)				{ return m_Style; }
	void		setStyle( UINT32 style)		{ m_Style = style; }
	void		addStyle( UINT32 style)		{ m_Style |= style;	}
	void		removeStyle( UINT32 style)	{ m_Style &= ~style; }

	i3LayerRootNode *	getLayerRoot(void)			{ return m_pRoot; }
	void		setLayerRoot( i3LayerRootNode * pLayerSet)
	{
		I3_REF_CHANGE( m_pRoot, pLayerSet);
	}

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;
};

#endif
