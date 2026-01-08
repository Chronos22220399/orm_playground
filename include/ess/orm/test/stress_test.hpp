#pragma once
#include <ess/orm/attribute.hpp>
#include <ess/orm/dsl.hpp>
#include <string>

using namespace ess::orm;
using namespace ess::orm::meta;

enum class NodeStatus { Active, Idle, Error, Maintenance, Unknown };

struct Massive240 {
  // --- 1-40: Integers ---
  int i1, i2, i3, i4, i5, i6, i7, i8, i9, i10, i11, i12, i13, i14, i15, i16,
      i17, i18, i19, i20;
  int i21, i22, i23, i24, i25, i26, i27, i28, i29, i30, i31, i32, i33, i34, i35,
      i36, i37, i38, i39, i40;

  // --- 41-80: Doubles ---
  double d1, d2, d3, d4, d5, d6, d7, d8, d9, d10, d11, d12, d13, d14, d15, d16,
      d17, d18, d19, d20;
  double d21, d22, d23, d24, d25, d26, d27, d28, d29, d30, d31, d32, d33, d34,
      d35, d36, d37, d38, d39, d40;

  // --- 81-120: Booleans ---
  bool b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16,
      b17, b18, b19, b20;
  bool b21, b22, b23, b24, b25, b26, b27, b28, b29, b30, b31, b32, b33, b34,
      b35, b36, b37, b38, b39, b40;

  // --- 121-160: Strings ---
  std::string s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15,
      s16, s17, s18, s19, s20;
  std::string s21, s22, s23, s24, s25, s26, s27, s28, s29, s30, s31, s32, s33,
      s34, s35, s36, s37, s38, s39, s40;

  // --- 161-200: Enums ---
  NodeStatus e1, e2, e3, e4, e5, e6, e7, e8, e9, e10, e11, e12, e13, e14, e15,
      e16, e17, e18, e19, e20;
  NodeStatus e21, e22, e23, e24, e25, e26, e27, e28, e29, e30, e31, e32, e33,
      e34, e35, e36, e37, e38, e39, e40;

  // --- 201-240: Long Longs ---
  long long l1, l2, l3, l4, l5, l6, l7, l8, l9, l10, l11, l12, l13, l14, l15,
      l16, l17, l18, l19, l20;
  long long l21, l22, l23, l24, l25, l26, l27, l28, l29, l30, l31, l32, l33,
      l34, l35, l36, l37, l38, l39, l40;

  using Schema = dsl::Schema<
      "massive_240",
      // Primary Key
      dsl::Field<"id", &Massive240::i1, attribute::PrimaryKey>,

      // Ints (2-40)
      dsl::Field<"i2", &Massive240::i2>, dsl::Field<"i3", &Massive240::i3>,
      dsl::Field<"i4", &Massive240::i4>, dsl::Field<"i5", &Massive240::i5>,
      dsl::Field<"i6", &Massive240::i6>, dsl::Field<"i7", &Massive240::i7>,
      dsl::Field<"i8", &Massive240::i8>, dsl::Field<"i9", &Massive240::i9>,
      dsl::Field<"i10", &Massive240::i10>, dsl::Field<"i11", &Massive240::i11>,
      dsl::Field<"i12", &Massive240::i12>, dsl::Field<"i13", &Massive240::i13>,
      dsl::Field<"i14", &Massive240::i14>, dsl::Field<"i15", &Massive240::i15>,
      dsl::Field<"i16", &Massive240::i16>, dsl::Field<"i17", &Massive240::i17>,
      dsl::Field<"i18", &Massive240::i18>, dsl::Field<"i19", &Massive240::i19>,
      dsl::Field<"i20", &Massive240::i20>, dsl::Field<"i21", &Massive240::i21>,
      dsl::Field<"i22", &Massive240::i22>, dsl::Field<"i23", &Massive240::i23>,
      dsl::Field<"i24", &Massive240::i24>, dsl::Field<"i25", &Massive240::i25>,
      dsl::Field<"i26", &Massive240::i26>, dsl::Field<"i27", &Massive240::i27>,
      dsl::Field<"i28", &Massive240::i28>, dsl::Field<"i29", &Massive240::i29>,
      dsl::Field<"i30", &Massive240::i30>, dsl::Field<"i31", &Massive240::i31>,
      dsl::Field<"i32", &Massive240::i32>, dsl::Field<"i33", &Massive240::i33>,
      dsl::Field<"i34", &Massive240::i34>, dsl::Field<"i35", &Massive240::i35>,
      dsl::Field<"i36", &Massive240::i36>, dsl::Field<"i37", &Massive240::i37>,
      dsl::Field<"i38", &Massive240::i38>, dsl::Field<"i39", &Massive240::i39>,
      dsl::Field<"i40", &Massive240::i40>,

      // Doubles (41-80)
      dsl::Field<"d1", &Massive240::d1>, dsl::Field<"d2", &Massive240::d2>,
      dsl::Field<"d3", &Massive240::d3>, dsl::Field<"d4", &Massive240::d4>,
      dsl::Field<"d5", &Massive240::d5>, dsl::Field<"d6", &Massive240::d6>,
      dsl::Field<"d7", &Massive240::d7>, dsl::Field<"d8", &Massive240::d8>,
      dsl::Field<"d9", &Massive240::d9>, dsl::Field<"d10", &Massive240::d10>,
      dsl::Field<"d11", &Massive240::d11>, dsl::Field<"d12", &Massive240::d12>,
      dsl::Field<"d13", &Massive240::d13>, dsl::Field<"d14", &Massive240::d14>,
      dsl::Field<"d15", &Massive240::d15>, dsl::Field<"d16", &Massive240::d16>,
      dsl::Field<"d17", &Massive240::d17>, dsl::Field<"d18", &Massive240::d18>,
      dsl::Field<"d19", &Massive240::d19>, dsl::Field<"d20", &Massive240::d20>,
      dsl::Field<"d21", &Massive240::d21>, dsl::Field<"d22", &Massive240::d22>,
      dsl::Field<"d23", &Massive240::d23>, dsl::Field<"d24", &Massive240::d24>,
      dsl::Field<"d25", &Massive240::d25>, dsl::Field<"d26", &Massive240::d26>,
      dsl::Field<"d27", &Massive240::d27>, dsl::Field<"d28", &Massive240::d28>,
      dsl::Field<"d29", &Massive240::d29>, dsl::Field<"d30", &Massive240::d30>,
      dsl::Field<"d31", &Massive240::d31>, dsl::Field<"d32", &Massive240::d32>,
      dsl::Field<"d33", &Massive240::d33>, dsl::Field<"d34", &Massive240::d34>,
      dsl::Field<"d35", &Massive240::d35>, dsl::Field<"d36", &Massive240::d36>,
      dsl::Field<"d37", &Massive240::d37>, dsl::Field<"d38", &Massive240::d38>,
      dsl::Field<"d39", &Massive240::d39>, dsl::Field<"d40", &Massive240::d40>,

      // Bools (81-120)
      dsl::Field<"b1", &Massive240::b1>, dsl::Field<"b2", &Massive240::b2>,
      dsl::Field<"b3", &Massive240::b3>, dsl::Field<"b4", &Massive240::b4>,
      dsl::Field<"b5", &Massive240::b5>, dsl::Field<"b6", &Massive240::b6>,
      dsl::Field<"b7", &Massive240::b7>, dsl::Field<"b8", &Massive240::b8>,
      dsl::Field<"b9", &Massive240::b9>, dsl::Field<"b10", &Massive240::b10>,
      dsl::Field<"b11", &Massive240::b11>, dsl::Field<"b12", &Massive240::b12>,
      dsl::Field<"b13", &Massive240::b13>, dsl::Field<"b14", &Massive240::b14>,
      dsl::Field<"b15", &Massive240::b15>, dsl::Field<"b16", &Massive240::b16>,
      dsl::Field<"b17", &Massive240::b17>, dsl::Field<"b18", &Massive240::b18>,
      dsl::Field<"b19", &Massive240::b19>, dsl::Field<"b20", &Massive240::b20>,
      dsl::Field<"b21", &Massive240::b21>, dsl::Field<"b22", &Massive240::b22>,
      dsl::Field<"b23", &Massive240::b23>, dsl::Field<"b24", &Massive240::b24>,
      dsl::Field<"b25", &Massive240::b25>, dsl::Field<"b26", &Massive240::b26>,
      dsl::Field<"b27", &Massive240::b27>, dsl::Field<"b28", &Massive240::b28>,
      dsl::Field<"b29", &Massive240::b29>, dsl::Field<"b30", &Massive240::b30>,
      dsl::Field<"b31", &Massive240::b31>, dsl::Field<"b32", &Massive240::b32>,
      dsl::Field<"b33", &Massive240::b33>, dsl::Field<"b34", &Massive240::b34>,
      dsl::Field<"b35", &Massive240::b35>, dsl::Field<"b36", &Massive240::b36>,
      dsl::Field<"b37", &Massive240::b37>, dsl::Field<"b38", &Massive240::b38>,
      dsl::Field<"b39", &Massive240::b39>, dsl::Field<"b40", &Massive240::b40>,

      // Strings (121-160)
      dsl::Field<"s1", &Massive240::s1>, dsl::Field<"s2", &Massive240::s2>,
      dsl::Field<"s3", &Massive240::s3>, dsl::Field<"s4", &Massive240::s4>,
      dsl::Field<"s5", &Massive240::s5>, dsl::Field<"s6", &Massive240::s6>,
      dsl::Field<"s7", &Massive240::s7>, dsl::Field<"s8", &Massive240::s8>,
      dsl::Field<"s9", &Massive240::s9>, dsl::Field<"s10", &Massive240::s10>,
      dsl::Field<"s11", &Massive240::s11>, dsl::Field<"s12", &Massive240::s12>,
      dsl::Field<"s13", &Massive240::s13>, dsl::Field<"s14", &Massive240::s14>,
      dsl::Field<"s15", &Massive240::s15>, dsl::Field<"s16", &Massive240::s16>,
      dsl::Field<"s17", &Massive240::s17>, dsl::Field<"s18", &Massive240::s18>,
      dsl::Field<"s19", &Massive240::s19>, dsl::Field<"s20", &Massive240::s20>,
      dsl::Field<"s21", &Massive240::s21>, dsl::Field<"s22", &Massive240::s22>,
      dsl::Field<"s23", &Massive240::s23>, dsl::Field<"s24", &Massive240::s24>,
      dsl::Field<"s25", &Massive240::s25>, dsl::Field<"s26", &Massive240::s26>,
      dsl::Field<"s27", &Massive240::s27>, dsl::Field<"s28", &Massive240::s28>,
      dsl::Field<"s29", &Massive240::s29>, dsl::Field<"s30", &Massive240::s30>,
      dsl::Field<"s31", &Massive240::s31>, dsl::Field<"s32", &Massive240::s32>,
      dsl::Field<"s33", &Massive240::s33>, dsl::Field<"s34", &Massive240::s34>,
      dsl::Field<"s35", &Massive240::s35>, dsl::Field<"s36", &Massive240::s36>,
      dsl::Field<"s37", &Massive240::s37>, dsl::Field<"s38", &Massive240::s38>,
      dsl::Field<"s39", &Massive240::s39>, dsl::Field<"s40", &Massive240::s40>,

      // Enums (161-200)
      dsl::Field<"e1", &Massive240::e1>, dsl::Field<"e2", &Massive240::e2>,
      dsl::Field<"e3", &Massive240::e3>, dsl::Field<"e4", &Massive240::e4>,
      dsl::Field<"e5", &Massive240::e5>, dsl::Field<"e6", &Massive240::e6>,
      dsl::Field<"e7", &Massive240::e7>, dsl::Field<"e8", &Massive240::e8>,
      dsl::Field<"e9", &Massive240::e9>, dsl::Field<"e10", &Massive240::e10>,
      dsl::Field<"e11", &Massive240::e11>, dsl::Field<"e12", &Massive240::e12>,
      dsl::Field<"e13", &Massive240::e13>, dsl::Field<"e14", &Massive240::e14>,
      dsl::Field<"e15", &Massive240::e15>, dsl::Field<"e16", &Massive240::e16>,
      dsl::Field<"e17", &Massive240::e17>, dsl::Field<"e18", &Massive240::e18>,
      dsl::Field<"e19", &Massive240::e19>, dsl::Field<"e20", &Massive240::e20>,
      dsl::Field<"e21", &Massive240::e21>, dsl::Field<"e22", &Massive240::e22>,
      dsl::Field<"e23", &Massive240::e23>, dsl::Field<"e24", &Massive240::e24>,
      dsl::Field<"e25", &Massive240::e25>, dsl::Field<"e26", &Massive240::e26>,
      dsl::Field<"e27", &Massive240::e27>, dsl::Field<"e28", &Massive240::e28>,
      dsl::Field<"e29", &Massive240::e29>, dsl::Field<"e30", &Massive240::e30>,
      dsl::Field<"e31", &Massive240::e31>, dsl::Field<"e32", &Massive240::e32>,
      dsl::Field<"e33", &Massive240::e33>, dsl::Field<"e34", &Massive240::e34>,
      dsl::Field<"e35", &Massive240::e35>, dsl::Field<"e36", &Massive240::e36>,
      dsl::Field<"e37", &Massive240::e37>, dsl::Field<"e38", &Massive240::e38>,
      dsl::Field<"e39", &Massive240::e39>, dsl::Field<"e40", &Massive240::e40>,

      // Long Longs (201-240)
      dsl::Field<"l1", &Massive240::l1>, dsl::Field<"l2", &Massive240::l2>,
      dsl::Field<"l3", &Massive240::l3>, dsl::Field<"l4", &Massive240::l4>,
      dsl::Field<"l5", &Massive240::l5>, dsl::Field<"l6", &Massive240::l6>,
      dsl::Field<"l7", &Massive240::l7>, dsl::Field<"l8", &Massive240::l8>,
      dsl::Field<"l9", &Massive240::l9>, dsl::Field<"l10", &Massive240::l10>,
      dsl::Field<"l11", &Massive240::l11>, dsl::Field<"l12", &Massive240::l12>,
      dsl::Field<"l13", &Massive240::l13>, dsl::Field<"l14", &Massive240::l14>,
      dsl::Field<"l15", &Massive240::l15>, dsl::Field<"l16", &Massive240::l16>,
      dsl::Field<"l17", &Massive240::l17>, dsl::Field<"l18", &Massive240::l18>,
      dsl::Field<"l19", &Massive240::l19>, dsl::Field<"l20", &Massive240::l20>,
      dsl::Field<"l21", &Massive240::l21>, dsl::Field<"l22", &Massive240::l22>,
      dsl::Field<"l23", &Massive240::l23>, dsl::Field<"l24", &Massive240::l24>,
      dsl::Field<"l25", &Massive240::l25>, dsl::Field<"l26", &Massive240::l26>,
      dsl::Field<"l27", &Massive240::l27>, dsl::Field<"l28", &Massive240::l28>,
      dsl::Field<"l29", &Massive240::l29>, dsl::Field<"l30", &Massive240::l30>,
      dsl::Field<"l31", &Massive240::l31>, dsl::Field<"l32", &Massive240::l32>,
      dsl::Field<"l33", &Massive240::l33>, dsl::Field<"l34", &Massive240::l34>,
      dsl::Field<"l35", &Massive240::l35>, dsl::Field<"l36", &Massive240::l36>,
      dsl::Field<"l37", &Massive240::l37>, dsl::Field<"l38", &Massive240::l38>,
      dsl::Field<"l39", &Massive240::l39>, dsl::Field<"l40", &Massive240::l40>>;
};
