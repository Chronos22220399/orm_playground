#include <core.hpp>
#include <std.hpp>
#include <algorithm>
#include <array>
#include <cassert>
#include <deque>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#include <core_thirdparties.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <defines.h>
#include <ess/orm/core_orm.hpp>

#include <ess/orm/config/config.hpp>
#include <ess/orm/config/traits.hpp>

#include <chrono>
#include <concepts>
#include <ess/orm/config/default.hpp>

#include <chrono>
#include <ess/orm/dialect.hpp>

#include <ess/orm/meta.hpp>
#include <optional>

#include <ess/orm/meta.hpp>
#include <ess/orm/dialect.hpp>

#include <ess/orm/dialect.hpp>

#include <ess_orm_user_config.hpp>

namespace ess
{
  namespace orm
  {
    namespace meta
    {
      template<unsigned long N>
      struct FixedString
      {
        char m_str[N];
        inline constexpr FixedString(const char (&str)[N])
        {
          for(int i = 0; static_cast<unsigned long>(i) < N; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString()
        {
          for(int i = 0; static_cast<unsigned long>(i) < N; ++i) {
            this->m_str[i] = '\0';
          }
          
        }
        
        inline constexpr char & operator[](size_t idx)
        {
          (static_cast<bool>(__builtin_expect(static_cast<long>(!((idx < N) && "index out of range")), 0)) ? __assert_rtn(__func__, "meta.hpp", 19, "idx < N && \"index out of range\"") : static_cast<void>(0));
          return this->m_str[idx];
        }
        
        inline constexpr char operator[](size_t idx) const
        {
          (static_cast<bool>(__builtin_expect(static_cast<long>(!((idx < N) && "index out of range")), 0)) ? __assert_rtn(__func__, "meta.hpp", 24, "idx < N && \"index out of range\"") : static_cast<void>(0));
          return this->m_str[idx];
        }
        
        template<unsigned long Idx>
        inline constexpr const char & get() const
        {
          /* PASSED: static_assert(Idx < N, "index out of range"); */
          return this->m_str[Idx];
        }
        inline constexpr unsigned long size() const
        {
          return std::size(this->m_str);
        }
        
        inline constexpr const char * data() const
        {
          return this->m_str;
        }
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{{this->m_str, N - 1}};
        }
        
      };
      
      /* First instantiated from: schema.cpp:23 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<3>
      {
        char m_str[3];
        inline constexpr FixedString(const char (&str)[3])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 3UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 3UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<3> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: schema.cpp:25 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<6>
      {
        char m_str[6];
        inline constexpr FixedString(const char (&str)[6])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 6UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 6UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<6> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: meta.hpp:215 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<1>
      {
        char m_str[1];
        inline constexpr FixedString(const char (&str)[1])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 1UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const;
        
      };
      
      #endif
      /* First instantiated from: meta.hpp:359 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<4>
      {
        char m_str[4];
        inline constexpr FixedString(const char (&str)[4])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 4UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 4UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<4> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: meta.hpp:363 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<5>
      {
        char m_str[5];
        inline constexpr FixedString(const char (&str)[5])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 5UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 5UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<5> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: meta.hpp:371 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<8>
      {
        char m_str[8];
        inline constexpr FixedString(const char (&str)[8])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 8UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 8UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<8> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:195 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<12>
      {
        char m_str[12];
        inline constexpr FixedString(const char (&str)[12])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 12UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 12UL - 1};
        }
        
      };
      
      #endif
      /* First instantiated from: attribute.hpp:199 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<7>
      {
        char m_str[7];
        inline constexpr FixedString(const char (&str)[7])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 7UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 7UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<7> &) noexcept = default;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:203 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct FixedString<9>
      {
        char m_str[9];
        inline constexpr FixedString(const char (&str)[9])
        {
          for(int i = 0; static_cast<unsigned long>(i) < 9UL; ++i) {
            this->m_str[i] = str[i];
          }
          
        }
        
        inline constexpr FixedString();
        
        inline constexpr char & operator[](size_t idx);
        
        inline constexpr char operator[](size_t idx) const;
        
        template<unsigned long Idx>
        inline constexpr const char & get() const;
        inline constexpr unsigned long size() const;
        
        inline constexpr const char * data() const;
        
        inline constexpr operator std::basic_string_view<char, std::char_traits<char> > () const
        {
          return std::basic_string_view<char, std::char_traits<char> >{this->m_str, 9UL - 1};
        }
        
        // inline constexpr FixedString(const FixedString<9> &) noexcept = default;
      };
      
      #endif
      template<FixedString Str>
      inline constexpr auto operator""_fs()
      {
        return Str;
      }
      
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr meta::FixedString<4> FixedString<4>_"INT"_{"INT"};
      template<>
      inline constexpr meta::FixedString<4> operator""_fs<FixedString<4>_"INT"_>()
      {
        return meta::FixedString<4>(FixedString<4>_"INT"_);
      }
      #endif
      
      
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr meta::FixedString<5> FixedString<5>_"REAL"_{"REAL"};
      template<>
      inline constexpr meta::FixedString<5> operator""_fs<FixedString<5>_"REAL"_>()
      {
        return meta::FixedString<5>(FixedString<5>_"REAL"_);
      }
      #endif
      
      
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr meta::FixedString<5> FixedString<5>_"TEXT"_{"TEXT"};
      template<>
      inline constexpr meta::FixedString<5> operator""_fs<FixedString<5>_"TEXT"_>()
      {
        return meta::FixedString<5>(FixedString<5>_"TEXT"_);
      }
      #endif
      
      
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr meta::FixedString<8> FixedString<8>_"BOOLEAN"_{"BOOLEAN"};
      template<>
      inline constexpr meta::FixedString<8> operator""_fs<FixedString<8>_"BOOLEAN"_>()
      {
        return meta::FixedString<8>(FixedString<8>_"BOOLEAN"_);
      }
      #endif
      
      
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr meta::FixedString<9> FixedString<9>_"untitled"_{"untitled"};
      template<>
      inline constexpr meta::FixedString<9> operator""_fs<FixedString<9>_"untitled"_>()
      {
        return meta::FixedString<9>(FixedString<9>_"untitled"_);
      }
      #endif
      
      template<unsigned long N1, unsigned long N2>
      inline constexpr bool fs_equal(FixedString<N1> lhs, FixedString<N2> rhs)
      {
        if constexpr(N2 != N1) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (N1 - 1); ++i) {
          if(lhs.m_str[i] != rhs.m_str[i]) {
            return false;
          } 
          
        }
        
        return true;
      }
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<3, 8>(meta::FixedString<3> lhs, meta::FixedString<8> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (3UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<3, 7>(meta::FixedString<3> lhs, meta::FixedString<7> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (3UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<3, 6>(meta::FixedString<3> lhs, meta::FixedString<6> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (3UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<6, 8>(meta::FixedString<6> lhs, meta::FixedString<8> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (6UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<6, 7>(meta::FixedString<6> lhs, meta::FixedString<7> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (6UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<6, 6>(meta::FixedString<6> lhs, meta::FixedString<6> rhs)
      {
        if constexpr(false) {
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (6UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool fs_equal<7, 8>(meta::FixedString<7> lhs, meta::FixedString<8> rhs)
      {
        if constexpr(true) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (7UL - 1); ++i) {
          if(static_cast<int>(lhs.m_str[i]) != static_cast<int>(rhs.m_str[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      #endif
      
      template<unsigned long N1, unsigned long N2>
      inline constexpr auto fs_concat(FixedString<N1> str_1, FixedString<N2> str_2)
      {
        FixedString<N1 + N2 - 1> result = {};
        for(unsigned long i = 0; i < (N1 - 1); ++i) {
          result[i] = str_1[i];
        }
        
        for(unsigned long i = 0; i < (N2 - 1); ++i) {
          result[(i + N1) - 1] = str_2[i];
        }
        
        result[(N1 + N2) - 2] = '\0';
        return result;
      }
      struct FindResult
      {
        unsigned long index;
        bool success;
      };
      
      template<unsigned long N>
      inline constexpr FindResult fs_find(FixedString<N> str, char c)
      {
        for(unsigned long i = 0; i < (N - 1); ++i) {
          if(str[i] == c) {
            return {i, true};
          } 
          
        }
        
        return {0, false};
      }
      template<unsigned long Pos, unsigned long Len, unsigned long N>
      inline constexpr std::optional<FixedString<Len + 1> > fs_substr(FixedString<N> str)
      {
        if((Pos >= N) || ((Pos + Len) > (N - 1))) {
          return std::nullopt;
        } 
        
        FixedString<Len + 1> result = {} /* NRVO variable */;
        for(unsigned long i = 0; i < Len; ++i) {
          result[i] = str[Pos + i];
        }
        
        result[Len] = '\0';
        return result;
      }
      template<unsigned long N>
      inline constexpr std::basic_string_view<char, std::char_traits<char> > fs_substr_view(FixedString<N> str, unsigned long pos, unsigned long len)
      {
        return std::basic_string_view<char, std::char_traits<char> >{{str.data() + pos, len}};
      }
      template<unsigned long N>
      inline constexpr std::basic_string_view<char, std::char_traits<char> > fs_string_view(FixedString<N> str)
      {
        return std::basic_string_view<char, std::char_traits<char> >{{str.data()}};
      }
      template<unsigned long N1, unsigned long N2>
      inline constexpr bool fs_starts_with(FixedString<N1> str, FixedString<N2> prefix)
      {
        if constexpr(N1 < N2) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (N2 - 1); ++i) {
          if(str[i] != prefix[i]) {
            return false;
          } 
          
        }
        
        return true;
      }
      template<unsigned long N>
      inline constexpr auto fs_to_upper(FixedString<N> str)
      {
        FixedString<N> result = {};
        for(int i = 0; static_cast<unsigned long>(i) < (N - 1); ++i) {
          if((str[i] >= 'a') && (str[i] <= 'z')) {
            result[i] = (str[i] - 32);
          } else {
            result[i] = str[i];
          } 
          
        }
        
        return result;
      }
      template<unsigned long N>
      inline constexpr auto str_to_lower(FixedString<N> str)
      {
        FixedString<N> result = {};
        for(int i = 0; static_cast<unsigned long>(i) < (N - 1); ++i) {
          if((str[i] >= 'A') && (str[i] <= 'Z')) {
            result[i] = (str[i] + 32);
          } else {
            result[i] = str[i];
          } 
          
        }
        
        return result;
      }
      inline constexpr char to_upper(char c)
      {
        if((static_cast<int>(c) >= static_cast<int>('a')) && (static_cast<int>(c) <= static_cast<int>('z'))) {
          return static_cast<char>(static_cast<int>(c) - 32);
        } 
        
        return c;
      }
      inline constexpr char to_lower(char c)
      {
        if((static_cast<int>(c) >= static_cast<int>('A')) && (static_cast<int>(c) <= static_cast<int>('Z'))) {
          return static_cast<char>(static_cast<int>(c) + 32);
        } 
        
        return c;
      }
      inline constexpr bool is_space(char c)
      {
        return (((((static_cast<int>(c) == static_cast<int>(' ')) || (static_cast<int>(c) == static_cast<int>('\t'))) || (static_cast<int>(c) == static_cast<int>('\n'))) || (static_cast<int>(c) == static_cast<int>('\r'))) || (static_cast<int>(c) == static_cast<int>('\f'))) || (static_cast<int>(c) == static_cast<int>('\v'));
      }
      template<unsigned long N1, unsigned long N2>
      inline consteval bool fs_equal_ignore_case(FixedString<N1> lhs, FixedString<N2> rhs)
      {
        if constexpr(N1 != N2) {
          return false;
        } 
        
        for(int i = 0; static_cast<unsigned long>(i) < (N1 - 1); ++i) {
          if(to_lower(lhs[i]) != to_lower(rhs[i])) {
            return false;
          } 
          
        }
        
        return true;
      }
      template<unsigned long N>
      inline constexpr FindResult fs_find_first_non_space(FixedString<N> str)
      {
        for(unsigned long i = 0; i < (N - 1); ++i) {
          if(!is_space(str[i])) {
            return {i, true};
          } 
          
        }
        
        return {N, false};
      }
      template<unsigned long N>
      inline consteval FindResult fs_find_last_non_space(FixedString<N> str)
      {
        unsigned long i = N - 1;
        while(i > 0) {
          i--;
          if(!is_space(str[i])) {
            return {static_cast<unsigned long>(i), true};
          } 
          
        }
        
        return {N, false};
      }
      template<FixedString Str>
      inline consteval auto fs_trim()
      {
        constexpr const auto begin_res = fs_find_first_non_space(Str);
        constexpr const auto end_res = fs_find_last_non_space(Str);
        constexpr const bool is_empty_or_blank = ((!begin_res.success || !end_res.success) || (begin_res.index > end_res.index));
        if constexpr(is_empty_or_blank) {
          return meta::FixedString<1>{""};
        } else /* constexpr */ {
          constexpr const auto len = (end_res.index - begin_res.index) + 1;
          return fs_substr<begin_res.index, len>(Str).value();
        } 
        
      }
      struct SqlNull
      {
      };
      
      struct SqlNow
      {
      };
      
      struct sql_integer
      {
      };
      
      struct sql_floating
      {
      };
      
      struct sql_text
      {
      };
      
      struct sql_boolean
      {
      };
      
      struct sql_null
      {
      };
      
      struct sql_expr
      {
      };
      
      namespace detail
      {
        template<typename Column, typename Value>
        struct sql_compatible_impl : public std::integral_constant<bool, false>
        {
        };
        
        template<>
        struct sql_compatible_impl<ess::orm::meta::sql_boolean, ess::orm::meta::sql_boolean> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_compatible_impl<ess::orm::meta::sql_integer, ess::orm::meta::sql_integer> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_compatible_impl<ess::orm::meta::sql_floating, ess::orm::meta::sql_floating> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_compatible_impl<ess::orm::meta::sql_text, ess::orm::meta::sql_text> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_compatible_impl<ess::orm::meta::sql_null, ess::orm::meta::sql_null> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename T>
        struct cpp_type_to_sql_semantic_impl
        {
          
          /* PASSED: static_assert(sizeof(T) == 0, "\ncpp_type_to_sql_semantic: \344\270\215\346\224\257\346\214\201\347\232\204\347\261\273\345\236\213\n"); */
        };
        
        /* First instantiated from: meta.hpp:351 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct cpp_type_to_sql_semantic_impl<long long>
        {
          using type = ess::orm::meta::sql_integer;
        };
        
        #endif
        /* First instantiated from: meta.hpp:351 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct cpp_type_to_sql_semantic_impl<GoodsStatus>
        {
          using type = ess::orm::meta::sql_integer;
        };
        
        #endif
        /* First instantiated from: meta.hpp:351 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct cpp_type_to_sql_semantic_impl<int>
        {
          using type = ess::orm::meta::sql_integer;
        };
        
        #endif
        /* First instantiated from: meta.hpp:351 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct cpp_type_to_sql_semantic_impl<float>
        {
          using type = ess::orm::meta::sql_floating;
        };
        
        #endif
        template<>
        struct cpp_type_to_sql_semantic_impl<bool>
        {
          using type = ess::orm::meta::sql_boolean;
        };
        
        template<std::integral T>
        struct cpp_type_to_sql_semantic_impl<T>
        {
          using type = ess::orm::meta::sql_integer;
        };
        
        template<std::floating_point T>
        struct cpp_type_to_sql_semantic_impl<T>
        {
          using type = ess::orm::meta::sql_floating;
        };
        
        template<unsigned long N>
        struct cpp_type_to_sql_semantic_impl<FixedString<N> >
        {
          using type = ess::orm::meta::sql_text;
        };
        
        template<>
        struct cpp_type_to_sql_semantic_impl<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >
        {
          using type = ess::orm::meta::sql_text;
        };
        
        template<>
        struct cpp_type_to_sql_semantic_impl<const char *>
        {
          using type = ess::orm::meta::sql_text;
        };
        
        template<>
        struct cpp_type_to_sql_semantic_impl<ess::orm::meta::SqlNull>
        {
          using type = ess::orm::meta::sql_null;
        };
        
        template<typename T>
        requires std::is_enum_v<T>
        struct cpp_type_to_sql_semantic_impl<T>
        {
          using type = ess::orm::meta::sql_integer;
        };
        
        
      }
      template<typename type_parameter_0_0>
      struct sql_value_from_type
      {
      };
      
      /* First instantiated from: meta.hpp:336 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_from_type<int>
      {
        using type = sql_integer;
      };
      
      #endif
      /* First instantiated from: meta.hpp:336 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_from_type<GoodsStatus>
      {
        using type = sql_integer;
      };
      
      #endif
      /* First instantiated from: meta.hpp:336 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_from_type<double>
      {
        using type = sql_floating;
      };
      
      #endif
      /* First instantiated from: meta.hpp:336 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_from_type<FixedString<9> >
      {
        using type = sql_text;
      };
      
      #endif
      template<>
      struct sql_value_from_type<bool>
      {
        using type = sql_boolean;
      };
      
      template<typename T>
      requires std::is_enum_v<T>
      struct sql_value_from_type<T>
      {
        using type = sql_integer;
      };
      
      template<std::integral T>
      struct sql_value_from_type<T>
      {
        using type = sql_integer;
      };
      
      template<std::floating_point T>
      struct sql_value_from_type<T>
      {
        using type = sql_floating;
      };
      
      template<unsigned long N>
      struct sql_value_from_type<meta::FixedString<N> >
      {
        using type = sql_text;
      };
      
      template<>
      struct sql_value_from_type<SqlNull>
      {
        using type = sql_null;
      };
      
      template<auto Value>
      struct sql_value_tag
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(Value)> >::type;
      };
      
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<1>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(1)> >::type;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<true>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(true)> >::type;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<2>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype((GoodsStatus)2)> >::type;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<0>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(0)> >::type;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<0.0>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(0.)> >::type;
      };
      
      #endif
      /* First instantiated from: attribute.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_value_tag<FixedString<9>_"untitled"_>
      {
        using type = typename sql_value_from_type<std::remove_cvref_t<decltype(FixedString<9>{"untitled"})> >::type;
      };
      
      #endif
      template<typename Column, typename Value>
      struct sql_compatible : public detail::sql_compatible_impl<Column, Value>
      {
      };
      
      /* First instantiated from: meta.hpp:345 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_compatible<sql_integer, sql_integer> : public detail::sql_compatible_impl<sql_integer, sql_integer>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:345 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_compatible<sql_boolean, sql_boolean> : public detail::sql_compatible_impl<sql_boolean, sql_boolean>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:345 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_compatible<sql_floating, sql_floating> : public detail::sql_compatible_impl<sql_floating, sql_floating>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:345 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct sql_compatible<sql_text, sql_text> : public detail::sql_compatible_impl<sql_text, sql_text>
      {
      };
      
      #endif
      template<typename Column, typename Value>
      inline constexpr const bool sql_compatible_v = sql_compatible<std::remove_cvref_t<Column>, std::remove_cvref_t<Value> >::value;
      
      template<>
      inline constexpr const bool sql_compatible_v<sql_integer, sql_integer> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_boolean, sql_boolean> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_floating, sql_floating> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_text, sql_text> = std::integral_constant<bool, true>::value;
      template<typename T>
      struct cpp_type_to_sql_semantic : public detail::cpp_type_to_sql_semantic_impl<std::remove_cvref_t<T> >
      {
      };
      
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<long long> : public detail::cpp_type_to_sql_semantic_impl<long long>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<bool> : public detail::cpp_type_to_sql_semantic_impl<bool>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<GoodsStatus> : public detail::cpp_type_to_sql_semantic_impl<GoodsStatus>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<int> : public detail::cpp_type_to_sql_semantic_impl<int>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<float> : public detail::cpp_type_to_sql_semantic_impl<float>
      {
      };
      
      #endif
      /* First instantiated from: meta.hpp:354 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct cpp_type_to_sql_semantic<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > : public detail::cpp_type_to_sql_semantic_impl<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >
      {
      };
      
      #endif
      template<typename T>
      using cpp_type_to_sql_semantic_t = typename cpp_type_to_sql_semantic<T>::type;
      template<typename type_parameter_0_0>
      struct sql_semantic_to_type_str
      {
      };
      
      template<>
      struct sql_semantic_to_type_str<sql_integer>
      {
        inline static constexpr const FixedString<4> type_str = operator""_fs<FixedString<4>_"INT"_>();
      };
      
      template<unsigned long N>
      FixedString(const char (&str)[N]) -> FixedString<N>;
      
      /* First instantiated from: meta.hpp:359 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[4]) -> meta::FixedString<4>;
      #endif
      
      
      /* First instantiated from: meta.hpp:363 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[5]) -> meta::FixedString<5>;
      #endif
      
      
      /* First instantiated from: meta.hpp:371 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[8]) -> meta::FixedString<8>;
      #endif
      
      
      /* First instantiated from: attribute.hpp:195 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[12]) -> meta::FixedString<12>;
      #endif
      
      
      /* First instantiated from: attribute.hpp:199 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[7]) -> meta::FixedString<7>;
      #endif
      
      
      /* First instantiated from: attribute.hpp:203 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[9]) -> meta::FixedString<9>;
      #endif
      
      
      /* First instantiated from: schema.cpp:23 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[3]) -> meta::FixedString<3>;
      #endif
      
      
      /* First instantiated from: schema.cpp:25 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      FixedString(const char (&str)[6]) -> meta::FixedString<6>;
      #endif
      
      
      
      
      
      
      
      
      
      template<>
      struct sql_semantic_to_type_str<sql_floating>
      {
        inline static constexpr const FixedString<5> type_str = operator""_fs<FixedString<5>_"REAL"_>();
      };
      
      template<>
      struct sql_semantic_to_type_str<sql_text>
      {
        inline static constexpr const FixedString<5> type_str = operator""_fs<FixedString<5>_"TEXT"_>();
      };
      
      template<>
      struct sql_semantic_to_type_str<sql_boolean>
      {
        inline static constexpr const FixedString<8> type_str = operator""_fs<FixedString<8>_"BOOLEAN"_>();
      };
      
      template<unsigned long N>
      inline constexpr std::basic_string_view<char, std::char_traits<char> > format_as(const FixedString<N> & str)
      {
        return std::basic_string_view<char, std::char_traits<char> >(str);
      }
      
      /* First instantiated from: format.h:3805 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr std::basic_string_view<char, std::char_traits<char> > format_as<9>(const meta::FixedString<9> & str)
      {
        return std::basic_string_view<char, std::char_traits<char> >(str.operator std::basic_string_view<char, std::char_traits<char> >());
      }
      #endif
      
      static std::basic_string<char, std::char_traits<char>, std::allocator<char> > join(const std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > & elements, std::basic_string_view<char, std::char_traits<char> > delimeter)
      {
        if(elements.empty()) {
          return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
        } 
        
        unsigned long total_len = 0;
        std::basic_string<char, std::char_traits<char>, std::allocator<char> > result = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{} /* NRVO variable */;
        {
          const std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > & __range2 = elements;
          std::__wrap_iter<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > *> __begin2 = __range2.begin();
          std::__wrap_iter<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > *> __end2 = __range2.end();
          for(; !std::operator==(__begin2, __end2); __begin2.operator++()) {
            const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & elem = __begin2.operator*();
            total_len = static_cast<unsigned long>(total_len + elem.size());
          }
          
        }
        total_len = static_cast<unsigned long>(total_len + (delimeter.size() * (elements.size() - 1)));
        result.reserve(total_len);
        for(unsigned long i = 0; i < elements.size(); ++i) {
          result.operator+=(elements.operator[](i));
          if(i < (elements.size() - 1)) {
            result.operator+=(delimeter);
          } 
          
        }
        
        return result;
      }
      template<>
      inline constexpr const bool sql_compatible_v<sql_integer, sql_integer> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_boolean, sql_boolean> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_floating, sql_floating> = std::integral_constant<bool, true>::value;
      template<>
      inline constexpr const bool sql_compatible_v<sql_text, sql_text> = std::integral_constant<bool, true>::value;
      
    }
    
  }
  
}

#include <ess/orm/dialect.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/row.hpp>

#include <std.hpp>

#include <ess/orm/runtime.hpp>

#include <ess/orm/config/config.hpp>
#include <ess/orm/dialect.hpp>
#include <ess/orm/dsl.hpp>

#include <ess/orm/attribute.hpp>
#include <ess/orm/common_concept.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/common_concept.hpp>
#include <ess/orm/meta.hpp>
#include <ess/orm/traits.hpp>
#include <fmt/ranges.h>
#include <sqlite3.h>

#include <ess/orm/row.hpp>
#include <fmt/format.h>
#include <sqlite3.h>

namespace ess
{
  namespace orm
  {
    namespace dialect
    {
      struct DialectTag
      {
      };
      
      struct Sqlite : public DialectTag
      {
      };
      
      struct Postgres : public DialectTag
      {
      };
      
      inline constexpr Sqlite get_orm_dialect(, ...)
      {
        return {{}};
      }
      template<typename Dialect>
      concept dialect_type = std::derived_from<Dialect, DialectTag>;
      
      template<dialect_type Dialect, meta::meta::FixedString SQL>
      static bool validate()
      {
        return true;
      }
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    namespace config
    {
      struct DefaultConfig
      {
        using dialect = dialect::Sqlite;
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > connection_url = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > password = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
        inline static constexpr const bool enable_sql_logging = false;
        inline static constexpr const unsigned long pool_size = 10;
        inline static constexpr const std::chrono::duration<long long, std::ratio<1, 1000> > query_timeout = std::chrono::duration<long long, std::ratio<1, 1000> >{3000};
      };
      
      
    }
    
  }
  
}
namespace ess
{
  namespace orm
  {
    namespace config
    {
      namespace detail
      {
        template<typename T>
        concept has_dialect = requires{
          typename T::dialect;
          requires ess::orm::dialect::dialect_type<typename T::dialect>;
        };
        
        template<typename T, typename Default>
        struct get_dialect_or
        {
          using type = Default;
        };
        
        /* First instantiated from: traits.hpp:56 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct get_dialect_or<ess::orm::config::UserConfig, ess::orm::dialect::Sqlite>
        {
          using type = typename UserConfig::dialect;
        };
        
        #endif
        template<typename T, typename Default>
        requires (has_dialect<T>)
        struct get_dialect_or<T, Default>
        {
          using type = typename T::dialect;
        };
        
        template<typename T>
        concept has_connection_url = requires{
          {T::connection_url} -> std::convertible_to<std::string>;
        };
        
        template<typename T>
        concept has_password = requires{
          {T::password} -> std::convertible_to<std::string>;
        };
        
        template<typename T>
        concept has_enable_sql_logging = requires{
          {T::enable_sql_logging} -> std::convertible_to<bool>;
        };
        
        template<typename T>
        concept has_pool_size = requires{
          {T::pool_size} -> std::convertible_to<std::size_t>;
        };
        
        template<typename T>
        concept has_query_timeout = requires{
          {T::query_timeout} -> std::convertible_to<std::chrono::milliseconds>;
        };
        
        
      }
      template<typename UserCfg, typename DefaultCfg = config::DefaultConfig>
      struct ConfigTrait
      {
        using dialect = typename detail::get_dialect_or<UserCfg, typename DefaultCfg::dialect>::type;
                        
        class __lambda_59_49
        {
          public: 
          inline auto operator()() const
          {
            if constexpr(ess::orm::config::detail::has_connection_url<UserCfg>) {
              return UserCfg::connection_url;
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::connection_url;
            } 
            
          }
          
          using retType_59_49 = auto (*)();
          inline constexpr operator retType_59_49 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline auto __invoke()
          {
            return __lambda_59_49{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_59_49() = default;
          
        };
        
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > connection_url = __lambda_59_49{}();
        
        public: 
                        
        class __lambda_68_43
        {
          public: 
          inline auto operator()() const
          {
            if constexpr(ess::orm::config::detail::has_password<UserCfg>) {
              return UserCfg::password;
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::password;
            } 
            
          }
          
          using retType_68_43 = auto (*)();
          inline constexpr operator retType_68_43 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline auto __invoke()
          {
            return __lambda_68_43{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_68_43() = default;
          
        };
        
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > password = __lambda_68_43{}();
        
        public: 
                        
        class __lambda_77_46
        {
          public: 
          inline auto operator()() const
          {
            if constexpr(ess::orm::config::detail::has_enable_sql_logging<UserCfg>) {
              return UserCfg::enable_sql_logging;
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::enable_sql_logging;
            } 
            
          }
          
          using retType_77_46 = auto (*)();
          inline constexpr operator retType_77_46 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline auto __invoke()
          {
            return __lambda_77_46{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_77_46() = default;
          
        };
        
        inline static constexpr const bool enable_sql_logging = __lambda_77_46{}();
        
        public: 
                        
        class __lambda_86_44
        {
          public: 
          inline auto operator()() const
          {
            if constexpr(ess::orm::config::detail::has_pool_size<UserCfg>) {
              return UserCfg::pool_size;
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::pool_size;
            } 
            
          }
          
          using retType_86_44 = auto (*)();
          inline constexpr operator retType_86_44 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline auto __invoke()
          {
            return __lambda_86_44{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_86_44() = default;
          
        };
        
        inline static constexpr const unsigned long pool_size = __lambda_86_44{}();
        
        public: 
                        
        class __lambda_95_62
        {
          public: 
          inline auto operator()() const
          {
            if constexpr(ess::orm::config::detail::has_query_timeout<UserCfg>) {
              return UserCfg::query_timeout;
            } else /* constexpr */ {
              return DefaultCfg::query_timeout;
            } 
            
          }
          
          using retType_95_62 = auto (*)();
          inline constexpr operator retType_95_62 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline auto __invoke()
          {
            return __lambda_95_62{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_95_62() = default;
          
        };
        
        inline static constexpr const std::chrono::duration<long long, std::ratio<1, 1000> > query_timeout = __lambda_95_62{}();
        
      };
      
      /* First instantiated from: config.hpp:20 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct ConfigTrait<UserConfig, DefaultConfig>
      {
        using dialect = typename detail::get_dialect_or<UserConfig, typename DefaultConfig::dialect>::type;
        static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > connection_url;
        static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > password;
                        
        class __lambda_77_46
        {
          public: 
          inline /*constexpr */ bool operator()() const
          {
            if constexpr(false) {
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::enable_sql_logging;
            } 
            
          }
          
          using retType_77_46 = bool (*)();
          inline constexpr operator retType_77_46 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline /*constexpr */ bool __invoke()
          {
            return __lambda_77_46{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_77_46() = default;
          
        } __lambda_77_46{};
        
        inline static constexpr const bool enable_sql_logging = __lambda_77_46.operator()();
                        
        class __lambda_86_44
        {
          public: 
          inline /*constexpr */ unsigned long operator()() const
          {
            if constexpr(false) {
            } else /* constexpr */ {
              return ess::orm::config::DefaultConfig::pool_size;
            } 
            
          }
          
          using retType_86_44 = unsigned long *;
          inline constexpr operator retType_86_44 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline /*constexpr */ unsigned long __invoke()
          {
            return __lambda_86_44{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_86_44() = default;
          
        } __lambda_86_44{};
        
        inline static constexpr const unsigned long pool_size = __lambda_86_44.operator()();
                        
        class __lambda_95_62
        {
          public: 
          inline /*constexpr */ std::chrono::duration<long long, std::ratio<1, 1000> > operator()() const
          {
            if constexpr(false) {
            } else /* constexpr */ {
              return std::chrono::duration<long long, std::ratio<1, 1000> >(ess::orm::config::DefaultConfig::query_timeout);
            } 
            
          }
          
          using retType_95_62 = std::chrono::milliseconds (*)();
          inline constexpr operator retType_95_62 () const noexcept
          {
            return __invoke;
          };
          
          private: 
          static inline /*constexpr */ std::chrono::duration<long long, std::ratio<1, 1000> > __invoke()
          {
            return __lambda_95_62{}.operator()();
          }
          
          
          public:
          // /*constexpr */ __lambda_95_62() = default;
          
        } __lambda_95_62{};
        
        inline static constexpr const std::chrono::duration<long long, std::ratio<1, 1000> > query_timeout = __lambda_95_62.operator()();
        
      };
      
      #endif
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    namespace config
    {
      struct UserConfig
      {
        using dialect = ess::orm::dialect::Postgres;
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > connection_url = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("./data/test.db");
        inline static constexpr const std::basic_string<char, std::char_traits<char>, std::allocator<char> > password = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("xxx");
      };
      
      inline constexpr const bool has_user_config = true;
      using config = ConfigTrait<UserConfig, DefaultConfig>;
      using dialect = config::dialect;
      static constexpr const bool enable_sql_logging = ConfigTrait<UserConfig, DefaultConfig>::enable_sql_logging;
      static constexpr const unsigned long pool_size = ConfigTrait<UserConfig, DefaultConfig>::pool_size;
      static constexpr const std::chrono::duration<long long, std::ratio<1, 1000> > query_timeout = std::chrono::duration<long long, std::ratio<1, 1000> >(ConfigTrait<UserConfig, DefaultConfig>::query_timeout);
      static inline constexpr std::basic_string<char, std::char_traits<char>, std::allocator<char> > get_dialect_string()
      {
        if constexpr(false) {
          return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("Sqlite");
        } else /* constexpr */ {
          if constexpr(true) {
            return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("Postgres");
          } else /* constexpr */ {
            throw ;
          } 
          
        } 
        
      }
      static void print_config()
      {
        fmt::print(fmt::fg(detail::color_type(fmt::color::cyan)), fmt::fstring<const char (&)[13], const char *, const char (&)[9], std::basic_string<char, std::char_traits<char>, std::allocator<char> >, const char (&)[13], const char *, const char (&)[11], const unsigned long &, const char (&)[15], long long>("\n[Ess ORM Configuration]\n  {:<18} {}\n  {:<18} {}\n  {:<18} {}\n  {:<18} {}\n  {:<18} {}ms\n"), "User config:", (has_user_config ? "detected" : "not detected"), "Dialect:", get_dialect_string(), "SQL Logging:", (enable_sql_logging ? "true" : "false"), "Pool size:", pool_size, "Query timeout:", query_timeout.count());
      }
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    using DBValue = std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>;
    class Row
    {
      std::unordered_map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>, std::hash<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::equal_to<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> > > > m_data;
      
      public: 
      struct Proxy
      {
        const std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> & m_value;
        template<typename T>
        inline operator T () const
        {
                              
          class __lambda_17_11
          {
            public: 
            template<class type_parameter_1_0>
            inline T operator()(type_parameter_1_0 && val) const
            {
              using V = std::decay_t<decltype(val)>;
              if constexpr(std::is_same_v<T, V>) {
                return val;
              } else /* constexpr */ {
                if constexpr(std::is_arithmetic_v<T> && std::is_arithmetic_v<V>) {
                  return static_cast<T>(val);
                } else /* constexpr */ {
                  if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, T> && std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                    return val;
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            private: 
            template<class type_parameter_1_0>
            static inline T __invoke(type_parameter_1_0 && val)
            {
              return __lambda_17_11{}.operator()<type_parameter_1_0>(val);
            }
            
            public:
            // /*constexpr */ __lambda_17_11() = default;
            
          };
          
          return std::visit(__lambda_17_11{}, this->m_value);
        }
        
        /* First instantiated from: schema.cpp:69 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline operator int () const
        {
                              
          class __lambda_17_11
          {
            public: 
            template<class type_parameter_0_0>
            inline /*constexpr */ int operator()(type_parameter_0_0 && val) const
            {
              using V = std::decay_t<decltype(val)>;
              if constexpr(std::is_same_v<int, V>) {
                return val;
              } else /* constexpr */ {
                if constexpr(std::is_arithmetic_v<int> && std::is_arithmetic_v<V>) {
                  return static_cast<int>(val);
                } else /* constexpr */ {
                  if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, int> && std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                    return val;
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ int operator()<const int &>(const int & val) const
            {
              using V = std::decay_t<const int &>;
              if constexpr(true) {
                return val;
              } else /* constexpr */ {
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ int operator()<const long long &>(const long long & val) const
            {
              using V = std::decay_t<const long long &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<int>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ int operator()<const double &>(const double & val) const
            {
              using V = std::decay_t<const double &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<int>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ int operator()<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & val) const
            {
              using V = std::decay_t<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ int operator()<const bool &>(const bool & val) const
            {
              using V = std::decay_t<const bool &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<int>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            private: 
            template<class type_parameter_0_0>
            static inline /*constexpr */ int __invoke(type_parameter_0_0 && val)
            {
              return __lambda_17_11{}.operator()<type_parameter_0_0>(val);
            }
            
            public:
            // /*constexpr */ __lambda_17_11() = default;
            
          };
          
          return std::visit(__lambda_17_11{}, this->m_value);
        }
        #endif
        
        
        
        
        /* First instantiated from: schema.cpp:69 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline operator std::basic_string<char, std::char_traits<char>, std::allocator<char> > () const
        {
                              
          class __lambda_17_11
          {
            public: 
            template<class type_parameter_0_0>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()(type_parameter_0_0 && val) const
            {
              using V = std::decay_t<decltype(val)>;
              if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                return val;
              } else /* constexpr */ {
                if constexpr(std::is_arithmetic_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > && std::is_arithmetic_v<V>) {
                  return static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(val);
                } else /* constexpr */ {
                  if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > > && std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                    return val;
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()<const int &>(const int & val) const
            {
              using V = std::decay_t<const int &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()<const long long &>(const long long & val) const
            {
              using V = std::decay_t<const long long &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()<const double &>(const double & val) const
            {
              using V = std::decay_t<const double &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & val) const
            {
              using V = std::decay_t<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>;
              if constexpr(true) {
                return std::basic_string<char, std::char_traits<char>, std::allocator<char> >(val);
              } else /* constexpr */ {
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > operator()<const bool &>(const bool & val) const
            {
              using V = std::decay_t<const bool &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            private: 
            template<class type_parameter_0_0>
            static inline /*constexpr */ std::basic_string<char, std::char_traits<char>, std::allocator<char> > __invoke(type_parameter_0_0 && val)
            {
              return __lambda_17_11{}.operator()<type_parameter_0_0>(val);
            }
            
            public:
            // /*constexpr */ __lambda_17_11() = default;
            
          };
          
          return std::visit(__lambda_17_11{}, this->m_value);
        }
        #endif
        
        
        
        
        
        
        
        
        /* First instantiated from: schema.cpp:70 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline operator double () const
        {
                              
          class __lambda_17_11
          {
            public: 
            template<class type_parameter_0_0>
            inline /*constexpr */ double operator()(type_parameter_0_0 && val) const
            {
              using V = std::decay_t<decltype(val)>;
              if constexpr(std::is_same_v<double, V>) {
                return val;
              } else /* constexpr */ {
                if constexpr(std::is_arithmetic_v<double> && std::is_arithmetic_v<V>) {
                  return static_cast<double>(val);
                } else /* constexpr */ {
                  if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double> && std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                    return val;
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ double operator()<const int &>(const int & val) const
            {
              using V = std::decay_t<const int &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<double>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ double operator()<const long long &>(const long long & val) const
            {
              using V = std::decay_t<const long long &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<double>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ double operator()<const double &>(const double & val) const
            {
              using V = std::decay_t<const double &>;
              if constexpr(true) {
                return val;
              } else /* constexpr */ {
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ double operator()<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & val) const
            {
              using V = std::decay_t<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ double operator()<const bool &>(const bool & val) const
            {
              using V = std::decay_t<const bool &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<double>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            private: 
            template<class type_parameter_0_0>
            static inline /*constexpr */ double __invoke(type_parameter_0_0 && val)
            {
              return __lambda_17_11{}.operator()<type_parameter_0_0>(val);
            }
            
            public:
            // /*constexpr */ __lambda_17_11() = default;
            
          };
          
          return std::visit(__lambda_17_11{}, this->m_value);
        }
        #endif
        
        
        /* First instantiated from: schema.cpp:71 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline operator bool () const
        {
                              
          class __lambda_17_11
          {
            public: 
            template<class type_parameter_0_0>
            inline /*constexpr */ bool operator()(type_parameter_0_0 && val) const
            {
              using V = std::decay_t<decltype(val)>;
              if constexpr(std::is_same_v<bool, V>) {
                return val;
              } else /* constexpr */ {
                if constexpr(std::is_arithmetic_v<bool> && std::is_arithmetic_v<V>) {
                  return static_cast<bool>(val);
                } else /* constexpr */ {
                  if constexpr(std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> && std::is_same_v<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, V>) {
                    return val;
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<const int &>(const int & val) const
            {
              using V = std::decay_t<const int &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<bool>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<const long long &>(const long long & val) const
            {
              using V = std::decay_t<const long long &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<bool>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<const double &>(const double & val) const
            {
              using V = std::decay_t<const double &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(true) {
                  return static_cast<bool>(val);
                } else /* constexpr */ {
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & val) const
            {
              using V = std::decay_t<const std::basic_string<char, std::char_traits<char>, std::allocator<char> > &>;
              if constexpr(false) {
              } else /* constexpr */ {
                if constexpr(false) {
                } else /* constexpr */ {
                  if constexpr(false) {
                  } else /* constexpr */ {
                    throw std::runtime_error(std::runtime_error("Type missmatch: cannot convert"));
                  } 
                  
                } 
                
              } 
              
            }
            #endif
            
            
            /* First instantiated from: invoke.h:179 */
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<const bool &>(const bool & val) const
            {
              using V = std::decay_t<const bool &>;
              if constexpr(true) {
                return val;
              } else /* constexpr */ {
              } 
              
            }
            #endif
            
            private: 
            template<class type_parameter_0_0>
            static inline /*constexpr */ bool __invoke(type_parameter_0_0 && val)
            {
              return __lambda_17_11{}.operator()<type_parameter_0_0>(val);
            }
            
            public:
            // /*constexpr */ __lambda_17_11() = default;
            
          };
          
          return std::visit(__lambda_17_11{}, this->m_value);
        }
        #endif
        
        template<typename T>
        inline T as() const
        {
          return std::get<T>(this->m_value);
        }
        inline const std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> & get_variant() const
        {
          return this->m_value;
        }
        
      };
      
      inline Row() noexcept = default;
      inline Row(Row &&) noexcept = default;
      inline Row & operator=(Row &&) noexcept = default;
      inline Row(const Row &) /* noexcept */ = default;
      inline Row & operator=(const Row &) /* noexcept */ = default;
      inline Row(std::basic_string<char, std::char_traits<char>, std::allocator<char> > key, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> value)
      : m_data{std::unordered_map<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>, std::hash<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::equal_to<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >, std::allocator<std::pair<const std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> > > >()}
      {
        this->m_data.emplace<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >(std::move(key), std::move(value));
      }
      
      template<typename Iter>
      inline Row(Iter beg, Iter end)
      : m_data(beg, end)
      {
      }
      
      inline Proxy operator[](const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & key) const
      {
        std::__hash_map_const_iterator<std::__hash_const_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > it = this->m_data.find(key);
        if(operator==(it, this->m_data.end())) {
          throw std::out_of_range(std::out_of_range(operator+("column not found: ", key)));
        } 
        
        return {it.operator->()->second};
      }
      
      template<typename T>
      inline std::optional<T> get_if(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & key) const
      {
        std::__hash_map_const_iterator<std::__hash_const_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > it = this->m_data.find(key);
        if(!operator==(it, this->m_data.end())) {
          {
            auto * p = std::get_if<T>(&it.operator->()->second);
            if(p) {
              return *p;
            } 
            
          }
          
        } 
        
        return std::nullopt;
      }
      inline void add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> > key, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> value)
      {
        this->m_data.emplace<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >(std::move(key), std::move(value));
      }
      
      inline size_t column_count() const noexcept
      {
        return this->m_data.size();
      }
      
      inline bool has_column(const std::basic_string<char, std::char_traits<char>, std::allocator<char> > & key) const noexcept
      {
        return this->m_data.contains(key);
      }
      
      inline bool empty() const noexcept
      {
        return this->m_data.empty();
      }
      
      inline std::__hash_map_iterator<std::__hash_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > begin() noexcept
      {
        return this->m_data.begin();
      }
      
      inline std::__hash_map_iterator<std::__hash_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > end() noexcept
      {
        return this->m_data.end();
      }
      
      inline std::__hash_map_const_iterator<std::__hash_const_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > begin() const noexcept
      {
        return this->m_data.begin();
      }
      
      inline std::__hash_map_const_iterator<std::__hash_const_iterator<std::__hash_node<std::__hash_value_type<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool> >, void *> *> > end() const noexcept
      {
        return this->m_data.end();
      }
      
      // inline ~Row() noexcept = default;
    };
    
    
  }
  
}
namespace ess
{
  namespace orm
  {
    namespace concepts
    {
      namespace detail
      {
        template<typename type_parameter_0_0>
        struct is_fixed_string_impl : public std::integral_constant<bool, false>
        {
        };
        
        /* First instantiated from: common_concept.hpp:36 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct is_fixed_string_impl<int> : public std::integral_constant<bool, false>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:36 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct is_fixed_string_impl<ess::orm::meta::FixedString<9> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:36 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct is_fixed_string_impl<double> : public std::integral_constant<bool, false>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:36 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct is_fixed_string_impl<GoodsStatus> : public std::integral_constant<bool, false>
        {
        };
        
        #endif
        template<unsigned long N>
        struct is_fixed_string_impl<meta::FixedString<N> > : public std::integral_constant<bool, true>
        {
        };
        
        template<typename type_parameter_0_0>
        struct sql_value_impl : public std::integral_constant<bool, false>
        {
        };
        
        /* First instantiated from: common_concept.hpp:50 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct sql_value_impl<int> : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:50 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct sql_value_impl<ess::orm::meta::FixedString<9> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:50 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct sql_value_impl<double> : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: common_concept.hpp:50 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct sql_value_impl<GoodsStatus> : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        template<std::integral T>
        struct sql_value_impl<T> : public std::integral_constant<bool, true>
        {
        };
        
        template<std::floating_point T>
        struct sql_value_impl<T> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_value_impl<bool> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename E>
        requires std::is_enum_v<E>
        struct sql_value_impl<E> : public std::integral_constant<bool, true>
        {
        };
        
        
      }
      template<auto Ptr>
      concept not_null_pointer = (concepts::Ptr != nullptr);
      
      template<typename T>
      concept fixed_string_type = detail::is_fixed_string_impl<std::remove_cvref_t<T> >::value;
      
      namespace detail
      {
        template<fixed_string_type S>
        struct sql_value_impl<S> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_value_impl<meta::SqlNull> : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct sql_value_impl<meta::SqlNow> : public std::integral_constant<bool, true>
        {
        };
        
        
      }
      template<typename T>
      concept sql_default_value = detail::sql_value_impl<T>::value;
      
      
    }
    
  }
  
}
namespace ess
{
  namespace orm
  {
    namespace attribute
    {
      namespace detail
      {
        struct AttributeTag
        {
          // inline constexpr AttributeTag(const AttributeTag &) noexcept = default;
          // inline constexpr AttributeTag() noexcept = default;
        };
        
        
      }
      template<meta::meta::FixedString Expr>
      constexpr const bool is_valid_default_expr = (((((attribute::Expr.size() > 0) && (attribute::Expr.get() != '\'')) && (attribute::Expr.get() != '"')) && (attribute::Expr.get() != '\'')) && (attribute::Expr.get() != '"'));
      template<typename Attr>
      concept attribute_type = std::derived_from<Attr, attribute::detail::AttributeTag>;
      
      struct PrimaryKey : public detail::AttributeTag
      {
        // inline constexpr PrimaryKey(const PrimaryKey &) noexcept = default;
        // inline constexpr PrimaryKey() noexcept = default;
      };
      
      struct Unique : public detail::AttributeTag
      {
      };
      
      struct AutoIncrement : public detail::AttributeTag
      {
        // inline constexpr AutoIncrement(const AutoIncrement &) noexcept = default;
        // inline constexpr AutoIncrement() noexcept = default;
      };
      
      struct NotNull : public detail::AttributeTag
      {
      };
      
      template<meta::meta::FixedString Name>
      struct SerializedName : public detail::AttributeTag
      {
        inline static constexpr const meta::meta::FixedString name = Name;
      };
      
      template<sql_default_value auto Value>
      struct DefaultValue : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<Value>::type;
        inline static constexpr const auto value = Value;
      };
      
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<1> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<1>::type;
        inline static constexpr const int value = 1;
        // inline constexpr DefaultValue(const DefaultValue<1> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<FixedString<9>_"untitled"_> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<FixedString<9>{"untitled"}>::type;
        inline static constexpr const ess::orm::meta::FixedString<9> value = ess::orm::meta::FixedString<9>(FixedString<9>_"untitled"_);
        // inline constexpr DefaultValue(const DefaultValue<FixedString<9>_"untitled"_> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<0.0> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<0.>::type;
        inline static constexpr const double value = 0.0;
        // inline constexpr DefaultValue(const DefaultValue<0.0> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<0> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<0>::type;
        inline static constexpr const int value = 0;
        // inline constexpr DefaultValue(const DefaultValue<0> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<2> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<(GoodsStatus)2>::type;
        inline static constexpr const GoodsStatus value = static_cast<GoodsStatus>(2);
        // inline constexpr DefaultValue(const DefaultValue<2> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      /* First instantiated from: is_base_of.h:27 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct DefaultValue<true> : public detail::AttributeTag
      {
        using semantic_type = typename meta::sql_value_tag<true>::type;
        inline static constexpr const bool value = true;
        // inline constexpr DefaultValue(const DefaultValue<true> &) noexcept = default;
        // inline constexpr DefaultValue() noexcept = default;
      };
      
      #endif
      template<meta::meta::FixedString Expr>
      struct DefaultExpr : public detail::AttributeTag
      {
        using type = meta::sql_expr;
        inline static constexpr const auto expr = Expr;
        
        /* PASSED: static_assert(is_valid_default_expr<static_cast<meta::FixedString>(expr)>, "\n\351\273\230\350\256\244\350\241\250\350\276\276\345\274\217\351\224\231\350\257\257\357\274\232\n1. \350\241\250\350\276\276\345\274\217\351\225\277\345\272\246\345\277\205\351\241\273\345\244\247\344\272\2161\n2. \350\241\250\350\276\276\345\274\217\345\206\205\351\203\250\344\270\215\345\214\205\345\220\253\345\274\225\345\217\267\n"); */
      };
      
      namespace detail
      {
        template<typename SqlSemantic, typename Attr>
        struct valid_attribute_semantic : public std::integral_constant<bool, false>
        {
        };
        
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_integer, ess::orm::attribute::DefaultValue<1> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_integer, ess::orm::attribute::PrimaryKey> : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_boolean, ess::orm::attribute::DefaultValue<true> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_integer, ess::orm::attribute::DefaultValue<2> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_integer, ess::orm::attribute::DefaultValue<0> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_floating, ess::orm::attribute::DefaultValue<0.0> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        /* First instantiated from: attribute.hpp:103 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_semantic<ess::orm::meta::sql_text, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > : public std::integral_constant<bool, true>
        {
        };
        
        #endif
        template<typename SqlSemantic>
        struct valid_attribute_semantic<SqlSemantic, ess::orm::attribute::PrimaryKey> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename SqlSemantic>
        struct valid_attribute_semantic<SqlSemantic, ess::orm::attribute::Unique> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename SqlSemantic>
        struct valid_attribute_semantic<SqlSemantic, ess::orm::attribute::NotNull> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename SqlSemantic, meta::meta::FixedString Name>
        struct valid_attribute_semantic<SqlSemantic, SerializedName<Name> > : public std::integral_constant<bool, true>
        {
        };
        
        template<>
        struct valid_attribute_semantic<meta::sql_integer, ess::orm::attribute::AutoIncrement> : public std::integral_constant<bool, true>
        {
        };
        
        template<typename ColumnSemantic, auto Value>
        struct valid_attribute_semantic<ColumnSemantic, DefaultValue<Value> > : public std::bool_constant<meta::sql_compatible_v<ColumnSemantic, typename meta::sql_value_tag<Value>::type> >
        {
        };
        
        template<typename SqlSemantic, meta::meta::FixedString Expr>
        struct valid_attribute_semantic<SqlSemantic, DefaultExpr<Expr> > : public std::integral_constant<bool, true>
        {
        };
        
        template<typename MemberType, typename Attr>
        struct valid_attribute_impl
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<MemberType>;
          
          public: 
          inline static constexpr const bool value = valid_attribute_semantic<column_semantic, Attr>::value;
        };
        
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<long long, ess::orm::attribute::DefaultValue<1> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<long long>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<long long, ess::orm::attribute::AutoIncrement>
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<long long>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<long long, ess::orm::attribute::PrimaryKey>
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<long long>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<bool, ess::orm::attribute::DefaultValue<true> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<bool>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<GoodsStatus, ess::orm::attribute::DefaultValue<2> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<GoodsStatus>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<int, ess::orm::attribute::DefaultValue<0> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<int>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<float, ess::orm::attribute::DefaultValue<0.0> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<float>;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:167 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct valid_attribute_impl<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >
        {
          
          private: 
          using column_semantic = meta::cpp_type_to_sql_semantic_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
          
          public: 
          inline static constexpr const bool value = std::integral_constant<bool, true>::value;
        };
        
        #endif
        template<typename Attr>
        struct attribute_category
        {
          using type = Attr;
        };
        
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<1> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<true> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<2> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<0> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<0.0> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:135 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >
        {
          using type = default_value_or_expr_tag;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:143 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::AutoIncrement>
        {
          using type = ess::orm::attribute::AutoIncrement;
        };
        
        #endif
        /* First instantiated from: attribute.hpp:143 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        struct attribute_category<ess::orm::attribute::PrimaryKey>
        {
          using type = ess::orm::attribute::PrimaryKey;
        };
        
        #endif
        template<meta::meta::FixedString Name>
        struct attribute_category<SerializedName<Name> >
        {
          struct serialized_name_tag
          {
          };
          
          using type = serialized_name_tag;
        };
        
        struct default_value_or_expr_tag
        {
        };
        
        template<auto Value>
        struct attribute_category<DefaultValue<Value> >
        {
          using type = default_value_or_expr_tag;
        };
        
        template<meta::meta::FixedString Expr>
        struct attribute_category<DefaultExpr<Expr> >
        {
          using type = default_value_or_expr_tag;
        };
        
        template<typename Tuple, size_t I>
        inline constexpr bool check_at_prev()
        {
          if constexpr(I == 0) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<I, Tuple> >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, Tuple> >::type>)) , ...);
            }
            
          };
          
          __lambda_138_3{}(std::make_index_sequence<I>{});
          return found;
        }
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<PrimaryKey, AutoIncrement, DefaultValue<1> >, 2>()
        {
          if constexpr(false) {
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<2UL, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0, 1>(std::integer_sequence<unsigned long, 0, 1>) const
            {
              return (found = (found || std::is_same_v<default_value_or_expr_tag, ess::orm::attribute::PrimaryKey>)) , (found = (found || std::is_same_v<default_value_or_expr_tag, ess::orm::attribute::AutoIncrement>));
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long, 0, 1>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<PrimaryKey, AutoIncrement, DefaultValue<1> >, 1>()
        {
          if constexpr(false) {
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<1UL, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (found = (found || std::is_same_v<ess::orm::attribute::AutoIncrement, ess::orm::attribute::PrimaryKey>));
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long, 0>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<PrimaryKey, AutoIncrement, DefaultValue<1> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<DefaultValue<true> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<DefaultValue<true> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<DefaultValue<true> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<DefaultValue<2> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<DefaultValue<GoodsStatus::Deleted> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<DefaultValue<GoodsStatus::Deleted> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<DefaultValue<0> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<DefaultValue<0> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<DefaultValue<0> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<DefaultValue<0.0> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<DefaultValue<0.000000e+00> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<DefaultValue<0.000000e+00> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:160 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool check_at_prev<std::tuple<DefaultValue<FixedString<9>_"untitled"_> >, 0>()
        {
          if constexpr(true) {
            return false;
          } 
          
          using current_ctg = typename attribute_category<std::tuple_element_t<0UL, tuple<DefaultValue<FixedString<9>{"untitled"}> > > >::type;
          bool found = false;
                    
          class __lambda_138_3
          {
            public: 
            template<size_t ...Prev>
            inline /*constexpr */ auto operator()(std::index_sequence<Prev...>) const
            {
              return ((found = (found || std::is_same_v<current_ctg, typename attribute_category<std::tuple_element_t<Prev, tuple<DefaultValue<FixedString<9>{"untitled"}> > > >::type>)) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<>(std::integer_sequence<unsigned long>) const
            {
              return void();
            }
            #endif
            
            private: 
            bool & found;
            
            public:
            __lambda_138_3(bool & _found)
            : found{_found}
            {}
            
          } __lambda_138_3{found};
          
          __lambda_138_3.operator()(std::integer_sequence<unsigned long>{});
          return found;
        }
        #endif
        
        template<typename Tuple>
        inline constexpr bool has_dup_attrs_in_tuple()
        {
          constexpr const size_t N = std::tuple_size_v<Tuple>;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const auto i_seq = std::make_index_sequence<N>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<Tuple, I>())) , ...);
            }
            
          };
          
          __lambda_159_3{}(i_seq);
          return dup;
        }
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<PrimaryKey, AutoIncrement, DefaultValue<1> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0, 1, 2> i_seq = std::integer_sequence<unsigned long, 0, 1, 2>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0, 1, 2>(std::integer_sequence<unsigned long, 0, 1, 2>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, 0UL>())) , ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, 1UL>())) , (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, 2UL>())));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0, 1, 2>(i_seq));
          return dup;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<DefaultValue<true> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<DefaultValue<true> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0> i_seq = std::integer_sequence<unsigned long, 0>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<true> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<true> >, 0UL>()));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0>(i_seq));
          return dup;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<DefaultValue<2> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<DefaultValue<2> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0> i_seq = std::integer_sequence<unsigned long, 0>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<2> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<2> >, 0UL>()));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0>(i_seq));
          return dup;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<DefaultValue<0> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<DefaultValue<0> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0> i_seq = std::integer_sequence<unsigned long, 0>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<0> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<0> >, 0UL>()));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0>(i_seq));
          return dup;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<DefaultValue<0.0> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<DefaultValue<0.0> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0> i_seq = std::integer_sequence<unsigned long, 0>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<0.0> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<0.0> >, 0UL>()));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0>(i_seq));
          return dup;
        }
        #endif
        
        
        /* First instantiated from: attribute.hpp:187 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        inline constexpr bool has_dup_attrs_in_tuple<std::tuple<DefaultValue<FixedString<9>_"untitled"_> > >()
        {
          constexpr const size_t N = std::tuple_size_v<std::tuple<DefaultValue<FixedString<9>_"untitled"_> > >;
          if(N <= 1) {
            return false;
          } 
          
          bool dup = false;
          constexpr const std::integer_sequence<unsigned long, 0> i_seq = std::integer_sequence<unsigned long, 0>{};
                    
          class __lambda_159_3
          {
            public: 
            template<size_t ...I>
            inline /*constexpr */ auto operator()(std::index_sequence<I...>) const
            {
              return ((dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, I>())) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ bool operator()<0>(std::integer_sequence<unsigned long, 0>) const
            {
              return (dup = (dup || check_at_prev<std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, 0UL>()));
            }
            #endif
            
            private: 
            bool & dup;
            
            public:
            __lambda_159_3(bool & _dup)
            : dup{_dup}
            {}
            
          } __lambda_159_3{dup};
          
          __lambda_159_3.operator()(std::integer_sequence<unsigned long, 0>(i_seq));
          return dup;
        }
        #endif
        
        
      }
      template<typename MemberType, typename Attr>
      concept valid_attribute = detail::valid_attribute_impl<MemberType, Attr>::value;
      
      template<typename MemberType, typename Attr>
      inline constexpr void check_one_attribute()
      {
        /* PASSED: static_assert(attribute_type<Attr>, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<MemberType, Attr>, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<long long, attribute::DefaultValue<1> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<1> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<long long, attribute::DefaultValue<1> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<long long, AutoIncrement>()
      {
        /* PASSED: static_assert(attribute_type<AutoIncrement>, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<long long, AutoIncrement>, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<long long, PrimaryKey>()
      {
        /* PASSED: static_assert(attribute_type<PrimaryKey>, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<long long, PrimaryKey>, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<bool, attribute::DefaultValue<true> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<true> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<bool, attribute::DefaultValue<true> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<GoodsStatus, attribute::DefaultValue<2> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<2> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<GoodsStatus, attribute::DefaultValue<2> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<int, attribute::DefaultValue<0> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<0> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<int, attribute::DefaultValue<0> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<float, attribute::DefaultValue<0.0> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<0.0> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<float, attribute::DefaultValue<0.0> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      
      /* First instantiated from: attribute.hpp:182 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_one_attribute<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, attribute::DefaultValue<FixedString<9>_"untitled"_> >()
      {
        /* PASSED: static_assert(attribute_type<attribute::DefaultValue<FixedString<9>_"untitled"_> >, "\344\270\215\350\203\275\344\275\277\347\224\250\347\232\204\345\261\236\346\200\247"); */
        /* PASSED: static_assert(valid_attribute<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, attribute::DefaultValue<FixedString<9>_"untitled"_> >, "\n\345\261\236\346\200\247\351\252\214\350\257\201\345\244\261\350\264\245\357\274\214\350\257\267\346\243\200\346\237\245\346\230\257\345\220\246\344\275\277\347\224\250\344\272\206\344\270\215\345\214\271\351\205\215\347\232\204\345\261\236\346\200\247\357\274\214\347\211\271\345\210\253\346\243\200\346\237\245\351\273\230\350\256\244\345\200\274\347\261\273\345\236\213\346\230\257\345\220\246\345\222\214\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\347\261\273\345\236\213\347\233\270\345\220\214\n"); */
      }
      #endif
      
      template<typename MemberType, typename ... Attrs>
      inline constexpr void check_attributes()
      {
        (check_one_attribute<MemberType, Attrs>() , ...);
      }
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<long long, PrimaryKey, AutoIncrement, attribute::DefaultValue<1> >()
      {
        check_one_attribute<long long, PrimaryKey>() , (check_one_attribute<long long, AutoIncrement>() , check_one_attribute<long long, attribute::DefaultValue<1> >());
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<bool, attribute::DefaultValue<true> >()
      {
        check_one_attribute<bool, attribute::DefaultValue<true> >();
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<GoodsStatus, attribute::DefaultValue<2> >()
      {
        check_one_attribute<GoodsStatus, attribute::DefaultValue<2> >();
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<int, attribute::DefaultValue<0> >()
      {
        check_one_attribute<int, attribute::DefaultValue<0> >();
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<float, attribute::DefaultValue<0.0> >()
      {
        check_one_attribute<float, attribute::DefaultValue<0.0> >();
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:29 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr void check_attributes<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, attribute::DefaultValue<FixedString<9>_"untitled"_> >()
      {
        check_one_attribute<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, attribute::DefaultValue<FixedString<9>_"untitled"_> >();
      }
      #endif
      
      template<typename Tuple>
      constexpr const bool has_dup_attrs_in_tuple = detail::has_dup_attrs_in_tuple<Tuple>();
      
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<PrimaryKey, AutoIncrement, attribute::DefaultValue<1> > > = detail::has_dup_attrs_in_tuple<std::tuple<PrimaryKey, AutoIncrement, attribute::DefaultValue<1> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<true> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<true> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<2> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<2> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0.0> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0.0> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<FixedString<9>_"untitled"_> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<FixedString<9>_"untitled"_> > >();
      struct PlaceHolder
      {
      };
      
      template<typename type_parameter_0_0, auto >
      struct attr_traits
      {
      };
      
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr PlaceHolder PlaceHolder__{};
      template<>
      struct attr_traits<DefaultValue<true>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<2>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<0>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<0.0>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<FixedString<9>_"untitled"_>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<1>, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:218 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<AutoIncrement, PlaceHolder__>
      {
      };
      
      #endif
      /* First instantiated from: attribute.hpp:227 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<2>, 2>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      /* First instantiated from: attribute.hpp:229 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<true>, true>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      /* First instantiated from: attribute.hpp:229 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<0>, 0>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      /* First instantiated from: attribute.hpp:229 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<0.0>, 0.0>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      /* First instantiated from: attribute.hpp:229 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<FixedString<9>_"untitled"_>, FixedString<9>_"untitled"_>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      /* First instantiated from: attribute.hpp:229 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct attr_traits<DefaultValue<1>, 1>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      #endif
      template<>
      struct attr_traits<PrimaryKey, PlaceHolder__>
      {
        inline static constexpr const ess::orm::meta::FixedString<12> attr_str = ess::orm::meta::FixedString<12>{"PRIMARY KEY"};
      };
      
      template<>
      struct attr_traits<Unique, PlaceHolder__>
      {
        inline static constexpr const ess::orm::meta::FixedString<7> attr_str = ess::orm::meta::FixedString<7>{"UNIQUE"};
      };
      
      template<>
      struct attr_traits<NotNull, PlaceHolder__>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>{"NOT NULL"};
      };
      
      template<auto Value>
      struct attr_traits<DefaultValue<Value>, Value>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      template<meta::meta::FixedString Expr>
      struct attr_traits<DefaultExpr<Expr>, Expr>
      {
        inline static constexpr const ess::orm::meta::FixedString<9> attr_str = ess::orm::meta::FixedString<9>("DEFAULT ");
      };
      
      template<typename Attr>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment(Attr)
      {
        using T = std::remove_cvref_t<Attr>;
        if constexpr(requires{
          attr_traits<T, PlaceHolder{}>::attr_str;
        }) {
          return std::basic_string<char, std::char_traits<char>, std::allocator<char> >(std::basic_string_view<char, std::char_traits<char> >(attr_traits<T, PlaceHolder{}>::attr_str));
        } else /* constexpr */ {
          if constexpr(requires{
            T::value;
          }) {
            using Trait = attr_traits<T, T::value>;
            using ValueType = std::remove_cvref_t<decltype(T::value)>;
            if constexpr(std::is_enum_v<ValueType>) {
              return fmt::format("{} {}", Trait::attr_str, static_cast<int>(T::value));
            } else /* constexpr */ {
              return fmt::format("{} {}", Trait::attr_str, T::value);
            } 
            
          } else /* constexpr */ {
            if constexpr(requires{
              T::expr;
            }) {
              using Trait = attr_traits<T, T::expr>;
              return fmt::format("{} {}", Trait::attr_str, T::expr);
            } 
            
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<true> >(attribute::DefaultValue<true>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<true> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<true>, DefaultValue<true>::value>;
            using ValueType = std::remove_cvref_t<const bool>;
            if constexpr(false) {
            } else /* constexpr */ {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, const bool &>("{} {}"), attr_traits<attribute::DefaultValue<true>, true>::attr_str, DefaultValue<true>::value);
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<2> >(attribute::DefaultValue<2>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<2> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<2>, DefaultValue<2>::value>;
            using ValueType = std::remove_cvref_t<const >;
            if constexpr(true) {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, int>("{} {}"), attr_traits<attribute::DefaultValue<2>, 2>::attr_str, static_cast<int>(DefaultValue<2>::value));
            } else /* constexpr */ {
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<0> >(attribute::DefaultValue<0>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<0> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<0>, DefaultValue<0>::value>;
            using ValueType = std::remove_cvref_t<const int>;
            if constexpr(false) {
            } else /* constexpr */ {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, const int &>("{} {}"), attr_traits<attribute::DefaultValue<0>, 0>::attr_str, DefaultValue<0>::value);
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<0.0> >(attribute::DefaultValue<0.0>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<0.0> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<0.0>, DefaultValue<0.0>::value>;
            using ValueType = std::remove_cvref_t<const double>;
            if constexpr(false) {
            } else /* constexpr */ {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, const double &>("{} {}"), attr_traits<attribute::DefaultValue<0.0>, 0.0>::attr_str, DefaultValue<0.0>::value);
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<FixedString<9>_"untitled"_> >(attribute::DefaultValue<FixedString<9>_"untitled"_>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<FixedString<9>_"untitled"_> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<FixedString<9>_"untitled"_>, meta::FixedString<9>(DefaultValue<FixedString<9>_"untitled"_>::value)>;
            using ValueType = std::remove_cvref_t<const meta::FixedString<9> >;
            if constexpr(false) {
            } else /* constexpr */ {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, const meta::FixedString<9> &>("{} {}"), attr_traits<attribute::DefaultValue<FixedString<9>_"untitled"_>, FixedString<9>_"untitled"_>::attr_str, DefaultValue<FixedString<9>_"untitled"_>::value);
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<attribute::DefaultValue<1> >(attribute::DefaultValue<1>)
      {
        using T = std::remove_cvref_t<attribute::DefaultValue<1> >;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            using Trait = attr_traits<attribute::DefaultValue<1>, DefaultValue<1>::value>;
            using ValueType = std::remove_cvref_t<const int>;
            if constexpr(false) {
            } else /* constexpr */ {
              return fmt::format(fmt::fstring<const meta::FixedString<9> &, const int &>("{} {}"), attr_traits<attribute::DefaultValue<1>, 1>::attr_str, DefaultValue<1>::value);
            } 
            
          } else /* constexpr */ {
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<AutoIncrement>(AutoIncrement)
      {
        using T = std::remove_cvref_t<AutoIncrement>;
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(false) {
          } else /* constexpr */ {
            if constexpr(false) {
            } 
            
          } 
          
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:109 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      std::basic_string<char, std::char_traits<char>, std::allocator<char> > to_sql_fragment<PrimaryKey>(PrimaryKey)
      {
        using T = std::remove_cvref_t<PrimaryKey>;
        if constexpr(true) {
          return std::basic_string<char, std::char_traits<char>, std::allocator<char> >(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(std::basic_string_view<char, std::char_traits<char> >(attr_traits<PrimaryKey, PlaceHolder__>::attr_str.operator std::basic_string_view<char, std::char_traits<char> >())));
        } else /* constexpr */ {
        } 
        
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >("");
      }
      #endif
      
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<PrimaryKey, AutoIncrement, attribute::DefaultValue<1> > > = detail::has_dup_attrs_in_tuple<std::tuple<PrimaryKey, AutoIncrement, attribute::DefaultValue<1> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<true> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<true> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<2> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<2> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0.0> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<0.0> > >();
      template<>
      constexpr const bool has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<FixedString<9>_"untitled"_> > > = detail::has_dup_attrs_in_tuple<std::tuple<attribute::DefaultValue<FixedString<9>_"untitled"_> > >();
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    namespace traits
    {
      template<auto >
      inline constexpr const bool dependent_false_v = false;
      template<auto T>
      struct MemberPointerTraits
      {
        
        /* PASSED: static_assert(dependent_false_v<T>, "unsupported member pointer type"); */
      };
      
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::id>
      {
        using class_type = Goods;
        using member_type = long long;
        using pointer_type = long long(Goods::*);
        using MemberVarPtr_16 = long long Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::id;
        static constexpr const bool is_static;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::enabled>
      {
        using class_type = Goods;
        using member_type = bool;
        using pointer_type = bool(Goods::*);
        using MemberVarPtr_16 = bool Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::enabled;
        static constexpr const bool is_static;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::status>
      {
        using class_type = Goods;
        using member_type = GoodsStatus;
        using pointer_type = (Goods::*);
        using MemberVarPtr_16 = GoodsStatus Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::status;
        static constexpr const bool is_static;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::stock>
      {
        using class_type = Goods;
        using member_type = int;
        using pointer_type = int(Goods::*);
        using MemberVarPtr_16 = int Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::stock;
        static constexpr const bool is_static;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::price>
      {
        using class_type = Goods;
        using member_type = float;
        using pointer_type = float(Goods::*);
        using MemberVarPtr_16 = float Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::price;
        static constexpr const bool is_static;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:21 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct MemberPointerTraits<&Goods::title>
      {
        using class_type = Goods;
        using member_type = std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
        using pointer_type = (Goods::*);
        using MemberVarPtr_16 = std::string Goods::*const;
        inline static constexpr MemberVarPtr_16 pointer = &Goods::title;
        static constexpr const bool is_static;
      };
      
      #endif
      template<typename Class, typename Member, Member Class::* Ptr>
      struct MemberPointerTraits<Ptr>
      {
        using class_type = Class;
        using member_type = Member;
        using pointer_type = decltype(Ptr);
        using MemberVarPtr_16 = Member Class::*const;
        inline static constexpr MemberVarPtr_16 pointer = Ptr;
        inline static constexpr const bool is_static = false;
      };
      
      template<typename Member, Member * Ptr>
      struct MemberPointerTraits<Ptr>
      {
        using member_type = Member;
        using pointer_type = decltype(Ptr);
        inline static constexpr Member *const pointer = Ptr;
        inline static constexpr const bool is_static = true;
      };
      
      template<auto Ptr>
      struct MemberAccessor
      {
        using traits = ess::orm::traits::MemberPointerTraits<Ptr>;
        static inline typename traits::member_type & get(typename traits::class_type & obj)
        {
          return obj .* (traits::pointer);
        }
        
      };
      
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    namespace dsl
    {
      template<meta::meta::FixedString ColumnName, auto Ptr = nullptr, typename ... Attrs>
      struct Field
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<Ptr>;
        
        public: 
        inline static constexpr const meta::meta::FixedString column_name = ColumnName;
        using pointer_type = typename member_traits::pointer_type;
        inline static constexpr const decltype(auto) pointer = member_traits::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<Attrs...>;
        
        private: 
        static inline constexpr bool _check()
        {
          attribute::check_attributes<member_type, Attrs...>();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!attribute::has_dup_attrs_in_tuple<attributes>, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
      };
      
      /* First instantiated from: dsl.hpp:72 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<3> meta::FixedString<3>_"id"_{"id"};
      template<>
      struct Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::id>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<3> column_name = ess::orm::meta::FixedString<3>(meta::FixedString<3>_"id"_);
        using pointer_type = long long(Goods::*);
        using MemberVarPtr_22 = long long Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::id>::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<Field<meta::FixedString<3>{"id"}, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >::member_type, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:73 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<6> meta::FixedString<6>_"title"_{"title"};
      template<>
      struct Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::title>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<6> column_name = ess::orm::meta::FixedString<6>(meta::FixedString<6>_"title"_);
        using pointer_type = (Goods::*);
        using MemberVarPtr_22 = std::string Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::title>::pointer;
        using member_type = std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
        using attributes = std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:73 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<6> meta::FixedString<6>_"price"_{"price"};
      template<>
      struct Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::price>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<6> column_name = ess::orm::meta::FixedString<6>(meta::FixedString<6>_"price"_);
        using pointer_type = float(Goods::*);
        using MemberVarPtr_22 = float Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::price>::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<ess::orm::attribute::DefaultValue<0.0> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<Field<meta::FixedString<6>{"price"}, &Goods::price, ess::orm::attribute::DefaultValue<0.000000e+00> >::member_type, ess::orm::attribute::DefaultValue<0.0> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::DefaultValue<0.0> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:73 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<6> meta::FixedString<6>_"stock"_{"stock"};
      template<>
      struct Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::stock>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<6> column_name = ess::orm::meta::FixedString<6>(meta::FixedString<6>_"stock"_);
        using pointer_type = int(Goods::*);
        using MemberVarPtr_22 = int Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::stock>::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<ess::orm::attribute::DefaultValue<0> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<Field<meta::FixedString<6>{"stock"}, &Goods::stock, ess::orm::attribute::DefaultValue<0> >::member_type, ess::orm::attribute::DefaultValue<0> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::DefaultValue<0> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:73 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<7> meta::FixedString<7>_"status"_{"status"};
      template<>
      struct Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::status>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<7> column_name = ess::orm::meta::FixedString<7>(meta::FixedString<7>_"status"_);
        using pointer_type = (Goods::*);
        using MemberVarPtr_22 = GoodsStatus Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::status>::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<ess::orm::attribute::DefaultValue<2> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<Field<meta::FixedString<7>{"status"}, &Goods::status, ess::orm::attribute::DefaultValue<GoodsStatus::Deleted> >::member_type, ess::orm::attribute::DefaultValue<2> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::DefaultValue<2> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      /* First instantiated from: dsl.hpp:73 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<8> meta::FixedString<8>_"enabled"_{"enabled"};
      template<>
      struct Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> >
      {
        
        private: 
        using member_traits = ess::orm::traits::MemberPointerTraits<&Goods::enabled>;
        
        public: 
        inline static constexpr const ess::orm::meta::FixedString<8> column_name = ess::orm::meta::FixedString<8>(meta::FixedString<8>_"enabled"_);
        using pointer_type = bool(Goods::*);
        using MemberVarPtr_22 = bool Goods::*const;
        inline static constexpr MemberVarPtr_22 pointer = ess::orm::traits::MemberPointerTraits<&Goods::enabled>::pointer;
        using member_type = typename member_traits::member_type;
        using attributes = std::tuple<ess::orm::attribute::DefaultValue<true> >;
        
        private: 
        static inline constexpr bool _check()
        {
          ess::orm::attribute::check_attributes<Field<meta::FixedString<8>{"enabled"}, &Goods::enabled, ess::orm::attribute::DefaultValue<true> >::member_type, ess::orm::attribute::DefaultValue<true> >();
          return true;
        }
        
        
        /* PASSED: static_assert(_check()); */
        /* PASSED: static_assert(!ess::orm::attribute::has_dup_attrs_in_tuple<std::tuple<ess::orm::attribute::DefaultValue<true> > >, "\n\345\255\230\345\234\250\351\207\215\345\244\215\347\261\273\345\236\213\347\232\204\345\261\236\346\200\247\357\274\232 \n1. \344\273\273\346\204\217\345\261\236\346\200\247\345\234\250\344\270\200\344\270\252Field\344\270\255\344\270\215\350\203\275\351\207\215\345\244\215\345\243\260\346\230\216\n2. DefaultValue \344\270\216 DefaultExpr \344\272\222\346\226\245\n3. \344\270\215\345\217\257\345\255\230\345\234\250\345\244\232\344\270\252 DefaultValue \346\210\226 DefaultExpr"); */
        public: 
        // inline constexpr Field(Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > &&) noexcept = default;
        // inline constexpr Field() noexcept = default;
      };
      
      #endif
      template<typename T>
      struct is_field : public std::integral_constant<bool, false>
      {
      };
      
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:47 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct is_field<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      template<meta::meta::FixedString FiledName, auto Ptr, typename ... Attrs>
      struct is_field<Field<FiledName, Ptr, Attrs...> > : public std::integral_constant<bool, true>
      {
      };
      
      template<typename T>
      concept field_type = is_field<T>::value;
      
      template<typename LField, typename RField>
      inline constexpr bool is_same_binding()
      {
        using L_traits = traits::MemberPointerTraits<LField::pointer>;
        using R_traits = traits::MemberPointerTraits<RField::pointer>;
        if constexpr(!std::is_same_v<typename L_traits::member_type, typename R_traits::member_type>) {
          return false;
        } else /* constexpr */ {
          if constexpr(L_traits::is_static != R_traits::is_static) {
            return false;
          } else /* constexpr */ {
            return L_traits::pointer == R_traits::pointer;
          } 
          
        } 
        
      }
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      
      /* First instantiated from: dsl.hpp:74 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      inline constexpr bool is_same_binding<dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >()
      {
        using L_traits = traits::MemberPointerTraits<Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer>;
        using R_traits = traits::MemberPointerTraits<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer>;
        if constexpr(true) {
          return false;
        } else /* constexpr */ {
        } 
        
      }
      #endif
      
      template<typename ... type_parameter_0_0>
      struct no_duplicate_detector;
      /* First instantiated from: dsl.hpp:76 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:76 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:76 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:76 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:76 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      /* First instantiated from: dsl.hpp:79 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      struct no_duplicate_detector<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > : public std::integral_constant<bool, true>
      {
      };
      
      #endif
      template<>
      struct no_duplicate_detector<> : public std::integral_constant<bool, true>
      {
      };
      
      template<typename Field>
      struct no_duplicate_detector<Field> : public std::integral_constant<bool, true>
      {
      };
      
      template<typename First, typename ... Rest>
      struct no_duplicate_detector<First, Rest...> : public std::bool_constant<((!meta::fs_equal(First::column_name, Rest::column_name) && (!is_same_binding<First, Rest>())) && ...) && no_duplicate_detector<Rest...>::value>
      {
      };
      
      template<typename ... Fields>
      concept no_duplicated_key_field_words = no_duplicate_detector<Fields...>::value;
      
      template<typename ... MemPtrs>
      struct GlobalPrimaryKey
      {
        using member_pointers = std::tuple<MemPtrs...>;
      };
      
      template<meta::meta::FixedString TableName, ... field_type Fields>
      requires (no_duplicated_key_field_words<Fields...>)
      struct Schema
      {
        inline static constexpr const meta::meta::FixedString table_name = TableName;
        using fields = std::tuple<Fields...>;
        
        private: 
        template<typename Field>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field::member_type>;
          auto attributes = typename Field::attributes{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field::column_name)};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(meta::sql_semantic_to_type_str<member_semantic_type>::type_str));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_2_0>
            inline auto operator()(type_parameter_2_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
          };
          
          std::apply(__lambda_108_9{}, attributes);
          return operator+(col_def, attrs_str);
        }
        
        public: 
        static inline auto make_fields()
        {
          return std::make_tuple(Fields{}... );
        }
        
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_create_table_ddl(bool not_replace)
        {
          std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > col_defs = std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >{};
          col_defs.reserve(sizeof...(Fields));
          (push_back(make_col_def<Fields>()) , ...);
          return fmt::format(fmt::fstring<const char *, std::basic_string_view<char, std::char_traits<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >("CREATE TABLE {}{} ({});"), (not_replace ? "IF NOT EXISTS " : ""), std::basic_string_view<char, std::char_traits<char> >(table_name), ess::orm::meta::join(col_defs, std::basic_string_view<char, std::char_traits<char> >(",\n")));
        }
        
        
        private: 
        
        /* PASSED: static_assert(no_duplicated_key_field_words<Fields...>, "\345\255\230\345\234\250\345\244\232\344\270\252\344\270\215\345\220\214Field\347\232\204\345\220\215\347\247\260\346\210\226\346\230\257\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\346\214\207\351\222\210\347\233\270\345\220\214"); */
      };
      
      /* First instantiated from: schema.cpp:50 */
      #ifdef INSIGHTS_USE_TEMPLATE
      static constexpr ess::orm::meta::FixedString<6> meta::FixedString<6>_"goods"_{"goods"};
      template<>
      struct Schema<meta::FixedString<6>_"goods"_, Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >
      {
        inline static constexpr const ess::orm::meta::FixedString<6> table_name = ess::orm::meta::FixedString<6>(meta::FixedString<6>_"goods"_);
        using fields = std::tuple<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >;
        
        private: 
        template<typename Field>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def();
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field<meta::FixedString<8>{"enabled"}, &Goods::enabled, DefaultValue<true> >::member_type>;
          std::tuple<ess::orm::attribute::DefaultValue<true> > attributes = std::tuple<ess::orm::attribute::DefaultValue<true> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_boolean>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::DefaultValue<true> >(ess::orm::attribute::DefaultValue<true> __attrs0) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<true>(__attrs0)))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field<meta::FixedString<7>{"status"}, &Goods::status, DefaultValue<GoodsStatus::Deleted> >::member_type>;
          std::tuple<ess::orm::attribute::DefaultValue<2> > attributes = std::tuple<ess::orm::attribute::DefaultValue<2> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_integer>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::DefaultValue<2> >(ess::orm::attribute::DefaultValue<2> __attrs0) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<2>(__attrs0)))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field<meta::FixedString<6>{"stock"}, &Goods::stock, DefaultValue<0> >::member_type>;
          std::tuple<ess::orm::attribute::DefaultValue<0> > attributes = std::tuple<ess::orm::attribute::DefaultValue<0> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_integer>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::DefaultValue<0> >(ess::orm::attribute::DefaultValue<0> __attrs0) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<0>(__attrs0)))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field<meta::FixedString<6>{"price"}, &Goods::price, DefaultValue<0.000000e+00> >::member_type>;
          std::tuple<ess::orm::attribute::DefaultValue<0.0> > attributes = std::tuple<ess::orm::attribute::DefaultValue<0.0> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_floating>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::DefaultValue<0.0> >(ess::orm::attribute::DefaultValue<0.0> __attrs0) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<0.0>(__attrs0)))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >;
          std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > attributes = std::tuple<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_text>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >(ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> __attrs0) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_>(__attrs0)))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        /* First instantiated from: dsl.hpp:123 */
        #ifdef INSIGHTS_USE_TEMPLATE
        template<>
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_col_def<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > >()
        {
          using member_semantic_type = meta::cpp_type_to_sql_semantic_t<typename Field<meta::FixedString<3>{"id"}, &Goods::id, PrimaryKey, AutoIncrement, DefaultValue<1> >::member_type>;
          std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > attributes = std::tuple<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >{};
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > col_def = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{std::basic_string_view<char, std::char_traits<char> >(Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >::column_name.operator std::basic_string_view<char, std::char_traits<char> >())};
          col_def.operator+=(" ");
          col_def.operator+=(std::basic_string_view<char, std::char_traits<char> >(ess::orm::meta::sql_semantic_to_type_str<ess::orm::meta::sql_integer>::type_str.operator std::basic_string_view<char, std::char_traits<char> >()));
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > attrs_str = std::basic_string<char, std::char_traits<char>, std::allocator<char> >{};
                    
          class __lambda_108_9
          {
            public: 
            template<class ... type_parameter_0_0>
            inline /*constexpr */ auto operator()(type_parameter_0_0... attrs) const
            {
              ((attrs_str = static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(static_cast<<dependent type>>(attrs_str) + (" " + attribute::to_sql_fragment(attrs)))) , ...);
            }
            
            #ifdef INSIGHTS_USE_TEMPLATE
            template<>
            inline /*constexpr */ void operator()<ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >(ess::orm::attribute::PrimaryKey __attrs0, ess::orm::attribute::AutoIncrement __attrs1, ess::orm::attribute::DefaultValue<1> __attrs2) const
            {
              (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::PrimaryKey(__attrs0))))) , ((attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::AutoIncrement(__attrs1))))) , (attrs_str.operator+=(std::operator+(" ", ess::orm::attribute::to_sql_fragment(ess::orm::attribute::DefaultValue<1>(__attrs2))))));
            }
            #endif
            
            private: 
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > & attrs_str;
            
            public:
            __lambda_108_9(std::basic_string<char, std::char_traits<char>, std::allocator<char> > & _attrs_str)
            : attrs_str{_attrs_str}
            {}
            
          };
          
          std::apply(__lambda_108_9{attrs_str}, attributes);
          return operator+(col_def, attrs_str);
        }
        #endif
        
        
        public: 
        static inline std::tuple<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > > make_fields()
        {
          return std::make_tuple(Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >{}, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >{}, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >{}, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >{}, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >{}, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> >{});
        }
        
        static inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > make_create_table_ddl(bool not_replace)
        {
          std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > > col_defs = std::vector<std::basic_string<char, std::char_traits<char>, std::allocator<char> >, std::allocator<std::basic_string<char, std::char_traits<char>, std::allocator<char> > > >{};
          col_defs.reserve(6);
          col_defs.push_back(make_col_def<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > >()) , (col_defs.push_back(make_col_def<Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > >()) , (col_defs.push_back(make_col_def<Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> > >()) , (col_defs.push_back(make_col_def<Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> > >()) , (col_defs.push_back(make_col_def<Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> > >()) , col_defs.push_back(make_col_def<Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >())))));
          return fmt::format(fmt::fstring<const char *, std::basic_string_view<char, std::char_traits<char> >, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >("CREATE TABLE {}{} ({});"), (not_replace ? "IF NOT EXISTS " : ""), std::basic_string_view<char, std::char_traits<char> >(table_name.operator std::basic_string_view<char, std::char_traits<char> >()), ess::orm::meta::join(col_defs, std::basic_string_view<char, std::char_traits<char> >(",\n")));
        }
        
        
        private: 
        
        /* PASSED: static_assert(no_duplicated_key_field_words<Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >, "\345\255\230\345\234\250\345\244\232\344\270\252\344\270\215\345\220\214Field\347\232\204\345\220\215\347\247\260\346\210\226\346\230\257\347\273\221\345\256\232\347\232\204\346\210\220\345\221\230\346\214\207\351\222\210\347\233\270\345\220\214"); */
      };
      
      #endif
      template<typename Table>
      concept table_type = requires{
        typename Table::Schema;
      };
      
      
    }
    
  }
  
}

namespace ess
{
  namespace orm
  {
    template<typename T>
    inline T get_column(sqlite3_stmt * stmt, int index)
    {
      if constexpr(std::is_same_v<T, std::basic_string<char, std::char_traits<char>, std::allocator<char> > >) {
        return reinterpret_cast<const char *>(sqlite3_column_text(stmt, index));
      } else /* constexpr */ {
        if constexpr(std::is_floating_point_v<T>) {
          return static_cast<T>(sqlite3_column_double(stmt, index));
        } else /* constexpr */ {
          if constexpr(std::is_integral_v<T>) {
            return static_cast<T>(sqlite3_column_int(stmt, index));
          } else /* constexpr */ {
            if constexpr(std::is_enum_v<T>) {
              return static_cast<T>(sqlite3_column_int(stmt, index));
            } else /* constexpr */ {
            } 
            
          } 
          
        } 
        
      } 
      
    }
    
    /* First instantiated from: runtime.hpp:102 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline double get_column<double>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(true) {
          return static_cast<double>(sqlite3_column_double(stmt, index));
        } else /* constexpr */ {
        } 
        
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:106 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > get_column<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >(sqlite3_stmt * stmt, int index)
    {
      if constexpr(true) {
        return std::basic_string<char, std::char_traits<char>, std::allocator<char> >(reinterpret_cast<const char *>(sqlite3_column_text(stmt, index)));
      } else /* constexpr */ {
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:53 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline long long get_column<long long>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            return static_cast<long long>(sqlite3_column_int(stmt, index));
          } else /* constexpr */ {
          } 
          
        } 
        
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:53 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline float get_column<float>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(true) {
          return static_cast<float>(sqlite3_column_double(stmt, index));
        } else /* constexpr */ {
        } 
        
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:53 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline int get_column<int>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            return static_cast<int>(sqlite3_column_int(stmt, index));
          } else /* constexpr */ {
          } 
          
        } 
        
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:53 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline GoodsStatus get_column<GoodsStatus>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(false) {
          } else /* constexpr */ {
            if constexpr(true) {
              return static_cast<GoodsStatus>(sqlite3_column_int(stmt, index));
            } else /* constexpr */ {
            } 
            
          } 
          
        } 
        
      } 
      
    }
    #endif
    
    
    /* First instantiated from: runtime.hpp:53 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    inline bool get_column<bool>(sqlite3_stmt * stmt, int index)
    {
      if constexpr(false) {
      } else /* constexpr */ {
        if constexpr(false) {
        } else /* constexpr */ {
          if constexpr(true) {
            return static_cast<bool>(sqlite3_column_int(stmt, index));
          } else /* constexpr */ {
          } 
          
        } 
        
      } 
      
    }
    #endif
    
    inline std::basic_string<char, std::char_traits<char>, std::allocator<char> > get_column_name(sqlite3_stmt * stmt, int index)
    {
      return std::basic_string<char, std::char_traits<char>, std::allocator<char> >(reinterpret_cast<const char *>(sqlite3_column_name(stmt, index)));
    }
    template<dsl::table_type Table>
    struct SchemaMapper
    {
      
      private: 
      using schema = typename Table::Schema;
      using field_tuple = typename schema::fields;
      
      public: 
      static inline constexpr int find_field_index(std::basic_string_view<char, std::char_traits<char> > field_name)
      {
        int found_idx = -1;
                
        class __lambda_37_9
        {
          public: 
          template<class ... type_parameter_1_0>
          inline auto operator()(type_parameter_1_0... fields) const
          {
            int current_idx = 0;
            (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(fields.column_name))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , ...);
          }
          
        };
        
        std::apply(__lambda_37_9{}, field_tuple{});
        return found_idx;
      }
      
      template<unsigned long FieldIdx>
      static inline constexpr void fill_field(Table & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<FieldIdx, field_tuple>;
        using member_type = typename cur_field::member_type;
        (obj .* (cur_field::pointer)) = get_column<member_type>(stmt, col_idx);
      }
    };
    
    /* First instantiated from: schema.cpp:86 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    struct SchemaMapper<Goods>
    {
      
      private: 
      using schema = dsl::Schema<meta::FixedString<6>("goods"), dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >;
      using field_tuple = dsl::Schema<meta::FixedString<6>_"goods"_, dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >;
      
      public: 
      static inline constexpr int find_field_index(std::basic_string_view<char, std::char_traits<char> > field_name)
      {
        int found_idx = -1;
                
        class __lambda_37_9
        {
          public: 
          template<class ... type_parameter_0_0>
          inline /*constexpr */ auto operator()(type_parameter_0_0... fields) const
          {
            int current_idx = 0;
            (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(fields.column_name))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , ...);
          }
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<ess::orm::dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, ess::orm::dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, ess::orm::dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, ess::orm::dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, ess::orm::dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, ess::orm::dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >(ess::orm::dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> > __fields0, ess::orm::dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> > __fields1, ess::orm::dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> > __fields2, ess::orm::dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> > __fields3, ess::orm::dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> > __fields4, ess::orm::dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > __fields5) const
          {
            int current_idx = 0;
            static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields0.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields1.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields2.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields3.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , (static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields4.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false))) , static_cast<void>(((std::operator==(std::basic_string_view<char, std::char_traits<char> >(field_name), std::basic_string_view<char, std::char_traits<char> >(__fields5.column_name.operator std::basic_string_view<char, std::char_traits<char> >()))) ? ((found_idx = current_idx) , true) : (current_idx++ , false)))))));
          }
          #endif
          
          private: 
          std::basic_string_view<char, std::char_traits<char> > & field_name;
          int & found_idx;
          
          public:
          __lambda_37_9(std::basic_string_view<char, std::char_traits<char> > & _field_name, int & _found_idx)
          : field_name{_field_name}
          , found_idx{_found_idx}
          {}
          
        };
        
        std::apply(__lambda_37_9{field_name, found_idx}, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >{});
        return found_idx;
      }
      
      template<unsigned long FieldIdx>
      static inline constexpr void fill_field(Goods & obj, sqlite3_stmt * stmt, int col_idx);
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<0>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<0UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = typename cur_field::member_type;
        (obj .* (dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >::pointer)) = get_column<member_type>(stmt, col_idx);
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<1>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<1UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = std::basic_string<char, std::char_traits<char>, std::allocator<char> >;
        obj .* (dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >::pointer).operator=(get_column<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >(stmt, col_idx));
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<2>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<2UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = typename cur_field::member_type;
        (obj .* (dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >::pointer)) = get_column<member_type>(stmt, col_idx);
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<3>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<3UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = typename cur_field::member_type;
        (obj .* (dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >::pointer)) = get_column<member_type>(stmt, col_idx);
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<4>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<4UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = typename cur_field::member_type;
        (obj .* (dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >::pointer)) = get_column<member_type>(stmt, col_idx);
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:125 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline constexpr void fill_field<5>(Goods & obj, sqlite3_stmt * stmt, int col_idx)
      {
        using cur_field = std::tuple_element_t<5UL, std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >;
        using member_type = typename cur_field::member_type;
        (obj .* (dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> >::pointer)) = get_column<member_type>(stmt, col_idx);
      }
      #endif
      
    };
    
    #endif
    template<dsl::table_type Table>
    struct ResultSetMapper
    {
      std::vector<int, std::allocator<int> > m_col_to_field_map = std::vector<int, std::allocator<int> >{};
      bool is_inialized = false;
      using schema_mapper = SchemaMapper<Table>;
      inline void init_mapper(sqlite3_stmt * stmt)
      {
        if(!this->is_inialized) {
          int size = sqlite3_column_count(stmt);
          this->m_col_to_field_map.resize(static_cast<unsigned long>(size));
          for(int i = 0; i < size; ++i) {
            std::basic_string_view<char, std::char_traits<char> > name = std::basic_string_view<char, std::char_traits<char> >(sqlite3_column_name(stmt, i));
            int idx = schema_mapper::find_field_index(name);
            this->m_col_to_field_map.operator[](static_cast<unsigned long>(i)) = idx;
          }
          
          this->is_inialized = true;
        } 
        
      }
      
      inline void map_row(sqlite3_stmt * stmt, Table & obj)
      {
        for(int col_idx = 0; static_cast<unsigned long>(col_idx) < this->m_col_to_field_map.size(); ++col_idx) {
          int field_idx = this->m_col_to_field_map.operator[](static_cast<unsigned long>(col_idx));
          if(field_idx == -1) {
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > error_info = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("unknown column: ");
            error_info.operator+=(sqlite3_column_name(stmt, col_idx));
            throw std::runtime_error(std::runtime_error(error_info));
          } 
          
          this->dispatch_fill(stmt, obj, field_idx, col_idx);
        }
        
      }
      
      inline Row map_row(sqlite3_stmt * stmt)
      {
        Row row = Row() /* NRVO variable */;
        int size = sqlite3_column_count(stmt);
        for(int idx = 0; idx < size; ++idx) {
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > name = get_column_name(stmt, idx);
          int type = sqlite3_column_type(stmt, idx);
          switch(type) {
            case 1: {
              long long val = sqlite3_column_int64(stmt, idx);
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>(val));
              break;
            };
            case 2: {
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>{(get_column<double>(stmt, idx))});
              break;
            };
            case 3: {
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>{(get_column<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >(stmt, idx))});
              break;
            };
            case 5: break;
            default: break;
          }
        }
        
        return row;
      }
      
      
      private: 
      inline void dispatch_fill(sqlite3_stmt * stmt, Table & obj, int field_idx, int col_idx)
      {
        auto fields = Table::Schema::make_fields();
        constexpr const int N = std::tuple_size_v<decltype(fields)>;
                
        class __lambda_123_22
        {
          public: 
          template<class type_parameter_1_0>
          inline auto operator()(type_parameter_1_0 I) const
          {
            if(I == field_idx) {
              schema_mapper::template fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          
        };
        
        static_for<0, N>(__lambda_123_22{});
      }
      
      template<unsigned long Beg, unsigned long End, typename Func>
      static inline void static_for(Func && func)
      {
        if constexpr(Beg < End) {
          std::invoke(func, std::integral_constant<std::size_t, Beg>());
          static_for<Beg + 1, End>(func);
        } 
        
      }
    };
    
    /* First instantiated from: schema.cpp:66 */
    #ifdef INSIGHTS_USE_TEMPLATE
    template<>
    struct ResultSetMapper<Goods>
    {
      std::vector<int, std::allocator<int> > m_col_to_field_map = std::vector<int, std::allocator<int> >{};
      bool is_inialized = false;
      using schema_mapper = SchemaMapper<Goods>;
      inline void init_mapper(sqlite3_stmt * stmt)
      {
        if(!this->is_inialized) {
          int size = sqlite3_column_count(stmt);
          this->m_col_to_field_map.resize(static_cast<unsigned long>(size));
          for(int i = 0; i < size; ++i) {
            std::basic_string_view<char, std::char_traits<char> > name = std::basic_string_view<char, std::char_traits<char> >(sqlite3_column_name(stmt, i));
            int idx = SchemaMapper<Goods>::find_field_index(std::basic_string_view<char, std::char_traits<char> >(name));
            this->m_col_to_field_map.operator[](static_cast<unsigned long>(i)) = idx;
          }
          
          this->is_inialized = true;
        } 
        
      }
      
      inline void map_row(sqlite3_stmt * stmt, Goods & obj)
      {
        for(int col_idx = 0; static_cast<unsigned long>(col_idx) < this->m_col_to_field_map.size(); ++col_idx) {
          int field_idx = this->m_col_to_field_map.operator[](static_cast<unsigned long>(col_idx));
          if(field_idx == -1) {
            std::basic_string<char, std::char_traits<char>, std::allocator<char> > error_info = std::basic_string<char, std::char_traits<char>, std::allocator<char> >("unknown column: ");
            error_info.operator+=(sqlite3_column_name(stmt, col_idx));
            throw std::runtime_error(std::runtime_error(error_info));
          } 
          
          this->dispatch_fill(stmt, obj, field_idx, col_idx);
        }
        
      }
      
      inline Row map_row(sqlite3_stmt * stmt)
      {
        Row row = Row() /* NRVO variable */;
        int size = sqlite3_column_count(stmt);
        for(int idx = 0; idx < size; ++idx) {
          std::basic_string<char, std::char_traits<char>, std::allocator<char> > name = get_column_name(stmt, idx);
          int type = sqlite3_column_type(stmt, idx);
          switch(type) {
            case 1: {
              long long val = sqlite3_column_int64(stmt, idx);
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>(val));
              break;
            };
            case 2: {
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>{(get_column<double>(stmt, idx))});
              break;
            };
            case 3: {
              row.add_column(std::basic_string<char, std::char_traits<char>, std::allocator<char> >(name), std::variant<int, long long, double, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, bool>{(get_column<std::basic_string<char, std::char_traits<char>, std::allocator<char> > >(stmt, idx))});
              break;
            };
            case 5: break;
            default: break;
          }
        }
        
        return row;
      }
      
      
      private: 
      inline void dispatch_fill(sqlite3_stmt * stmt, Goods & obj, int field_idx, int col_idx)
      {
        std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > fields = dsl::Schema<meta::FixedString<6>_"goods"_, dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > >::make_fields();
        constexpr const int N = static_cast<const int>(std::tuple_size_v<std::tuple<dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, attribute::PrimaryKey, attribute::AutoIncrement, attribute::DefaultValue<1> >, dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, attribute::DefaultValue<FixedString<9>_"untitled"_> >, dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, attribute::DefaultValue<0.0> >, dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, attribute::DefaultValue<0> >, dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, attribute::DefaultValue<2> >, dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, attribute::DefaultValue<true> > > >);
                
        class __lambda_123_22
        {
          public: 
          template<class type_parameter_0_0>
          inline /*constexpr */ auto operator()(type_parameter_0_0 I) const
          {
            if(I == field_idx) {
              SchemaMapper<Goods>::template fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 0> >(std::integral_constant<unsigned long, 0> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 0>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 1> >(std::integral_constant<unsigned long, 1> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 1>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 2> >(std::integral_constant<unsigned long, 2> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 2>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 3> >(std::integral_constant<unsigned long, 3> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 3>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 4> >(std::integral_constant<unsigned long, 4> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 4>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          
          #ifdef INSIGHTS_USE_TEMPLATE
          template<>
          inline /*constexpr */ void operator()<std::integral_constant<unsigned long, 5> >(std::integral_constant<unsigned long, 5> I) const
          {
            if(I.operator std::integral_constant<unsigned long, 5>::value_type() == static_cast<unsigned long>(field_idx)) {
              ess::orm::SchemaMapper<Goods>::fill_field<I>(obj, stmt, col_idx);
            } 
            
          }
          #endif
          
          private: 
          int & field_idx;
          Goods & obj;
          sqlite3_stmt *& stmt;
          int & col_idx;
          
          public:
          __lambda_123_22(int & _field_idx, Goods & _obj, sqlite3_stmt *& _stmt, int & _col_idx)
          : field_idx{_field_idx}
          , obj{_obj}
          , stmt{_stmt}
          , col_idx{_col_idx}
          {}
          
        };
        
        static_for<0, N>(__lambda_123_22{field_idx, obj, stmt, col_idx});
      }
      
      template<unsigned long Beg, unsigned long End, typename Func>
      static inline void static_for(Func && func);
      
      /* First instantiated from: runtime.hpp:123 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<0, 6, __lambda_123_22>(__lambda_123_22 && func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 0>());
          static_for<0UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<1, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 1>());
          static_for<1UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<2, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 2>());
          static_for<2UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<3, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 3>());
          static_for<3UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<4, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 4>());
          static_for<4UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<5, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(true) {
          std::invoke(func, std::integral_constant<unsigned long, 5>());
          static_for<5UL + 1, 6UL>(func);
        } 
        
      }
      #endif
      
      
      /* First instantiated from: runtime.hpp:134 */
      #ifdef INSIGHTS_USE_TEMPLATE
      template<>
      static inline void static_for<6, 6, __lambda_123_22 &>(__lambda_123_22 & func)
      {
        if constexpr(false) {
        } 
        
      }
      #endif
      
      public: 
      // inline constexpr ~ResultSetMapper() noexcept = default;
    };
    
    #endif
    template<typename Table, meta::meta::FixedString SQL, class ... type_parameter_0_2>
    auto query(type_parameter_0_2... && args)
    {
      /* PASSED: static_assert(dsl::table_type<Table>, "\350\257\267\344\275\277\347\224\250\346\214\201\346\234\211 Schema \347\232\204 Table \347\261\273\345\236\213"); */
      if(std::is_same_v<dialect::Postgres, dialect::Postgres>) {
        fmt::println(fmt::fstring<>("Postgres"));
      } else {
        if(std::is_same_v<dialect::Postgres, dialect::Sqlite>) {
          fmt::println(fmt::fstring<>("Sqlite"));
        } 
        
      } 
      
    }
    
  }
  
}
using namespace ess::orm;
using namespace ess::orm::meta;

enum class GoodsStatus : int
{
  Normal = 0, 
  Disabled, 
  Deleted
};


struct Goods
{
  long long id = 0;
  std::basic_string<char, std::char_traits<char>, std::allocator<char> > title;
  float price = static_cast<float>(0.0);
  int stock = 0;
  GoodsStatus status = GoodsStatus::Normal;
  bool enabled = true;
  using Schema = dsl::Schema<ess::orm::meta::FixedString<6>("goods"), ess::orm::dsl::Field<meta::FixedString<3>_"id"_, &id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, ess::orm::dsl::Field<meta::FixedString<6>_"title"_, &title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, ess::orm::dsl::Field<meta::FixedString<6>_"price"_, &price, ess::orm::attribute::DefaultValue<0.0> >, ess::orm::dsl::Field<meta::FixedString<6>_"stock"_, &stock, ess::orm::attribute::DefaultValue<0> >, ess::orm::dsl::Field<meta::FixedString<7>_"status"_, &status, ess::orm::attribute::DefaultValue<2> >, ess::orm::dsl::Field<meta::FixedString<8>_"enabled"_, &enabled, ess::orm::attribute::DefaultValue<true> > >;
  // inline constexpr ~Goods() noexcept = default;
};


template<size_t N>
void println(const ess::orm::meta::FixedString<N> & str)
{
  fmt::println(fmt::fstring<std::basic_string_view<char, std::char_traits<char> > >("{}"), std::basic_string_view<char, std::char_traits<char> >(str));
}

struct Foo
{
};


void func()
{
  sqlite3 * db = nullptr;
  int rc = sqlite3_open_v2("data/test.db", &db, 2, nullptr);
  if(rc != 0) {
    std::operator<<(std::operator<<(std::cerr, "\346\227\240\346\263\225\346\211\223\345\274\200\346\225\260\346\215\256\345\272\223: "), sqlite3_errmsg(db)).operator<<(std::endl);
    sqlite3_close(db);
    return;
  } 
  
  sqlite3_stmt * stmt;
  std::basic_string<char, std::char_traits<char>, std::allocator<char> > ddl = ess::orm::dsl::Schema<meta::FixedString<6>_"goods"_, ess::orm::dsl::Field<meta::FixedString<3>_"id"_, &Goods::id, ess::orm::attribute::PrimaryKey, ess::orm::attribute::AutoIncrement, ess::orm::attribute::DefaultValue<1> >, ess::orm::dsl::Field<meta::FixedString<6>_"title"_, &Goods::title, ess::orm::attribute::DefaultValue<FixedString<9>_"untitled"_> >, ess::orm::dsl::Field<meta::FixedString<6>_"price"_, &Goods::price, ess::orm::attribute::DefaultValue<0.0> >, ess::orm::dsl::Field<meta::FixedString<6>_"stock"_, &Goods::stock, ess::orm::attribute::DefaultValue<0> >, ess::orm::dsl::Field<meta::FixedString<7>_"status"_, &Goods::status, ess::orm::attribute::DefaultValue<2> >, ess::orm::dsl::Field<meta::FixedString<8>_"enabled"_, &Goods::enabled, ess::orm::attribute::DefaultValue<true> > >::make_create_table_ddl(true);
  rc = sqlite3_prepare_v2(db, "SELECT * FROM goods WHERE id = 1", -1, &stmt, nullptr);
  if(rc != 0) {
    std::operator<<(std::operator<<(std::cerr, "select stmt prepare failed: "), sqlite3_errmsg(db)).operator<<(std::endl);
    sqlite3_close(db);
    return;
  } 
  
  Goods goods = {{0}, std::basic_string<char, std::char_traits<char>, std::allocator<char> >{}, {static_cast<float>(0.0)}, {0}, {GoodsStatus::Normal}, {true}};
  int res = sqlite3_step(stmt);
  if(res == 100) {
    ess::orm::ResultSetMapper<Goods> mapper = ess::orm::ResultSetMapper<Goods>{{std::vector<int, std::allocator<int> >{}}, {false}};
    mapper.init_mapper(stmt);
    ess::orm::Row row = mapper.map_row(stmt);
    fmt::println(fmt::fstring<int, std::basic_string<char, std::char_traits<char>, std::allocator<char> >, double, double, int, bool>("{} {} {} {} {} {}"), static_cast<int>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("id")).operator int()), static_cast<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("title")).operator std::basic_string<char, std::char_traits<char>, std::allocator<char> >()), static_cast<double>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("price")).operator double()), static_cast<double>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("stock")).operator double()), static_cast<int>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("status")).operator int()), static_cast<bool>(row.operator[](std::basic_string<char, std::char_traits<char>, std::allocator<char> >("enabled")).operator bool()));
    mapper.map_row(stmt, goods);
    fmt::println(fmt::fstring<long long &, std::basic_string<char, std::char_traits<char>, std::allocator<char> > &, float &, int &, int, bool &>("{} {} {} {} {} {}"), goods.id, goods.title, goods.price, goods.stock, static_cast<int>(goods.status), goods.enabled);
  } 
  
  sqlite3_finalize(stmt);
  sqlite3_close(db);
}

int main()
{
  Goods goods = {{0}, std::basic_string<char, std::char_traits<char>, std::allocator<char> >{}, {static_cast<float>(0.0)}, {0}, {GoodsStatus::Normal}, {true}};
  constexpr const int idx = ess::orm::SchemaMapper<Goods>::find_field_index(std::basic_string_view<char, std::char_traits<char> >("status"));
  fmt::println(fmt::fstring<const int &>("{}"), idx);
  func();
  return 0;
}
