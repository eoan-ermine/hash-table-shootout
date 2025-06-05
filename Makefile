CXX=clang++
CXX_FLAGS=-stdlib=libc++ -O3 -march=native -std=c++23

all: build/boost_flat_map build/boost_map build/boost_unordered_map build/std_flat_map build/std_map build/std_unordered_map build/etl_flat_map

build/boost_flat_map: src/boost_flat_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/boost_flat_map src/boost_flat_map.cc

build/boost_map: src/boost_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/boost_map src/boost_map.cc

build/boost_unordered_map: src/boost_unordered_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/boost_unordered_map src/boost_unordered_map.cc

build/std_flat_map: src/std_flat_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/std_flat_map src/std_flat_map.cc

build/std_map: src/std_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/std_map src/std_map.cc

build/std_unordered_map: src/std_unordered_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/std_unordered_map src/std_unordered_map.cc

build/loki_assoc_vector: src/loki_assoc_vector.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/loki_assoc_vector src/loki_assoc_vector.cc

build/etl_flat_map: src/etl_flat_map.cc src/template.c
	$(CXX) $(CXX_FLAGS) -lm -o build/etl_flat_map src/etl_flat_map.cc
