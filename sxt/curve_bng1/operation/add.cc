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
/**
 * Adopted from zkcrypto/bls12_381
 *
 * Copyright (c) 2021
 * Sean Bowe <ewillbefull@gmail.com>
 * Jack Grigg <thestr4d@gmail.com>
 *
 * See third_party/license/zkcrypto.LICENSE
 */
#include "sxt/curve_bng1/operation/add.h"

#include "sxt/curve_bng1/operation/cmov.h"
#include "sxt/curve_bng1/property/identity.h"
#include "sxt/curve_bng1/type/element_affine.h"

namespace sxt::cn1o {
//--------------------------------------------------------------------------------------------------
// add
//--------------------------------------------------------------------------------------------------
CUDA_CALLABLE
void add(cn1t::element_p2& h, const cn1t::element_p2& p, const cn1t::element_affine& q) noexcept {
  f25t::element t0, t1, t2, t3, t4;
  f25t::element x3, y3, z3;

  f25o::mul(t0, p.X, q.X);
  f25o::mul(t1, p.Y, q.Y);
  f25o::add(t3, q.X, q.Y);
  f25o::add(t4, p.X, p.Y);
  f25o::mul(t3, t3, t4);
  f25o::add(t4, t0, t1);
  f25o::sub(t3, t3, t4);
  f25o::mul(t4, q.Y, p.Z);
  f25o::add(t4, t4, p.Y);
  f25o::mul(y3, q.X, p.Z);
  f25o::add(y3, y3, p.X);
  f25o::add(x3, t0, t0);
  f25o::add(t0, x3, t0);
  mul_by_3b(t2, p.Z);
  f25o::add(z3, t1, t2);
  f25o::sub(t1, t1, t2);
  mul_by_3b(y3, y3);
  f25o::mul(x3, t4, y3);
  f25o::mul(t2, t3, t1);
  f25o::sub(x3, t2, x3);
  f25o::mul(y3, y3, t0);
  f25o::mul(t1, t1, z3);
  f25o::add(y3, t1, y3);
  f25o::mul(t0, t0, t3);
  f25o::mul(z3, z3, t4);
  f25o::add(z3, z3, t0);

  h.X = x3;
  h.Y = y3;
  h.Z = z3;

  cmov(h, p, cn1p::is_identity(q));
}
} // namespace sxt::cn1o
