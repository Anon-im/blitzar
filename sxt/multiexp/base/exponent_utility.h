#pragma once

#include <cstdint>

#include "sxt/base/container/span.h"

namespace sxt::mtxb {
struct exponent;

//--------------------------------------------------------------------------------------------------
// or_equal
//--------------------------------------------------------------------------------------------------
void or_equal(exponent& e, basct::cspan<uint8_t> value) noexcept;

//--------------------------------------------------------------------------------------------------
// count_nonzero_digits
//--------------------------------------------------------------------------------------------------
size_t count_nonzero_digits(const exponent& e, size_t radix_log2) noexcept;

//--------------------------------------------------------------------------------------------------
// count_num_digits
//--------------------------------------------------------------------------------------------------
size_t count_num_digits(const exponent& e, size_t radix_log2) noexcept;

//--------------------------------------------------------------------------------------------------
// extract_digit
//--------------------------------------------------------------------------------------------------
uint8_t extract_digit(const exponent& e, size_t radix_log2, size_t digit_index) noexcept;
} // namespace sxt::mtxb
