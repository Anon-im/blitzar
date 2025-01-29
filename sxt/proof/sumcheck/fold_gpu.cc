/** Proofs GPU - Space and Time's cryptographic proof algorithms on the CPU and GPU.
 *
 * Copyright 2025-present Space and Time Labs, Inc.
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
#include "sxt/proof/sumcheck/fold_gpu.h"

#include <cassert>

#include "sxt/algorithm/iteration/kernel_fit.h"
#include "sxt/base/device/memory_utility.h"
#include "sxt/base/device/property.h"
#include "sxt/base/device/stream.h"
#include "sxt/base/error/assert.h"
#include "sxt/base/iterator/split.h"
#include "sxt/base/num/ceil_log2.h"
#include "sxt/execution/async/coroutine.h"
#include "sxt/execution/async/future.h"
#include "sxt/execution/device/for_each.h"
#include "sxt/execution/device/synchronization.h"
#include "sxt/execution/kernel/kernel_dims.h"
#include "sxt/memory/management/managed_array.h"
#include "sxt/memory/resource/async_device_resource.h"
#include "sxt/memory/resource/device_resource.h"
#include "sxt/proof/sumcheck/mle_utility.h"
#include "sxt/scalar25/operation/mul.h"
#include "sxt/scalar25/operation/muladd.h"
#include "sxt/scalar25/operation/sub.h"
#include "sxt/scalar25/type/element.h"
#include "sxt/scalar25/type/literal.h"

namespace sxt::prfsk {
//--------------------------------------------------------------------------------------------------
// fold_kernel
//--------------------------------------------------------------------------------------------------
static __global__ void fold_kernel(s25t::element* __restrict__ mles, unsigned np, unsigned split,
                                   s25t::element r, s25t::element one_m_r) noexcept {
  auto thread_index = threadIdx.x;
  auto block_index = blockIdx.x;
  auto block_size = blockDim.x;
  auto k = basn::divide_up(split, gridDim.x * block_size) * block_size;
  auto block_first = block_index * k;
  assert(block_first < split && "every block should be active");
  auto m = umin(block_first + k, split);

  // adjust mles
  mles += np * blockIdx.y;

  // fold
  auto index = block_first + thread_index;
  for (; index < m; index += block_size) {
    auto x = mles[index];
    s25o::mul(x, x, one_m_r);
    auto index_p = split + index;
    if (index_p < np) {
      s25o::muladd(x, mles[index_p], r, x);
    }
    mles[index] = x;
  }
}

//--------------------------------------------------------------------------------------------------
// fold_impl
//--------------------------------------------------------------------------------------------------
static xena::future<> fold_impl(basct::span<s25t::element> mles_p, basct::cspan<s25t::element> mles,
                                unsigned n, unsigned mid, unsigned a, unsigned b,
                                const s25t::element& r, const s25t::element one_m_r) noexcept {
  auto num_mles = mles.size() / n;
  auto split = b - a;

  // copy MLEs to device
  basdv::stream stream;
  memr::async_device_resource resource{stream};
  memmg::managed_array<s25t::element> mles_dev{&resource};
  copy_partial_mles(mles_dev, stream, mles, n, a, b);

  // fold
  auto np = mles_dev.size() / num_mles;
  auto dims = algi::fit_iteration_kernel(split);
  fold_kernel<<<dim3(dims.num_blocks, num_mles, 1), static_cast<unsigned>(dims.block_size), 0,
                stream>>>(mles_dev.data(), np, split, r, one_m_r);

  // copy results back
  copy_folded_mles(mles_p, stream, mles_dev, mid, a, b);

  co_await xendv::await_stream(stream);
}

//--------------------------------------------------------------------------------------------------
// fold_gpu
//--------------------------------------------------------------------------------------------------
xena::future<> fold_gpu(basct::span<s25t::element> mles_p,
                        const basit::split_options& split_options, basct::cspan<s25t::element> mles,
                        unsigned n, const s25t::element& r) noexcept {
  using s25t::operator""_s25;
  auto num_mles = mles.size() / n;
  auto num_variables = std::max(basn::ceil_log2(n), 1);
  auto mid = 1u << (num_variables - 1u);
  SXT_DEBUG_ASSERT(
      // clang-format off
      n > 1 && mles.size() == num_mles * n
      // clang-format on
  );
  s25t::element one_m_r = 0x1_s25;
  s25o::sub(one_m_r, one_m_r, r);

  // split
  auto [chunk_first, chunk_last] = basit::split(basit::index_range{0, mid}, split_options);

  // fold
  co_await xendv::concurrent_for_each(
      chunk_first, chunk_last, [&](basit::index_range rng) noexcept -> xena::future<> {
        co_await fold_impl(mles_p, mles, n, mid, rng.a(), rng.b(), r, one_m_r);
      });
}

xena::future<> fold_gpu(basct::span<s25t::element> mles_p, basct::cspan<s25t::element> mles,
                        unsigned n, const s25t::element& r) noexcept {
  using s25t::operator""_s25;
  auto num_mles = mles.size() / n;
  auto num_variables = std::max(basn::ceil_log2(n), 1);
  auto mid = 1u << (num_variables - 1u);
  SXT_DEBUG_ASSERT(
      // clang-format off
      n > 1 && mles.size() == num_mles * n
      // clang-format on
  );
  s25t::element one_m_r = 0x1_s25;
  s25o::sub(one_m_r, one_m_r, r);

  // split
  basit::split_options split_options{
      .min_chunk_size = 1024u * 128u,
      .max_chunk_size = 1024u * 256u,
      .split_factor = basdv::get_num_devices(),
  };
  auto [chunk_first, chunk_last] = basit::split(basit::index_range{0, mid}, split_options);

  // fold
  co_await xendv::concurrent_for_each(
      chunk_first, chunk_last, [&](basit::index_range rng) noexcept -> xena::future<> {
        co_await fold_impl(mles_p, mles, n, mid, rng.a(), rng.b(), r, one_m_r);
      });
}
} // namespace sxt::prfsk
