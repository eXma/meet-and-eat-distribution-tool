# Meet and eat - Team distribution tool

This should be a rewrite of the old python-only tool in c++ accelerated python.


## Requirements

To build and run this you need:

* cmake (tested with 2.8.12.2)
* gcc (tested with g++ 4.9.1)
* python3.4 (tested with 3.4.1)
* boost (tested with 1.55)
* boost.python (tested with 1.55)


## How to build and install:

    git clone https://github.com/janLo/meet-and-eat-distribution-tool.git
    cd meet-and-eat-distribution-tool
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_PATH> -DCMAKE_BUILD_TYPE=[Debug|Release] ..
    make all test install

