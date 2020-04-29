#ifndef ROUTING_KIT2_GEO_INDEX_SCHEMA_H
#define ROUTING_KIT2_GEO_INDEX_SCHEMA_H

#include "span.h"
#include "dir.h"
#include "file_array.h"
#include "data_sink.h"
#include "data_source.h"
#include "geo_pos.h"
#include <vector>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

namespace RoutingKit2 {


struct RefGeoIndex;
struct ConstRefGeoIndex;

struct RefGeoIndex{
	size_t item_count;
	GeoPos* __restrict__ pos; // size = item_count
	uint32_t* __restrict__ id; // size = item_count

	Span<GeoPos> pos_as_ref()noexcept;
	Span<uint32_t> id_as_ref()noexcept;
	Span<const GeoPos> pos_as_ref()const noexcept;
	Span<const uint32_t> id_as_ref()const noexcept;
	Span<const GeoPos> pos_as_cref()const noexcept;
	Span<const uint32_t> id_as_cref()const noexcept;

	RefGeoIndex();
	RefGeoIndex(
		size_t item_count,
		GeoPos* __restrict__ pos,
		uint32_t* __restrict__ id
	);
};

struct ConstRefGeoIndex{
	size_t item_count;
	const GeoPos* __restrict__ pos; // size = item_count
	const uint32_t* __restrict__ id; // size = item_count

	Span<const GeoPos> pos_as_ref()const noexcept;
	Span<const uint32_t> id_as_ref()const noexcept;
	Span<const GeoPos> pos_as_cref()const noexcept;
	Span<const uint32_t> id_as_cref()const noexcept;

	ConstRefGeoIndex();
	ConstRefGeoIndex(RefGeoIndex);
	ConstRefGeoIndex(
		size_t item_count,
		const GeoPos* __restrict__ pos,
		const uint32_t* __restrict__ id
	);
};

struct VecGeoIndex{
	size_t item_count;
	std::vector<GeoPos>pos;
	std::vector<uint32_t>id;

	VecGeoIndex();
	explicit VecGeoIndex(
		size_t item_count
	);
	void resize(
		size_t item_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefGeoIndex as_ref()const noexcept;
	RefGeoIndex as_ref()noexcept;
	ConstRefGeoIndex as_cref()const noexcept;

	Span<GeoPos> pos_as_ref()noexcept;
	Span<uint32_t> id_as_ref()noexcept;
	Span<const GeoPos> pos_as_ref()const noexcept;
	Span<const uint32_t> id_as_ref()const noexcept;
	Span<const GeoPos> pos_as_cref()const noexcept;
	Span<const uint32_t> id_as_cref()const noexcept;
};

struct DirGeoIndex{
	size_t item_count;
	FileArray<GeoPos>pos;
	FileArray<uint32_t>id;

	DirGeoIndex();
	explicit DirGeoIndex(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefGeoIndex as_cref()const noexcept;
	ConstRefGeoIndex as_ref()const noexcept;

	Span<const GeoPos> pos_as_ref()const noexcept;
	Span<const uint32_t> id_as_ref()const noexcept;
	Span<const GeoPos> pos_as_cref()const noexcept;
	Span<const uint32_t> id_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefGeoIndex data);
inline void dump_into_dir(std::string dir, ConstRefGeoIndex data);

inline Span<GeoPos> RefGeoIndex::pos_as_ref()noexcept{
	return {pos, pos+item_count};
}

inline Span<uint32_t> RefGeoIndex::id_as_ref()noexcept{
	return {id, id+item_count};
}

inline Span<const GeoPos> RefGeoIndex::pos_as_ref()const noexcept{
	return pos_as_cref();
}

inline Span<const uint32_t> RefGeoIndex::id_as_ref()const noexcept{
	return id_as_cref();
}

inline Span<const GeoPos> RefGeoIndex::pos_as_cref()const noexcept{
	return {pos, pos+item_count};
}

inline Span<const uint32_t> RefGeoIndex::id_as_cref()const noexcept{
	return {id, id+item_count};
}


inline RefGeoIndex::RefGeoIndex(){}

inline RefGeoIndex::RefGeoIndex(
	size_t item_count,
	GeoPos* __restrict__ pos,
	uint32_t* __restrict__ id
):
	item_count(item_count),
	pos(pos),
	id(id){}

inline Span<const GeoPos> ConstRefGeoIndex::pos_as_ref()const noexcept{
	return pos_as_cref();
}

inline Span<const uint32_t> ConstRefGeoIndex::id_as_ref()const noexcept{
	return id_as_cref();
}

inline Span<const GeoPos> ConstRefGeoIndex::pos_as_cref()const noexcept{
	return {pos, pos+item_count};
}

inline Span<const uint32_t> ConstRefGeoIndex::id_as_cref()const noexcept{
	return {id, id+item_count};
}


inline ConstRefGeoIndex::ConstRefGeoIndex() {}

inline ConstRefGeoIndex::ConstRefGeoIndex(RefGeoIndex o):
	item_count(o.item_count),
	pos(o.pos),
	id(o.id){}

inline ConstRefGeoIndex::ConstRefGeoIndex(
	size_t item_count,
	const GeoPos* __restrict__ pos,
	const uint32_t* __restrict__ id
):
	item_count(item_count),
	pos(pos),
	id(id){}

inline Span<GeoPos> VecGeoIndex::pos_as_ref()noexcept{
	return {&pos[0], &pos[0]+pos.size()};
}

inline Span<uint32_t> VecGeoIndex::id_as_ref()noexcept{
	return {&id[0], &id[0]+id.size()};
}

inline Span<const GeoPos> VecGeoIndex::pos_as_ref()const noexcept{
	return pos_as_cref();
}

inline Span<const uint32_t> VecGeoIndex::id_as_ref()const noexcept{
	return id_as_cref();
}

inline Span<const GeoPos> VecGeoIndex::pos_as_cref()const noexcept{
	return {pos.data(), pos.data()+pos.size()};
}

inline Span<const uint32_t> VecGeoIndex::id_as_cref()const noexcept{
	return {id.data(), id.data()+id.size()};
}


inline VecGeoIndex::VecGeoIndex(){}

inline VecGeoIndex::VecGeoIndex(
	size_t item_count
):
	item_count(item_count),
	pos(item_count),
	id(item_count){}

inline void VecGeoIndex::resize(
	size_t item_count
){
	this->item_count = item_count;
	pos.resize(item_count);
	id.resize(item_count);
}

inline void VecGeoIndex::throw_if_wrong_size()const{
	std::string err;
	if(pos.size() != item_count)
		 err += ("Column pos has wrong size. Expected: "+std::to_string(item_count)+" Actual: "+std::to_string(pos.size())+"\n");
	if(id.size() != item_count)
		 err += ("Column id has wrong size. Expected: "+std::to_string(item_count)+" Actual: "+std::to_string(id.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecGeoIndex::assert_correct_size()const noexcept{
	assert(pos.size() == item_count);
	assert(id.size() == item_count);
}

inline void VecGeoIndex::shrink_to_fit(){
	pos.shrink_to_fit();
	id.shrink_to_fit();
}

inline ConstRefGeoIndex VecGeoIndex::as_ref()const noexcept{
	return as_cref();
}

inline RefGeoIndex VecGeoIndex::as_ref()noexcept{
	return {
		item_count,
		&pos[0],
		&id[0]
	};
}

inline ConstRefGeoIndex VecGeoIndex::as_cref()const noexcept{
	return {
		item_count,
		pos.data(),
		id.data()
	};
}

inline DirGeoIndex::DirGeoIndex(){}

inline DirGeoIndex::DirGeoIndex(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"item_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&item_count, sizeof(size_t));
	}
	pos = FileArray<GeoPos>(dir+"pos");
	id = FileArray<uint32_t>(dir+"id");
	throw_if_wrong_col_size();
}

inline void DirGeoIndex::open(std::string dir){
	*this = DirGeoIndex(std::move(dir));
}

inline void DirGeoIndex::close(){
	*this = DirGeoIndex();
}

inline void DirGeoIndex::throw_if_wrong_col_size()const{
	std::string err;
	if(pos.size() != item_count)
		 err += ("Column pos has wrong size. Expected: "+std::to_string(item_count)+" Actual: "+std::to_string(pos.size())+"\n");
	if(id.size() != item_count)
		 err += ("Column id has wrong size. Expected: "+std::to_string(item_count)+" Actual: "+std::to_string(id.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefGeoIndex DirGeoIndex::as_cref()const noexcept{
	return {
		item_count,
		pos.data(),
		id.data()
	};
}

inline ConstRefGeoIndex DirGeoIndex::as_ref()const noexcept{
	return as_cref();
}

inline Span<const GeoPos> DirGeoIndex::pos_as_ref()const noexcept{
	return pos_as_cref();
}

inline Span<const uint32_t> DirGeoIndex::id_as_ref()const noexcept{
	return id_as_cref();
}

inline Span<const GeoPos> DirGeoIndex::pos_as_cref()const noexcept{
	return {pos.data(), pos.data()+pos.size()};
}

inline Span<const uint32_t> DirGeoIndex::id_as_cref()const noexcept{
	return {id.data(), id.data()+id.size()};
}


inline void dump_into_dir(std::string dir, RefGeoIndex data){
	dump_into_dir(std::move(dir), ConstRefGeoIndex(data));
}

inline void dump_into_dir(std::string dir, ConstRefGeoIndex data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"item_count")((const uint8_t*)&data.item_count, sizeof(size_t));
	FileDataSink(dir+"pos")((const uint8_t*)data.pos, (data.item_count)*sizeof(GeoPos));
	FileDataSink(dir+"id")((const uint8_t*)data.id, (data.item_count)*sizeof(uint32_t));
}

} // namespace RoutingKit2

#endif
