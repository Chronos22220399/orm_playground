#pragma once
#include <ess/orm/dsl/attribute.hpp>

namespace ess::orm::dsl::traits {
template <auto> inline constexpr bool dependent_false_v = false;

template <auto T> struct MemberPointerTraits {
  static_assert(dependent_false_v<T>, "unsupported member pointer type");
};

template <typename Class, typename Member, Member Class::*Ptr>
struct MemberPointerTraits<Ptr> {
  using class_type = Class;
  using member_type = Member;
  using pointer_type = decltype(Ptr);

  static constexpr Member Class::*pointer = Ptr;
  static constexpr bool is_static = false;
};

template <typename Member, Member *Ptr> struct MemberPointerTraits<Ptr> {
  using member_type = Member;
  using pointer_type = decltype(Ptr);

  static constexpr Member *pointer = Ptr;
  static constexpr bool is_static = true;
};

template <auto Ptr> struct MemberAccessor {
  using traits = MemberPointerTraits<Ptr>;

  static traits::member_type &get(traits::class_type &obj) {
    return obj.*(traits::pointer);
  }
};

} // namespace ess::orm::dsl::traits
