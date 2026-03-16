#pragma once

#define pp_error_impl(code)				pp_cat(pp_error_, code)

# define pp_error_0x0000 pp_error(0x0000, PP_INDEX_OUT_OF_BOUNDS)
# define pp_error_0x0001 pp_error(0x0001, PP_WHILE_OVERFLOW)
# define pp_error_0x0002 pp_error(0x0002, PP_FOR_OVERFLOW)
# define pp_error_0x0003 pp_error(0x0003, PP_REPEAT_OVERFLOW)
# define pp_error_0x0004 pp_error(0x0004, PP_LIST_FOLD_OVERFLOW)
# define pp_error_0x0005 pp_error(0x0005, PP_SEQ_FOLD_OVERFLOW)
# define pp_error_0x0006 pp_error(0x0006, PP_ARITHMETIC_OVERFLOW)
# define pp_error_0x0007 pp_error(0x0007, PP_DIVISION_BY_ZERO)

