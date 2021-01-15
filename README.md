# vinecopulib
[![Build Status](https://github.com/vinecopulib/vinecopulib/workflows/Build%20Status/badge.svg)](https://github.com/vinecopulib/vinecopulib/actions)
[![Coverage Status](https://img.shields.io/codecov/c/github/vinecopulib/vinecopulib/master.svg)](https://codecov.io/github/vinecopulib/vinecopulib?branch=master)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/a81e08da0aa344418114298d3ba04931)](https://www.codacy.com/gh/vinecopulib/vinecopulib?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=vinecopulib/vinecopulib&amp;utm_campaign=Badge_Grade)

#### What are vine copulas?

Vine copulas are a flexible class of dependence models consisting of bivariate
building blocks (see e.g.,
[Aas et al., 2009](https://mediatum.ub.tum.de/doc/1083600/1083600.pdf)).
You can find a comprehensive list of publications and other materials on
[vine-copula.org](http://www.statistics.ma.tum.de/en/research/vine-copula-models/).

#### What is vinecopulib?

[vinecopulib](https://vinecopulib.github.io/vinecopulib/) is a header-only C++ library for vine copula models based on
[Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page). It provides
high-performance implementations of the core features of the popular
[VineCopula R library](https://github.com/tnagler/VineCopula), in particular
inference algorithms for both vine copula and bivariate copula models.
Advantages over VineCopula are  
  * a stand-alone C++ library with interfaces to both R and Python,
  * a sleaker and more modern API,
  * shorter runtimes and lower memory consumption, especially in high dimensions,
  * nonparametric and multiparameter families.

#### Status

Version [0.5.4](https://github.com/vinecopulib/vinecopulib/releases) was
released on September 30, 2020. While we did our best to
design a user-friendly API, the library is still under active development and
changes are to be expected. We are also working on interfaces for
[R](https://github.com/vinecopulib/rvinecopulib) and
[Python](https://github.com/vinecopulib/pyvinecopulib).

#### Contact

If you have any questions regarding the library, feel free to
[open an issue](https://github.com/vinecopulib/vinecopulib/issues/new) or
send a mail to <info@vinecopulib.org>.

#### Documentation

For documentation of the library's functionality and
instructions how to use it, check out our
[website](https://vinecopulib.github.io/vinecopulib/) or the `docs/` folder
in this repository.

#### Torch

First, follow the [libtorch installation instructions](https://pytorch.org/cppdocs/installing.html). Note that, in the `test/test_torch.cpp`, it is assumed that a GPU-enabled libtorch is installed, but this can be disabled by commenting lines 17-18.

Then, the following cmake options can be used:

* `TORCH`: allows to turn torch on (default is `OFF`).
* `TORCH_INSTALL_DIR`: can be used to specify the `/absolute/path/to/libtorch` as the absolute (!) path to the unzipped LibTorch distribution. If PyTorch was installed via conda or pip, this can be queried using something like `python -c 'import torch;print(torch.utils.cmake_prefix_path)'`.

<!-- cmake .. -DVINECOPULIB_ -->
<!-- SHARED_LIB=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_CXX_COMPILER=clang++ -DTORCH=ON -DTORCH_INST -->
<!-- ALL_DIR=../libtorch  && cp compile_commands.json ../. -->
<!--  -->
