#pragma once

#define pp_tuple_to_list_impl(size, tuple) pp_tuple_to_list_i(size, tuple)

#define pp_tuple_to_list_i(s, t) pp_tuple_to_list_ii(pp_tuple_to_list_ ## s t)
#define pp_tuple_to_list_ii(res) res

#define pp_tuple_to_list_0() pp_nil
#define pp_tuple_to_list_1(a) (a, pp_nil)
#define pp_tuple_to_list_2(a, b) (a, (b, pp_nil))
#define pp_tuple_to_list_3(a, b, c) (a, (b, (c, pp_nil)))
#define pp_tuple_to_list_4(a, b, c, d) (a, (b, (c, (d, pp_nil))))
#define pp_tuple_to_list_5(a, b, c, d, e) (a, (b, (c, (d, (e, pp_nil)))))
#define pp_tuple_to_list_6(a, b, c, d, e, f) (a, (b, (c, (d, (e, (f, pp_nil))))))
#define pp_tuple_to_list_7(a, b, c, d, e, f, g) (a, (b, (c, (d, (e, (f, (g, pp_nil)))))))
#define pp_tuple_to_list_8(a, b, c, d, e, f, g, h) (a, (b, (c, (d, (e, (f, (g, (h, pp_nil))))))))
#define pp_tuple_to_list_9(a, b, c, d, e, f, g, h, i) (a, (b, (c, (d, (e, (f, (g, (h, (i, pp_nil)))))))))
#define pp_tuple_to_list_10(a, b, c, d, e, f, g, h, i, j) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, pp_nil))))))))))
#define pp_tuple_to_list_11(a, b, c, d, e, f, g, h, i, j, k) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, pp_nil)))))))))))
#define pp_tuple_to_list_12(a, b, c, d, e, f, g, h, i, j, k, l) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, pp_nil))))))))))))
#define pp_tuple_to_list_13(a, b, c, d, e, f, g, h, i, j, k, l, m) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, pp_nil)))))))))))))
#define pp_tuple_to_list_14(a, b, c, d, e, f, g, h, i, j, k, l, m, n) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, pp_nil))))))))))))))
#define pp_tuple_to_list_15(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, pp_nil)))))))))))))))
#define pp_tuple_to_list_16(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, pp_nil))))))))))))))))
#define pp_tuple_to_list_17(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, pp_nil)))))))))))))))))
#define pp_tuple_to_list_18(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, pp_nil))))))))))))))))))
#define pp_tuple_to_list_19(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, pp_nil)))))))))))))))))))
#define pp_tuple_to_list_20(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, pp_nil))))))))))))))))))))
#define pp_tuple_to_list_21(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, (u, pp_nil)))))))))))))))))))))
#define pp_tuple_to_list_22(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, (u, (v, pp_nil))))))))))))))))))))))
#define pp_tuple_to_list_23(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, (u, (v, (w, pp_nil)))))))))))))))))))))))
#define pp_tuple_to_list_24(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, (u, (v, (w, (x, pp_nil))))))))))))))))))))))))
#define pp_tuple_to_list_25(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y) (a, (b, (c, (d, (e, (f, (g, (h, (i, (j, (k, (l, (m, (n, (o, (p, (q, (r, (s, (t, (u, (v, (w, (x, (y, pp_nil)))))))))))))))))))))))))
