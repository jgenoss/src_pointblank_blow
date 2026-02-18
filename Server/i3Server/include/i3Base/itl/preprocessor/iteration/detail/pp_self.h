# if !defined(pp_indirect_self)
#    error pp_error:  no indirect file to include
# endif

#define pp_is_selfish 1

#include pp_indirect_self

#undef pp_is_selfish
#undef pp_indirect_self
