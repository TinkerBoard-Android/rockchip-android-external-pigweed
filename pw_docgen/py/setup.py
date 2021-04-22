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
"""pw_docgen"""

import setuptools  # type: ignore

setuptools.setup(
    name='pw_docgen',
    version='0.0.1',
    author='Pigweed Authors',
    author_email='pigweed-developers@googlegroups.com',
    description='Generate Sphinx documentation',
    packages=setuptools.find_packages(),
    package_data={'pw_docgen': ['py.typed']},
    zip_safe=False,
    install_requires=[
        'sphinx>=2, <3',  # Restrict version for compatibility with m2r plugin
        'sphinx-rtd-theme',
        # Markdown to REST for documentation.
        'm2r',
        # Diagram generation modules.
        'sphinxcontrib-actdiag',
        'sphinxcontrib-blockdiag',
        'sphinxcontrib-nwdiag',
        'sphinxcontrib-seqdiag',
    ],
)
