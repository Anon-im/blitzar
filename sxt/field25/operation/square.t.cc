/** Proofs GPU - Space and Time's cryptographic proof algorithms on the CPU and GPU.
 *
 * Copyright 2023-present Space and Time Labs, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "sxt/field25/operation/square.h"

#include "sxt/base/num/fast_random_number_generator.h"
#include "sxt/base/test/unit_test.h"
#include "sxt/field25/operation/mul.h"
#include "sxt/field25/random/element.h"
#include "sxt/field25/type/element.h"

using namespace sxt;
using namespace sxt::f25o;

TEST_CASE("squaring") {
  SECTION(
      "of a random field element is equal to the multiplication of the same value with itself") {
    f25t::element a;
    basn::fast_random_number_generator rng{1, 2};
    f25rn::generate_random_element(a, rng);

    f25t::element ret_square;
    square(ret_square, a);

    f25t::element ret_mul;
    mul(ret_mul, a, a);

    REQUIRE(ret_square == ret_mul);
  }
}
