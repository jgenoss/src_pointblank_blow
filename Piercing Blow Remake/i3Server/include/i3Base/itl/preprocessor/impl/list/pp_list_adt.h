#pragma once


#define pp_list_cons(head, tail) (head, tail)
#define pp_list_nil pp_nil

#define pp_list_first_impl(list) pp_list_first_d(list)
#define pp_list_first_d(list) pp_list_first_i list
#define pp_list_first_i(head, tail) head

#define pp_list_rest_impl(list) pp_list_rest_d(list)
#define pp_list_rest_d(list) pp_list_rest_i list
#define pp_list_rest_i(head, tail) tail

#define pp_list_is_cons_impl(list)	pp_is_binary(list)
#define pp_list_is_nil_impl(list)	pp_compl(pp_is_binary(list))

