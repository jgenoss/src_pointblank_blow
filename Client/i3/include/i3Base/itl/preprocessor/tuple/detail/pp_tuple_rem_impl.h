#pragma once

#define pp_tuple_rem_impl(size) pp_tuple_rem_i(size)
#define pp_tuple_rem_i(size) pp_tuple_rem_ ## size

#define pp_tuple_rem_0()
#define pp_tuple_rem_1(a) a
#define pp_tuple_rem_2(a, b) a, b
#define pp_tuple_rem_3(a, b, c) a, b, c
#define pp_tuple_rem_4(a, b, c, d) a, b, c, d
#define pp_tuple_rem_5(a, b, c, d, e) a, b, c, d, e
#define pp_tuple_rem_6(a, b, c, d, e, f) a, b, c, d, e, f
#define pp_tuple_rem_7(a, b, c, d, e, f, g) a, b, c, d, e, f, g
#define pp_tuple_rem_8(a, b, c, d, e, f, g, h) a, b, c, d, e, f, g, h
#define pp_tuple_rem_9(a, b, c, d, e, f, g, h, i) a, b, c, d, e, f, g, h, i
#define pp_tuple_rem_10(a, b, c, d, e, f, g, h, i, j) a, b, c, d, e, f, g, h, i, j
#define pp_tuple_rem_11(a, b, c, d, e, f, g, h, i, j, k) a, b, c, d, e, f, g, h, i, j, k
#define pp_tuple_rem_12(a, b, c, d, e, f, g, h, i, j, k, l) a, b, c, d, e, f, g, h, i, j, k, l
#define pp_tuple_rem_13(a, b, c, d, e, f, g, h, i, j, k, l, m) a, b, c, d, e, f, g, h, i, j, k, l, m
#define pp_tuple_rem_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) a, b, c, d, e, f, g, h, i, j, k, l, m, n
#define pp_tuple_rem_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o
#define pp_tuple_rem_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p
#define pp_tuple_rem_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q
#define pp_tuple_rem_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r
#define pp_tuple_rem_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s
#define pp_tuple_rem_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t
#define pp_tuple_rem_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u
#define pp_tuple_rem_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v
#define pp_tuple_rem_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w
#define pp_tuple_rem_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x
#define pp_tuple_rem_25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y


#define pp_tuple_rem_ctor_impl(size, tuple)	pp_tuple_rem_ctor_i(pp_tuple_rem_impl(size), tuple)
#define pp_tuple_rem_ctor_i(ext, tuple) ext tuple

