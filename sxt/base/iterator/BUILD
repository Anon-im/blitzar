load(
    "//bazel:sxt_build_system.bzl",
    "sxt_cc_component",
)

sxt_cc_component(
    name = "counting_iterator",
    test_deps = [
        "//sxt/base/test:unit_test",
    ],
    deps = [
        ":iterator_facade",
    ],
)

sxt_cc_component(
    name = "iterator_facade",
    test_deps = [
        "//sxt/base/test:unit_test",
    ],
    deps = [
        "//sxt/base/type:remove_cvref",
    ],
)

sxt_cc_component(
    name = "index_range",
    impl_deps = [
        "//sxt/base/error:assert",
    ],
    with_test = False,
)

sxt_cc_component(
    name = "index_range_iterator",
    test_deps = [
        "//sxt/base/test:unit_test",
    ],
    deps = [
        ":index_range",
        ":iterator_facade",
        "//sxt/base/error:assert",
        "//sxt/base/num:divide_up",
        "//sxt/base/type:narrow_cast",
    ],
)

sxt_cc_component(
    name = "split",
    impl_deps = [
        "//sxt/base/error:assert",
        "//sxt/base/num:divide_up",
    ],
    test_deps = [
        "//sxt/base/test:unit_test",
    ],
    deps = [
        ":index_range",
        ":index_range_iterator",
    ],
)
