# Copyright 2021 The Pigweed Authors
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
)

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Apache License 2.0

pw_cc_library(
    name = "pw_boot_armv7m",
    srcs = [
        "core_init.c",
        "public/pw_boot_armv7m/boot.h",
    ],
    includes = ["public"],
    target_compatible_with = select({
        "@platforms//cpu:armv7e-m": [],
        "@platforms//cpu:armv7-m": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//pw_preprocessor",
    ],
)
