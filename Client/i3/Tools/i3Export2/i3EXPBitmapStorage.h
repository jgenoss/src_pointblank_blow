#if !defined( __I3_EXP_BITMAP_STORAGE_H)
#define __I3_EXP_BITMAP_STORAGE_H

class i3EXPBitmapStorage : public BitmapStorage
{
public:
	int		getPaletteSlot(void)			{ return paletteSlots; }
	
};

#endif
