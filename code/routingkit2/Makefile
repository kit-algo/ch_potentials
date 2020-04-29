# This makefile was automatically generated. Run ./generate_make_file to regenerate the file.
CC=g++
AR=ar
CFLAGS=-Wall -O3 -DNDEBUG -march=native -std=c++11 -fPIC -Iinclude
LDFLAGS=-fsanitize=address

all: bin/geo_index_test_server bin/run_osm_import bin/run_tests

build/test_bit_select.o: src/bit_select.h src/catch.hpp src/test_bit_select.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_bit_select.cpp -o build/test_bit_select.o

build/test_polyline.o: src/catch.hpp src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_pos.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h src/test_polyline.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_polyline.cpp -o build/test_polyline.o

build/osm_import.o: src/data_sink.h src/data_source.h src/dir.h src/enumerator.h src/file_array.h src/geo_pos.h src/gpoly.h src/inverse_func.h src/map.h src/map_schema.h src/min_max.h src/optional.h src/osm_decoder.h src/osm_import.cpp src/osm_import.h src/osm_profile.h src/osm_types.h src/permutation.h src/polyline.h src/prefix_sum.h src/protobuf_var_int.h src/sort.h src/span.h src/str.h src/tag_map.h src/turn.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_import.cpp -o build/osm_import.o

build/buffered_async_reader.o: src/buffered_async_reader.cpp src/buffered_async_reader.h src/data_source.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/buffered_async_reader.cpp -o build/buffered_async_reader.o

build/test_map.o: src/catch.hpp src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_pos.h src/map.h src/map_schema.h src/span.h src/test_map.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_map.cpp -o build/test_map.o

build/test_tag_map.o: src/catch.hpp src/tag_map.h src/test_tag_map.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_tag_map.cpp -o build/test_tag_map.o

build/geo_index_test_server.o: src/data_sink.h src/data_source.h src/dir.h src/enumerator.h src/file_array.h src/geo_index.h src/geo_index_schema.h src/geo_index_test_server.cpp src/geo_pos.h src/gpoly.h src/http_server.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h src/timestamp_flags.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/geo_index_test_server.cpp -o build/geo_index_test_server.o

build/bit_select.o: src/bit_select.cpp src/bit_select.h src/emulate_gcc_builtin.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/bit_select.cpp -o build/bit_select.o

build/osm_decoder.o: src/buffered_async_reader.h src/data_source.h src/geo_pos.h src/osm_decoder.cpp src/osm_decoder.h src/osm_types.h src/protobuf.h src/protobuf_var_int.h src/span.h src/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_decoder.cpp -o build/osm_decoder.o

build/id_mapper.o: src/bit_select.h src/emulate_gcc_builtin.h src/id_mapper.cpp src/id_mapper.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/id_mapper.cpp -o build/id_mapper.o

build/run_tests.o: src/catch.hpp src/run_tests.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/run_tests.cpp -o build/run_tests.o

build/test_inverse_func.o: src/catch.hpp src/inverse_func.h src/min_max.h src/permutation.h src/sort.h src/span.h src/test_inverse_func.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_inverse_func.cpp -o build/test_inverse_func.o

build/str.o: src/str.cpp src/str.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/str.cpp -o build/str.o

build/test_permutation.o: src/catch.hpp src/permutation.h src/span.h src/test_permutation.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_permutation.cpp -o build/test_permutation.o

build/osm_profile.o: src/geo_pos.h src/min_max.h src/osm_profile.cpp src/osm_profile.h src/osm_types.h src/span.h src/str.h src/tag_map.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/osm_profile.cpp -o build/osm_profile.o

build/turn.o: src/data_sink.h src/data_source.h src/dir.h src/enumerator.h src/file_array.h src/geo_pos.h src/gpoly.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h src/turn.cpp src/turn.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/turn.cpp -o build/turn.o

build/test_buffered_async_reader.o: src/buffered_async_reader.h src/catch.hpp src/data_source.h src/test_buffered_async_reader.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_buffered_async_reader.cpp -o build/test_buffered_async_reader.o

build/map.o: src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_pos.h src/map.cpp src/map.h src/map_schema.h src/optional.h src/polyline.h src/prefix_sum.h src/protobuf_var_int.h src/span.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/map.cpp -o build/map.o

build/test_osm_profile.o: src/catch.hpp src/geo_pos.h src/osm_profile.h src/osm_types.h src/tag_map.h src/test_osm_profile.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_osm_profile.cpp -o build/test_osm_profile.o

build/protobuf_var_int.o: src/protobuf_var_int.cpp src/protobuf_var_int.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/protobuf_var_int.cpp -o build/protobuf_var_int.o

build/geo_pos.o: src/geo_pos.cpp src/geo_pos.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/geo_pos.cpp -o build/geo_pos.o

build/test_sort.o: src/catch.hpp src/permutation.h src/sort.h src/span.h src/test_sort.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_sort.cpp -o build/test_sort.o

build/test_gpoly.o: src/catch.hpp src/data_sink.h src/data_source.h src/dir.h src/enumerator.h src/file_array.h src/geo_pos.h src/gpoly.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h src/test_gpoly.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_gpoly.cpp -o build/test_gpoly.o

build/http_server.o: src/http_server.cpp src/http_server.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/http_server.cpp -o build/http_server.o

build/bit_vector.o: src/bit_vector.cpp src/bit_vector.h src/emulate_gcc_builtin.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/bit_vector.cpp -o build/bit_vector.o

build/test_id_mapper.o: src/bit_vector.h src/catch.hpp src/id_mapper.h src/test_id_mapper.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_id_mapper.cpp -o build/test_id_mapper.o

build/geo_index.o: src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_index.cpp src/geo_index.h src/geo_index_schema.h src/geo_pos.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h src/timestamp_flags.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/geo_index.cpp -o build/geo_index.o

build/gpoly.o: src/data_sink.h src/data_source.h src/dir.h src/enumerator.h src/file_array.h src/geo_pos.h src/gpoly.cpp src/gpoly.h src/map.h src/map_schema.h src/optional.h src/polyline.h src/protobuf_var_int.h src/span.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/gpoly.cpp -o build/gpoly.o

build/test_bit_vector.o: src/bit_vector.h src/catch.hpp src/test_bit_vector.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_bit_vector.cpp -o build/test_bit_vector.o

build/run_osm_import.o: src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_pos.h src/map.h src/map_schema.h src/osm_import.h src/run_osm_import.cpp src/span.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/run_osm_import.cpp -o build/run_osm_import.o

build/test_geo_index.o: src/catch.hpp src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_index.h src/geo_index_schema.h src/geo_pos.h src/map_schema.h src/optional.h src/span.h src/test_geo_index.cpp src/timestamp_flags.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_geo_index.cpp -o build/test_geo_index.o

build/test_turn.o: src/catch.hpp src/data_sink.h src/data_source.h src/dir.h src/file_array.h src/geo_pos.h src/map.h src/map_schema.h src/span.h src/test_turn.cpp src/turn.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_turn.cpp -o build/test_turn.o

build/data_sink.o: src/data_sink.cpp src/data_sink.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/data_sink.cpp -o build/data_sink.o

build/test_geo_pos.o: src/catch.hpp src/geo_pos.h src/test_geo_pos.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_geo_pos.cpp -o build/test_geo_pos.o

build/data_source.o: src/data_source.cpp src/data_source.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/data_source.cpp -o build/data_source.o

build/test_protobuf_var_int.o: src/catch.hpp src/protobuf_var_int.h src/test_protobuf_var_int.cpp generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/test_protobuf_var_int.cpp -o build/test_protobuf_var_int.o

build/file_array.o: src/file_array.cpp src/file_array.h generate_make_file
	mkdir -p build
	$(CC) $(CFLAGS)  -c src/file_array.cpp -o build/file_array.o

bin/geo_index_test_server: build/data_sink.o build/data_source.o build/file_array.o build/geo_index.o build/geo_index_test_server.o build/geo_pos.o build/gpoly.o build/http_server.o build/map.o build/protobuf_var_int.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/data_sink.o build/data_source.o build/file_array.o build/geo_index.o build/geo_index_test_server.o build/geo_pos.o build/gpoly.o build/http_server.o build/map.o build/protobuf_var_int.o -lm -pthread  -o bin/geo_index_test_server

bin/run_osm_import: build/buffered_async_reader.o build/data_sink.o build/data_source.o build/file_array.o build/geo_pos.o build/gpoly.o build/map.o build/osm_decoder.o build/osm_import.o build/osm_profile.o build/protobuf_var_int.o build/run_osm_import.o build/str.o build/turn.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/buffered_async_reader.o build/data_sink.o build/data_source.o build/file_array.o build/geo_pos.o build/gpoly.o build/map.o build/osm_decoder.o build/osm_import.o build/osm_profile.o build/protobuf_var_int.o build/run_osm_import.o build/str.o build/turn.o -lm -lz -pthread  -o bin/run_osm_import

bin/run_tests: build/bit_select.o build/bit_vector.o build/buffered_async_reader.o build/data_sink.o build/data_source.o build/file_array.o build/geo_index.o build/geo_pos.o build/gpoly.o build/id_mapper.o build/map.o build/osm_profile.o build/protobuf_var_int.o build/run_tests.o build/str.o build/test_bit_select.o build/test_bit_vector.o build/test_buffered_async_reader.o build/test_geo_index.o build/test_geo_pos.o build/test_gpoly.o build/test_id_mapper.o build/test_inverse_func.o build/test_map.o build/test_osm_profile.o build/test_permutation.o build/test_polyline.o build/test_protobuf_var_int.o build/test_sort.o build/test_tag_map.o build/test_turn.o build/turn.o
	mkdir -p bin
	$(CC) $(LDFLAGS) build/bit_select.o build/bit_vector.o build/buffered_async_reader.o build/data_sink.o build/data_source.o build/file_array.o build/geo_index.o build/geo_pos.o build/gpoly.o build/id_mapper.o build/map.o build/osm_profile.o build/protobuf_var_int.o build/run_tests.o build/str.o build/test_bit_select.o build/test_bit_vector.o build/test_buffered_async_reader.o build/test_geo_index.o build/test_geo_pos.o build/test_gpoly.o build/test_id_mapper.o build/test_inverse_func.o build/test_map.o build/test_osm_profile.o build/test_permutation.o build/test_polyline.o build/test_protobuf_var_int.o build/test_sort.o build/test_tag_map.o build/test_turn.o build/turn.o -lm -pthread  -o bin/run_tests

