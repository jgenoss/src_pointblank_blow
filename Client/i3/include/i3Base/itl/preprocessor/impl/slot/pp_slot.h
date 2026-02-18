#pragma once


# define pp_slot_offset_10(x) (x) % 1000000000UL
# define pp_slot_offset_9(x) pp_slot_offset_10(x) % 100000000UL
# define pp_slot_offset_8(x) pp_slot_offset_9(x) % 10000000UL
# define pp_slot_offset_7(x) pp_slot_offset_8(x) % 1000000UL
# define pp_slot_offset_6(x) pp_slot_offset_7(x) % 100000UL
# define pp_slot_offset_5(x) pp_slot_offset_6(x) % 10000UL
# define pp_slot_offset_4(x) pp_slot_offset_5(x) % 1000UL
# define pp_slot_offset_3(x) pp_slot_offset_4(x) % 100UL
# define pp_slot_offset_2(x) pp_slot_offset_3(x) % 10UL
#
#
# define pp_slot_cc_2(a, b) pp_slot_cc_2_d(a, b)
# define pp_slot_cc_3(a, b, c) pp_slot_cc_3_d(a, b, c)
# define pp_slot_cc_4(a, b, c, d) pp_slot_cc_4_d(a, b, c, d)
# define pp_slot_cc_5(a, b, c, d, e) pp_slot_cc_5_d(a, b, c, d, e)
# define pp_slot_cc_6(a, b, c, d, e, f) pp_slot_cc_6_d(a, b, c, d, e, f)
# define pp_slot_cc_7(a, b, c, d, e, f, g) pp_slot_cc_7_d(a, b, c, d, e, f, g)
# define pp_slot_cc_8(a, b, c, d, e, f, g, h) pp_slot_cc_8_d(a, b, c, d, e, f, g, h)
# define pp_slot_cc_9(a, b, c, d, e, f, g, h, i) pp_slot_cc_9_d(a, b, c, d, e, f, g, h, i)
# define pp_slot_cc_10(a, b, c, d, e, f, g, h, i, j) pp_slot_cc_10_d(a, b, c, d, e, f, g, h, i, j)
#
# define pp_slot_cc_2_d(a, b) a ## b
# define pp_slot_cc_3_d(a, b, c) a ## b ## c
# define pp_slot_cc_4_d(a, b, c, d) a ## b ## c ## d
# define pp_slot_cc_5_d(a, b, c, d, e) a ## b ## c ## d ## e
# define pp_slot_cc_6_d(a, b, c, d, e, f) a ## b ## c ## d ## e ## f
# define pp_slot_cc_7_d(a, b, c, d, e, f, g) a ## b ## c ## d ## e ## f ## g
# define pp_slot_cc_8_d(a, b, c, d, e, f, g, h) a ## b ## c ## d ## e ## f ## g ## h
# define pp_slot_cc_9_d(a, b, c, d, e, f, g, h, i) a ## b ## c ## d ## e ## f ## g ## h ## i
# define pp_slot_cc_10_d(a, b, c, d, e, f, g, h, i, j) a ## b ## c ## d ## e ## f ## g ## h ## i ## j
#

# define pp_assign_slot(i) pp_cat(pp_assign_slot_, i)

# define pp_assign_slot_1 "i3Base/itl/preprocessor/impl/slot/pp_slot1.h"
# define pp_assign_slot_2 "i3Base/itl/preprocessor/impl/slot/pp_slot2.h"
# define pp_assign_slot_3 "i3Base/itl/preprocessor/impl/slot/pp_slot3.h"
# define pp_assign_slot_4 "i3Base/itl/preprocessor/impl/slot/pp_slot4.h"
# define pp_assign_slot_5 "i3Base/itl/preprocessor/impl/slot/pp_slot5.h"


# define pp_slot(i) pp_cat(pp_slot_, i)()
