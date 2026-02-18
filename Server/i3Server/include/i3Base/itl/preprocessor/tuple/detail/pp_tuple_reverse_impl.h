#pragma once


#define pp_tuple_reverse_impl(size, tuple)	pp_tuple_reverse_i(size, tuple)

#define pp_tuple_reverse_i(s, t) pp_tuple_reverse_ii(pp_tuple_reverse_ ## s t)
#define pp_tuple_reverse_ii(res) res


#define pp_tuple_reverse_0() ()
#define pp_tuple_reverse_1(a) (a)
#define pp_tuple_reverse_2(a, b) (b, a)
#define pp_tuple_reverse_3(a, b, c) (c, b, a)
#define pp_tuple_reverse_4(a, b, c, d) (d, c, b, a)
#define pp_tuple_reverse_5(a, b, c, d, e) (e, d, c, b, a)
#define pp_tuple_reverse_6(a, b, c, d, e, f) (f, e, d, c, b, a)
#define pp_tuple_reverse_7(a, b, c, d, e, f, g) (g, f, e, d, c, b, a)
#define pp_tuple_reverse_8(a, b, c, d, e, f, g, h) (h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_9(a, b, c, d, e, f, g, h, i) (i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_10(a, b, c, d, e, f, g, h, i, j) (j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_11(a, b, c, d, e, f, g, h, i, j, k) (k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_12(a, b, c, d, e, f, g, h, i, j, k, l) (l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_13(a, b, c, d, e, f, g, h, i, j, k, l, m) (m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) (n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) (o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) (p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) (q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) (r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) (s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) (t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) (u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) (v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) (w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) (x, w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)
#define pp_tuple_reverse_25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) (y, x, w, v, u, t, s, r, q, p, o, n, m, l, k, j, i, h, g, f, e, d, c, b, a)

