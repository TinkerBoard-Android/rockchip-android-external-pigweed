# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

load(
    "//pw_build:pigweed.bzl",
    "pw_cc_binary",
    "pw_cc_library",
    "pw_cc_test",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

# TODO(pwbug/101): Need to add support for facades/backends to Bazel.

pw_cc_library(
    name = "headers",
    hdrs = [
        "public/pw_trace_tokenized/internal/trace_tokenized_internal.h",
        "public/pw_trace_tokenized/trace_callback.h",
        "public/pw_trace_tokenized/trace_tokenized.h",
        "public_overrides/pw_trace_backend/trace_backend.h",
    ],
    includes = [
        "public",
        "public_overrides",
    ],
    deps = [
        "//pw_preprocessor",
        "//pw_tokenizer",
    ],
)

pw_cc_library(
    name = "pw_trace_tokenized",
    srcs = [
        "trace.cc",
    ],
    deps = [
        ":headers",
        "//pw_assert",
        "//pw_log",
        "//pw_status",
        "//pw_tokenizer",
        "//pw_trace:facade",
        "//pw_varint",
    ],
)

pw_cc_library(
    name = "trace_buffer_headers",
    hdrs = [
        "public/pw_trace_tokenized/trace_buffer.h",
    ],
    includes = [
        "public",
    ],
    deps = [
        "//pw_ring_buffer",
        "//pw_status",
        "//pw_trace_tokenized",
    ],
)

pw_cc_library(
    name = "pw_trace_tokenized_buffer",
    srcs = [
        "trace_buffer.cc",
    ],
    deps = [
        ":trace_buffer_headers",
        "//pw_ring_buffer",
        "//pw_status",
    ],
)

pw_cc_library(
    name = "pw_trace_tokenized_fake_time",
    srcs = [
        "fake_trace_time.cc",
    ],
    deps = [
        "//pw_trace",
    ],
)

pw_cc_test(
    name = "trace_tokenized_test",
    srcs = [
        "trace_test.cc",
    ],
    includes = [
        "pw_trace_test",
        "pw_trace_test/public_overrides"
    ],
    deps = [
        ":backend",
        ":facade",
        ":pw_trace",
        "//pw_preprocessor",
        "//pw_unit_test",
    ],
)

pw_cc_test(
    name = "trace_tokenized_buffer_test",
    srcs = [
        "trace_buffer_test.cc",
    ],
    deps = [
        ":backend",
        ":facade",
        ":pw_trace",
        "//pw_preprocessor",
        "//pw_unit_test",
    ],
)

pw_cc_library(
    name = "pw_trace_host_trace_time",
    includes = [ "example/public" ],
    deps = [ "//pw_trace" ],
    srcs = [ "host_trace_time.cc" ]
)

pw_cc_library(
    name = "pw_trace_example_to_file",
    includes = [ "example/public" ],
    deps = [ "//pw_trace" ],
    hdrs = [ "example/public/pw_trace_tokenized/example/trace_to_file.h" ]
)

pw_cc_binary(
    name = "trace_tokenized_example_basic",
    deps = [
        ":pw_trace_example_to_file",
        "//pw_log",
        "//dir_pw_trace",
        "//dir_pw_trace:pw_trace_sample_app",
    ],
    srcs = [ "example/basic.cc" ]
)

pw_cc_binary(
    name = "trace_tokenized_example_trigger",
    deps = [
        ":pw_trace_example_to_file",
        "//pw_log",
        "//dir_pw_trace",
        "//dir_pw_trace:pw_trace_sample_app",
    ],
    srcs = [ "example/trigger.cc" ]
)

pw_cc_binary(
    name = "trace_tokenized_example_filter",
    deps = [
        ":pw_trace_example_to_file",
        "//pw_log",
        "//dir_pw_trace",
        "//dir_pw_trace:pw_trace_sample_app",
    ],
    srcs = [ "example/filter.cc" ]
)