
# ifndef pp_value
#    error pp_error:  pp_value is not defined
# endif
#
# undef pp_slot_temp_1
# undef pp_slot_temp_2
# undef pp_slot_temp_3
# undef pp_slot_temp_4
# undef pp_slot_temp_5
# undef pp_slot_temp_6
# undef pp_slot_temp_7
# undef pp_slot_temp_8
# undef pp_slot_temp_9
# undef pp_slot_temp_10
#
# if (pp_value) / 1000000000UL == 0
#    define pp_slot_temp_10 0
# elif (pp_value) / 1000000000UL == 1
#    define pp_slot_temp_10 1
# elif (pp_value) / 1000000000UL == 2
#    define pp_slot_temp_10 2
# elif (pp_value) / 1000000000UL == 3
#    define pp_slot_temp_10 3
# elif (pp_value) / 1000000000UL == 4
#    define pp_slot_temp_10 4
# elif (pp_value) / 1000000000UL == 5
#    define pp_slot_temp_10 5
# elif (pp_value) / 1000000000UL == 6
#    define pp_slot_temp_10 6
# elif (pp_value) / 1000000000UL == 7
#    define pp_slot_temp_10 7
# elif (pp_value) / 1000000000UL == 8
#    define pp_slot_temp_10 8
# elif (pp_value) / 1000000000UL == 9
#    define pp_slot_temp_10 9
# endif
#
# if pp_slot_offset_10(pp_value) / 100000000UL == 0
#    define pp_slot_temp_9 0
# elif pp_slot_offset_10(pp_value) / 100000000UL == 1
#    define pp_slot_temp_9 1
# elif pp_slot_offset_10(pp_value) / 100000000UL == 2
#    define pp_slot_temp_9 2
# elif pp_slot_offset_10(pp_value) / 100000000UL == 3
#    define pp_slot_temp_9 3
# elif pp_slot_offset_10(pp_value) / 100000000UL == 4
#    define pp_slot_temp_9 4
# elif pp_slot_offset_10(pp_value) / 100000000UL == 5
#    define pp_slot_temp_9 5
# elif pp_slot_offset_10(pp_value) / 100000000UL == 6
#    define pp_slot_temp_9 6
# elif pp_slot_offset_10(pp_value) / 100000000UL == 7
#    define pp_slot_temp_9 7
# elif pp_slot_offset_10(pp_value) / 100000000UL == 8
#    define pp_slot_temp_9 8
# elif pp_slot_offset_10(pp_value) / 100000000UL == 9
#    define pp_slot_temp_9 9
# endif
#
# if pp_slot_offset_9(pp_value) / 10000000UL == 0
#    define pp_slot_temp_8 0
# elif pp_slot_offset_9(pp_value) / 10000000UL == 1
#    define pp_slot_temp_8 1
# elif pp_slot_offset_9(pp_value) / 10000000UL == 2
#    define pp_slot_temp_8 2
# elif pp_slot_offset_9(pp_value) / 10000000UL == 3
#    define pp_slot_temp_8 3
# elif pp_slot_offset_9(pp_value) / 10000000UL == 4
#    define pp_slot_temp_8 4
# elif pp_slot_offset_9(pp_value) / 10000000UL == 5
#    define pp_slot_temp_8 5
# elif pp_slot_offset_9(pp_value) / 10000000UL == 6
#    define pp_slot_temp_8 6
# elif pp_slot_offset_9(pp_value) / 10000000UL == 7
#    define pp_slot_temp_8 7
# elif pp_slot_offset_9(pp_value) / 10000000UL == 8
#    define pp_slot_temp_8 8
# elif pp_slot_offset_9(pp_value) / 10000000UL == 9
#    define pp_slot_temp_8 9
# endif
#
# if pp_slot_offset_8(pp_value) / 1000000UL == 0
#    define pp_slot_temp_7 0
# elif pp_slot_offset_8(pp_value) / 1000000UL == 1
#    define pp_slot_temp_7 1
# elif pp_slot_offset_8(pp_value) / 1000000UL == 2
#    define pp_slot_temp_7 2
# elif pp_slot_offset_8(pp_value) / 1000000UL == 3
#    define pp_slot_temp_7 3
# elif pp_slot_offset_8(pp_value) / 1000000UL == 4
#    define pp_slot_temp_7 4
# elif pp_slot_offset_8(pp_value) / 1000000UL == 5
#    define pp_slot_temp_7 5
# elif pp_slot_offset_8(pp_value) / 1000000UL == 6
#    define pp_slot_temp_7 6
# elif pp_slot_offset_8(pp_value) / 1000000UL == 7
#    define pp_slot_temp_7 7
# elif pp_slot_offset_8(pp_value) / 1000000UL == 8
#    define pp_slot_temp_7 8
# elif pp_slot_offset_8(pp_value) / 1000000UL == 9
#    define pp_slot_temp_7 9
# endif
#
# if pp_slot_offset_7(pp_value) / 100000UL == 0
#    define pp_slot_temp_6 0
# elif pp_slot_offset_7(pp_value) / 100000UL == 1
#    define pp_slot_temp_6 1
# elif pp_slot_offset_7(pp_value) / 100000UL == 2
#    define pp_slot_temp_6 2
# elif pp_slot_offset_7(pp_value) / 100000UL == 3
#    define pp_slot_temp_6 3
# elif pp_slot_offset_7(pp_value) / 100000UL == 4
#    define pp_slot_temp_6 4
# elif pp_slot_offset_7(pp_value) / 100000UL == 5
#    define pp_slot_temp_6 5
# elif pp_slot_offset_7(pp_value) / 100000UL == 6
#    define pp_slot_temp_6 6
# elif pp_slot_offset_7(pp_value) / 100000UL == 7
#    define pp_slot_temp_6 7
# elif pp_slot_offset_7(pp_value) / 100000UL == 8
#    define pp_slot_temp_6 8
# elif pp_slot_offset_7(pp_value) / 100000UL == 9
#    define pp_slot_temp_6 9
# endif
#
# if pp_slot_offset_6(pp_value) / 10000UL == 0
#    define pp_slot_temp_5 0
# elif pp_slot_offset_6(pp_value) / 10000UL == 1
#    define pp_slot_temp_5 1
# elif pp_slot_offset_6(pp_value) / 10000UL == 2
#    define pp_slot_temp_5 2
# elif pp_slot_offset_6(pp_value) / 10000UL == 3
#    define pp_slot_temp_5 3
# elif pp_slot_offset_6(pp_value) / 10000UL == 4
#    define pp_slot_temp_5 4
# elif pp_slot_offset_6(pp_value) / 10000UL == 5
#    define pp_slot_temp_5 5
# elif pp_slot_offset_6(pp_value) / 10000UL == 6
#    define pp_slot_temp_5 6
# elif pp_slot_offset_6(pp_value) / 10000UL == 7
#    define pp_slot_temp_5 7
# elif pp_slot_offset_6(pp_value) / 10000UL == 8
#    define pp_slot_temp_5 8
# elif pp_slot_offset_6(pp_value) / 10000UL == 9
#    define pp_slot_temp_5 9
# endif
#
# if pp_slot_offset_5(pp_value) / 1000UL == 0
#    define pp_slot_temp_4 0
# elif pp_slot_offset_5(pp_value) / 1000UL == 1
#    define pp_slot_temp_4 1
# elif pp_slot_offset_5(pp_value) / 1000UL == 2
#    define pp_slot_temp_4 2
# elif pp_slot_offset_5(pp_value) / 1000UL == 3
#    define pp_slot_temp_4 3
# elif pp_slot_offset_5(pp_value) / 1000UL == 4
#    define pp_slot_temp_4 4
# elif pp_slot_offset_5(pp_value) / 1000UL == 5
#    define pp_slot_temp_4 5
# elif pp_slot_offset_5(pp_value) / 1000UL == 6
#    define pp_slot_temp_4 6
# elif pp_slot_offset_5(pp_value) / 1000UL == 7
#    define pp_slot_temp_4 7
# elif pp_slot_offset_5(pp_value) / 1000UL == 8
#    define pp_slot_temp_4 8
# elif pp_slot_offset_5(pp_value) / 1000UL == 9
#    define pp_slot_temp_4 9
# endif
#
# if pp_slot_offset_4(pp_value) / 100UL == 0
#    define pp_slot_temp_3 0
# elif pp_slot_offset_4(pp_value) / 100UL == 1
#    define pp_slot_temp_3 1
# elif pp_slot_offset_4(pp_value) / 100UL == 2
#    define pp_slot_temp_3 2
# elif pp_slot_offset_4(pp_value) / 100UL == 3
#    define pp_slot_temp_3 3
# elif pp_slot_offset_4(pp_value) / 100UL == 4
#    define pp_slot_temp_3 4
# elif pp_slot_offset_4(pp_value) / 100UL == 5
#    define pp_slot_temp_3 5
# elif pp_slot_offset_4(pp_value) / 100UL == 6
#    define pp_slot_temp_3 6
# elif pp_slot_offset_4(pp_value) / 100UL == 7
#    define pp_slot_temp_3 7
# elif pp_slot_offset_4(pp_value) / 100UL == 8
#    define pp_slot_temp_3 8
# elif pp_slot_offset_4(pp_value) / 100UL == 9
#    define pp_slot_temp_3 9
# endif
#
# if pp_slot_offset_3(pp_value) / 10UL == 0
#    define pp_slot_temp_2 0
# elif pp_slot_offset_3(pp_value) / 10UL == 1
#    define pp_slot_temp_2 1
# elif pp_slot_offset_3(pp_value) / 10UL == 2
#    define pp_slot_temp_2 2
# elif pp_slot_offset_3(pp_value) / 10UL == 3
#    define pp_slot_temp_2 3
# elif pp_slot_offset_3(pp_value) / 10UL == 4
#    define pp_slot_temp_2 4
# elif pp_slot_offset_3(pp_value) / 10UL == 5
#    define pp_slot_temp_2 5
# elif pp_slot_offset_3(pp_value) / 10UL == 6
#    define pp_slot_temp_2 6
# elif pp_slot_offset_3(pp_value) / 10UL == 7
#    define pp_slot_temp_2 7
# elif pp_slot_offset_3(pp_value) / 10UL == 8
#    define pp_slot_temp_2 8
# elif pp_slot_offset_3(pp_value) / 10UL == 9
#    define pp_slot_temp_2 9
# endif
#
# if pp_slot_offset_2(pp_value) == 0
#    define pp_slot_temp_1 0
# elif pp_slot_offset_2(pp_value) == 1
#    define pp_slot_temp_1 1
# elif pp_slot_offset_2(pp_value) == 2
#    define pp_slot_temp_1 2
# elif pp_slot_offset_2(pp_value) == 3
#    define pp_slot_temp_1 3
# elif pp_slot_offset_2(pp_value) == 4
#    define pp_slot_temp_1 4
# elif pp_slot_offset_2(pp_value) == 5
#    define pp_slot_temp_1 5
# elif pp_slot_offset_2(pp_value) == 6
#    define pp_slot_temp_1 6
# elif pp_slot_offset_2(pp_value) == 7
#    define pp_slot_temp_1 7
# elif pp_slot_offset_2(pp_value) == 8
#    define pp_slot_temp_1 8
# elif pp_slot_offset_2(pp_value) == 9
#    define pp_slot_temp_1 9
# endif
#
# undef pp_value
