#ifndef ROUTING_KIT2_TAG_MAP_H
#define ROUTING_KIT2_TAG_MAP_H

#include <algorithm>
#include <vector>
#include <string.h>
#include <cstdint>

namespace RoutingKit2{

//! A hashmap for OSM tags.
//! The hashmap references the key-value pairs of strings from which the TagMap is constructed.
//! It is the responsability of the caller to make sure that the TagMap outlives these strings.
class TagMap{
private:

	static constexpr unsigned char_hash_count = 16;

	// computes a 4-bit hash value between 0 and char_hash_count-1
	static uint32_t compute_char_hash(char c){
		switch(c){
			// make sure that common chars are mapped onto different hashes
			case 'e': case 'E': return 7;
			case 't': case 'T': return 8;
			case 'a': case 'A': return 9;
			case 'o': case 'O': return 10;
			case 'i': case 'I': return 11;
			case 'n': case 'N': return 12;
			case 's': case 'S': return 13;
			case 'h': case 'H': return 14;
			case 'r': case 'R': return 15;
			// we do not care about the less frequent chars. Map them somehow onto the remaining 6 values
			default:return static_cast<uint32_t>(c)%7;
		}
	}

	// Computes a 12-bit hash value for a string. This is a value between 0 and hash_count-1
	static uint32_t compute_hash(const char*str){
		uint32_t last_char = strlen(str);
		if(last_char == 0)
			return 0;
		else
			--last_char;

		// for efficiency reasons we only hash the first, the last, and the middle chars.

		return
			((
				((
					compute_char_hash(str[0])
				)<<4) | compute_char_hash(str[last_char])
			)<<4) | compute_char_hash(str[last_char/2])
		;
	}

	static constexpr unsigned hash_count = char_hash_count*char_hash_count*char_hash_count;

public:
	TagMap():entry_begin(hash_count, 0), entry_end(hash_count, 0){}

	void clear(){
		for(auto x:hash_element_list){
			entry_begin[x>>16] = 0;
			entry_end[x>>16] = 0;
		}
	}

	template<class GetKey, class GetValue>
	void build(uint32_t key_value_count, const GetKey&get_key, const GetValue&get_value){

		clear();

		// hash_element_list contains pairs of hash_value and element index in one uint32_t
		// the lower 16 bits contain the index and the higher 16 bits the hash
		hash_element_list.resize(key_value_count);
		for(uint32_t i=0; i<key_value_count; ++i)
			hash_element_list[i] = (compute_hash(get_key(i))<<16) | i;

		// sort first by hash and then by index
		std::sort(hash_element_list.begin(), hash_element_list.end());

		entry.resize(key_value_count);
		uint32_t prev_hash = hash_count;
		for(uint32_t i=0; i<key_value_count; ++i){
			uint32_t hash = hash_element_list[i] >> 16;
			uint32_t j = hash_element_list[i] & 0xFFFFu;
			entry[i].key = get_key(j);
			entry[i].value = get_value(j);

			uint32_t current_hash = hash;
			if(current_hash != prev_hash)
				entry_begin[current_hash] = i;
			entry_end[current_hash] = i+1;
			prev_hash = current_hash;
		}
	}

	const char*operator[](const char*key) const {
		unsigned hash = compute_hash(key);

		for(unsigned i=entry_begin[hash]; i<entry_end[hash]; ++i)
			if(!strcmp(key, entry[i].key)){
				return entry[i].value;
			}
		return nullptr;
	}

	bool empty() const {
		return entry.empty();
	}

	unsigned size() const {
		return entry.size();
	}

	struct Entry{
		const char*key;
		const char*value;
	};

	std::vector<Entry>::const_iterator begin()const{
		return entry.begin();
	}

	std::vector<Entry>::const_iterator end()const{
		return entry.end();
	}

private:
	std::vector<uint32_t>hash_element_list;
	std::vector<Entry>entry;
	std::vector<unsigned>entry_begin, entry_end;
};

} // RoutingKit2

#endif
