#if !defined( __I3_EMIT_EFFECT_H)
#define __I3_EMIT_EFFECT_H

class i3EmitEffect : public i3Node
{
	I3_CLASS_DEFINE( i3EmitEffect);
public:
	enum EMIT_TYPE
	{
		EMIT_ISOLATE,
		EMIT_ATTACH,
	};

protected:
	EMIT_TYPE		m_EmitType;

public:
	i3EmitEffect(void);
	virtual ~i3EmitEffect(void);

	EMIT_TYPE		getEmitType(void)				{ return m_EmitType; }
	void			setEmitType( EMIT_TYPE type)	{ m_EmitType = type; }

	virtual void	CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void	OnBuildObjectList( i3List * pList);
	virtual UINT32	OnSave( i3ResourceFile * pResFile);
	virtual UINT32	OnLoad( i3ResourceFile * pResFile);
};

#endif
