#if !defined( __HITPART_CONTEXT_H)
#define __HITPART_CONTEXT_H

class HitPartContext : public i3::shared_ginst<HitPartContext>
{
public:
	UINT8				HitPartIndex[ CHARA_HIT_MAXCOUNT ];
	
	void				reset(void);
	void				SetHitPart(INT32 Count,UINT8 Index);
	UINT8				EncryptHitPart( CHARA_HIT_PART part);
	CHARA_HIT_PART		DecryptHitPart(INT32 ArrayIndex);
};

#endif

