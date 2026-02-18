#pragma once

namespace inven
{	
	bool GetItemTypeArg( T_ItemID ItemID, UINT8* attr, UINT32* arg);
	bool GetItemTypeArg( T_ItemID ItemID, T_ItemDBIdx ware_idx, UINT8* attr, UINT32* arg);

	bool is_valid_item( T_ItemID ItemID);
	bool is_valid_item( T_ItemID ItemID, T_ItemDBIdx ware_idx);


}
