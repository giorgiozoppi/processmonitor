# Process Monitor

This is a simple exercise to test my C++17 knowledge. Exercise done during the [Udacity C++ Nanodegree] (https://d20vrrgs8k4bvw.cloudfront.net/documents/en-US/C%2B%2B+Nanodegree+Program+Syllabus.pdf)

![System Monitor](images/monitor.png)

### Dependencies
This project requires g++-11. It has been tested also with g++9 and clang-14.

1. `sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test`	
2. `sudo apt install -y gcc-11`

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts

## Instructions

1. Clone the project repository: `git clone https://github.com/giorgiozoppi/processmonitor`

2. Build the project: `make build`
