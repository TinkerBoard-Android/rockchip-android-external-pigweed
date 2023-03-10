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
    "pw_cc_library",
    "pw_cc_test",
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

pw_cc_library(
    name = "headers",
    hdrs = [
        "public/pw_log_tokenized/config.h",
        "public/pw_log_tokenized/log_tokenized.h",
        "public_overrides/pw_log_backend/log_backend.h",
    ],
    includes = [
        "public",
        "public_overrides",
    ],
    deps = [
        "//pw_tokenizer",
    ],
)

pw_cc_library(
    name = "pw_log_tokenized",
    deps = [
        ":headers",
        "//pw_log:facade",
    ],
)

pw_cc_library(
    name = "base64_over_hdlc",
    srcs = ["base64_over_hdlc.cc"],
    hdrs = ["public/pw_log_tokenized/base64_over_hdlc.h"],
    includes = ["public"],
    deps = [
        "//pw_hdlc:encoder",
        "//pw_tokenizer:base64",
        "//pw_tokenizer:global_handler_with_payload.facade",
    ],
)

pw_cc_test(
    name = "test",
    srcs = [
        "test.cc",
    ],
    deps = [
        ":headers",
        "//pw_unit_test",
    ],
)
