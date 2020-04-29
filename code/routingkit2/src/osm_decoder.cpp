#include "osm_decoder.h"
#include "protobuf_var_int.h"
#include "buffered_async_reader.h"
#include "data_source.h"
#include "protobuf.h"

#include <zlib.h>
#include <stdexcept>
#include <thread>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <atomic>
#include <string.h>
#include <assert.h>

static uint32_t ntohl(uint32_t netlong){
	return
		(netlong << 24) |
		((netlong << 16) >> 8) |
		((netlong << 8) >> 16) |
		(netlong >> 24);
}

// link with -lz

namespace RoutingKit2{

namespace{

	// formally
	//
	// uint8_t*p = ...;
	// uint32_t x = *((uint32_t*)p);
	//
	// is undefined behavior. We therefore use memcpy.

	template<class T>
	void unaligned_store(uint8_t*dest, const T&val){
		memcpy(dest, (const uint8_t*)&val, sizeof(T));
	}

	template<class T>
	T unaligned_load(const uint8_t*src){
		T ret;
		memcpy((uint8_t*)&ret, src, sizeof(T));
		return ret;
	}

	constexpr uint64_t is_header_info_available_bit = 1;
	constexpr uint64_t is_ordered_bit = 2;
	constexpr uint64_t was_blob_read_bit = 4;
	constexpr uint64_t was_header_read_bit = 8;

	constexpr uint32_t pbf_decompressor_read_size = 64 << 20;

	class OsmPBFDecompressor {
	public:
		OsmPBFDecompressor():status(0){}
		OsmPBFDecompressor(RefDataSource data_source):
			status(0),
			reader(data_source, pbf_decompressor_read_size){
		}

		uint64_t get_status() const {
			return status;
		}

		RefDataSource as_ref(){
			return [&](uint8_t*buffer, size_t how_much_to_read) -> size_t{
				assert(how_much_to_read == pbf_decompressor_read_size);

				const uint8_t*blob_begin, *blob_end;

				for(;;){
					uint8_t*p = reader.read(4);
					if(p == nullptr)
						return 0;

					uint32_t header_size = ntohl(unaligned_load<uint32_t>(p));

					uint32_t data_size = (uint32_t)-1;

					char block_type[16] = "";

					const uint8_t*buffer = reader.read_or_throw(header_size);
					decode_protobuf_message_with_callbacks(
						buffer, buffer+header_size,
						[&](uint64_t key_id, uint64_t num){
							if(key_id == 3)
								data_size = num;
						},
						[&](uint64_t key_id, double num){},
						[&](uint64_t key_id, const uint8_t*str_begin, const uint8_t*str_end){
							if(key_id == 1){
								size_t len = str_end - str_begin;
								if(len > sizeof(block_type)-1)
									len = sizeof(block_type)-1;
								memcpy(block_type, str_begin, len);
								block_type[len] = '\0';
							}
						}
					);

					if(data_size == (uint32_t)-1)
						throw std::runtime_error("Cannot parse OSM blob header because it is missing the data size");

					if(!strcmp(block_type, "OSMData")){
						status |= is_header_info_available_bit | was_blob_read_bit;

						blob_begin = reader.read_or_throw(data_size);
						blob_end = blob_begin + data_size;
						break;
					} else if(!strcmp(block_type, "OSMHeader")) {
						if((status & is_header_info_available_bit) != 0 && (status & was_blob_read_bit) != 0)
							throw std::runtime_error("OSM PBF file header block must preceed all blob blocks");
						if((status & is_header_info_available_bit) != 0 && (status & was_header_read_bit) != 0)
							throw std::runtime_error("OSM PBF file contains two header blocks");

						bool is_ordered = false;
						const uint8_t*buffer = reader.read_or_throw(data_size);
						decode_protobuf_message_with_callbacks(
							buffer, buffer+data_size,
							[&](uint64_t key_id, uint64_t num){},
							[&](uint64_t key_id, double num){},
							[&](uint64_t key_id, const uint8_t*blob_begin, const uint8_t*blob_end){
								const char*str_begin = (const char*)blob_begin;
								const char*str_end = (const char*)blob_end;

								if(key_id == 4){ // must support
									if(!std::equal(str_begin, str_end, "DenseNodes"))
										throw std::runtime_error("Required OSM PBF feature \""+std::string(str_begin, str_end)+"\" is unknown");
								}else if(key_id == 5){ // may exploit
									if(std::equal(str_begin, str_end, "Sort.Type_then_ID"))
										is_ordered = true;
								}
							}
						);
						if(is_ordered)
							status = is_header_info_available_bit | is_ordered_bit | was_header_read_bit;
						else
							status = is_header_info_available_bit | was_header_read_bit;
					} else {
						reader.read(data_size);
						continue;
					}
				}

				const uint8_t
					*uncompressed_begin = nullptr,
					*uncompressed_end = nullptr,
					*compressed_begin = nullptr,
					*compressed_end = nullptr;
				uint64_t uncompressed_data_size = (uint64_t)-1;

				decode_protobuf_message_with_callbacks(
					blob_begin, blob_end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 2)
							uncompressed_data_size = num;
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*str_begin, const uint8_t*str_end){
						if(key_id == 1){
							uncompressed_begin = str_begin;
							uncompressed_end = str_end;
						}else if(key_id == 3){
							compressed_begin = str_begin;
							compressed_end = str_end;
						}
					}
				);

				if(uncompressed_begin != nullptr && compressed_begin != nullptr)
					throw std::runtime_error("PBF error: Blob must not contain both compressed and uncompressed data");
				if(uncompressed_begin == nullptr && compressed_begin == nullptr)
					throw std::runtime_error("PBF error: Blob contains neither compressed nor uncompressed data");
				if(uncompressed_data_size == (uint64_t)-1)
					throw std::runtime_error("PBF error: Blob does not contain the size of the uncompressed data");
				if(uncompressed_begin){
					if(uncompressed_data_size > how_much_to_read-4)
						throw std::runtime_error("PBF error: Blob is too large. It is "+std::to_string(uncompressed_data_size) + " but may be at most "+std::to_string(how_much_to_read-4));
					if(uncompressed_data_size != (std::uint64_t)(uncompressed_end - uncompressed_begin))
						throw std::runtime_error("PBF error: claimed uncompressed blob size does not correspond to actual blob size");
					unaligned_store<uint32_t>(buffer, uncompressed_data_size);
					buffer += 4;
					memcpy(buffer, uncompressed_begin, uncompressed_data_size);
					return uncompressed_data_size + 4;
				}else{
					uint64_t compressed_data_size = compressed_end - compressed_begin;
					if(uncompressed_data_size > how_much_to_read-4)
						throw std::runtime_error("PBF error: Blob is too large. It is "+std::to_string(uncompressed_data_size) + " but may be at most "+std::to_string(how_much_to_read-4));

					unaligned_store<uint32_t>(buffer, uncompressed_data_size);
					buffer += 4;

					z_stream z;
					z.next_in   = (unsigned char*) compressed_begin;
					z.avail_in  = compressed_data_size;
					z.next_out  = (unsigned char*) buffer;
					z.avail_out = how_much_to_read-4;
					z.zalloc    = Z_NULL;
					z.zfree     = Z_NULL;
					z.opaque    = Z_NULL;

					if(inflateInit(&z) != Z_OK) {
						throw std::runtime_error("PBF error: Failed to initialize zlib stream.");
					}
					if(inflate(&z, Z_FINISH) != Z_STREAM_END) {
						throw std::runtime_error("PBF error: Failed to completely inflate zlib stream. Probably the OSM blob decompresses to something larger than reported in the header.");
					}
					if(inflateEnd(&z) != Z_OK) {
						throw std::runtime_error("PBF error: Failed to cleanup zlib stream.");
					}
					if(z.total_out != uncompressed_data_size) {
						throw std::runtime_error("PBF error: OSM blob decompresses to fewer bytes than reported in the header.");
					}
					return uncompressed_data_size + 4;
				}
			};
		}
	private:
		uint64_t status;
		BufferedAsyncReader reader;
	};
}

namespace {
	void internal_read_osm_pbf(
		BufferedAsyncReader&reader,
		std::function<void(uint64_t osm_node_id, LatLon p, const TagMap&tags)>node_callback,
		std::function<void(uint64_t osm_way_id, const std::vector<std::uint64_t>&osm_node_id_list, const TagMap&tags)>way_callback,
		std::function<void(uint64_t osm_relation_id, const std::vector<OSMRelationMember>&member_list, const TagMap&tags)>relation_callback,
		std::function<void(std::string msg)>log_message
	){
		TagMap tag_map;
		std::vector<OSMRelationMember>member_list;
		std::vector<uint64_t>node_list;

		std::vector<const char*>string_table;
		std::vector<uint32_t>key_list;
		std::vector<uint32_t>value_list;

		std::vector<std::pair<const uint8_t*, const uint8_t*>>group_list;

		for(;;){
			uint8_t*primblock_begin, *primblock_end;
			{
				uint8_t*s_ptr = reader.read(4);
				if(s_ptr == nullptr)
					break;
				uint32_t s = unaligned_load<uint32_t>(s_ptr);
				primblock_begin = reader.read_or_throw(s);
				primblock_end = primblock_begin + s;
			}

			string_table.clear();
			group_list.clear();

			uint64_t latlon_granularity = 100;
			int64_t offset_of_latitude = 0;
			int64_t offset_of_longitude = 0;

			decode_protobuf_message_with_callbacks(
				primblock_begin, primblock_end,
				[&](uint64_t key_id, uint64_t num){
					if(key_id == 17)
						latlon_granularity = num;
					else if(key_id == 19)
						offset_of_latitude = zigzag_convert_uint64_to_int64(num);
					else if(key_id == 20)
						offset_of_longitude = zigzag_convert_uint64_to_int64(num);

				},
				[&](uint64_t key_id, double num){},
				[&](uint64_t key_id, uint8_t*outter_blob_begin, uint8_t*outter_blob_end){
					if(key_id == 1){
						decode_protobuf_message_with_callbacks(
							outter_blob_begin, outter_blob_end,
							[&](uint64_t key_id, uint64_t num){},
							[&](uint64_t key_id, double num){},
							[&](uint64_t key_id, uint8_t*inner_blob_begin, uint8_t*inner_blob_end){
								// This is safe, see description of decode_protobuf_message_with_callbacks
								// for why.
								char*str_begin = (char*)inner_blob_begin-1;
								size_t len = inner_blob_end - inner_blob_begin;
								memmove(str_begin, inner_blob_begin, len);
								str_begin[len] = '\0';
								string_table.push_back(str_begin);
							}
						);
					}else if(key_id == 2){
						group_list.push_back({outter_blob_begin, outter_blob_end});
					}
				}
			);

			if(latlon_granularity == 0)
				throw std::runtime_error("PBF error: latlon_granularity of a block must not be zero.");

			double primblock_lon_offset = 0.000000001 * offset_of_latitude;
			double primblock_lat_offset = 0.000000001 * offset_of_longitude;
			double primblock_granularity = 0.000000001 * latlon_granularity;

			auto decode_sparse_node = [&](const uint8_t*begin, const uint8_t*end){
				uint64_t osm_node_id = (uint64_t)-1;
				double latitude = 0.0, longitude = 0.0;

				const uint8_t
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1)
							osm_node_id = num;
						else if(key_id == 19)
							latitude = primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(num);
						else if(key_id == 20)
							longitude = primblock_lat_offset + primblock_granularity * zigzag_convert_uint64_to_int64(num);
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*begin, const uint8_t*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}
					}
				);

				if(osm_node_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_var_uint_from_front_and_advance(key_begin, key_end));
					value_list.push_back(decode_var_uint_from_front_and_advance(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				// FIXME: eliminate doubles
				node_callback(osm_node_id, LatLon::from_lat_lon(latitude, longitude), tag_map);
			};

			auto decode_dense_node = [&](const uint8_t*begin, const uint8_t*end){
				const uint8_t
					*osm_node_id_begin = nullptr, *osm_node_id_end = nullptr,
					*key_value_pairs_begin = nullptr, *key_value_pairs_end = nullptr,
					*latitude_begin = nullptr, *latitude_end = nullptr,
					*longitude_begin = nullptr, *longitude_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*begin, const uint8_t*end){
						if(key_id == 1){
							osm_node_id_begin = begin;
							osm_node_id_end = end;
						}else if(key_id == 8){
							latitude_begin = begin;
							latitude_end = end;
						}else if(key_id == 9){
							longitude_begin = begin;
							longitude_end = end;
						}else if(key_id == 10){
							key_value_pairs_begin = begin;
							key_value_pairs_end = end;
						}
					}
				);

				if(osm_node_id_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain node IDs.");
				if(latitude_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain latitudes.");
				if(longitude_begin == nullptr)
					throw std::runtime_error("PBF error: dense node must contain longitudes.");

				tag_map.clear();

				uint64_t osm_node_id = 0;
				double latitude = 0.0, longitude = 0.0;
				while(osm_node_id_begin != osm_node_id_end){
					osm_node_id += zigzag_convert_uint64_to_int64(decode_var_uint_from_front_and_advance(osm_node_id_begin, osm_node_id_end));
					latitude += primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(decode_var_uint_from_front_and_advance(latitude_begin, latitude_end));
					longitude += primblock_lon_offset + primblock_granularity * zigzag_convert_uint64_to_int64(decode_var_uint_from_front_and_advance(longitude_begin, longitude_end));
					if(key_value_pairs_begin != nullptr){
						key_list.clear();
						value_list.clear();
						for(;;){
							uint64_t x = decode_var_uint_from_front_and_advance(key_value_pairs_begin, key_value_pairs_end);
							if(x == 0)
								break;
							uint64_t y = decode_var_uint_from_front_and_advance(key_value_pairs_begin, key_value_pairs_end);
							key_list.push_back(x);
							value_list.push_back(y);
						}
						tag_map.build(
							key_list.size(),
							[&](uint64_t i){
								i = key_list[i];
								if(i > string_table.size())
									throw std::runtime_error("PBF error: key string ID is out of bounds.");
								return string_table[i];
							},
							[&](uint64_t i){
								i = value_list[i];
								if(i > string_table.size())
									throw std::runtime_error("PBF error: value string ID is out of bounds.");
								return string_table[i];
							}
						);
					}


					// FIXME: eliminate doubles
					node_callback(osm_node_id, LatLon::from_lat_lon(latitude, longitude), tag_map);
				}
				if(latitude_begin != latitude_end)
					throw std::runtime_error("PBF error: dense node latitude array has a different length than the node ID array.");
				if(longitude_begin != longitude_end)
					throw std::runtime_error("PBF error: dense node longitude array has a different length than the node ID array.");
				if(key_value_pairs_begin != key_value_pairs_end)
					throw std::runtime_error("PBF error: dense node key-value array is too long.");


			};

			auto decode_way = [&](const uint8_t*begin, const uint8_t*end){
				uint64_t osm_way_id = (uint64_t)-1;

				const uint8_t
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr,
					*node_list_begin = nullptr, *node_list_end = nullptr;

				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1){
							osm_way_id = num;
						}
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*begin, const uint8_t*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}else if(key_id == 8){
							node_list_begin = begin;
							node_list_end = end;
						}
					}
				);

				if(osm_way_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_var_uint_from_front_and_advance(key_begin, key_end));
					value_list.push_back(decode_var_uint_from_front_and_advance(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				node_list.clear();
				uint64_t id = 0;

				while(node_list_begin != node_list_end){
					id += zigzag_convert_uint64_to_int64(decode_var_uint_from_front_and_advance(node_list_begin, node_list_end));
					node_list.push_back(id);
				}

				if(node_list.size() == 0)
					log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has no nodes; ignoring way");
				else if(node_list.size() == 1)
					log_message("Warning: OSM way "+std::to_string(osm_way_id)+" has only one node; ignoring way");
				else
					way_callback(osm_way_id, node_list, tag_map);
			};

			auto decode_relation = [&](const uint8_t*begin, const uint8_t*end){
				uint64_t osm_relation_id = (uint64_t)-1;

				const uint8_t
					*key_begin = nullptr, *key_end = nullptr,
					*value_begin = nullptr, *value_end = nullptr,
					*member_role_begin = nullptr, *member_role_end = nullptr,
					*member_id_begin = nullptr, *member_id_end = nullptr,
					*member_type_begin = nullptr, *member_type_end = nullptr;


				decode_protobuf_message_with_callbacks(
					begin, end,
					[&](uint64_t key_id, uint64_t num){
						if(key_id == 1){
							osm_relation_id = num;
						}
					},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*begin, const uint8_t*end){
						if(key_id == 2){
							key_begin = begin;
							key_end = end;
						}else if(key_id == 3){
							value_begin = begin;
							value_end = end;
						}else if(key_id == 8){
							member_role_begin = begin;
							member_role_end = end;
						}else if(key_id == 9){
							member_id_begin = begin;
							member_id_end = end;
						}else if(key_id == 10){
							member_type_begin = begin;
							member_type_end = end;
						}
					}
				);

				if(osm_relation_id == (uint64_t)-1)
					throw std::runtime_error("PBF error: way is missing its OSM ID.");

				key_list.clear();
				value_list.clear();
				while(key_begin != key_end && value_begin != value_end){
					key_list.push_back(decode_var_uint_from_front_and_advance(key_begin, key_end));
					value_list.push_back(decode_var_uint_from_front_and_advance(value_begin, value_end));
				}
				if(key_begin != key_end || value_begin != value_end)
					throw std::runtime_error("PBF error: key and value arrays do not decode to equal length.");

				tag_map.build(
					key_list.size(),
					[&](uint64_t i){
						i = key_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: key string ID is out of bounds.");
						return string_table[i];
					},
					[&](uint64_t i){
						i = value_list[i];
						if(i > string_table.size())
							throw std::runtime_error("PBF error: value string ID is out of bounds.");
						return string_table[i];
					}
				);

				member_list.clear();

				uint64_t member_id = 0;
				while(member_id_begin != member_id_end){
					member_id += zigzag_convert_uint64_to_int64(decode_var_uint_from_front_and_advance(member_id_begin, member_id_end));

					auto x = decode_var_uint_from_front_and_advance(member_role_begin, member_role_end);
					if(x >= string_table.size())
						throw std::runtime_error("PBF error: relation member role string ID is out of bounds.");
					const char*role = string_table[x];
					uint64_t type_id = decode_var_uint_from_front_and_advance(member_type_begin, member_type_end);
					OSMIDType member_type;
					if(type_id == 0)
						member_type = OSMIDType::node;
					else if(type_id == 1)
						member_type = OSMIDType::way;
					else if(type_id == 2)
						member_type = OSMIDType::relation;
					else
						throw std::runtime_error("PBF error: Unknown relation type.");
					member_list.push_back({member_type, member_id, role});
				}
				relation_callback(osm_relation_id, member_list, tag_map);
			};

			for(auto g:group_list){
				decode_protobuf_message_with_callbacks(
					g.first, g.second,
					[&](uint64_t key_id, uint64_t num){},
					[&](uint64_t key_id, double num){},
					[&](uint64_t key_id, const uint8_t*blob_begin, const uint8_t*blob_end){
						if(key_id == 1 && node_callback) {
							decode_sparse_node(blob_begin, blob_end);
						} else if(key_id == 2 && node_callback) {
							decode_dense_node(blob_begin, blob_end);
						} else if(key_id == 3 && way_callback) {
							decode_way(blob_begin, blob_end);
						} else if(key_id == 4 && relation_callback) {
							decode_relation(blob_begin, blob_end);
						}
					}
				);
			}
		}
	}
}

void unordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, LatLon p, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, Span<const uint64_t>osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, Span<const OSMRelationMember>member_list, const TagMap&tags)>relation_callback,
	std::function<void(std::string msg)>log_message
){
	assert(node_callback || way_callback || relation_callback);

	FileDataSource data_source(file_name);
	OsmPBFDecompressor decompressor(data_source.as_ref());
	BufferedAsyncReader reader(decompressor.as_ref(), pbf_decompressor_read_size);
	internal_read_osm_pbf(reader, node_callback, way_callback, relation_callback, log_message);
}

void ordered_read_osm_pbf(
	const std::string&file_name,
	std::function<void(uint64_t osm_node_id, LatLon p, const TagMap&tags)>node_callback,
	std::function<void(uint64_t osm_way_id, Span<const uint64_t>osm_node_id_list, const TagMap&tags)>way_callback,
	std::function<void(uint64_t osm_relation_id, Span<const OSMRelationMember>member_list, const TagMap&tags)>relation_callback,
	std::function<void(std::string msg)>log_message,
	bool file_is_ordered_even_though_file_header_says_that_it_is_unordered
){
	assert(node_callback || way_callback || relation_callback);

	FileDataSource data_source(file_name);
	OsmPBFDecompressor decompressor(data_source.as_ref());
	BufferedAsyncReader reader(decompressor.as_ref(), pbf_decompressor_read_size);

	if(!file_is_ordered_even_though_file_header_says_that_it_is_unordered){
		while((decompressor.get_status() & is_header_info_available_bit) == 0){
			std::atomic_thread_fence(std::memory_order::memory_order_seq_cst);
			std::this_thread::yield();
		}
	}

	if(file_is_ordered_even_though_file_header_says_that_it_is_unordered || (decompressor.get_status() & is_ordered_bit) != 0){
		internal_read_osm_pbf(reader, node_callback, way_callback, relation_callback, log_message);
	} else {
		if(node_callback){
			internal_read_osm_pbf(reader, node_callback, nullptr, nullptr, log_message);
			if(relation_callback || way_callback){
				reader = BufferedAsyncReader();
				decompressor = OsmPBFDecompressor();
				data_source.rewind();
				decompressor = OsmPBFDecompressor(data_source.as_ref());
				reader = BufferedAsyncReader(decompressor.as_ref(), pbf_decompressor_read_size);
			}
		}

		if(way_callback){
			internal_read_osm_pbf(reader, nullptr, way_callback, nullptr, log_message);
			if(relation_callback){
				reader = BufferedAsyncReader();
				decompressor = OsmPBFDecompressor();
				data_source.rewind();
				decompressor = OsmPBFDecompressor(data_source.as_ref());
				reader = BufferedAsyncReader(decompressor.as_ref(), pbf_decompressor_read_size);
			}
		}

		if(relation_callback){
			internal_read_osm_pbf(reader, nullptr, nullptr, relation_callback, log_message);
		}
	}
}

} // RoutingKit2
