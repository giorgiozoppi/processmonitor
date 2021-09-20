.PHONY: all
all: format test build

.PHONY: format
format:
	clang-format src/*.cpp include/*.h -i

.PHONY: build
build:
	mkdir -p build
	cd build && \
	cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ .. && \
	make


.PHONY: debug   
debug:
	mkdir -p build
	cd build  \
	cmake -DCMAKE_CXX_COMPILER=/usr/bin/g++ .. && \
	make

.PHONY: clean
clean:
	rm -rf build
