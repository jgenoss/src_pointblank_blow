#if !defined( __FALLOFF_MTL_H)
#define __FALLOFF_MTL_H

#define FALLOFF_MTL_CLASS_ID		0x6EC3730C

class FalloffMtl : public Texmap
{
public:
	FalloffMtl(void);
	virtual ~FalloffMtl(void);

	void			Bind(void);

	REAL32			GetIndexOfRefract(void);
	void			GetColor2( COLORREAL * pColor);

	void DumpParam(void);
};

#endif
