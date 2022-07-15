# Taken from https://github.com/bazelbuild/bazel/issues/2670#issuecomment-369674735
# with modifications.
def sxt_private_include_copts(includes, is_system = False):
    copts = []
    prefix = ""

    # convert "@" to "external/" unless in the main workspace
    repo_name = native.repository_name()
    if repo_name != "@":
        prefix = "external/{}/".format(repo_name[1:])

    inc_flag = "-isystem " if is_system else "-I"

    for inc in includes:
        copts.append("{}{}{}".format(inc_flag, prefix, inc))
        copts.append("{}$(GENDIR){}/{}".format(inc_flag, prefix, inc))

    return copts

# We add this -std=c++17 flag, because
# benchmarks could not be compiled without it.
# The `build --cxxopt -std=c++17` flag set in the
# `.bazelrc` file was not passed to the compiler.
# However, this flag is relevant to some modules.
def sxt_copts():
    return [
        "-std=c++17",
    ]

def sxt_cc_benchmark(
        name,
        srcs = [],
        visibility = None,
        external_deps = [],
        deps = [],
        copts = [],
        linkopts = []):
    native.genrule(
        name = name + "_callgrind",
        srcs = [],
        outs = [
            name + "_profile.sh",
        ],
        tools = [
            ":" + name,
            "//bazel:bench_profile",
        ],
        local = 1,
        executable = 1,
        output_to_bindir = 1,
        visibility = visibility,
        cmd_bash = "$(location //bazel:bench_profile) $$PWD/$(location :%s) $@" % name,
    )

    native.cc_binary(
        name = name,
        srcs = srcs,
        copts = copts + sxt_copts(),
        deps = deps,
        linkopts = linkopts,
        visibility = visibility,
    )
