
# include "../slot/pp_slot_shared.h"
#
# undef pp_iteration_finish_2
#
# undef pp_iteration_finish_2_digit_1
# undef pp_iteration_finish_2_digit_2
# undef pp_iteration_finish_2_digit_3
# undef pp_iteration_finish_2_digit_4
# undef pp_iteration_finish_2_digit_5
# undef pp_iteration_finish_2_digit_6
# undef pp_iteration_finish_2_digit_7
# undef pp_iteration_finish_2_digit_8
# undef pp_iteration_finish_2_digit_9
# undef pp_iteration_finish_2_digit_10
#
# if pp_slot_temp_3 == 0
#    define pp_iteration_finish_2_digit_3 0
# elif pp_slot_temp_3 == 1
#    define pp_iteration_finish_2_digit_3 1
# elif pp_slot_temp_3 == 2
#    define pp_iteration_finish_2_digit_3 2
# elif pp_slot_temp_3 == 3
#    define pp_iteration_finish_2_digit_3 3
# elif pp_slot_temp_3 == 4
#    define pp_iteration_finish_2_digit_3 4
# elif pp_slot_temp_3 == 5
#    define pp_iteration_finish_2_digit_3 5
# elif pp_slot_temp_3 == 6
#    define pp_iteration_finish_2_digit_3 6
# elif pp_slot_temp_3 == 7
#    define pp_iteration_finish_2_digit_3 7
# elif pp_slot_temp_3 == 8
#    define pp_iteration_finish_2_digit_3 8
# elif pp_slot_temp_3 == 9
#    define pp_iteration_finish_2_digit_3 9
# endif
#
# if pp_slot_temp_2 == 0
#    define pp_iteration_finish_2_digit_2 0
# elif pp_slot_temp_2 == 1
#    define pp_iteration_finish_2_digit_2 1
# elif pp_slot_temp_2 == 2
#    define pp_iteration_finish_2_digit_2 2
# elif pp_slot_temp_2 == 3
#    define pp_iteration_finish_2_digit_2 3
# elif pp_slot_temp_2 == 4
#    define pp_iteration_finish_2_digit_2 4
# elif pp_slot_temp_2 == 5
#    define pp_iteration_finish_2_digit_2 5
# elif pp_slot_temp_2 == 6
#    define pp_iteration_finish_2_digit_2 6
# elif pp_slot_temp_2 == 7
#    define pp_iteration_finish_2_digit_2 7
# elif pp_slot_temp_2 == 8
#    define pp_iteration_finish_2_digit_2 8
# elif pp_slot_temp_2 == 9
#    define pp_iteration_finish_2_digit_2 9
# endif
#
# if pp_slot_temp_1 == 0
#    define pp_iteration_finish_2_digit_1 0
# elif pp_slot_temp_1 == 1
#    define pp_iteration_finish_2_digit_1 1
# elif pp_slot_temp_1 == 2
#    define pp_iteration_finish_2_digit_1 2
# elif pp_slot_temp_1 == 3
#    define pp_iteration_finish_2_digit_1 3
# elif pp_slot_temp_1 == 4
#    define pp_iteration_finish_2_digit_1 4
# elif pp_slot_temp_1 == 5
#    define pp_iteration_finish_2_digit_1 5
# elif pp_slot_temp_1 == 6
#    define pp_iteration_finish_2_digit_1 6
# elif pp_slot_temp_1 == 7
#    define pp_iteration_finish_2_digit_1 7
# elif pp_slot_temp_1 == 8
#    define pp_iteration_finish_2_digit_1 8
# elif pp_slot_temp_1 == 9
#    define pp_iteration_finish_2_digit_1 9
# endif
#
# if pp_iteration_finish_2_digit_3
#    define pp_iteration_finish_2 pp_slot_cc_3(pp_iteration_finish_2_digit_3, pp_iteration_finish_2_digit_2, pp_iteration_finish_2_digit_1)
# elif pp_iteration_finish_2_digit_2
#    define pp_iteration_finish_2 pp_slot_cc_2(pp_iteration_finish_2_digit_2, pp_iteration_finish_2_digit_1)
# else
#    define pp_iteration_finish_2 pp_iteration_finish_2_digit_1
# endif
