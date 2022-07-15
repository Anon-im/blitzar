#include "sxt/field51/type/element.h"

#include <array>
#include <iomanip>
#include <iostream>

#include "sxt/field51/base/byte_conversion.h"
#include "sxt/field51/base/reduce.h"

namespace sxt::f51t {
//--------------------------------------------------------------------------------------------------
// print_impl
//--------------------------------------------------------------------------------------------------
static std::ostream& print_impl(std::ostream& out, const std::array<uint8_t, 32>& bytes,
                                int start) noexcept {
  out << std::hex << static_cast<int>(bytes[start]);
  for (int i = start; i-- > 0;) {
    out << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
  }
  out << "_f51";
  return out;
}

//--------------------------------------------------------------------------------------------------
// operator<<
//--------------------------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& out, const element& e) noexcept {
  std::array<uint8_t, 32> bytes = {};
  f51b::to_bytes(bytes.data(), e.data());
  auto flags = out.flags();
  out << "0x";
  for (int i = 32; i-- > 0;) {
    if (bytes[i] != 0) {
      print_impl(out, bytes, i);
      out.flags(flags);
      return out;
    }
  }
  out << "0_f51";
  out.flags(flags);
  return out;
}

//--------------------------------------------------------------------------------------------------
// operator==
//--------------------------------------------------------------------------------------------------
bool operator==(const element& lhs, const element& rhs) noexcept {
  element lhs_p;
  f51b::reduce(lhs_p.data(), lhs.data());

  element rhs_p;
  f51b::reduce(rhs_p.data(), rhs.data());

  return lhs_p[0] == rhs_p[0] && lhs_p[1] == rhs_p[1] && lhs_p[2] == rhs_p[2] &&
         lhs_p[3] == rhs_p[3] && lhs_p[4] == rhs_p[4];
}
} // namespace sxt::f51t
