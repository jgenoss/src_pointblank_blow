#pragma once

#define pp_for_impl pp_cat(pp_for_, pp_auto_rec(pp_for_p, 256))

#define pp_for_p(n) pp_cat(pp_for_check_, pp_for_ ## n(1, pp_for_sr_p, pp_for_sr_o, pp_for_sr_m))

#define pp_for_sr_p(r, s) s
#define pp_for_sr_o(r, s) 0
#define pp_for_sr_m(r, s) pp_nil

#define pp_for_1(s, p, o, m) pp_if(p(2, s), m, pp_tuple_eat_2)(2, s) pp_if(p(2, s), pp_for_2, pp_tuple_eat_4)(o(2, s), p, o, m)
#define pp_for_2(s, p, o, m) pp_if(p(3, s), m, pp_tuple_eat_2)(3, s) pp_if(p(3, s), pp_for_3, pp_tuple_eat_4)(o(3, s), p, o, m)
#define pp_for_3(s, p, o, m) pp_if(p(4, s), m, pp_tuple_eat_2)(4, s) pp_if(p(4, s), pp_for_4, pp_tuple_eat_4)(o(4, s), p, o, m)
#define pp_for_4(s, p, o, m) pp_if(p(5, s), m, pp_tuple_eat_2)(5, s) pp_if(p(5, s), pp_for_5, pp_tuple_eat_4)(o(5, s), p, o, m)
#define pp_for_5(s, p, o, m) pp_if(p(6, s), m, pp_tuple_eat_2)(6, s) pp_if(p(6, s), pp_for_6, pp_tuple_eat_4)(o(6, s), p, o, m)
#define pp_for_6(s, p, o, m) pp_if(p(7, s), m, pp_tuple_eat_2)(7, s) pp_if(p(7, s), pp_for_7, pp_tuple_eat_4)(o(7, s), p, o, m)
#define pp_for_7(s, p, o, m) pp_if(p(8, s), m, pp_tuple_eat_2)(8, s) pp_if(p(8, s), pp_for_8, pp_tuple_eat_4)(o(8, s), p, o, m)
#define pp_for_8(s, p, o, m) pp_if(p(9, s), m, pp_tuple_eat_2)(9, s) pp_if(p(9, s), pp_for_9, pp_tuple_eat_4)(o(9, s), p, o, m)
#define pp_for_9(s, p, o, m) pp_if(p(10, s), m, pp_tuple_eat_2)(10, s) pp_if(p(10, s), pp_for_10, pp_tuple_eat_4)(o(10, s), p, o, m)
#define pp_for_10(s, p, o, m) pp_if(p(11, s), m, pp_tuple_eat_2)(11, s) pp_if(p(11, s), pp_for_11, pp_tuple_eat_4)(o(11, s), p, o, m)
#define pp_for_11(s, p, o, m) pp_if(p(12, s), m, pp_tuple_eat_2)(12, s) pp_if(p(12, s), pp_for_12, pp_tuple_eat_4)(o(12, s), p, o, m)
#define pp_for_12(s, p, o, m) pp_if(p(13, s), m, pp_tuple_eat_2)(13, s) pp_if(p(13, s), pp_for_13, pp_tuple_eat_4)(o(13, s), p, o, m)
#define pp_for_13(s, p, o, m) pp_if(p(14, s), m, pp_tuple_eat_2)(14, s) pp_if(p(14, s), pp_for_14, pp_tuple_eat_4)(o(14, s), p, o, m)
#define pp_for_14(s, p, o, m) pp_if(p(15, s), m, pp_tuple_eat_2)(15, s) pp_if(p(15, s), pp_for_15, pp_tuple_eat_4)(o(15, s), p, o, m)
#define pp_for_15(s, p, o, m) pp_if(p(16, s), m, pp_tuple_eat_2)(16, s) pp_if(p(16, s), pp_for_16, pp_tuple_eat_4)(o(16, s), p, o, m)
#define pp_for_16(s, p, o, m) pp_if(p(17, s), m, pp_tuple_eat_2)(17, s) pp_if(p(17, s), pp_for_17, pp_tuple_eat_4)(o(17, s), p, o, m)
#define pp_for_17(s, p, o, m) pp_if(p(18, s), m, pp_tuple_eat_2)(18, s) pp_if(p(18, s), pp_for_18, pp_tuple_eat_4)(o(18, s), p, o, m)
#define pp_for_18(s, p, o, m) pp_if(p(19, s), m, pp_tuple_eat_2)(19, s) pp_if(p(19, s), pp_for_19, pp_tuple_eat_4)(o(19, s), p, o, m)
#define pp_for_19(s, p, o, m) pp_if(p(20, s), m, pp_tuple_eat_2)(20, s) pp_if(p(20, s), pp_for_20, pp_tuple_eat_4)(o(20, s), p, o, m)
#define pp_for_20(s, p, o, m) pp_if(p(21, s), m, pp_tuple_eat_2)(21, s) pp_if(p(21, s), pp_for_21, pp_tuple_eat_4)(o(21, s), p, o, m)
#define pp_for_21(s, p, o, m) pp_if(p(22, s), m, pp_tuple_eat_2)(22, s) pp_if(p(22, s), pp_for_22, pp_tuple_eat_4)(o(22, s), p, o, m)
#define pp_for_22(s, p, o, m) pp_if(p(23, s), m, pp_tuple_eat_2)(23, s) pp_if(p(23, s), pp_for_23, pp_tuple_eat_4)(o(23, s), p, o, m)
#define pp_for_23(s, p, o, m) pp_if(p(24, s), m, pp_tuple_eat_2)(24, s) pp_if(p(24, s), pp_for_24, pp_tuple_eat_4)(o(24, s), p, o, m)
#define pp_for_24(s, p, o, m) pp_if(p(25, s), m, pp_tuple_eat_2)(25, s) pp_if(p(25, s), pp_for_25, pp_tuple_eat_4)(o(25, s), p, o, m)
#define pp_for_25(s, p, o, m) pp_if(p(26, s), m, pp_tuple_eat_2)(26, s) pp_if(p(26, s), pp_for_26, pp_tuple_eat_4)(o(26, s), p, o, m)
#define pp_for_26(s, p, o, m) pp_if(p(27, s), m, pp_tuple_eat_2)(27, s) pp_if(p(27, s), pp_for_27, pp_tuple_eat_4)(o(27, s), p, o, m)
#define pp_for_27(s, p, o, m) pp_if(p(28, s), m, pp_tuple_eat_2)(28, s) pp_if(p(28, s), pp_for_28, pp_tuple_eat_4)(o(28, s), p, o, m)
#define pp_for_28(s, p, o, m) pp_if(p(29, s), m, pp_tuple_eat_2)(29, s) pp_if(p(29, s), pp_for_29, pp_tuple_eat_4)(o(29, s), p, o, m)
#define pp_for_29(s, p, o, m) pp_if(p(30, s), m, pp_tuple_eat_2)(30, s) pp_if(p(30, s), pp_for_30, pp_tuple_eat_4)(o(30, s), p, o, m)
#define pp_for_30(s, p, o, m) pp_if(p(31, s), m, pp_tuple_eat_2)(31, s) pp_if(p(31, s), pp_for_31, pp_tuple_eat_4)(o(31, s), p, o, m)
#define pp_for_31(s, p, o, m) pp_if(p(32, s), m, pp_tuple_eat_2)(32, s) pp_if(p(32, s), pp_for_32, pp_tuple_eat_4)(o(32, s), p, o, m)
#define pp_for_32(s, p, o, m) pp_if(p(33, s), m, pp_tuple_eat_2)(33, s) pp_if(p(33, s), pp_for_33, pp_tuple_eat_4)(o(33, s), p, o, m)
#define pp_for_33(s, p, o, m) pp_if(p(34, s), m, pp_tuple_eat_2)(34, s) pp_if(p(34, s), pp_for_34, pp_tuple_eat_4)(o(34, s), p, o, m)
#define pp_for_34(s, p, o, m) pp_if(p(35, s), m, pp_tuple_eat_2)(35, s) pp_if(p(35, s), pp_for_35, pp_tuple_eat_4)(o(35, s), p, o, m)
#define pp_for_35(s, p, o, m) pp_if(p(36, s), m, pp_tuple_eat_2)(36, s) pp_if(p(36, s), pp_for_36, pp_tuple_eat_4)(o(36, s), p, o, m)
#define pp_for_36(s, p, o, m) pp_if(p(37, s), m, pp_tuple_eat_2)(37, s) pp_if(p(37, s), pp_for_37, pp_tuple_eat_4)(o(37, s), p, o, m)
#define pp_for_37(s, p, o, m) pp_if(p(38, s), m, pp_tuple_eat_2)(38, s) pp_if(p(38, s), pp_for_38, pp_tuple_eat_4)(o(38, s), p, o, m)
#define pp_for_38(s, p, o, m) pp_if(p(39, s), m, pp_tuple_eat_2)(39, s) pp_if(p(39, s), pp_for_39, pp_tuple_eat_4)(o(39, s), p, o, m)
#define pp_for_39(s, p, o, m) pp_if(p(40, s), m, pp_tuple_eat_2)(40, s) pp_if(p(40, s), pp_for_40, pp_tuple_eat_4)(o(40, s), p, o, m)
#define pp_for_40(s, p, o, m) pp_if(p(41, s), m, pp_tuple_eat_2)(41, s) pp_if(p(41, s), pp_for_41, pp_tuple_eat_4)(o(41, s), p, o, m)
#define pp_for_41(s, p, o, m) pp_if(p(42, s), m, pp_tuple_eat_2)(42, s) pp_if(p(42, s), pp_for_42, pp_tuple_eat_4)(o(42, s), p, o, m)
#define pp_for_42(s, p, o, m) pp_if(p(43, s), m, pp_tuple_eat_2)(43, s) pp_if(p(43, s), pp_for_43, pp_tuple_eat_4)(o(43, s), p, o, m)
#define pp_for_43(s, p, o, m) pp_if(p(44, s), m, pp_tuple_eat_2)(44, s) pp_if(p(44, s), pp_for_44, pp_tuple_eat_4)(o(44, s), p, o, m)
#define pp_for_44(s, p, o, m) pp_if(p(45, s), m, pp_tuple_eat_2)(45, s) pp_if(p(45, s), pp_for_45, pp_tuple_eat_4)(o(45, s), p, o, m)
#define pp_for_45(s, p, o, m) pp_if(p(46, s), m, pp_tuple_eat_2)(46, s) pp_if(p(46, s), pp_for_46, pp_tuple_eat_4)(o(46, s), p, o, m)
#define pp_for_46(s, p, o, m) pp_if(p(47, s), m, pp_tuple_eat_2)(47, s) pp_if(p(47, s), pp_for_47, pp_tuple_eat_4)(o(47, s), p, o, m)
#define pp_for_47(s, p, o, m) pp_if(p(48, s), m, pp_tuple_eat_2)(48, s) pp_if(p(48, s), pp_for_48, pp_tuple_eat_4)(o(48, s), p, o, m)
#define pp_for_48(s, p, o, m) pp_if(p(49, s), m, pp_tuple_eat_2)(49, s) pp_if(p(49, s), pp_for_49, pp_tuple_eat_4)(o(49, s), p, o, m)
#define pp_for_49(s, p, o, m) pp_if(p(50, s), m, pp_tuple_eat_2)(50, s) pp_if(p(50, s), pp_for_50, pp_tuple_eat_4)(o(50, s), p, o, m)
#define pp_for_50(s, p, o, m) pp_if(p(51, s), m, pp_tuple_eat_2)(51, s) pp_if(p(51, s), pp_for_51, pp_tuple_eat_4)(o(51, s), p, o, m)
#define pp_for_51(s, p, o, m) pp_if(p(52, s), m, pp_tuple_eat_2)(52, s) pp_if(p(52, s), pp_for_52, pp_tuple_eat_4)(o(52, s), p, o, m)
#define pp_for_52(s, p, o, m) pp_if(p(53, s), m, pp_tuple_eat_2)(53, s) pp_if(p(53, s), pp_for_53, pp_tuple_eat_4)(o(53, s), p, o, m)
#define pp_for_53(s, p, o, m) pp_if(p(54, s), m, pp_tuple_eat_2)(54, s) pp_if(p(54, s), pp_for_54, pp_tuple_eat_4)(o(54, s), p, o, m)
#define pp_for_54(s, p, o, m) pp_if(p(55, s), m, pp_tuple_eat_2)(55, s) pp_if(p(55, s), pp_for_55, pp_tuple_eat_4)(o(55, s), p, o, m)
#define pp_for_55(s, p, o, m) pp_if(p(56, s), m, pp_tuple_eat_2)(56, s) pp_if(p(56, s), pp_for_56, pp_tuple_eat_4)(o(56, s), p, o, m)
#define pp_for_56(s, p, o, m) pp_if(p(57, s), m, pp_tuple_eat_2)(57, s) pp_if(p(57, s), pp_for_57, pp_tuple_eat_4)(o(57, s), p, o, m)
#define pp_for_57(s, p, o, m) pp_if(p(58, s), m, pp_tuple_eat_2)(58, s) pp_if(p(58, s), pp_for_58, pp_tuple_eat_4)(o(58, s), p, o, m)
#define pp_for_58(s, p, o, m) pp_if(p(59, s), m, pp_tuple_eat_2)(59, s) pp_if(p(59, s), pp_for_59, pp_tuple_eat_4)(o(59, s), p, o, m)
#define pp_for_59(s, p, o, m) pp_if(p(60, s), m, pp_tuple_eat_2)(60, s) pp_if(p(60, s), pp_for_60, pp_tuple_eat_4)(o(60, s), p, o, m)
#define pp_for_60(s, p, o, m) pp_if(p(61, s), m, pp_tuple_eat_2)(61, s) pp_if(p(61, s), pp_for_61, pp_tuple_eat_4)(o(61, s), p, o, m)
#define pp_for_61(s, p, o, m) pp_if(p(62, s), m, pp_tuple_eat_2)(62, s) pp_if(p(62, s), pp_for_62, pp_tuple_eat_4)(o(62, s), p, o, m)
#define pp_for_62(s, p, o, m) pp_if(p(63, s), m, pp_tuple_eat_2)(63, s) pp_if(p(63, s), pp_for_63, pp_tuple_eat_4)(o(63, s), p, o, m)
#define pp_for_63(s, p, o, m) pp_if(p(64, s), m, pp_tuple_eat_2)(64, s) pp_if(p(64, s), pp_for_64, pp_tuple_eat_4)(o(64, s), p, o, m)
#define pp_for_64(s, p, o, m) pp_if(p(65, s), m, pp_tuple_eat_2)(65, s) pp_if(p(65, s), pp_for_65, pp_tuple_eat_4)(o(65, s), p, o, m)
#define pp_for_65(s, p, o, m) pp_if(p(66, s), m, pp_tuple_eat_2)(66, s) pp_if(p(66, s), pp_for_66, pp_tuple_eat_4)(o(66, s), p, o, m)
#define pp_for_66(s, p, o, m) pp_if(p(67, s), m, pp_tuple_eat_2)(67, s) pp_if(p(67, s), pp_for_67, pp_tuple_eat_4)(o(67, s), p, o, m)
#define pp_for_67(s, p, o, m) pp_if(p(68, s), m, pp_tuple_eat_2)(68, s) pp_if(p(68, s), pp_for_68, pp_tuple_eat_4)(o(68, s), p, o, m)
#define pp_for_68(s, p, o, m) pp_if(p(69, s), m, pp_tuple_eat_2)(69, s) pp_if(p(69, s), pp_for_69, pp_tuple_eat_4)(o(69, s), p, o, m)
#define pp_for_69(s, p, o, m) pp_if(p(70, s), m, pp_tuple_eat_2)(70, s) pp_if(p(70, s), pp_for_70, pp_tuple_eat_4)(o(70, s), p, o, m)
#define pp_for_70(s, p, o, m) pp_if(p(71, s), m, pp_tuple_eat_2)(71, s) pp_if(p(71, s), pp_for_71, pp_tuple_eat_4)(o(71, s), p, o, m)
#define pp_for_71(s, p, o, m) pp_if(p(72, s), m, pp_tuple_eat_2)(72, s) pp_if(p(72, s), pp_for_72, pp_tuple_eat_4)(o(72, s), p, o, m)
#define pp_for_72(s, p, o, m) pp_if(p(73, s), m, pp_tuple_eat_2)(73, s) pp_if(p(73, s), pp_for_73, pp_tuple_eat_4)(o(73, s), p, o, m)
#define pp_for_73(s, p, o, m) pp_if(p(74, s), m, pp_tuple_eat_2)(74, s) pp_if(p(74, s), pp_for_74, pp_tuple_eat_4)(o(74, s), p, o, m)
#define pp_for_74(s, p, o, m) pp_if(p(75, s), m, pp_tuple_eat_2)(75, s) pp_if(p(75, s), pp_for_75, pp_tuple_eat_4)(o(75, s), p, o, m)
#define pp_for_75(s, p, o, m) pp_if(p(76, s), m, pp_tuple_eat_2)(76, s) pp_if(p(76, s), pp_for_76, pp_tuple_eat_4)(o(76, s), p, o, m)
#define pp_for_76(s, p, o, m) pp_if(p(77, s), m, pp_tuple_eat_2)(77, s) pp_if(p(77, s), pp_for_77, pp_tuple_eat_4)(o(77, s), p, o, m)
#define pp_for_77(s, p, o, m) pp_if(p(78, s), m, pp_tuple_eat_2)(78, s) pp_if(p(78, s), pp_for_78, pp_tuple_eat_4)(o(78, s), p, o, m)
#define pp_for_78(s, p, o, m) pp_if(p(79, s), m, pp_tuple_eat_2)(79, s) pp_if(p(79, s), pp_for_79, pp_tuple_eat_4)(o(79, s), p, o, m)
#define pp_for_79(s, p, o, m) pp_if(p(80, s), m, pp_tuple_eat_2)(80, s) pp_if(p(80, s), pp_for_80, pp_tuple_eat_4)(o(80, s), p, o, m)
#define pp_for_80(s, p, o, m) pp_if(p(81, s), m, pp_tuple_eat_2)(81, s) pp_if(p(81, s), pp_for_81, pp_tuple_eat_4)(o(81, s), p, o, m)
#define pp_for_81(s, p, o, m) pp_if(p(82, s), m, pp_tuple_eat_2)(82, s) pp_if(p(82, s), pp_for_82, pp_tuple_eat_4)(o(82, s), p, o, m)
#define pp_for_82(s, p, o, m) pp_if(p(83, s), m, pp_tuple_eat_2)(83, s) pp_if(p(83, s), pp_for_83, pp_tuple_eat_4)(o(83, s), p, o, m)
#define pp_for_83(s, p, o, m) pp_if(p(84, s), m, pp_tuple_eat_2)(84, s) pp_if(p(84, s), pp_for_84, pp_tuple_eat_4)(o(84, s), p, o, m)
#define pp_for_84(s, p, o, m) pp_if(p(85, s), m, pp_tuple_eat_2)(85, s) pp_if(p(85, s), pp_for_85, pp_tuple_eat_4)(o(85, s), p, o, m)
#define pp_for_85(s, p, o, m) pp_if(p(86, s), m, pp_tuple_eat_2)(86, s) pp_if(p(86, s), pp_for_86, pp_tuple_eat_4)(o(86, s), p, o, m)
#define pp_for_86(s, p, o, m) pp_if(p(87, s), m, pp_tuple_eat_2)(87, s) pp_if(p(87, s), pp_for_87, pp_tuple_eat_4)(o(87, s), p, o, m)
#define pp_for_87(s, p, o, m) pp_if(p(88, s), m, pp_tuple_eat_2)(88, s) pp_if(p(88, s), pp_for_88, pp_tuple_eat_4)(o(88, s), p, o, m)
#define pp_for_88(s, p, o, m) pp_if(p(89, s), m, pp_tuple_eat_2)(89, s) pp_if(p(89, s), pp_for_89, pp_tuple_eat_4)(o(89, s), p, o, m)
#define pp_for_89(s, p, o, m) pp_if(p(90, s), m, pp_tuple_eat_2)(90, s) pp_if(p(90, s), pp_for_90, pp_tuple_eat_4)(o(90, s), p, o, m)
#define pp_for_90(s, p, o, m) pp_if(p(91, s), m, pp_tuple_eat_2)(91, s) pp_if(p(91, s), pp_for_91, pp_tuple_eat_4)(o(91, s), p, o, m)
#define pp_for_91(s, p, o, m) pp_if(p(92, s), m, pp_tuple_eat_2)(92, s) pp_if(p(92, s), pp_for_92, pp_tuple_eat_4)(o(92, s), p, o, m)
#define pp_for_92(s, p, o, m) pp_if(p(93, s), m, pp_tuple_eat_2)(93, s) pp_if(p(93, s), pp_for_93, pp_tuple_eat_4)(o(93, s), p, o, m)
#define pp_for_93(s, p, o, m) pp_if(p(94, s), m, pp_tuple_eat_2)(94, s) pp_if(p(94, s), pp_for_94, pp_tuple_eat_4)(o(94, s), p, o, m)
#define pp_for_94(s, p, o, m) pp_if(p(95, s), m, pp_tuple_eat_2)(95, s) pp_if(p(95, s), pp_for_95, pp_tuple_eat_4)(o(95, s), p, o, m)
#define pp_for_95(s, p, o, m) pp_if(p(96, s), m, pp_tuple_eat_2)(96, s) pp_if(p(96, s), pp_for_96, pp_tuple_eat_4)(o(96, s), p, o, m)
#define pp_for_96(s, p, o, m) pp_if(p(97, s), m, pp_tuple_eat_2)(97, s) pp_if(p(97, s), pp_for_97, pp_tuple_eat_4)(o(97, s), p, o, m)
#define pp_for_97(s, p, o, m) pp_if(p(98, s), m, pp_tuple_eat_2)(98, s) pp_if(p(98, s), pp_for_98, pp_tuple_eat_4)(o(98, s), p, o, m)
#define pp_for_98(s, p, o, m) pp_if(p(99, s), m, pp_tuple_eat_2)(99, s) pp_if(p(99, s), pp_for_99, pp_tuple_eat_4)(o(99, s), p, o, m)
#define pp_for_99(s, p, o, m) pp_if(p(100, s), m, pp_tuple_eat_2)(100, s) pp_if(p(100, s), pp_for_100, pp_tuple_eat_4)(o(100, s), p, o, m)
#define pp_for_100(s, p, o, m) pp_if(p(101, s), m, pp_tuple_eat_2)(101, s) pp_if(p(101, s), pp_for_101, pp_tuple_eat_4)(o(101, s), p, o, m)
#define pp_for_101(s, p, o, m) pp_if(p(102, s), m, pp_tuple_eat_2)(102, s) pp_if(p(102, s), pp_for_102, pp_tuple_eat_4)(o(102, s), p, o, m)
#define pp_for_102(s, p, o, m) pp_if(p(103, s), m, pp_tuple_eat_2)(103, s) pp_if(p(103, s), pp_for_103, pp_tuple_eat_4)(o(103, s), p, o, m)
#define pp_for_103(s, p, o, m) pp_if(p(104, s), m, pp_tuple_eat_2)(104, s) pp_if(p(104, s), pp_for_104, pp_tuple_eat_4)(o(104, s), p, o, m)
#define pp_for_104(s, p, o, m) pp_if(p(105, s), m, pp_tuple_eat_2)(105, s) pp_if(p(105, s), pp_for_105, pp_tuple_eat_4)(o(105, s), p, o, m)
#define pp_for_105(s, p, o, m) pp_if(p(106, s), m, pp_tuple_eat_2)(106, s) pp_if(p(106, s), pp_for_106, pp_tuple_eat_4)(o(106, s), p, o, m)
#define pp_for_106(s, p, o, m) pp_if(p(107, s), m, pp_tuple_eat_2)(107, s) pp_if(p(107, s), pp_for_107, pp_tuple_eat_4)(o(107, s), p, o, m)
#define pp_for_107(s, p, o, m) pp_if(p(108, s), m, pp_tuple_eat_2)(108, s) pp_if(p(108, s), pp_for_108, pp_tuple_eat_4)(o(108, s), p, o, m)
#define pp_for_108(s, p, o, m) pp_if(p(109, s), m, pp_tuple_eat_2)(109, s) pp_if(p(109, s), pp_for_109, pp_tuple_eat_4)(o(109, s), p, o, m)
#define pp_for_109(s, p, o, m) pp_if(p(110, s), m, pp_tuple_eat_2)(110, s) pp_if(p(110, s), pp_for_110, pp_tuple_eat_4)(o(110, s), p, o, m)
#define pp_for_110(s, p, o, m) pp_if(p(111, s), m, pp_tuple_eat_2)(111, s) pp_if(p(111, s), pp_for_111, pp_tuple_eat_4)(o(111, s), p, o, m)
#define pp_for_111(s, p, o, m) pp_if(p(112, s), m, pp_tuple_eat_2)(112, s) pp_if(p(112, s), pp_for_112, pp_tuple_eat_4)(o(112, s), p, o, m)
#define pp_for_112(s, p, o, m) pp_if(p(113, s), m, pp_tuple_eat_2)(113, s) pp_if(p(113, s), pp_for_113, pp_tuple_eat_4)(o(113, s), p, o, m)
#define pp_for_113(s, p, o, m) pp_if(p(114, s), m, pp_tuple_eat_2)(114, s) pp_if(p(114, s), pp_for_114, pp_tuple_eat_4)(o(114, s), p, o, m)
#define pp_for_114(s, p, o, m) pp_if(p(115, s), m, pp_tuple_eat_2)(115, s) pp_if(p(115, s), pp_for_115, pp_tuple_eat_4)(o(115, s), p, o, m)
#define pp_for_115(s, p, o, m) pp_if(p(116, s), m, pp_tuple_eat_2)(116, s) pp_if(p(116, s), pp_for_116, pp_tuple_eat_4)(o(116, s), p, o, m)
#define pp_for_116(s, p, o, m) pp_if(p(117, s), m, pp_tuple_eat_2)(117, s) pp_if(p(117, s), pp_for_117, pp_tuple_eat_4)(o(117, s), p, o, m)
#define pp_for_117(s, p, o, m) pp_if(p(118, s), m, pp_tuple_eat_2)(118, s) pp_if(p(118, s), pp_for_118, pp_tuple_eat_4)(o(118, s), p, o, m)
#define pp_for_118(s, p, o, m) pp_if(p(119, s), m, pp_tuple_eat_2)(119, s) pp_if(p(119, s), pp_for_119, pp_tuple_eat_4)(o(119, s), p, o, m)
#define pp_for_119(s, p, o, m) pp_if(p(120, s), m, pp_tuple_eat_2)(120, s) pp_if(p(120, s), pp_for_120, pp_tuple_eat_4)(o(120, s), p, o, m)
#define pp_for_120(s, p, o, m) pp_if(p(121, s), m, pp_tuple_eat_2)(121, s) pp_if(p(121, s), pp_for_121, pp_tuple_eat_4)(o(121, s), p, o, m)
#define pp_for_121(s, p, o, m) pp_if(p(122, s), m, pp_tuple_eat_2)(122, s) pp_if(p(122, s), pp_for_122, pp_tuple_eat_4)(o(122, s), p, o, m)
#define pp_for_122(s, p, o, m) pp_if(p(123, s), m, pp_tuple_eat_2)(123, s) pp_if(p(123, s), pp_for_123, pp_tuple_eat_4)(o(123, s), p, o, m)
#define pp_for_123(s, p, o, m) pp_if(p(124, s), m, pp_tuple_eat_2)(124, s) pp_if(p(124, s), pp_for_124, pp_tuple_eat_4)(o(124, s), p, o, m)
#define pp_for_124(s, p, o, m) pp_if(p(125, s), m, pp_tuple_eat_2)(125, s) pp_if(p(125, s), pp_for_125, pp_tuple_eat_4)(o(125, s), p, o, m)
#define pp_for_125(s, p, o, m) pp_if(p(126, s), m, pp_tuple_eat_2)(126, s) pp_if(p(126, s), pp_for_126, pp_tuple_eat_4)(o(126, s), p, o, m)
#define pp_for_126(s, p, o, m) pp_if(p(127, s), m, pp_tuple_eat_2)(127, s) pp_if(p(127, s), pp_for_127, pp_tuple_eat_4)(o(127, s), p, o, m)
#define pp_for_127(s, p, o, m) pp_if(p(128, s), m, pp_tuple_eat_2)(128, s) pp_if(p(128, s), pp_for_128, pp_tuple_eat_4)(o(128, s), p, o, m)
#define pp_for_128(s, p, o, m) pp_if(p(129, s), m, pp_tuple_eat_2)(129, s) pp_if(p(129, s), pp_for_129, pp_tuple_eat_4)(o(129, s), p, o, m)
#define pp_for_129(s, p, o, m) pp_if(p(130, s), m, pp_tuple_eat_2)(130, s) pp_if(p(130, s), pp_for_130, pp_tuple_eat_4)(o(130, s), p, o, m)
#define pp_for_130(s, p, o, m) pp_if(p(131, s), m, pp_tuple_eat_2)(131, s) pp_if(p(131, s), pp_for_131, pp_tuple_eat_4)(o(131, s), p, o, m)
#define pp_for_131(s, p, o, m) pp_if(p(132, s), m, pp_tuple_eat_2)(132, s) pp_if(p(132, s), pp_for_132, pp_tuple_eat_4)(o(132, s), p, o, m)
#define pp_for_132(s, p, o, m) pp_if(p(133, s), m, pp_tuple_eat_2)(133, s) pp_if(p(133, s), pp_for_133, pp_tuple_eat_4)(o(133, s), p, o, m)
#define pp_for_133(s, p, o, m) pp_if(p(134, s), m, pp_tuple_eat_2)(134, s) pp_if(p(134, s), pp_for_134, pp_tuple_eat_4)(o(134, s), p, o, m)
#define pp_for_134(s, p, o, m) pp_if(p(135, s), m, pp_tuple_eat_2)(135, s) pp_if(p(135, s), pp_for_135, pp_tuple_eat_4)(o(135, s), p, o, m)
#define pp_for_135(s, p, o, m) pp_if(p(136, s), m, pp_tuple_eat_2)(136, s) pp_if(p(136, s), pp_for_136, pp_tuple_eat_4)(o(136, s), p, o, m)
#define pp_for_136(s, p, o, m) pp_if(p(137, s), m, pp_tuple_eat_2)(137, s) pp_if(p(137, s), pp_for_137, pp_tuple_eat_4)(o(137, s), p, o, m)
#define pp_for_137(s, p, o, m) pp_if(p(138, s), m, pp_tuple_eat_2)(138, s) pp_if(p(138, s), pp_for_138, pp_tuple_eat_4)(o(138, s), p, o, m)
#define pp_for_138(s, p, o, m) pp_if(p(139, s), m, pp_tuple_eat_2)(139, s) pp_if(p(139, s), pp_for_139, pp_tuple_eat_4)(o(139, s), p, o, m)
#define pp_for_139(s, p, o, m) pp_if(p(140, s), m, pp_tuple_eat_2)(140, s) pp_if(p(140, s), pp_for_140, pp_tuple_eat_4)(o(140, s), p, o, m)
#define pp_for_140(s, p, o, m) pp_if(p(141, s), m, pp_tuple_eat_2)(141, s) pp_if(p(141, s), pp_for_141, pp_tuple_eat_4)(o(141, s), p, o, m)
#define pp_for_141(s, p, o, m) pp_if(p(142, s), m, pp_tuple_eat_2)(142, s) pp_if(p(142, s), pp_for_142, pp_tuple_eat_4)(o(142, s), p, o, m)
#define pp_for_142(s, p, o, m) pp_if(p(143, s), m, pp_tuple_eat_2)(143, s) pp_if(p(143, s), pp_for_143, pp_tuple_eat_4)(o(143, s), p, o, m)
#define pp_for_143(s, p, o, m) pp_if(p(144, s), m, pp_tuple_eat_2)(144, s) pp_if(p(144, s), pp_for_144, pp_tuple_eat_4)(o(144, s), p, o, m)
#define pp_for_144(s, p, o, m) pp_if(p(145, s), m, pp_tuple_eat_2)(145, s) pp_if(p(145, s), pp_for_145, pp_tuple_eat_4)(o(145, s), p, o, m)
#define pp_for_145(s, p, o, m) pp_if(p(146, s), m, pp_tuple_eat_2)(146, s) pp_if(p(146, s), pp_for_146, pp_tuple_eat_4)(o(146, s), p, o, m)
#define pp_for_146(s, p, o, m) pp_if(p(147, s), m, pp_tuple_eat_2)(147, s) pp_if(p(147, s), pp_for_147, pp_tuple_eat_4)(o(147, s), p, o, m)
#define pp_for_147(s, p, o, m) pp_if(p(148, s), m, pp_tuple_eat_2)(148, s) pp_if(p(148, s), pp_for_148, pp_tuple_eat_4)(o(148, s), p, o, m)
#define pp_for_148(s, p, o, m) pp_if(p(149, s), m, pp_tuple_eat_2)(149, s) pp_if(p(149, s), pp_for_149, pp_tuple_eat_4)(o(149, s), p, o, m)
#define pp_for_149(s, p, o, m) pp_if(p(150, s), m, pp_tuple_eat_2)(150, s) pp_if(p(150, s), pp_for_150, pp_tuple_eat_4)(o(150, s), p, o, m)
#define pp_for_150(s, p, o, m) pp_if(p(151, s), m, pp_tuple_eat_2)(151, s) pp_if(p(151, s), pp_for_151, pp_tuple_eat_4)(o(151, s), p, o, m)
#define pp_for_151(s, p, o, m) pp_if(p(152, s), m, pp_tuple_eat_2)(152, s) pp_if(p(152, s), pp_for_152, pp_tuple_eat_4)(o(152, s), p, o, m)
#define pp_for_152(s, p, o, m) pp_if(p(153, s), m, pp_tuple_eat_2)(153, s) pp_if(p(153, s), pp_for_153, pp_tuple_eat_4)(o(153, s), p, o, m)
#define pp_for_153(s, p, o, m) pp_if(p(154, s), m, pp_tuple_eat_2)(154, s) pp_if(p(154, s), pp_for_154, pp_tuple_eat_4)(o(154, s), p, o, m)
#define pp_for_154(s, p, o, m) pp_if(p(155, s), m, pp_tuple_eat_2)(155, s) pp_if(p(155, s), pp_for_155, pp_tuple_eat_4)(o(155, s), p, o, m)
#define pp_for_155(s, p, o, m) pp_if(p(156, s), m, pp_tuple_eat_2)(156, s) pp_if(p(156, s), pp_for_156, pp_tuple_eat_4)(o(156, s), p, o, m)
#define pp_for_156(s, p, o, m) pp_if(p(157, s), m, pp_tuple_eat_2)(157, s) pp_if(p(157, s), pp_for_157, pp_tuple_eat_4)(o(157, s), p, o, m)
#define pp_for_157(s, p, o, m) pp_if(p(158, s), m, pp_tuple_eat_2)(158, s) pp_if(p(158, s), pp_for_158, pp_tuple_eat_4)(o(158, s), p, o, m)
#define pp_for_158(s, p, o, m) pp_if(p(159, s), m, pp_tuple_eat_2)(159, s) pp_if(p(159, s), pp_for_159, pp_tuple_eat_4)(o(159, s), p, o, m)
#define pp_for_159(s, p, o, m) pp_if(p(160, s), m, pp_tuple_eat_2)(160, s) pp_if(p(160, s), pp_for_160, pp_tuple_eat_4)(o(160, s), p, o, m)
#define pp_for_160(s, p, o, m) pp_if(p(161, s), m, pp_tuple_eat_2)(161, s) pp_if(p(161, s), pp_for_161, pp_tuple_eat_4)(o(161, s), p, o, m)
#define pp_for_161(s, p, o, m) pp_if(p(162, s), m, pp_tuple_eat_2)(162, s) pp_if(p(162, s), pp_for_162, pp_tuple_eat_4)(o(162, s), p, o, m)
#define pp_for_162(s, p, o, m) pp_if(p(163, s), m, pp_tuple_eat_2)(163, s) pp_if(p(163, s), pp_for_163, pp_tuple_eat_4)(o(163, s), p, o, m)
#define pp_for_163(s, p, o, m) pp_if(p(164, s), m, pp_tuple_eat_2)(164, s) pp_if(p(164, s), pp_for_164, pp_tuple_eat_4)(o(164, s), p, o, m)
#define pp_for_164(s, p, o, m) pp_if(p(165, s), m, pp_tuple_eat_2)(165, s) pp_if(p(165, s), pp_for_165, pp_tuple_eat_4)(o(165, s), p, o, m)
#define pp_for_165(s, p, o, m) pp_if(p(166, s), m, pp_tuple_eat_2)(166, s) pp_if(p(166, s), pp_for_166, pp_tuple_eat_4)(o(166, s), p, o, m)
#define pp_for_166(s, p, o, m) pp_if(p(167, s), m, pp_tuple_eat_2)(167, s) pp_if(p(167, s), pp_for_167, pp_tuple_eat_4)(o(167, s), p, o, m)
#define pp_for_167(s, p, o, m) pp_if(p(168, s), m, pp_tuple_eat_2)(168, s) pp_if(p(168, s), pp_for_168, pp_tuple_eat_4)(o(168, s), p, o, m)
#define pp_for_168(s, p, o, m) pp_if(p(169, s), m, pp_tuple_eat_2)(169, s) pp_if(p(169, s), pp_for_169, pp_tuple_eat_4)(o(169, s), p, o, m)
#define pp_for_169(s, p, o, m) pp_if(p(170, s), m, pp_tuple_eat_2)(170, s) pp_if(p(170, s), pp_for_170, pp_tuple_eat_4)(o(170, s), p, o, m)
#define pp_for_170(s, p, o, m) pp_if(p(171, s), m, pp_tuple_eat_2)(171, s) pp_if(p(171, s), pp_for_171, pp_tuple_eat_4)(o(171, s), p, o, m)
#define pp_for_171(s, p, o, m) pp_if(p(172, s), m, pp_tuple_eat_2)(172, s) pp_if(p(172, s), pp_for_172, pp_tuple_eat_4)(o(172, s), p, o, m)
#define pp_for_172(s, p, o, m) pp_if(p(173, s), m, pp_tuple_eat_2)(173, s) pp_if(p(173, s), pp_for_173, pp_tuple_eat_4)(o(173, s), p, o, m)
#define pp_for_173(s, p, o, m) pp_if(p(174, s), m, pp_tuple_eat_2)(174, s) pp_if(p(174, s), pp_for_174, pp_tuple_eat_4)(o(174, s), p, o, m)
#define pp_for_174(s, p, o, m) pp_if(p(175, s), m, pp_tuple_eat_2)(175, s) pp_if(p(175, s), pp_for_175, pp_tuple_eat_4)(o(175, s), p, o, m)
#define pp_for_175(s, p, o, m) pp_if(p(176, s), m, pp_tuple_eat_2)(176, s) pp_if(p(176, s), pp_for_176, pp_tuple_eat_4)(o(176, s), p, o, m)
#define pp_for_176(s, p, o, m) pp_if(p(177, s), m, pp_tuple_eat_2)(177, s) pp_if(p(177, s), pp_for_177, pp_tuple_eat_4)(o(177, s), p, o, m)
#define pp_for_177(s, p, o, m) pp_if(p(178, s), m, pp_tuple_eat_2)(178, s) pp_if(p(178, s), pp_for_178, pp_tuple_eat_4)(o(178, s), p, o, m)
#define pp_for_178(s, p, o, m) pp_if(p(179, s), m, pp_tuple_eat_2)(179, s) pp_if(p(179, s), pp_for_179, pp_tuple_eat_4)(o(179, s), p, o, m)
#define pp_for_179(s, p, o, m) pp_if(p(180, s), m, pp_tuple_eat_2)(180, s) pp_if(p(180, s), pp_for_180, pp_tuple_eat_4)(o(180, s), p, o, m)
#define pp_for_180(s, p, o, m) pp_if(p(181, s), m, pp_tuple_eat_2)(181, s) pp_if(p(181, s), pp_for_181, pp_tuple_eat_4)(o(181, s), p, o, m)
#define pp_for_181(s, p, o, m) pp_if(p(182, s), m, pp_tuple_eat_2)(182, s) pp_if(p(182, s), pp_for_182, pp_tuple_eat_4)(o(182, s), p, o, m)
#define pp_for_182(s, p, o, m) pp_if(p(183, s), m, pp_tuple_eat_2)(183, s) pp_if(p(183, s), pp_for_183, pp_tuple_eat_4)(o(183, s), p, o, m)
#define pp_for_183(s, p, o, m) pp_if(p(184, s), m, pp_tuple_eat_2)(184, s) pp_if(p(184, s), pp_for_184, pp_tuple_eat_4)(o(184, s), p, o, m)
#define pp_for_184(s, p, o, m) pp_if(p(185, s), m, pp_tuple_eat_2)(185, s) pp_if(p(185, s), pp_for_185, pp_tuple_eat_4)(o(185, s), p, o, m)
#define pp_for_185(s, p, o, m) pp_if(p(186, s), m, pp_tuple_eat_2)(186, s) pp_if(p(186, s), pp_for_186, pp_tuple_eat_4)(o(186, s), p, o, m)
#define pp_for_186(s, p, o, m) pp_if(p(187, s), m, pp_tuple_eat_2)(187, s) pp_if(p(187, s), pp_for_187, pp_tuple_eat_4)(o(187, s), p, o, m)
#define pp_for_187(s, p, o, m) pp_if(p(188, s), m, pp_tuple_eat_2)(188, s) pp_if(p(188, s), pp_for_188, pp_tuple_eat_4)(o(188, s), p, o, m)
#define pp_for_188(s, p, o, m) pp_if(p(189, s), m, pp_tuple_eat_2)(189, s) pp_if(p(189, s), pp_for_189, pp_tuple_eat_4)(o(189, s), p, o, m)
#define pp_for_189(s, p, o, m) pp_if(p(190, s), m, pp_tuple_eat_2)(190, s) pp_if(p(190, s), pp_for_190, pp_tuple_eat_4)(o(190, s), p, o, m)
#define pp_for_190(s, p, o, m) pp_if(p(191, s), m, pp_tuple_eat_2)(191, s) pp_if(p(191, s), pp_for_191, pp_tuple_eat_4)(o(191, s), p, o, m)
#define pp_for_191(s, p, o, m) pp_if(p(192, s), m, pp_tuple_eat_2)(192, s) pp_if(p(192, s), pp_for_192, pp_tuple_eat_4)(o(192, s), p, o, m)
#define pp_for_192(s, p, o, m) pp_if(p(193, s), m, pp_tuple_eat_2)(193, s) pp_if(p(193, s), pp_for_193, pp_tuple_eat_4)(o(193, s), p, o, m)
#define pp_for_193(s, p, o, m) pp_if(p(194, s), m, pp_tuple_eat_2)(194, s) pp_if(p(194, s), pp_for_194, pp_tuple_eat_4)(o(194, s), p, o, m)
#define pp_for_194(s, p, o, m) pp_if(p(195, s), m, pp_tuple_eat_2)(195, s) pp_if(p(195, s), pp_for_195, pp_tuple_eat_4)(o(195, s), p, o, m)
#define pp_for_195(s, p, o, m) pp_if(p(196, s), m, pp_tuple_eat_2)(196, s) pp_if(p(196, s), pp_for_196, pp_tuple_eat_4)(o(196, s), p, o, m)
#define pp_for_196(s, p, o, m) pp_if(p(197, s), m, pp_tuple_eat_2)(197, s) pp_if(p(197, s), pp_for_197, pp_tuple_eat_4)(o(197, s), p, o, m)
#define pp_for_197(s, p, o, m) pp_if(p(198, s), m, pp_tuple_eat_2)(198, s) pp_if(p(198, s), pp_for_198, pp_tuple_eat_4)(o(198, s), p, o, m)
#define pp_for_198(s, p, o, m) pp_if(p(199, s), m, pp_tuple_eat_2)(199, s) pp_if(p(199, s), pp_for_199, pp_tuple_eat_4)(o(199, s), p, o, m)
#define pp_for_199(s, p, o, m) pp_if(p(200, s), m, pp_tuple_eat_2)(200, s) pp_if(p(200, s), pp_for_200, pp_tuple_eat_4)(o(200, s), p, o, m)
#define pp_for_200(s, p, o, m) pp_if(p(201, s), m, pp_tuple_eat_2)(201, s) pp_if(p(201, s), pp_for_201, pp_tuple_eat_4)(o(201, s), p, o, m)
#define pp_for_201(s, p, o, m) pp_if(p(202, s), m, pp_tuple_eat_2)(202, s) pp_if(p(202, s), pp_for_202, pp_tuple_eat_4)(o(202, s), p, o, m)
#define pp_for_202(s, p, o, m) pp_if(p(203, s), m, pp_tuple_eat_2)(203, s) pp_if(p(203, s), pp_for_203, pp_tuple_eat_4)(o(203, s), p, o, m)
#define pp_for_203(s, p, o, m) pp_if(p(204, s), m, pp_tuple_eat_2)(204, s) pp_if(p(204, s), pp_for_204, pp_tuple_eat_4)(o(204, s), p, o, m)
#define pp_for_204(s, p, o, m) pp_if(p(205, s), m, pp_tuple_eat_2)(205, s) pp_if(p(205, s), pp_for_205, pp_tuple_eat_4)(o(205, s), p, o, m)
#define pp_for_205(s, p, o, m) pp_if(p(206, s), m, pp_tuple_eat_2)(206, s) pp_if(p(206, s), pp_for_206, pp_tuple_eat_4)(o(206, s), p, o, m)
#define pp_for_206(s, p, o, m) pp_if(p(207, s), m, pp_tuple_eat_2)(207, s) pp_if(p(207, s), pp_for_207, pp_tuple_eat_4)(o(207, s), p, o, m)
#define pp_for_207(s, p, o, m) pp_if(p(208, s), m, pp_tuple_eat_2)(208, s) pp_if(p(208, s), pp_for_208, pp_tuple_eat_4)(o(208, s), p, o, m)
#define pp_for_208(s, p, o, m) pp_if(p(209, s), m, pp_tuple_eat_2)(209, s) pp_if(p(209, s), pp_for_209, pp_tuple_eat_4)(o(209, s), p, o, m)
#define pp_for_209(s, p, o, m) pp_if(p(210, s), m, pp_tuple_eat_2)(210, s) pp_if(p(210, s), pp_for_210, pp_tuple_eat_4)(o(210, s), p, o, m)
#define pp_for_210(s, p, o, m) pp_if(p(211, s), m, pp_tuple_eat_2)(211, s) pp_if(p(211, s), pp_for_211, pp_tuple_eat_4)(o(211, s), p, o, m)
#define pp_for_211(s, p, o, m) pp_if(p(212, s), m, pp_tuple_eat_2)(212, s) pp_if(p(212, s), pp_for_212, pp_tuple_eat_4)(o(212, s), p, o, m)
#define pp_for_212(s, p, o, m) pp_if(p(213, s), m, pp_tuple_eat_2)(213, s) pp_if(p(213, s), pp_for_213, pp_tuple_eat_4)(o(213, s), p, o, m)
#define pp_for_213(s, p, o, m) pp_if(p(214, s), m, pp_tuple_eat_2)(214, s) pp_if(p(214, s), pp_for_214, pp_tuple_eat_4)(o(214, s), p, o, m)
#define pp_for_214(s, p, o, m) pp_if(p(215, s), m, pp_tuple_eat_2)(215, s) pp_if(p(215, s), pp_for_215, pp_tuple_eat_4)(o(215, s), p, o, m)
#define pp_for_215(s, p, o, m) pp_if(p(216, s), m, pp_tuple_eat_2)(216, s) pp_if(p(216, s), pp_for_216, pp_tuple_eat_4)(o(216, s), p, o, m)
#define pp_for_216(s, p, o, m) pp_if(p(217, s), m, pp_tuple_eat_2)(217, s) pp_if(p(217, s), pp_for_217, pp_tuple_eat_4)(o(217, s), p, o, m)
#define pp_for_217(s, p, o, m) pp_if(p(218, s), m, pp_tuple_eat_2)(218, s) pp_if(p(218, s), pp_for_218, pp_tuple_eat_4)(o(218, s), p, o, m)
#define pp_for_218(s, p, o, m) pp_if(p(219, s), m, pp_tuple_eat_2)(219, s) pp_if(p(219, s), pp_for_219, pp_tuple_eat_4)(o(219, s), p, o, m)
#define pp_for_219(s, p, o, m) pp_if(p(220, s), m, pp_tuple_eat_2)(220, s) pp_if(p(220, s), pp_for_220, pp_tuple_eat_4)(o(220, s), p, o, m)
#define pp_for_220(s, p, o, m) pp_if(p(221, s), m, pp_tuple_eat_2)(221, s) pp_if(p(221, s), pp_for_221, pp_tuple_eat_4)(o(221, s), p, o, m)
#define pp_for_221(s, p, o, m) pp_if(p(222, s), m, pp_tuple_eat_2)(222, s) pp_if(p(222, s), pp_for_222, pp_tuple_eat_4)(o(222, s), p, o, m)
#define pp_for_222(s, p, o, m) pp_if(p(223, s), m, pp_tuple_eat_2)(223, s) pp_if(p(223, s), pp_for_223, pp_tuple_eat_4)(o(223, s), p, o, m)
#define pp_for_223(s, p, o, m) pp_if(p(224, s), m, pp_tuple_eat_2)(224, s) pp_if(p(224, s), pp_for_224, pp_tuple_eat_4)(o(224, s), p, o, m)
#define pp_for_224(s, p, o, m) pp_if(p(225, s), m, pp_tuple_eat_2)(225, s) pp_if(p(225, s), pp_for_225, pp_tuple_eat_4)(o(225, s), p, o, m)
#define pp_for_225(s, p, o, m) pp_if(p(226, s), m, pp_tuple_eat_2)(226, s) pp_if(p(226, s), pp_for_226, pp_tuple_eat_4)(o(226, s), p, o, m)
#define pp_for_226(s, p, o, m) pp_if(p(227, s), m, pp_tuple_eat_2)(227, s) pp_if(p(227, s), pp_for_227, pp_tuple_eat_4)(o(227, s), p, o, m)
#define pp_for_227(s, p, o, m) pp_if(p(228, s), m, pp_tuple_eat_2)(228, s) pp_if(p(228, s), pp_for_228, pp_tuple_eat_4)(o(228, s), p, o, m)
#define pp_for_228(s, p, o, m) pp_if(p(229, s), m, pp_tuple_eat_2)(229, s) pp_if(p(229, s), pp_for_229, pp_tuple_eat_4)(o(229, s), p, o, m)
#define pp_for_229(s, p, o, m) pp_if(p(230, s), m, pp_tuple_eat_2)(230, s) pp_if(p(230, s), pp_for_230, pp_tuple_eat_4)(o(230, s), p, o, m)
#define pp_for_230(s, p, o, m) pp_if(p(231, s), m, pp_tuple_eat_2)(231, s) pp_if(p(231, s), pp_for_231, pp_tuple_eat_4)(o(231, s), p, o, m)
#define pp_for_231(s, p, o, m) pp_if(p(232, s), m, pp_tuple_eat_2)(232, s) pp_if(p(232, s), pp_for_232, pp_tuple_eat_4)(o(232, s), p, o, m)
#define pp_for_232(s, p, o, m) pp_if(p(233, s), m, pp_tuple_eat_2)(233, s) pp_if(p(233, s), pp_for_233, pp_tuple_eat_4)(o(233, s), p, o, m)
#define pp_for_233(s, p, o, m) pp_if(p(234, s), m, pp_tuple_eat_2)(234, s) pp_if(p(234, s), pp_for_234, pp_tuple_eat_4)(o(234, s), p, o, m)
#define pp_for_234(s, p, o, m) pp_if(p(235, s), m, pp_tuple_eat_2)(235, s) pp_if(p(235, s), pp_for_235, pp_tuple_eat_4)(o(235, s), p, o, m)
#define pp_for_235(s, p, o, m) pp_if(p(236, s), m, pp_tuple_eat_2)(236, s) pp_if(p(236, s), pp_for_236, pp_tuple_eat_4)(o(236, s), p, o, m)
#define pp_for_236(s, p, o, m) pp_if(p(237, s), m, pp_tuple_eat_2)(237, s) pp_if(p(237, s), pp_for_237, pp_tuple_eat_4)(o(237, s), p, o, m)
#define pp_for_237(s, p, o, m) pp_if(p(238, s), m, pp_tuple_eat_2)(238, s) pp_if(p(238, s), pp_for_238, pp_tuple_eat_4)(o(238, s), p, o, m)
#define pp_for_238(s, p, o, m) pp_if(p(239, s), m, pp_tuple_eat_2)(239, s) pp_if(p(239, s), pp_for_239, pp_tuple_eat_4)(o(239, s), p, o, m)
#define pp_for_239(s, p, o, m) pp_if(p(240, s), m, pp_tuple_eat_2)(240, s) pp_if(p(240, s), pp_for_240, pp_tuple_eat_4)(o(240, s), p, o, m)
#define pp_for_240(s, p, o, m) pp_if(p(241, s), m, pp_tuple_eat_2)(241, s) pp_if(p(241, s), pp_for_241, pp_tuple_eat_4)(o(241, s), p, o, m)
#define pp_for_241(s, p, o, m) pp_if(p(242, s), m, pp_tuple_eat_2)(242, s) pp_if(p(242, s), pp_for_242, pp_tuple_eat_4)(o(242, s), p, o, m)
#define pp_for_242(s, p, o, m) pp_if(p(243, s), m, pp_tuple_eat_2)(243, s) pp_if(p(243, s), pp_for_243, pp_tuple_eat_4)(o(243, s), p, o, m)
#define pp_for_243(s, p, o, m) pp_if(p(244, s), m, pp_tuple_eat_2)(244, s) pp_if(p(244, s), pp_for_244, pp_tuple_eat_4)(o(244, s), p, o, m)
#define pp_for_244(s, p, o, m) pp_if(p(245, s), m, pp_tuple_eat_2)(245, s) pp_if(p(245, s), pp_for_245, pp_tuple_eat_4)(o(245, s), p, o, m)
#define pp_for_245(s, p, o, m) pp_if(p(246, s), m, pp_tuple_eat_2)(246, s) pp_if(p(246, s), pp_for_246, pp_tuple_eat_4)(o(246, s), p, o, m)
#define pp_for_246(s, p, o, m) pp_if(p(247, s), m, pp_tuple_eat_2)(247, s) pp_if(p(247, s), pp_for_247, pp_tuple_eat_4)(o(247, s), p, o, m)
#define pp_for_247(s, p, o, m) pp_if(p(248, s), m, pp_tuple_eat_2)(248, s) pp_if(p(248, s), pp_for_248, pp_tuple_eat_4)(o(248, s), p, o, m)
#define pp_for_248(s, p, o, m) pp_if(p(249, s), m, pp_tuple_eat_2)(249, s) pp_if(p(249, s), pp_for_249, pp_tuple_eat_4)(o(249, s), p, o, m)
#define pp_for_249(s, p, o, m) pp_if(p(250, s), m, pp_tuple_eat_2)(250, s) pp_if(p(250, s), pp_for_250, pp_tuple_eat_4)(o(250, s), p, o, m)
#define pp_for_250(s, p, o, m) pp_if(p(251, s), m, pp_tuple_eat_2)(251, s) pp_if(p(251, s), pp_for_251, pp_tuple_eat_4)(o(251, s), p, o, m)
#define pp_for_251(s, p, o, m) pp_if(p(252, s), m, pp_tuple_eat_2)(252, s) pp_if(p(252, s), pp_for_252, pp_tuple_eat_4)(o(252, s), p, o, m)
#define pp_for_252(s, p, o, m) pp_if(p(253, s), m, pp_tuple_eat_2)(253, s) pp_if(p(253, s), pp_for_253, pp_tuple_eat_4)(o(253, s), p, o, m)
#define pp_for_253(s, p, o, m) pp_if(p(254, s), m, pp_tuple_eat_2)(254, s) pp_if(p(254, s), pp_for_254, pp_tuple_eat_4)(o(254, s), p, o, m)
#define pp_for_254(s, p, o, m) pp_if(p(255, s), m, pp_tuple_eat_2)(255, s) pp_if(p(255, s), pp_for_255, pp_tuple_eat_4)(o(255, s), p, o, m)
#define pp_for_255(s, p, o, m) pp_if(p(256, s), m, pp_tuple_eat_2)(256, s) pp_if(p(256, s), pp_for_256, pp_tuple_eat_4)(o(256, s), p, o, m)
#define pp_for_256(s, p, o, m) pp_if(p(257, s), m, pp_tuple_eat_2)(257, s) pp_if(p(257, s), pp_for_257, pp_tuple_eat_4)(o(257, s), p, o, m)

#define pp_for_257(s, p, o, m) pp_error(0x0002)

#define pp_for_check_pp_nil 1

#define pp_for_check_pp_for_1(s, p, o, m) 0
#define pp_for_check_pp_for_2(s, p, o, m) 0
#define pp_for_check_pp_for_3(s, p, o, m) 0
#define pp_for_check_pp_for_4(s, p, o, m) 0
#define pp_for_check_pp_for_5(s, p, o, m) 0
#define pp_for_check_pp_for_6(s, p, o, m) 0
#define pp_for_check_pp_for_7(s, p, o, m) 0
#define pp_for_check_pp_for_8(s, p, o, m) 0
#define pp_for_check_pp_for_9(s, p, o, m) 0
#define pp_for_check_pp_for_10(s, p, o, m) 0
#define pp_for_check_pp_for_11(s, p, o, m) 0
#define pp_for_check_pp_for_12(s, p, o, m) 0
#define pp_for_check_pp_for_13(s, p, o, m) 0
#define pp_for_check_pp_for_14(s, p, o, m) 0
#define pp_for_check_pp_for_15(s, p, o, m) 0
#define pp_for_check_pp_for_16(s, p, o, m) 0
#define pp_for_check_pp_for_17(s, p, o, m) 0
#define pp_for_check_pp_for_18(s, p, o, m) 0
#define pp_for_check_pp_for_19(s, p, o, m) 0
#define pp_for_check_pp_for_20(s, p, o, m) 0
#define pp_for_check_pp_for_21(s, p, o, m) 0
#define pp_for_check_pp_for_22(s, p, o, m) 0
#define pp_for_check_pp_for_23(s, p, o, m) 0
#define pp_for_check_pp_for_24(s, p, o, m) 0
#define pp_for_check_pp_for_25(s, p, o, m) 0
#define pp_for_check_pp_for_26(s, p, o, m) 0
#define pp_for_check_pp_for_27(s, p, o, m) 0
#define pp_for_check_pp_for_28(s, p, o, m) 0
#define pp_for_check_pp_for_29(s, p, o, m) 0
#define pp_for_check_pp_for_30(s, p, o, m) 0
#define pp_for_check_pp_for_31(s, p, o, m) 0
#define pp_for_check_pp_for_32(s, p, o, m) 0
#define pp_for_check_pp_for_33(s, p, o, m) 0
#define pp_for_check_pp_for_34(s, p, o, m) 0
#define pp_for_check_pp_for_35(s, p, o, m) 0
#define pp_for_check_pp_for_36(s, p, o, m) 0
#define pp_for_check_pp_for_37(s, p, o, m) 0
#define pp_for_check_pp_for_38(s, p, o, m) 0
#define pp_for_check_pp_for_39(s, p, o, m) 0
#define pp_for_check_pp_for_40(s, p, o, m) 0
#define pp_for_check_pp_for_41(s, p, o, m) 0
#define pp_for_check_pp_for_42(s, p, o, m) 0
#define pp_for_check_pp_for_43(s, p, o, m) 0
#define pp_for_check_pp_for_44(s, p, o, m) 0
#define pp_for_check_pp_for_45(s, p, o, m) 0
#define pp_for_check_pp_for_46(s, p, o, m) 0
#define pp_for_check_pp_for_47(s, p, o, m) 0
#define pp_for_check_pp_for_48(s, p, o, m) 0
#define pp_for_check_pp_for_49(s, p, o, m) 0
#define pp_for_check_pp_for_50(s, p, o, m) 0
#define pp_for_check_pp_for_51(s, p, o, m) 0
#define pp_for_check_pp_for_52(s, p, o, m) 0
#define pp_for_check_pp_for_53(s, p, o, m) 0
#define pp_for_check_pp_for_54(s, p, o, m) 0
#define pp_for_check_pp_for_55(s, p, o, m) 0
#define pp_for_check_pp_for_56(s, p, o, m) 0
#define pp_for_check_pp_for_57(s, p, o, m) 0
#define pp_for_check_pp_for_58(s, p, o, m) 0
#define pp_for_check_pp_for_59(s, p, o, m) 0
#define pp_for_check_pp_for_60(s, p, o, m) 0
#define pp_for_check_pp_for_61(s, p, o, m) 0
#define pp_for_check_pp_for_62(s, p, o, m) 0
#define pp_for_check_pp_for_63(s, p, o, m) 0
#define pp_for_check_pp_for_64(s, p, o, m) 0
#define pp_for_check_pp_for_65(s, p, o, m) 0
#define pp_for_check_pp_for_66(s, p, o, m) 0
#define pp_for_check_pp_for_67(s, p, o, m) 0
#define pp_for_check_pp_for_68(s, p, o, m) 0
#define pp_for_check_pp_for_69(s, p, o, m) 0
#define pp_for_check_pp_for_70(s, p, o, m) 0
#define pp_for_check_pp_for_71(s, p, o, m) 0
#define pp_for_check_pp_for_72(s, p, o, m) 0
#define pp_for_check_pp_for_73(s, p, o, m) 0
#define pp_for_check_pp_for_74(s, p, o, m) 0
#define pp_for_check_pp_for_75(s, p, o, m) 0
#define pp_for_check_pp_for_76(s, p, o, m) 0
#define pp_for_check_pp_for_77(s, p, o, m) 0
#define pp_for_check_pp_for_78(s, p, o, m) 0
#define pp_for_check_pp_for_79(s, p, o, m) 0
#define pp_for_check_pp_for_80(s, p, o, m) 0
#define pp_for_check_pp_for_81(s, p, o, m) 0
#define pp_for_check_pp_for_82(s, p, o, m) 0
#define pp_for_check_pp_for_83(s, p, o, m) 0
#define pp_for_check_pp_for_84(s, p, o, m) 0
#define pp_for_check_pp_for_85(s, p, o, m) 0
#define pp_for_check_pp_for_86(s, p, o, m) 0
#define pp_for_check_pp_for_87(s, p, o, m) 0
#define pp_for_check_pp_for_88(s, p, o, m) 0
#define pp_for_check_pp_for_89(s, p, o, m) 0
#define pp_for_check_pp_for_90(s, p, o, m) 0
#define pp_for_check_pp_for_91(s, p, o, m) 0
#define pp_for_check_pp_for_92(s, p, o, m) 0
#define pp_for_check_pp_for_93(s, p, o, m) 0
#define pp_for_check_pp_for_94(s, p, o, m) 0
#define pp_for_check_pp_for_95(s, p, o, m) 0
#define pp_for_check_pp_for_96(s, p, o, m) 0
#define pp_for_check_pp_for_97(s, p, o, m) 0
#define pp_for_check_pp_for_98(s, p, o, m) 0
#define pp_for_check_pp_for_99(s, p, o, m) 0
#define pp_for_check_pp_for_100(s, p, o, m) 0
#define pp_for_check_pp_for_101(s, p, o, m) 0
#define pp_for_check_pp_for_102(s, p, o, m) 0
#define pp_for_check_pp_for_103(s, p, o, m) 0
#define pp_for_check_pp_for_104(s, p, o, m) 0
#define pp_for_check_pp_for_105(s, p, o, m) 0
#define pp_for_check_pp_for_106(s, p, o, m) 0
#define pp_for_check_pp_for_107(s, p, o, m) 0
#define pp_for_check_pp_for_108(s, p, o, m) 0
#define pp_for_check_pp_for_109(s, p, o, m) 0
#define pp_for_check_pp_for_110(s, p, o, m) 0
#define pp_for_check_pp_for_111(s, p, o, m) 0
#define pp_for_check_pp_for_112(s, p, o, m) 0
#define pp_for_check_pp_for_113(s, p, o, m) 0
#define pp_for_check_pp_for_114(s, p, o, m) 0
#define pp_for_check_pp_for_115(s, p, o, m) 0
#define pp_for_check_pp_for_116(s, p, o, m) 0
#define pp_for_check_pp_for_117(s, p, o, m) 0
#define pp_for_check_pp_for_118(s, p, o, m) 0
#define pp_for_check_pp_for_119(s, p, o, m) 0
#define pp_for_check_pp_for_120(s, p, o, m) 0
#define pp_for_check_pp_for_121(s, p, o, m) 0
#define pp_for_check_pp_for_122(s, p, o, m) 0
#define pp_for_check_pp_for_123(s, p, o, m) 0
#define pp_for_check_pp_for_124(s, p, o, m) 0
#define pp_for_check_pp_for_125(s, p, o, m) 0
#define pp_for_check_pp_for_126(s, p, o, m) 0
#define pp_for_check_pp_for_127(s, p, o, m) 0
#define pp_for_check_pp_for_128(s, p, o, m) 0
#define pp_for_check_pp_for_129(s, p, o, m) 0
#define pp_for_check_pp_for_130(s, p, o, m) 0
#define pp_for_check_pp_for_131(s, p, o, m) 0
#define pp_for_check_pp_for_132(s, p, o, m) 0
#define pp_for_check_pp_for_133(s, p, o, m) 0
#define pp_for_check_pp_for_134(s, p, o, m) 0
#define pp_for_check_pp_for_135(s, p, o, m) 0
#define pp_for_check_pp_for_136(s, p, o, m) 0
#define pp_for_check_pp_for_137(s, p, o, m) 0
#define pp_for_check_pp_for_138(s, p, o, m) 0
#define pp_for_check_pp_for_139(s, p, o, m) 0
#define pp_for_check_pp_for_140(s, p, o, m) 0
#define pp_for_check_pp_for_141(s, p, o, m) 0
#define pp_for_check_pp_for_142(s, p, o, m) 0
#define pp_for_check_pp_for_143(s, p, o, m) 0
#define pp_for_check_pp_for_144(s, p, o, m) 0
#define pp_for_check_pp_for_145(s, p, o, m) 0
#define pp_for_check_pp_for_146(s, p, o, m) 0
#define pp_for_check_pp_for_147(s, p, o, m) 0
#define pp_for_check_pp_for_148(s, p, o, m) 0
#define pp_for_check_pp_for_149(s, p, o, m) 0
#define pp_for_check_pp_for_150(s, p, o, m) 0
#define pp_for_check_pp_for_151(s, p, o, m) 0
#define pp_for_check_pp_for_152(s, p, o, m) 0
#define pp_for_check_pp_for_153(s, p, o, m) 0
#define pp_for_check_pp_for_154(s, p, o, m) 0
#define pp_for_check_pp_for_155(s, p, o, m) 0
#define pp_for_check_pp_for_156(s, p, o, m) 0
#define pp_for_check_pp_for_157(s, p, o, m) 0
#define pp_for_check_pp_for_158(s, p, o, m) 0
#define pp_for_check_pp_for_159(s, p, o, m) 0
#define pp_for_check_pp_for_160(s, p, o, m) 0
#define pp_for_check_pp_for_161(s, p, o, m) 0
#define pp_for_check_pp_for_162(s, p, o, m) 0
#define pp_for_check_pp_for_163(s, p, o, m) 0
#define pp_for_check_pp_for_164(s, p, o, m) 0
#define pp_for_check_pp_for_165(s, p, o, m) 0
#define pp_for_check_pp_for_166(s, p, o, m) 0
#define pp_for_check_pp_for_167(s, p, o, m) 0
#define pp_for_check_pp_for_168(s, p, o, m) 0
#define pp_for_check_pp_for_169(s, p, o, m) 0
#define pp_for_check_pp_for_170(s, p, o, m) 0
#define pp_for_check_pp_for_171(s, p, o, m) 0
#define pp_for_check_pp_for_172(s, p, o, m) 0
#define pp_for_check_pp_for_173(s, p, o, m) 0
#define pp_for_check_pp_for_174(s, p, o, m) 0
#define pp_for_check_pp_for_175(s, p, o, m) 0
#define pp_for_check_pp_for_176(s, p, o, m) 0
#define pp_for_check_pp_for_177(s, p, o, m) 0
#define pp_for_check_pp_for_178(s, p, o, m) 0
#define pp_for_check_pp_for_179(s, p, o, m) 0
#define pp_for_check_pp_for_180(s, p, o, m) 0
#define pp_for_check_pp_for_181(s, p, o, m) 0
#define pp_for_check_pp_for_182(s, p, o, m) 0
#define pp_for_check_pp_for_183(s, p, o, m) 0
#define pp_for_check_pp_for_184(s, p, o, m) 0
#define pp_for_check_pp_for_185(s, p, o, m) 0
#define pp_for_check_pp_for_186(s, p, o, m) 0
#define pp_for_check_pp_for_187(s, p, o, m) 0
#define pp_for_check_pp_for_188(s, p, o, m) 0
#define pp_for_check_pp_for_189(s, p, o, m) 0
#define pp_for_check_pp_for_190(s, p, o, m) 0
#define pp_for_check_pp_for_191(s, p, o, m) 0
#define pp_for_check_pp_for_192(s, p, o, m) 0
#define pp_for_check_pp_for_193(s, p, o, m) 0
#define pp_for_check_pp_for_194(s, p, o, m) 0
#define pp_for_check_pp_for_195(s, p, o, m) 0
#define pp_for_check_pp_for_196(s, p, o, m) 0
#define pp_for_check_pp_for_197(s, p, o, m) 0
#define pp_for_check_pp_for_198(s, p, o, m) 0
#define pp_for_check_pp_for_199(s, p, o, m) 0
#define pp_for_check_pp_for_200(s, p, o, m) 0
#define pp_for_check_pp_for_201(s, p, o, m) 0
#define pp_for_check_pp_for_202(s, p, o, m) 0
#define pp_for_check_pp_for_203(s, p, o, m) 0
#define pp_for_check_pp_for_204(s, p, o, m) 0
#define pp_for_check_pp_for_205(s, p, o, m) 0
#define pp_for_check_pp_for_206(s, p, o, m) 0
#define pp_for_check_pp_for_207(s, p, o, m) 0
#define pp_for_check_pp_for_208(s, p, o, m) 0
#define pp_for_check_pp_for_209(s, p, o, m) 0
#define pp_for_check_pp_for_210(s, p, o, m) 0
#define pp_for_check_pp_for_211(s, p, o, m) 0
#define pp_for_check_pp_for_212(s, p, o, m) 0
#define pp_for_check_pp_for_213(s, p, o, m) 0
#define pp_for_check_pp_for_214(s, p, o, m) 0
#define pp_for_check_pp_for_215(s, p, o, m) 0
#define pp_for_check_pp_for_216(s, p, o, m) 0
#define pp_for_check_pp_for_217(s, p, o, m) 0
#define pp_for_check_pp_for_218(s, p, o, m) 0
#define pp_for_check_pp_for_219(s, p, o, m) 0
#define pp_for_check_pp_for_220(s, p, o, m) 0
#define pp_for_check_pp_for_221(s, p, o, m) 0
#define pp_for_check_pp_for_222(s, p, o, m) 0
#define pp_for_check_pp_for_223(s, p, o, m) 0
#define pp_for_check_pp_for_224(s, p, o, m) 0
#define pp_for_check_pp_for_225(s, p, o, m) 0
#define pp_for_check_pp_for_226(s, p, o, m) 0
#define pp_for_check_pp_for_227(s, p, o, m) 0
#define pp_for_check_pp_for_228(s, p, o, m) 0
#define pp_for_check_pp_for_229(s, p, o, m) 0
#define pp_for_check_pp_for_230(s, p, o, m) 0
#define pp_for_check_pp_for_231(s, p, o, m) 0
#define pp_for_check_pp_for_232(s, p, o, m) 0
#define pp_for_check_pp_for_233(s, p, o, m) 0
#define pp_for_check_pp_for_234(s, p, o, m) 0
#define pp_for_check_pp_for_235(s, p, o, m) 0
#define pp_for_check_pp_for_236(s, p, o, m) 0
#define pp_for_check_pp_for_237(s, p, o, m) 0
#define pp_for_check_pp_for_238(s, p, o, m) 0
#define pp_for_check_pp_for_239(s, p, o, m) 0
#define pp_for_check_pp_for_240(s, p, o, m) 0
#define pp_for_check_pp_for_241(s, p, o, m) 0
#define pp_for_check_pp_for_242(s, p, o, m) 0
#define pp_for_check_pp_for_243(s, p, o, m) 0
#define pp_for_check_pp_for_244(s, p, o, m) 0
#define pp_for_check_pp_for_245(s, p, o, m) 0
#define pp_for_check_pp_for_246(s, p, o, m) 0
#define pp_for_check_pp_for_247(s, p, o, m) 0
#define pp_for_check_pp_for_248(s, p, o, m) 0
#define pp_for_check_pp_for_249(s, p, o, m) 0
#define pp_for_check_pp_for_250(s, p, o, m) 0
#define pp_for_check_pp_for_251(s, p, o, m) 0
#define pp_for_check_pp_for_252(s, p, o, m) 0
#define pp_for_check_pp_for_253(s, p, o, m) 0
#define pp_for_check_pp_for_254(s, p, o, m) 0
#define pp_for_check_pp_for_255(s, p, o, m) 0
#define pp_for_check_pp_for_256(s, p, o, m) 0
