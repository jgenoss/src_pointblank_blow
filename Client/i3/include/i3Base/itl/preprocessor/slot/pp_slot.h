#pragma once

#include "../pp_cat.h"
#include "detail/pp_slot_def.h"

#define pp_assign_slot(i) pp_cat(pp_assign_slot_, i)

#define pp_assign_slot_1 "i3Base/itl/preprocessor/slot/detail/pp_slot1.h"
#define pp_assign_slot_2 "i3Base/itl/preprocessor/slot/detail/pp_slot2.h"
#define pp_assign_slot_3 "i3Base/itl/preprocessor/slot/detail/pp_slot3.h"
#define pp_assign_slot_4 "i3Base/itl/preprocessor/slot/detail/pp_slot4.h"
#define pp_assign_slot_5 "i3Base/itl/preprocessor/slot/detail/pp_slot5.h"


#define pp_slot(i) pp_cat(pp_slot_, i)()
