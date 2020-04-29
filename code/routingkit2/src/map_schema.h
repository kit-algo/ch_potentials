#ifndef ROUTING_KIT2_MAP_SCHEMA_H
#define ROUTING_KIT2_MAP_SCHEMA_H

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


struct RefLinkEnds;
struct ConstRefLinkEnds;
struct RefLinkEndsAdjArray;
struct ConstRefLinkEndsAdjArray;
struct RefLinkShapes;
struct ConstRefLinkShapes;
struct RefCarRoads;
struct ConstRefCarRoads;
struct RefOSMCarRoads;
struct ConstRefOSMCarRoads;

struct RefLinkEnds{
	size_t node_count;
	size_t link_count;
	uint32_t* __restrict__ link_head; // size = link_count
	uint32_t* __restrict__ link_tail; // size = link_count

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;

	RefLinkEnds();
	RefLinkEnds(RefLinkShapes);
	RefLinkEnds(RefCarRoads);
	RefLinkEnds(RefOSMCarRoads);
	RefLinkEnds(
		size_t node_count,
		size_t link_count,
		uint32_t* __restrict__ link_head,
		uint32_t* __restrict__ link_tail
	);
};

struct ConstRefLinkEnds{
	size_t node_count;
	size_t link_count;
	const uint32_t* __restrict__ link_head; // size = link_count
	const uint32_t* __restrict__ link_tail; // size = link_count

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;

	ConstRefLinkEnds();
	ConstRefLinkEnds(RefLinkEnds);
	ConstRefLinkEnds(RefLinkShapes);
	ConstRefLinkEnds(ConstRefLinkShapes);
	ConstRefLinkEnds(RefCarRoads);
	ConstRefLinkEnds(ConstRefCarRoads);
	ConstRefLinkEnds(RefOSMCarRoads);
	ConstRefLinkEnds(ConstRefOSMCarRoads);
	ConstRefLinkEnds(
		size_t node_count,
		size_t link_count,
		const uint32_t* __restrict__ link_head,
		const uint32_t* __restrict__ link_tail
	);
};

struct VecLinkEnds{
	size_t node_count;
	size_t link_count;
	std::vector<uint32_t>link_head;
	std::vector<uint32_t>link_tail;

	VecLinkEnds();
	VecLinkEnds(
		size_t node_count,
		size_t link_count
	);
	void resize(
		size_t node_count,
		size_t link_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefLinkEnds as_ref()const noexcept;
	RefLinkEnds as_ref()noexcept;
	ConstRefLinkEnds as_cref()const noexcept;

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
};

struct DirLinkEnds{
	size_t node_count;
	size_t link_count;
	FileArray<uint32_t>link_head;
	FileArray<uint32_t>link_tail;

	DirLinkEnds();
	explicit DirLinkEnds(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefLinkEnds as_cref()const noexcept;
	ConstRefLinkEnds as_ref()const noexcept;

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefLinkEnds data);
inline void dump_into_dir(std::string dir, ConstRefLinkEnds data);

struct RefLinkEndsAdjArray{
	size_t node_count;
	size_t link_count;
	uint32_t* __restrict__ first_outgoing_dlink_index_of_node; // size = node_count + 1
	uint32_t* __restrict__ outgoing_dlink; // size = 2*link_count

	Span<uint32_t> first_outgoing_dlink_index_of_node_as_ref()noexcept;
	Span<uint32_t> outgoing_dlink_as_ref()noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_ref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_cref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_cref()const noexcept;

	RefLinkEndsAdjArray();
	RefLinkEndsAdjArray(
		size_t node_count,
		size_t link_count,
		uint32_t* __restrict__ first_outgoing_dlink_index_of_node,
		uint32_t* __restrict__ outgoing_dlink
	);
};

struct ConstRefLinkEndsAdjArray{
	size_t node_count;
	size_t link_count;
	const uint32_t* __restrict__ first_outgoing_dlink_index_of_node; // size = node_count + 1
	const uint32_t* __restrict__ outgoing_dlink; // size = 2*link_count

	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_ref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_cref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_cref()const noexcept;

	ConstRefLinkEndsAdjArray();
	ConstRefLinkEndsAdjArray(RefLinkEndsAdjArray);
	ConstRefLinkEndsAdjArray(
		size_t node_count,
		size_t link_count,
		const uint32_t* __restrict__ first_outgoing_dlink_index_of_node,
		const uint32_t* __restrict__ outgoing_dlink
	);
};

struct VecLinkEndsAdjArray{
	size_t node_count;
	size_t link_count;
	std::vector<uint32_t>first_outgoing_dlink_index_of_node;
	std::vector<uint32_t>outgoing_dlink;

	VecLinkEndsAdjArray();
	VecLinkEndsAdjArray(
		size_t node_count,
		size_t link_count
	);
	void resize(
		size_t node_count,
		size_t link_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefLinkEndsAdjArray as_ref()const noexcept;
	RefLinkEndsAdjArray as_ref()noexcept;
	ConstRefLinkEndsAdjArray as_cref()const noexcept;

	Span<uint32_t> first_outgoing_dlink_index_of_node_as_ref()noexcept;
	Span<uint32_t> outgoing_dlink_as_ref()noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_ref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_cref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_cref()const noexcept;
};

struct DirLinkEndsAdjArray{
	size_t node_count;
	size_t link_count;
	FileArray<uint32_t>first_outgoing_dlink_index_of_node;
	FileArray<uint32_t>outgoing_dlink;

	DirLinkEndsAdjArray();
	explicit DirLinkEndsAdjArray(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefLinkEndsAdjArray as_cref()const noexcept;
	ConstRefLinkEndsAdjArray as_ref()const noexcept;

	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_ref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_outgoing_dlink_index_of_node_as_cref()const noexcept;
	Span<const uint32_t> outgoing_dlink_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefLinkEndsAdjArray data);
inline void dump_into_dir(std::string dir, ConstRefLinkEndsAdjArray data);

struct RefLinkShapes{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	uint32_t* __restrict__ link_head; // size = link_count
	uint32_t* __restrict__ link_tail; // size = link_count
	uint32_t* __restrict__ link_length_in_cm; // size = link_count
	LatLon* __restrict__ node_pos; // size = node_count
	uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	LatLon* __restrict__ shape_pos; // size = shape_pos_count

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;

	RefLinkShapes();
	RefLinkShapes(RefCarRoads);
	RefLinkShapes(RefOSMCarRoads);
	RefLinkShapes(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		uint32_t* __restrict__ link_head,
		uint32_t* __restrict__ link_tail,
		uint32_t* __restrict__ link_length_in_cm,
		LatLon* __restrict__ node_pos,
		uint32_t* __restrict__ first_shape_pos_of_link,
		LatLon* __restrict__ shape_pos
	);
};

struct ConstRefLinkShapes{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	const uint32_t* __restrict__ link_head; // size = link_count
	const uint32_t* __restrict__ link_tail; // size = link_count
	const uint32_t* __restrict__ link_length_in_cm; // size = link_count
	const LatLon* __restrict__ node_pos; // size = node_count
	const uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	const LatLon* __restrict__ shape_pos; // size = shape_pos_count

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;

	ConstRefLinkShapes();
	ConstRefLinkShapes(RefLinkShapes);
	ConstRefLinkShapes(RefCarRoads);
	ConstRefLinkShapes(ConstRefCarRoads);
	ConstRefLinkShapes(RefOSMCarRoads);
	ConstRefLinkShapes(ConstRefOSMCarRoads);
	ConstRefLinkShapes(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		const uint32_t* __restrict__ link_head,
		const uint32_t* __restrict__ link_tail,
		const uint32_t* __restrict__ link_length_in_cm,
		const LatLon* __restrict__ node_pos,
		const uint32_t* __restrict__ first_shape_pos_of_link,
		const LatLon* __restrict__ shape_pos
	);
};

struct VecLinkShapes{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	std::vector<uint32_t>link_head;
	std::vector<uint32_t>link_tail;
	std::vector<uint32_t>link_length_in_cm;
	std::vector<LatLon>node_pos;
	std::vector<uint32_t>first_shape_pos_of_link;
	std::vector<LatLon>shape_pos;

	VecLinkShapes();
	VecLinkShapes(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count
	);
	void resize(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefLinkShapes as_ref()const noexcept;
	RefLinkShapes as_ref()noexcept;
	ConstRefLinkShapes as_cref()const noexcept;

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
};

struct DirLinkShapes{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	FileArray<uint32_t>link_head;
	FileArray<uint32_t>link_tail;
	FileArray<uint32_t>link_length_in_cm;
	FileArray<LatLon>node_pos;
	FileArray<uint32_t>first_shape_pos_of_link;
	FileArray<LatLon>shape_pos;

	DirLinkShapes();
	explicit DirLinkShapes(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefLinkShapes as_cref()const noexcept;
	ConstRefLinkShapes as_ref()const noexcept;

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefLinkShapes data);
inline void dump_into_dir(std::string dir, ConstRefLinkShapes data);

struct RefCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	uint32_t* __restrict__ link_head; // size = link_count
	uint32_t* __restrict__ link_tail; // size = link_count
	uint32_t* __restrict__ link_length_in_cm; // size = link_count
	LatLon* __restrict__ node_pos; // size = node_count
	uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	LatLon* __restrict__ shape_pos; // size = shape_pos_count
	uint32_t* __restrict__ dlink_traversal_time_in_ms; // size = 2*link_count
	uint32_t* __restrict__ first_dlink_of_forbidden_maneuver; // size = forbidden_maneuver_count + 1
	uint32_t* __restrict__ forbidden_maneuver_dlink; // size = forbidden_maneuver_dlink_count

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<uint32_t> dlink_traversal_time_in_ms_as_ref()noexcept;
	Span<uint32_t> first_dlink_of_forbidden_maneuver_as_ref()noexcept;
	Span<uint32_t> forbidden_maneuver_dlink_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;

	RefCarRoads();
	RefCarRoads(RefOSMCarRoads);
	RefCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		uint32_t* __restrict__ link_head,
		uint32_t* __restrict__ link_tail,
		uint32_t* __restrict__ link_length_in_cm,
		LatLon* __restrict__ node_pos,
		uint32_t* __restrict__ first_shape_pos_of_link,
		LatLon* __restrict__ shape_pos,
		uint32_t* __restrict__ dlink_traversal_time_in_ms,
		uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
		uint32_t* __restrict__ forbidden_maneuver_dlink
	);
};

struct ConstRefCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	const uint32_t* __restrict__ link_head; // size = link_count
	const uint32_t* __restrict__ link_tail; // size = link_count
	const uint32_t* __restrict__ link_length_in_cm; // size = link_count
	const LatLon* __restrict__ node_pos; // size = node_count
	const uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	const LatLon* __restrict__ shape_pos; // size = shape_pos_count
	const uint32_t* __restrict__ dlink_traversal_time_in_ms; // size = 2*link_count
	const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver; // size = forbidden_maneuver_count + 1
	const uint32_t* __restrict__ forbidden_maneuver_dlink; // size = forbidden_maneuver_dlink_count

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;

	ConstRefCarRoads();
	ConstRefCarRoads(RefCarRoads);
	ConstRefCarRoads(RefOSMCarRoads);
	ConstRefCarRoads(ConstRefOSMCarRoads);
	ConstRefCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		const uint32_t* __restrict__ link_head,
		const uint32_t* __restrict__ link_tail,
		const uint32_t* __restrict__ link_length_in_cm,
		const LatLon* __restrict__ node_pos,
		const uint32_t* __restrict__ first_shape_pos_of_link,
		const LatLon* __restrict__ shape_pos,
		const uint32_t* __restrict__ dlink_traversal_time_in_ms,
		const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
		const uint32_t* __restrict__ forbidden_maneuver_dlink
	);
};

struct VecCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	std::vector<uint32_t>link_head;
	std::vector<uint32_t>link_tail;
	std::vector<uint32_t>link_length_in_cm;
	std::vector<LatLon>node_pos;
	std::vector<uint32_t>first_shape_pos_of_link;
	std::vector<LatLon>shape_pos;
	std::vector<uint32_t>dlink_traversal_time_in_ms;
	std::vector<uint32_t>first_dlink_of_forbidden_maneuver;
	std::vector<uint32_t>forbidden_maneuver_dlink;

	VecCarRoads();
	VecCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count
	);
	void resize(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefCarRoads as_ref()const noexcept;
	RefCarRoads as_ref()noexcept;
	ConstRefCarRoads as_cref()const noexcept;

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<uint32_t> dlink_traversal_time_in_ms_as_ref()noexcept;
	Span<uint32_t> first_dlink_of_forbidden_maneuver_as_ref()noexcept;
	Span<uint32_t> forbidden_maneuver_dlink_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
};

struct DirCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	FileArray<uint32_t>link_head;
	FileArray<uint32_t>link_tail;
	FileArray<uint32_t>link_length_in_cm;
	FileArray<LatLon>node_pos;
	FileArray<uint32_t>first_shape_pos_of_link;
	FileArray<LatLon>shape_pos;
	FileArray<uint32_t>dlink_traversal_time_in_ms;
	FileArray<uint32_t>first_dlink_of_forbidden_maneuver;
	FileArray<uint32_t>forbidden_maneuver_dlink;

	DirCarRoads();
	explicit DirCarRoads(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefCarRoads as_cref()const noexcept;
	ConstRefCarRoads as_ref()const noexcept;

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefCarRoads data);
inline void dump_into_dir(std::string dir, ConstRefCarRoads data);

struct RefOSMCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	size_t way_count;
	uint32_t* __restrict__ link_head; // size = link_count
	uint32_t* __restrict__ link_tail; // size = link_count
	uint32_t* __restrict__ link_length_in_cm; // size = link_count
	LatLon* __restrict__ node_pos; // size = node_count
	uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	LatLon* __restrict__ shape_pos; // size = shape_pos_count
	uint32_t* __restrict__ dlink_traversal_time_in_ms; // size = 2*link_count
	uint32_t* __restrict__ first_dlink_of_forbidden_maneuver; // size = forbidden_maneuver_count + 1
	uint32_t* __restrict__ forbidden_maneuver_dlink; // size = forbidden_maneuver_dlink_count
	uint32_t* __restrict__ first_link_of_way; // size = way_count + 1
	uint64_t* __restrict__ node_osm_id; // size = node_count
	uint64_t* __restrict__ way_osm_id; // size = way_count
	uint64_t* __restrict__ forbidden_maneuver_osm_id; // size = forbidden_maneuver_count

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<uint32_t> dlink_traversal_time_in_ms_as_ref()noexcept;
	Span<uint32_t> first_dlink_of_forbidden_maneuver_as_ref()noexcept;
	Span<uint32_t> forbidden_maneuver_dlink_as_ref()noexcept;
	Span<uint32_t> first_link_of_way_as_ref()noexcept;
	Span<uint64_t> node_osm_id_as_ref()noexcept;
	Span<uint64_t> way_osm_id_as_ref()noexcept;
	Span<uint64_t> forbidden_maneuver_osm_id_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_ref()const noexcept;
	Span<const uint64_t> node_osm_id_as_ref()const noexcept;
	Span<const uint64_t> way_osm_id_as_ref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_cref()const noexcept;
	Span<const uint64_t> node_osm_id_as_cref()const noexcept;
	Span<const uint64_t> way_osm_id_as_cref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_cref()const noexcept;

	RefOSMCarRoads();
	RefOSMCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		size_t way_count,
		uint32_t* __restrict__ link_head,
		uint32_t* __restrict__ link_tail,
		uint32_t* __restrict__ link_length_in_cm,
		LatLon* __restrict__ node_pos,
		uint32_t* __restrict__ first_shape_pos_of_link,
		LatLon* __restrict__ shape_pos,
		uint32_t* __restrict__ dlink_traversal_time_in_ms,
		uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
		uint32_t* __restrict__ forbidden_maneuver_dlink,
		uint32_t* __restrict__ first_link_of_way,
		uint64_t* __restrict__ node_osm_id,
		uint64_t* __restrict__ way_osm_id,
		uint64_t* __restrict__ forbidden_maneuver_osm_id
	);
};

struct ConstRefOSMCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	size_t way_count;
	const uint32_t* __restrict__ link_head; // size = link_count
	const uint32_t* __restrict__ link_tail; // size = link_count
	const uint32_t* __restrict__ link_length_in_cm; // size = link_count
	const LatLon* __restrict__ node_pos; // size = node_count
	const uint32_t* __restrict__ first_shape_pos_of_link; // size = link_count + 1
	const LatLon* __restrict__ shape_pos; // size = shape_pos_count
	const uint32_t* __restrict__ dlink_traversal_time_in_ms; // size = 2*link_count
	const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver; // size = forbidden_maneuver_count + 1
	const uint32_t* __restrict__ forbidden_maneuver_dlink; // size = forbidden_maneuver_dlink_count
	const uint32_t* __restrict__ first_link_of_way; // size = way_count + 1
	const uint64_t* __restrict__ node_osm_id; // size = node_count
	const uint64_t* __restrict__ way_osm_id; // size = way_count
	const uint64_t* __restrict__ forbidden_maneuver_osm_id; // size = forbidden_maneuver_count

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_ref()const noexcept;
	Span<const uint64_t> node_osm_id_as_ref()const noexcept;
	Span<const uint64_t> way_osm_id_as_ref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_cref()const noexcept;
	Span<const uint64_t> node_osm_id_as_cref()const noexcept;
	Span<const uint64_t> way_osm_id_as_cref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_cref()const noexcept;

	ConstRefOSMCarRoads();
	ConstRefOSMCarRoads(RefOSMCarRoads);
	ConstRefOSMCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		size_t way_count,
		const uint32_t* __restrict__ link_head,
		const uint32_t* __restrict__ link_tail,
		const uint32_t* __restrict__ link_length_in_cm,
		const LatLon* __restrict__ node_pos,
		const uint32_t* __restrict__ first_shape_pos_of_link,
		const LatLon* __restrict__ shape_pos,
		const uint32_t* __restrict__ dlink_traversal_time_in_ms,
		const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
		const uint32_t* __restrict__ forbidden_maneuver_dlink,
		const uint32_t* __restrict__ first_link_of_way,
		const uint64_t* __restrict__ node_osm_id,
		const uint64_t* __restrict__ way_osm_id,
		const uint64_t* __restrict__ forbidden_maneuver_osm_id
	);
};

struct VecOSMCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	size_t way_count;
	std::vector<uint32_t>link_head;
	std::vector<uint32_t>link_tail;
	std::vector<uint32_t>link_length_in_cm;
	std::vector<LatLon>node_pos;
	std::vector<uint32_t>first_shape_pos_of_link;
	std::vector<LatLon>shape_pos;
	std::vector<uint32_t>dlink_traversal_time_in_ms;
	std::vector<uint32_t>first_dlink_of_forbidden_maneuver;
	std::vector<uint32_t>forbidden_maneuver_dlink;
	std::vector<uint32_t>first_link_of_way;
	std::vector<uint64_t>node_osm_id;
	std::vector<uint64_t>way_osm_id;
	std::vector<uint64_t>forbidden_maneuver_osm_id;

	VecOSMCarRoads();
	VecOSMCarRoads(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		size_t way_count
	);
	void resize(
		size_t node_count,
		size_t link_count,
		size_t shape_pos_count,
		size_t forbidden_maneuver_count,
		size_t forbidden_maneuver_dlink_count,
		size_t way_count
	);
	void throw_if_wrong_size()const;
	void assert_correct_size()const noexcept;
	void shrink_to_fit();
	ConstRefOSMCarRoads as_ref()const noexcept;
	RefOSMCarRoads as_ref()noexcept;
	ConstRefOSMCarRoads as_cref()const noexcept;

	Span<uint32_t> link_head_as_ref()noexcept;
	Span<uint32_t> link_tail_as_ref()noexcept;
	Span<uint32_t> link_length_in_cm_as_ref()noexcept;
	Span<LatLon> node_pos_as_ref()noexcept;
	Span<uint32_t> first_shape_pos_of_link_as_ref()noexcept;
	Span<LatLon> shape_pos_as_ref()noexcept;
	Span<uint32_t> dlink_traversal_time_in_ms_as_ref()noexcept;
	Span<uint32_t> first_dlink_of_forbidden_maneuver_as_ref()noexcept;
	Span<uint32_t> forbidden_maneuver_dlink_as_ref()noexcept;
	Span<uint32_t> first_link_of_way_as_ref()noexcept;
	Span<uint64_t> node_osm_id_as_ref()noexcept;
	Span<uint64_t> way_osm_id_as_ref()noexcept;
	Span<uint64_t> forbidden_maneuver_osm_id_as_ref()noexcept;
	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_ref()const noexcept;
	Span<const uint64_t> node_osm_id_as_ref()const noexcept;
	Span<const uint64_t> way_osm_id_as_ref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_cref()const noexcept;
	Span<const uint64_t> node_osm_id_as_cref()const noexcept;
	Span<const uint64_t> way_osm_id_as_cref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_cref()const noexcept;
};

struct DirOSMCarRoads{
	size_t node_count;
	size_t link_count;
	size_t shape_pos_count;
	size_t forbidden_maneuver_count;
	size_t forbidden_maneuver_dlink_count;
	size_t way_count;
	FileArray<uint32_t>link_head;
	FileArray<uint32_t>link_tail;
	FileArray<uint32_t>link_length_in_cm;
	FileArray<LatLon>node_pos;
	FileArray<uint32_t>first_shape_pos_of_link;
	FileArray<LatLon>shape_pos;
	FileArray<uint32_t>dlink_traversal_time_in_ms;
	FileArray<uint32_t>first_dlink_of_forbidden_maneuver;
	FileArray<uint32_t>forbidden_maneuver_dlink;
	FileArray<uint32_t>first_link_of_way;
	FileArray<uint64_t>node_osm_id;
	FileArray<uint64_t>way_osm_id;
	FileArray<uint64_t>forbidden_maneuver_osm_id;

	DirOSMCarRoads();
	explicit DirOSMCarRoads(std::string dir);
	void open(std::string dir);
	void close();
	void throw_if_wrong_col_size()const;
	ConstRefOSMCarRoads as_cref()const noexcept;
	ConstRefOSMCarRoads as_ref()const noexcept;

	Span<const uint32_t> link_head_as_ref()const noexcept;
	Span<const uint32_t> link_tail_as_ref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_ref()const noexcept;
	Span<const LatLon> node_pos_as_ref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_ref()const noexcept;
	Span<const LatLon> shape_pos_as_ref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_ref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_ref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_ref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_ref()const noexcept;
	Span<const uint64_t> node_osm_id_as_ref()const noexcept;
	Span<const uint64_t> way_osm_id_as_ref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_ref()const noexcept;
	Span<const uint32_t> link_head_as_cref()const noexcept;
	Span<const uint32_t> link_tail_as_cref()const noexcept;
	Span<const uint32_t> link_length_in_cm_as_cref()const noexcept;
	Span<const LatLon> node_pos_as_cref()const noexcept;
	Span<const uint32_t> first_shape_pos_of_link_as_cref()const noexcept;
	Span<const LatLon> shape_pos_as_cref()const noexcept;
	Span<const uint32_t> dlink_traversal_time_in_ms_as_cref()const noexcept;
	Span<const uint32_t> first_dlink_of_forbidden_maneuver_as_cref()const noexcept;
	Span<const uint32_t> forbidden_maneuver_dlink_as_cref()const noexcept;
	Span<const uint32_t> first_link_of_way_as_cref()const noexcept;
	Span<const uint64_t> node_osm_id_as_cref()const noexcept;
	Span<const uint64_t> way_osm_id_as_cref()const noexcept;
	Span<const uint64_t> forbidden_maneuver_osm_id_as_cref()const noexcept;
};

inline void dump_into_dir(std::string dir, RefOSMCarRoads data);
inline void dump_into_dir(std::string dir, ConstRefOSMCarRoads data);

inline Span<uint32_t> RefLinkEnds::link_head_as_ref()noexcept{
	return {link_head, link_head+link_count};
}

inline Span<uint32_t> RefLinkEnds::link_tail_as_ref()noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> RefLinkEnds::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> RefLinkEnds::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> RefLinkEnds::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> RefLinkEnds::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}


inline RefLinkEnds::RefLinkEnds(RefLinkShapes o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline RefLinkEnds::RefLinkEnds(RefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline RefLinkEnds::RefLinkEnds(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline RefLinkEnds::RefLinkEnds(){}

inline RefLinkEnds::RefLinkEnds(
	size_t node_count,
	size_t link_count,
	uint32_t* __restrict__ link_head,
	uint32_t* __restrict__ link_tail
):
	node_count(node_count),
	link_count(link_count),
	link_head(link_head),
	link_tail(link_tail){}

inline Span<const uint32_t> ConstRefLinkEnds::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> ConstRefLinkEnds::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> ConstRefLinkEnds::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> ConstRefLinkEnds::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}


inline ConstRefLinkEnds::ConstRefLinkEnds(RefLinkShapes o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(ConstRefLinkShapes o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(RefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(ConstRefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(ConstRefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds() {}

inline ConstRefLinkEnds::ConstRefLinkEnds(RefLinkEnds o):
	node_count(o.node_count),
	link_count(o.link_count),
	link_head(o.link_head),
	link_tail(o.link_tail){}

inline ConstRefLinkEnds::ConstRefLinkEnds(
	size_t node_count,
	size_t link_count,
	const uint32_t* __restrict__ link_head,
	const uint32_t* __restrict__ link_tail
):
	node_count(node_count),
	link_count(link_count),
	link_head(link_head),
	link_tail(link_tail){}

inline Span<uint32_t> VecLinkEnds::link_head_as_ref()noexcept{
	return {&link_head[0], &link_head[0]+link_head.size()};
}

inline Span<uint32_t> VecLinkEnds::link_tail_as_ref()noexcept{
	return {&link_tail[0], &link_tail[0]+link_tail.size()};
}

inline Span<const uint32_t> VecLinkEnds::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> VecLinkEnds::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> VecLinkEnds::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> VecLinkEnds::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}


inline VecLinkEnds::VecLinkEnds(){}

inline VecLinkEnds::VecLinkEnds(
	size_t node_count,
	size_t link_count
):
	node_count(node_count),
	link_count(link_count),
	link_head(link_count),
	link_tail(link_count){}

inline void VecLinkEnds::resize(
	size_t node_count,
	size_t link_count
){
	this->node_count = node_count;
	this->link_count = link_count;
	link_head.resize(link_count);
	link_tail.resize(link_count);
}

inline void VecLinkEnds::throw_if_wrong_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecLinkEnds::assert_correct_size()const noexcept{
	assert(link_head.size() == link_count);
	assert(link_tail.size() == link_count);
}

inline void VecLinkEnds::shrink_to_fit(){
	link_head.shrink_to_fit();
	link_tail.shrink_to_fit();
}

inline ConstRefLinkEnds VecLinkEnds::as_ref()const noexcept{
	return as_cref();
}

inline RefLinkEnds VecLinkEnds::as_ref()noexcept{
	return {
		node_count,
		link_count,
		&link_head[0],
		&link_tail[0]
	};
}

inline ConstRefLinkEnds VecLinkEnds::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		link_head.data(),
		link_tail.data()
	};
}

inline DirLinkEnds::DirLinkEnds(){}

inline DirLinkEnds::DirLinkEnds(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"node_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&node_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"link_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&link_count, sizeof(size_t));
	}
	link_head = FileArray<uint32_t>(dir+"link_head");
	link_tail = FileArray<uint32_t>(dir+"link_tail");
	throw_if_wrong_col_size();
}

inline void DirLinkEnds::open(std::string dir){
	*this = DirLinkEnds(std::move(dir));
}

inline void DirLinkEnds::close(){
	*this = DirLinkEnds();
}

inline void DirLinkEnds::throw_if_wrong_col_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefLinkEnds DirLinkEnds::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		link_head.data(),
		link_tail.data()
	};
}

inline ConstRefLinkEnds DirLinkEnds::as_ref()const noexcept{
	return as_cref();
}

inline Span<const uint32_t> DirLinkEnds::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> DirLinkEnds::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> DirLinkEnds::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> DirLinkEnds::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}


inline void dump_into_dir(std::string dir, RefLinkEnds data){
	dump_into_dir(std::move(dir), ConstRefLinkEnds(data));
}

inline void dump_into_dir(std::string dir, ConstRefLinkEnds data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"node_count")((const uint8_t*)&data.node_count, sizeof(size_t));
	FileDataSink(dir+"link_count")((const uint8_t*)&data.link_count, sizeof(size_t));
	FileDataSink(dir+"link_head")((const uint8_t*)data.link_head, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_tail")((const uint8_t*)data.link_tail, (data.link_count)*sizeof(uint32_t));
}

inline Span<uint32_t> RefLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()noexcept{
	return {first_outgoing_dlink_index_of_node, first_outgoing_dlink_index_of_node+node_count + 1};
}

inline Span<uint32_t> RefLinkEndsAdjArray::outgoing_dlink_as_ref()noexcept{
	return {outgoing_dlink, outgoing_dlink+2*link_count};
}

inline Span<const uint32_t> RefLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()const noexcept{
	return first_outgoing_dlink_index_of_node_as_cref();
}

inline Span<const uint32_t> RefLinkEndsAdjArray::outgoing_dlink_as_ref()const noexcept{
	return outgoing_dlink_as_cref();
}

inline Span<const uint32_t> RefLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_cref()const noexcept{
	return {first_outgoing_dlink_index_of_node, first_outgoing_dlink_index_of_node+node_count + 1};
}

inline Span<const uint32_t> RefLinkEndsAdjArray::outgoing_dlink_as_cref()const noexcept{
	return {outgoing_dlink, outgoing_dlink+2*link_count};
}


inline RefLinkEndsAdjArray::RefLinkEndsAdjArray(){}

inline RefLinkEndsAdjArray::RefLinkEndsAdjArray(
	size_t node_count,
	size_t link_count,
	uint32_t* __restrict__ first_outgoing_dlink_index_of_node,
	uint32_t* __restrict__ outgoing_dlink
):
	node_count(node_count),
	link_count(link_count),
	first_outgoing_dlink_index_of_node(first_outgoing_dlink_index_of_node),
	outgoing_dlink(outgoing_dlink){}

inline Span<const uint32_t> ConstRefLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()const noexcept{
	return first_outgoing_dlink_index_of_node_as_cref();
}

inline Span<const uint32_t> ConstRefLinkEndsAdjArray::outgoing_dlink_as_ref()const noexcept{
	return outgoing_dlink_as_cref();
}

inline Span<const uint32_t> ConstRefLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_cref()const noexcept{
	return {first_outgoing_dlink_index_of_node, first_outgoing_dlink_index_of_node+node_count + 1};
}

inline Span<const uint32_t> ConstRefLinkEndsAdjArray::outgoing_dlink_as_cref()const noexcept{
	return {outgoing_dlink, outgoing_dlink+2*link_count};
}


inline ConstRefLinkEndsAdjArray::ConstRefLinkEndsAdjArray() {}

inline ConstRefLinkEndsAdjArray::ConstRefLinkEndsAdjArray(RefLinkEndsAdjArray o):
	node_count(o.node_count),
	link_count(o.link_count),
	first_outgoing_dlink_index_of_node(o.first_outgoing_dlink_index_of_node),
	outgoing_dlink(o.outgoing_dlink){}

inline ConstRefLinkEndsAdjArray::ConstRefLinkEndsAdjArray(
	size_t node_count,
	size_t link_count,
	const uint32_t* __restrict__ first_outgoing_dlink_index_of_node,
	const uint32_t* __restrict__ outgoing_dlink
):
	node_count(node_count),
	link_count(link_count),
	first_outgoing_dlink_index_of_node(first_outgoing_dlink_index_of_node),
	outgoing_dlink(outgoing_dlink){}

inline Span<uint32_t> VecLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()noexcept{
	return {&first_outgoing_dlink_index_of_node[0], &first_outgoing_dlink_index_of_node[0]+first_outgoing_dlink_index_of_node.size()};
}

inline Span<uint32_t> VecLinkEndsAdjArray::outgoing_dlink_as_ref()noexcept{
	return {&outgoing_dlink[0], &outgoing_dlink[0]+outgoing_dlink.size()};
}

inline Span<const uint32_t> VecLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()const noexcept{
	return first_outgoing_dlink_index_of_node_as_cref();
}

inline Span<const uint32_t> VecLinkEndsAdjArray::outgoing_dlink_as_ref()const noexcept{
	return outgoing_dlink_as_cref();
}

inline Span<const uint32_t> VecLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_cref()const noexcept{
	return {first_outgoing_dlink_index_of_node.data(), first_outgoing_dlink_index_of_node.data()+first_outgoing_dlink_index_of_node.size()};
}

inline Span<const uint32_t> VecLinkEndsAdjArray::outgoing_dlink_as_cref()const noexcept{
	return {outgoing_dlink.data(), outgoing_dlink.data()+outgoing_dlink.size()};
}


inline VecLinkEndsAdjArray::VecLinkEndsAdjArray(){}

inline VecLinkEndsAdjArray::VecLinkEndsAdjArray(
	size_t node_count,
	size_t link_count
):
	node_count(node_count),
	link_count(link_count),
	first_outgoing_dlink_index_of_node(node_count + 1),
	outgoing_dlink(2*link_count){}

inline void VecLinkEndsAdjArray::resize(
	size_t node_count,
	size_t link_count
){
	this->node_count = node_count;
	this->link_count = link_count;
	first_outgoing_dlink_index_of_node.resize(node_count + 1);
	outgoing_dlink.resize(2*link_count);
}

inline void VecLinkEndsAdjArray::throw_if_wrong_size()const{
	std::string err;
	if(first_outgoing_dlink_index_of_node.size() != node_count + 1)
		 err += ("Column first_outgoing_dlink_index_of_node has wrong size. Expected: "+std::to_string(node_count + 1)+" Actual: "+std::to_string(first_outgoing_dlink_index_of_node.size())+"\n");
	if(outgoing_dlink.size() != 2*link_count)
		 err += ("Column outgoing_dlink has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(outgoing_dlink.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecLinkEndsAdjArray::assert_correct_size()const noexcept{
	assert(first_outgoing_dlink_index_of_node.size() == node_count + 1);
	assert(outgoing_dlink.size() == 2*link_count);
}

inline void VecLinkEndsAdjArray::shrink_to_fit(){
	first_outgoing_dlink_index_of_node.shrink_to_fit();
	outgoing_dlink.shrink_to_fit();
}

inline ConstRefLinkEndsAdjArray VecLinkEndsAdjArray::as_ref()const noexcept{
	return as_cref();
}

inline RefLinkEndsAdjArray VecLinkEndsAdjArray::as_ref()noexcept{
	return {
		node_count,
		link_count,
		&first_outgoing_dlink_index_of_node[0],
		&outgoing_dlink[0]
	};
}

inline ConstRefLinkEndsAdjArray VecLinkEndsAdjArray::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		first_outgoing_dlink_index_of_node.data(),
		outgoing_dlink.data()
	};
}

inline DirLinkEndsAdjArray::DirLinkEndsAdjArray(){}

inline DirLinkEndsAdjArray::DirLinkEndsAdjArray(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"node_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&node_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"link_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&link_count, sizeof(size_t));
	}
	first_outgoing_dlink_index_of_node = FileArray<uint32_t>(dir+"first_outgoing_dlink_index_of_node");
	outgoing_dlink = FileArray<uint32_t>(dir+"outgoing_dlink");
	throw_if_wrong_col_size();
}

inline void DirLinkEndsAdjArray::open(std::string dir){
	*this = DirLinkEndsAdjArray(std::move(dir));
}

inline void DirLinkEndsAdjArray::close(){
	*this = DirLinkEndsAdjArray();
}

inline void DirLinkEndsAdjArray::throw_if_wrong_col_size()const{
	std::string err;
	if(first_outgoing_dlink_index_of_node.size() != node_count + 1)
		 err += ("Column first_outgoing_dlink_index_of_node has wrong size. Expected: "+std::to_string(node_count + 1)+" Actual: "+std::to_string(first_outgoing_dlink_index_of_node.size())+"\n");
	if(outgoing_dlink.size() != 2*link_count)
		 err += ("Column outgoing_dlink has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(outgoing_dlink.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefLinkEndsAdjArray DirLinkEndsAdjArray::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		first_outgoing_dlink_index_of_node.data(),
		outgoing_dlink.data()
	};
}

inline ConstRefLinkEndsAdjArray DirLinkEndsAdjArray::as_ref()const noexcept{
	return as_cref();
}

inline Span<const uint32_t> DirLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_ref()const noexcept{
	return first_outgoing_dlink_index_of_node_as_cref();
}

inline Span<const uint32_t> DirLinkEndsAdjArray::outgoing_dlink_as_ref()const noexcept{
	return outgoing_dlink_as_cref();
}

inline Span<const uint32_t> DirLinkEndsAdjArray::first_outgoing_dlink_index_of_node_as_cref()const noexcept{
	return {first_outgoing_dlink_index_of_node.data(), first_outgoing_dlink_index_of_node.data()+first_outgoing_dlink_index_of_node.size()};
}

inline Span<const uint32_t> DirLinkEndsAdjArray::outgoing_dlink_as_cref()const noexcept{
	return {outgoing_dlink.data(), outgoing_dlink.data()+outgoing_dlink.size()};
}


inline void dump_into_dir(std::string dir, RefLinkEndsAdjArray data){
	dump_into_dir(std::move(dir), ConstRefLinkEndsAdjArray(data));
}

inline void dump_into_dir(std::string dir, ConstRefLinkEndsAdjArray data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"node_count")((const uint8_t*)&data.node_count, sizeof(size_t));
	FileDataSink(dir+"link_count")((const uint8_t*)&data.link_count, sizeof(size_t));
	FileDataSink(dir+"first_outgoing_dlink_index_of_node")((const uint8_t*)data.first_outgoing_dlink_index_of_node, (data.node_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"outgoing_dlink")((const uint8_t*)data.outgoing_dlink, (2*data.link_count)*sizeof(uint32_t));
}

inline Span<uint32_t> RefLinkShapes::link_head_as_ref()noexcept{
	return {link_head, link_head+link_count};
}

inline Span<uint32_t> RefLinkShapes::link_tail_as_ref()noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<uint32_t> RefLinkShapes::link_length_in_cm_as_ref()noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<LatLon> RefLinkShapes::node_pos_as_ref()noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<uint32_t> RefLinkShapes::first_shape_pos_of_link_as_ref()noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<LatLon> RefLinkShapes::shape_pos_as_ref()noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<const uint32_t> RefLinkShapes::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> RefLinkShapes::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> RefLinkShapes::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> RefLinkShapes::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> RefLinkShapes::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> RefLinkShapes::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> RefLinkShapes::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> RefLinkShapes::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> RefLinkShapes::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> RefLinkShapes::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> RefLinkShapes::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> RefLinkShapes::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}


inline RefLinkShapes::RefLinkShapes(RefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline RefLinkShapes::RefLinkShapes(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline RefLinkShapes::RefLinkShapes(){}

inline RefLinkShapes::RefLinkShapes(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	uint32_t* __restrict__ link_head,
	uint32_t* __restrict__ link_tail,
	uint32_t* __restrict__ link_length_in_cm,
	LatLon* __restrict__ node_pos,
	uint32_t* __restrict__ first_shape_pos_of_link,
	LatLon* __restrict__ shape_pos
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos){}

inline Span<const uint32_t> ConstRefLinkShapes::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> ConstRefLinkShapes::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> ConstRefLinkShapes::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> ConstRefLinkShapes::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> ConstRefLinkShapes::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> ConstRefLinkShapes::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> ConstRefLinkShapes::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> ConstRefLinkShapes::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> ConstRefLinkShapes::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> ConstRefLinkShapes::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> ConstRefLinkShapes::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> ConstRefLinkShapes::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}


inline ConstRefLinkShapes::ConstRefLinkShapes(RefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline ConstRefLinkShapes::ConstRefLinkShapes(ConstRefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline ConstRefLinkShapes::ConstRefLinkShapes(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline ConstRefLinkShapes::ConstRefLinkShapes(ConstRefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline ConstRefLinkShapes::ConstRefLinkShapes() {}

inline ConstRefLinkShapes::ConstRefLinkShapes(RefLinkShapes o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos){}

inline ConstRefLinkShapes::ConstRefLinkShapes(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	const uint32_t* __restrict__ link_head,
	const uint32_t* __restrict__ link_tail,
	const uint32_t* __restrict__ link_length_in_cm,
	const LatLon* __restrict__ node_pos,
	const uint32_t* __restrict__ first_shape_pos_of_link,
	const LatLon* __restrict__ shape_pos
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos){}

inline Span<uint32_t> VecLinkShapes::link_head_as_ref()noexcept{
	return {&link_head[0], &link_head[0]+link_head.size()};
}

inline Span<uint32_t> VecLinkShapes::link_tail_as_ref()noexcept{
	return {&link_tail[0], &link_tail[0]+link_tail.size()};
}

inline Span<uint32_t> VecLinkShapes::link_length_in_cm_as_ref()noexcept{
	return {&link_length_in_cm[0], &link_length_in_cm[0]+link_length_in_cm.size()};
}

inline Span<LatLon> VecLinkShapes::node_pos_as_ref()noexcept{
	return {&node_pos[0], &node_pos[0]+node_pos.size()};
}

inline Span<uint32_t> VecLinkShapes::first_shape_pos_of_link_as_ref()noexcept{
	return {&first_shape_pos_of_link[0], &first_shape_pos_of_link[0]+first_shape_pos_of_link.size()};
}

inline Span<LatLon> VecLinkShapes::shape_pos_as_ref()noexcept{
	return {&shape_pos[0], &shape_pos[0]+shape_pos.size()};
}

inline Span<const uint32_t> VecLinkShapes::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> VecLinkShapes::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> VecLinkShapes::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> VecLinkShapes::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> VecLinkShapes::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> VecLinkShapes::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> VecLinkShapes::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> VecLinkShapes::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> VecLinkShapes::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> VecLinkShapes::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> VecLinkShapes::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> VecLinkShapes::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}


inline VecLinkShapes::VecLinkShapes(){}

inline VecLinkShapes::VecLinkShapes(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	link_head(link_count),
	link_tail(link_count),
	link_length_in_cm(link_count),
	node_pos(node_count),
	first_shape_pos_of_link(link_count + 1),
	shape_pos(shape_pos_count){}

inline void VecLinkShapes::resize(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count
){
	this->node_count = node_count;
	this->link_count = link_count;
	this->shape_pos_count = shape_pos_count;
	link_head.resize(link_count);
	link_tail.resize(link_count);
	link_length_in_cm.resize(link_count);
	node_pos.resize(node_count);
	first_shape_pos_of_link.resize(link_count + 1);
	shape_pos.resize(shape_pos_count);
}

inline void VecLinkShapes::throw_if_wrong_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecLinkShapes::assert_correct_size()const noexcept{
	assert(link_head.size() == link_count);
	assert(link_tail.size() == link_count);
	assert(link_length_in_cm.size() == link_count);
	assert(node_pos.size() == node_count);
	assert(first_shape_pos_of_link.size() == link_count + 1);
	assert(shape_pos.size() == shape_pos_count);
}

inline void VecLinkShapes::shrink_to_fit(){
	link_head.shrink_to_fit();
	link_tail.shrink_to_fit();
	link_length_in_cm.shrink_to_fit();
	node_pos.shrink_to_fit();
	first_shape_pos_of_link.shrink_to_fit();
	shape_pos.shrink_to_fit();
}

inline ConstRefLinkShapes VecLinkShapes::as_ref()const noexcept{
	return as_cref();
}

inline RefLinkShapes VecLinkShapes::as_ref()noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		&link_head[0],
		&link_tail[0],
		&link_length_in_cm[0],
		&node_pos[0],
		&first_shape_pos_of_link[0],
		&shape_pos[0]
	};
}

inline ConstRefLinkShapes VecLinkShapes::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data()
	};
}

inline DirLinkShapes::DirLinkShapes(){}

inline DirLinkShapes::DirLinkShapes(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"node_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&node_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"link_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&link_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"shape_pos_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&shape_pos_count, sizeof(size_t));
	}
	link_head = FileArray<uint32_t>(dir+"link_head");
	link_tail = FileArray<uint32_t>(dir+"link_tail");
	link_length_in_cm = FileArray<uint32_t>(dir+"link_length_in_cm");
	node_pos = FileArray<LatLon>(dir+"node_pos");
	first_shape_pos_of_link = FileArray<uint32_t>(dir+"first_shape_pos_of_link");
	shape_pos = FileArray<LatLon>(dir+"shape_pos");
	throw_if_wrong_col_size();
}

inline void DirLinkShapes::open(std::string dir){
	*this = DirLinkShapes(std::move(dir));
}

inline void DirLinkShapes::close(){
	*this = DirLinkShapes();
}

inline void DirLinkShapes::throw_if_wrong_col_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefLinkShapes DirLinkShapes::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data()
	};
}

inline ConstRefLinkShapes DirLinkShapes::as_ref()const noexcept{
	return as_cref();
}

inline Span<const uint32_t> DirLinkShapes::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> DirLinkShapes::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> DirLinkShapes::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> DirLinkShapes::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> DirLinkShapes::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> DirLinkShapes::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> DirLinkShapes::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> DirLinkShapes::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> DirLinkShapes::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> DirLinkShapes::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> DirLinkShapes::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> DirLinkShapes::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}


inline void dump_into_dir(std::string dir, RefLinkShapes data){
	dump_into_dir(std::move(dir), ConstRefLinkShapes(data));
}

inline void dump_into_dir(std::string dir, ConstRefLinkShapes data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"node_count")((const uint8_t*)&data.node_count, sizeof(size_t));
	FileDataSink(dir+"link_count")((const uint8_t*)&data.link_count, sizeof(size_t));
	FileDataSink(dir+"shape_pos_count")((const uint8_t*)&data.shape_pos_count, sizeof(size_t));
	FileDataSink(dir+"link_head")((const uint8_t*)data.link_head, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_tail")((const uint8_t*)data.link_tail, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_length_in_cm")((const uint8_t*)data.link_length_in_cm, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"node_pos")((const uint8_t*)data.node_pos, (data.node_count)*sizeof(LatLon));
	FileDataSink(dir+"first_shape_pos_of_link")((const uint8_t*)data.first_shape_pos_of_link, (data.link_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"shape_pos")((const uint8_t*)data.shape_pos, (data.shape_pos_count)*sizeof(LatLon));
}

inline Span<uint32_t> RefCarRoads::link_head_as_ref()noexcept{
	return {link_head, link_head+link_count};
}

inline Span<uint32_t> RefCarRoads::link_tail_as_ref()noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<uint32_t> RefCarRoads::link_length_in_cm_as_ref()noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<LatLon> RefCarRoads::node_pos_as_ref()noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<uint32_t> RefCarRoads::first_shape_pos_of_link_as_ref()noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<LatLon> RefCarRoads::shape_pos_as_ref()noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<uint32_t> RefCarRoads::dlink_traversal_time_in_ms_as_ref()noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<uint32_t> RefCarRoads::first_dlink_of_forbidden_maneuver_as_ref()noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<uint32_t> RefCarRoads::forbidden_maneuver_dlink_as_ref()noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}

inline Span<const uint32_t> RefCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> RefCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> RefCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> RefCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> RefCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> RefCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> RefCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> RefCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> RefCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> RefCarRoads::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> RefCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> RefCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> RefCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> RefCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> RefCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<const uint32_t> RefCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<const uint32_t> RefCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<const uint32_t> RefCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}


inline RefCarRoads::RefCarRoads(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	forbidden_maneuver_count(o.forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(o.forbidden_maneuver_dlink_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos),
	dlink_traversal_time_in_ms(o.dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(o.first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(o.forbidden_maneuver_dlink){}

inline RefCarRoads::RefCarRoads(){}

inline RefCarRoads::RefCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	uint32_t* __restrict__ link_head,
	uint32_t* __restrict__ link_tail,
	uint32_t* __restrict__ link_length_in_cm,
	LatLon* __restrict__ node_pos,
	uint32_t* __restrict__ first_shape_pos_of_link,
	LatLon* __restrict__ shape_pos,
	uint32_t* __restrict__ dlink_traversal_time_in_ms,
	uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
	uint32_t* __restrict__ forbidden_maneuver_dlink
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos),
	dlink_traversal_time_in_ms(dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink){}

inline Span<const uint32_t> ConstRefCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> ConstRefCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> ConstRefCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> ConstRefCarRoads::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> ConstRefCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> ConstRefCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> ConstRefCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> ConstRefCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> ConstRefCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<const uint32_t> ConstRefCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<const uint32_t> ConstRefCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<const uint32_t> ConstRefCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}


inline ConstRefCarRoads::ConstRefCarRoads(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	forbidden_maneuver_count(o.forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(o.forbidden_maneuver_dlink_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos),
	dlink_traversal_time_in_ms(o.dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(o.first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(o.forbidden_maneuver_dlink){}

inline ConstRefCarRoads::ConstRefCarRoads(ConstRefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	forbidden_maneuver_count(o.forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(o.forbidden_maneuver_dlink_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos),
	dlink_traversal_time_in_ms(o.dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(o.first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(o.forbidden_maneuver_dlink){}

inline ConstRefCarRoads::ConstRefCarRoads() {}

inline ConstRefCarRoads::ConstRefCarRoads(RefCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	forbidden_maneuver_count(o.forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(o.forbidden_maneuver_dlink_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos),
	dlink_traversal_time_in_ms(o.dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(o.first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(o.forbidden_maneuver_dlink){}

inline ConstRefCarRoads::ConstRefCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	const uint32_t* __restrict__ link_head,
	const uint32_t* __restrict__ link_tail,
	const uint32_t* __restrict__ link_length_in_cm,
	const LatLon* __restrict__ node_pos,
	const uint32_t* __restrict__ first_shape_pos_of_link,
	const LatLon* __restrict__ shape_pos,
	const uint32_t* __restrict__ dlink_traversal_time_in_ms,
	const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
	const uint32_t* __restrict__ forbidden_maneuver_dlink
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos),
	dlink_traversal_time_in_ms(dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink){}

inline Span<uint32_t> VecCarRoads::link_head_as_ref()noexcept{
	return {&link_head[0], &link_head[0]+link_head.size()};
}

inline Span<uint32_t> VecCarRoads::link_tail_as_ref()noexcept{
	return {&link_tail[0], &link_tail[0]+link_tail.size()};
}

inline Span<uint32_t> VecCarRoads::link_length_in_cm_as_ref()noexcept{
	return {&link_length_in_cm[0], &link_length_in_cm[0]+link_length_in_cm.size()};
}

inline Span<LatLon> VecCarRoads::node_pos_as_ref()noexcept{
	return {&node_pos[0], &node_pos[0]+node_pos.size()};
}

inline Span<uint32_t> VecCarRoads::first_shape_pos_of_link_as_ref()noexcept{
	return {&first_shape_pos_of_link[0], &first_shape_pos_of_link[0]+first_shape_pos_of_link.size()};
}

inline Span<LatLon> VecCarRoads::shape_pos_as_ref()noexcept{
	return {&shape_pos[0], &shape_pos[0]+shape_pos.size()};
}

inline Span<uint32_t> VecCarRoads::dlink_traversal_time_in_ms_as_ref()noexcept{
	return {&dlink_traversal_time_in_ms[0], &dlink_traversal_time_in_ms[0]+dlink_traversal_time_in_ms.size()};
}

inline Span<uint32_t> VecCarRoads::first_dlink_of_forbidden_maneuver_as_ref()noexcept{
	return {&first_dlink_of_forbidden_maneuver[0], &first_dlink_of_forbidden_maneuver[0]+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<uint32_t> VecCarRoads::forbidden_maneuver_dlink_as_ref()noexcept{
	return {&forbidden_maneuver_dlink[0], &forbidden_maneuver_dlink[0]+forbidden_maneuver_dlink.size()};
}

inline Span<const uint32_t> VecCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> VecCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> VecCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> VecCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> VecCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> VecCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> VecCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> VecCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> VecCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> VecCarRoads::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> VecCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> VecCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> VecCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> VecCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> VecCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}

inline Span<const uint32_t> VecCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms.data(), dlink_traversal_time_in_ms.data()+dlink_traversal_time_in_ms.size()};
}

inline Span<const uint32_t> VecCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver.data(), first_dlink_of_forbidden_maneuver.data()+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<const uint32_t> VecCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink.data(), forbidden_maneuver_dlink.data()+forbidden_maneuver_dlink.size()};
}


inline VecCarRoads::VecCarRoads(){}

inline VecCarRoads::VecCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	link_head(link_count),
	link_tail(link_count),
	link_length_in_cm(link_count),
	node_pos(node_count),
	first_shape_pos_of_link(link_count + 1),
	shape_pos(shape_pos_count),
	dlink_traversal_time_in_ms(2*link_count),
	first_dlink_of_forbidden_maneuver(forbidden_maneuver_count + 1),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink_count){}

inline void VecCarRoads::resize(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count
){
	this->node_count = node_count;
	this->link_count = link_count;
	this->shape_pos_count = shape_pos_count;
	this->forbidden_maneuver_count = forbidden_maneuver_count;
	this->forbidden_maneuver_dlink_count = forbidden_maneuver_dlink_count;
	link_head.resize(link_count);
	link_tail.resize(link_count);
	link_length_in_cm.resize(link_count);
	node_pos.resize(node_count);
	first_shape_pos_of_link.resize(link_count + 1);
	shape_pos.resize(shape_pos_count);
	dlink_traversal_time_in_ms.resize(2*link_count);
	first_dlink_of_forbidden_maneuver.resize(forbidden_maneuver_count + 1);
	forbidden_maneuver_dlink.resize(forbidden_maneuver_dlink_count);
}

inline void VecCarRoads::throw_if_wrong_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(dlink_traversal_time_in_ms.size() != 2*link_count)
		 err += ("Column dlink_traversal_time_in_ms has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(dlink_traversal_time_in_ms.size())+"\n");
	if(first_dlink_of_forbidden_maneuver.size() != forbidden_maneuver_count + 1)
		 err += ("Column first_dlink_of_forbidden_maneuver has wrong size. Expected: "+std::to_string(forbidden_maneuver_count + 1)+" Actual: "+std::to_string(first_dlink_of_forbidden_maneuver.size())+"\n");
	if(forbidden_maneuver_dlink.size() != forbidden_maneuver_dlink_count)
		 err += ("Column forbidden_maneuver_dlink has wrong size. Expected: "+std::to_string(forbidden_maneuver_dlink_count)+" Actual: "+std::to_string(forbidden_maneuver_dlink.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecCarRoads::assert_correct_size()const noexcept{
	assert(link_head.size() == link_count);
	assert(link_tail.size() == link_count);
	assert(link_length_in_cm.size() == link_count);
	assert(node_pos.size() == node_count);
	assert(first_shape_pos_of_link.size() == link_count + 1);
	assert(shape_pos.size() == shape_pos_count);
	assert(dlink_traversal_time_in_ms.size() == 2*link_count);
	assert(first_dlink_of_forbidden_maneuver.size() == forbidden_maneuver_count + 1);
	assert(forbidden_maneuver_dlink.size() == forbidden_maneuver_dlink_count);
}

inline void VecCarRoads::shrink_to_fit(){
	link_head.shrink_to_fit();
	link_tail.shrink_to_fit();
	link_length_in_cm.shrink_to_fit();
	node_pos.shrink_to_fit();
	first_shape_pos_of_link.shrink_to_fit();
	shape_pos.shrink_to_fit();
	dlink_traversal_time_in_ms.shrink_to_fit();
	first_dlink_of_forbidden_maneuver.shrink_to_fit();
	forbidden_maneuver_dlink.shrink_to_fit();
}

inline ConstRefCarRoads VecCarRoads::as_ref()const noexcept{
	return as_cref();
}

inline RefCarRoads VecCarRoads::as_ref()noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		&link_head[0],
		&link_tail[0],
		&link_length_in_cm[0],
		&node_pos[0],
		&first_shape_pos_of_link[0],
		&shape_pos[0],
		&dlink_traversal_time_in_ms[0],
		&first_dlink_of_forbidden_maneuver[0],
		&forbidden_maneuver_dlink[0]
	};
}

inline ConstRefCarRoads VecCarRoads::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data(),
		dlink_traversal_time_in_ms.data(),
		first_dlink_of_forbidden_maneuver.data(),
		forbidden_maneuver_dlink.data()
	};
}

inline DirCarRoads::DirCarRoads(){}

inline DirCarRoads::DirCarRoads(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"node_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&node_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"link_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&link_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"shape_pos_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&shape_pos_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"forbidden_maneuver_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&forbidden_maneuver_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"forbidden_maneuver_dlink_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&forbidden_maneuver_dlink_count, sizeof(size_t));
	}
	link_head = FileArray<uint32_t>(dir+"link_head");
	link_tail = FileArray<uint32_t>(dir+"link_tail");
	link_length_in_cm = FileArray<uint32_t>(dir+"link_length_in_cm");
	node_pos = FileArray<LatLon>(dir+"node_pos");
	first_shape_pos_of_link = FileArray<uint32_t>(dir+"first_shape_pos_of_link");
	shape_pos = FileArray<LatLon>(dir+"shape_pos");
	dlink_traversal_time_in_ms = FileArray<uint32_t>(dir+"dlink_traversal_time_in_ms");
	first_dlink_of_forbidden_maneuver = FileArray<uint32_t>(dir+"first_dlink_of_forbidden_maneuver");
	forbidden_maneuver_dlink = FileArray<uint32_t>(dir+"forbidden_maneuver_dlink");
	throw_if_wrong_col_size();
}

inline void DirCarRoads::open(std::string dir){
	*this = DirCarRoads(std::move(dir));
}

inline void DirCarRoads::close(){
	*this = DirCarRoads();
}

inline void DirCarRoads::throw_if_wrong_col_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(dlink_traversal_time_in_ms.size() != 2*link_count)
		 err += ("Column dlink_traversal_time_in_ms has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(dlink_traversal_time_in_ms.size())+"\n");
	if(first_dlink_of_forbidden_maneuver.size() != forbidden_maneuver_count + 1)
		 err += ("Column first_dlink_of_forbidden_maneuver has wrong size. Expected: "+std::to_string(forbidden_maneuver_count + 1)+" Actual: "+std::to_string(first_dlink_of_forbidden_maneuver.size())+"\n");
	if(forbidden_maneuver_dlink.size() != forbidden_maneuver_dlink_count)
		 err += ("Column forbidden_maneuver_dlink has wrong size. Expected: "+std::to_string(forbidden_maneuver_dlink_count)+" Actual: "+std::to_string(forbidden_maneuver_dlink.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefCarRoads DirCarRoads::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data(),
		dlink_traversal_time_in_ms.data(),
		first_dlink_of_forbidden_maneuver.data(),
		forbidden_maneuver_dlink.data()
	};
}

inline ConstRefCarRoads DirCarRoads::as_ref()const noexcept{
	return as_cref();
}

inline Span<const uint32_t> DirCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> DirCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> DirCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> DirCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> DirCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> DirCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> DirCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> DirCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> DirCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> DirCarRoads::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> DirCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> DirCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> DirCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> DirCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> DirCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}

inline Span<const uint32_t> DirCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms.data(), dlink_traversal_time_in_ms.data()+dlink_traversal_time_in_ms.size()};
}

inline Span<const uint32_t> DirCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver.data(), first_dlink_of_forbidden_maneuver.data()+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<const uint32_t> DirCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink.data(), forbidden_maneuver_dlink.data()+forbidden_maneuver_dlink.size()};
}


inline void dump_into_dir(std::string dir, RefCarRoads data){
	dump_into_dir(std::move(dir), ConstRefCarRoads(data));
}

inline void dump_into_dir(std::string dir, ConstRefCarRoads data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"node_count")((const uint8_t*)&data.node_count, sizeof(size_t));
	FileDataSink(dir+"link_count")((const uint8_t*)&data.link_count, sizeof(size_t));
	FileDataSink(dir+"shape_pos_count")((const uint8_t*)&data.shape_pos_count, sizeof(size_t));
	FileDataSink(dir+"forbidden_maneuver_count")((const uint8_t*)&data.forbidden_maneuver_count, sizeof(size_t));
	FileDataSink(dir+"forbidden_maneuver_dlink_count")((const uint8_t*)&data.forbidden_maneuver_dlink_count, sizeof(size_t));
	FileDataSink(dir+"link_head")((const uint8_t*)data.link_head, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_tail")((const uint8_t*)data.link_tail, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_length_in_cm")((const uint8_t*)data.link_length_in_cm, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"node_pos")((const uint8_t*)data.node_pos, (data.node_count)*sizeof(LatLon));
	FileDataSink(dir+"first_shape_pos_of_link")((const uint8_t*)data.first_shape_pos_of_link, (data.link_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"shape_pos")((const uint8_t*)data.shape_pos, (data.shape_pos_count)*sizeof(LatLon));
	FileDataSink(dir+"dlink_traversal_time_in_ms")((const uint8_t*)data.dlink_traversal_time_in_ms, (2*data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"first_dlink_of_forbidden_maneuver")((const uint8_t*)data.first_dlink_of_forbidden_maneuver, (data.forbidden_maneuver_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"forbidden_maneuver_dlink")((const uint8_t*)data.forbidden_maneuver_dlink, (data.forbidden_maneuver_dlink_count)*sizeof(uint32_t));
}

inline Span<uint32_t> RefOSMCarRoads::link_head_as_ref()noexcept{
	return {link_head, link_head+link_count};
}

inline Span<uint32_t> RefOSMCarRoads::link_tail_as_ref()noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<uint32_t> RefOSMCarRoads::link_length_in_cm_as_ref()noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<LatLon> RefOSMCarRoads::node_pos_as_ref()noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<uint32_t> RefOSMCarRoads::first_shape_pos_of_link_as_ref()noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<LatLon> RefOSMCarRoads::shape_pos_as_ref()noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<uint32_t> RefOSMCarRoads::dlink_traversal_time_in_ms_as_ref()noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<uint32_t> RefOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<uint32_t> RefOSMCarRoads::forbidden_maneuver_dlink_as_ref()noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}

inline Span<uint32_t> RefOSMCarRoads::first_link_of_way_as_ref()noexcept{
	return {first_link_of_way, first_link_of_way+way_count + 1};
}

inline Span<uint64_t> RefOSMCarRoads::node_osm_id_as_ref()noexcept{
	return {node_osm_id, node_osm_id+node_count};
}

inline Span<uint64_t> RefOSMCarRoads::way_osm_id_as_ref()noexcept{
	return {way_osm_id, way_osm_id+way_count};
}

inline Span<uint64_t> RefOSMCarRoads::forbidden_maneuver_osm_id_as_ref()noexcept{
	return {forbidden_maneuver_osm_id, forbidden_maneuver_osm_id+forbidden_maneuver_count};
}

inline Span<const uint32_t> RefOSMCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> RefOSMCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> RefOSMCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::first_link_of_way_as_ref()const noexcept{
	return first_link_of_way_as_cref();
}

inline Span<const uint64_t> RefOSMCarRoads::node_osm_id_as_ref()const noexcept{
	return node_osm_id_as_cref();
}

inline Span<const uint64_t> RefOSMCarRoads::way_osm_id_as_ref()const noexcept{
	return way_osm_id_as_cref();
}

inline Span<const uint64_t> RefOSMCarRoads::forbidden_maneuver_osm_id_as_ref()const noexcept{
	return forbidden_maneuver_osm_id_as_cref();
}

inline Span<const uint32_t> RefOSMCarRoads::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> RefOSMCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> RefOSMCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> RefOSMCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> RefOSMCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> RefOSMCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<const uint32_t> RefOSMCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<const uint32_t> RefOSMCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<const uint32_t> RefOSMCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}

inline Span<const uint32_t> RefOSMCarRoads::first_link_of_way_as_cref()const noexcept{
	return {first_link_of_way, first_link_of_way+way_count + 1};
}

inline Span<const uint64_t> RefOSMCarRoads::node_osm_id_as_cref()const noexcept{
	return {node_osm_id, node_osm_id+node_count};
}

inline Span<const uint64_t> RefOSMCarRoads::way_osm_id_as_cref()const noexcept{
	return {way_osm_id, way_osm_id+way_count};
}

inline Span<const uint64_t> RefOSMCarRoads::forbidden_maneuver_osm_id_as_cref()const noexcept{
	return {forbidden_maneuver_osm_id, forbidden_maneuver_osm_id+forbidden_maneuver_count};
}


inline RefOSMCarRoads::RefOSMCarRoads(){}

inline RefOSMCarRoads::RefOSMCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	size_t way_count,
	uint32_t* __restrict__ link_head,
	uint32_t* __restrict__ link_tail,
	uint32_t* __restrict__ link_length_in_cm,
	LatLon* __restrict__ node_pos,
	uint32_t* __restrict__ first_shape_pos_of_link,
	LatLon* __restrict__ shape_pos,
	uint32_t* __restrict__ dlink_traversal_time_in_ms,
	uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
	uint32_t* __restrict__ forbidden_maneuver_dlink,
	uint32_t* __restrict__ first_link_of_way,
	uint64_t* __restrict__ node_osm_id,
	uint64_t* __restrict__ way_osm_id,
	uint64_t* __restrict__ forbidden_maneuver_osm_id
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	way_count(way_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos),
	dlink_traversal_time_in_ms(dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink),
	first_link_of_way(first_link_of_way),
	node_osm_id(node_osm_id),
	way_osm_id(way_osm_id),
	forbidden_maneuver_osm_id(forbidden_maneuver_osm_id){}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> ConstRefOSMCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> ConstRefOSMCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_link_of_way_as_ref()const noexcept{
	return first_link_of_way_as_cref();
}

inline Span<const uint64_t> ConstRefOSMCarRoads::node_osm_id_as_ref()const noexcept{
	return node_osm_id_as_cref();
}

inline Span<const uint64_t> ConstRefOSMCarRoads::way_osm_id_as_ref()const noexcept{
	return way_osm_id_as_cref();
}

inline Span<const uint64_t> ConstRefOSMCarRoads::forbidden_maneuver_osm_id_as_ref()const noexcept{
	return forbidden_maneuver_osm_id_as_cref();
}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_head_as_cref()const noexcept{
	return {link_head, link_head+link_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail, link_tail+link_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm, link_length_in_cm+link_count};
}

inline Span<const LatLon> ConstRefOSMCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos, node_pos+node_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link, first_shape_pos_of_link+link_count + 1};
}

inline Span<const LatLon> ConstRefOSMCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos, shape_pos+shape_pos_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms, dlink_traversal_time_in_ms+2*link_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver, first_dlink_of_forbidden_maneuver+forbidden_maneuver_count + 1};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink, forbidden_maneuver_dlink+forbidden_maneuver_dlink_count};
}

inline Span<const uint32_t> ConstRefOSMCarRoads::first_link_of_way_as_cref()const noexcept{
	return {first_link_of_way, first_link_of_way+way_count + 1};
}

inline Span<const uint64_t> ConstRefOSMCarRoads::node_osm_id_as_cref()const noexcept{
	return {node_osm_id, node_osm_id+node_count};
}

inline Span<const uint64_t> ConstRefOSMCarRoads::way_osm_id_as_cref()const noexcept{
	return {way_osm_id, way_osm_id+way_count};
}

inline Span<const uint64_t> ConstRefOSMCarRoads::forbidden_maneuver_osm_id_as_cref()const noexcept{
	return {forbidden_maneuver_osm_id, forbidden_maneuver_osm_id+forbidden_maneuver_count};
}


inline ConstRefOSMCarRoads::ConstRefOSMCarRoads() {}

inline ConstRefOSMCarRoads::ConstRefOSMCarRoads(RefOSMCarRoads o):
	node_count(o.node_count),
	link_count(o.link_count),
	shape_pos_count(o.shape_pos_count),
	forbidden_maneuver_count(o.forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(o.forbidden_maneuver_dlink_count),
	way_count(o.way_count),
	link_head(o.link_head),
	link_tail(o.link_tail),
	link_length_in_cm(o.link_length_in_cm),
	node_pos(o.node_pos),
	first_shape_pos_of_link(o.first_shape_pos_of_link),
	shape_pos(o.shape_pos),
	dlink_traversal_time_in_ms(o.dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(o.first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(o.forbidden_maneuver_dlink),
	first_link_of_way(o.first_link_of_way),
	node_osm_id(o.node_osm_id),
	way_osm_id(o.way_osm_id),
	forbidden_maneuver_osm_id(o.forbidden_maneuver_osm_id){}

inline ConstRefOSMCarRoads::ConstRefOSMCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	size_t way_count,
	const uint32_t* __restrict__ link_head,
	const uint32_t* __restrict__ link_tail,
	const uint32_t* __restrict__ link_length_in_cm,
	const LatLon* __restrict__ node_pos,
	const uint32_t* __restrict__ first_shape_pos_of_link,
	const LatLon* __restrict__ shape_pos,
	const uint32_t* __restrict__ dlink_traversal_time_in_ms,
	const uint32_t* __restrict__ first_dlink_of_forbidden_maneuver,
	const uint32_t* __restrict__ forbidden_maneuver_dlink,
	const uint32_t* __restrict__ first_link_of_way,
	const uint64_t* __restrict__ node_osm_id,
	const uint64_t* __restrict__ way_osm_id,
	const uint64_t* __restrict__ forbidden_maneuver_osm_id
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	way_count(way_count),
	link_head(link_head),
	link_tail(link_tail),
	link_length_in_cm(link_length_in_cm),
	node_pos(node_pos),
	first_shape_pos_of_link(first_shape_pos_of_link),
	shape_pos(shape_pos),
	dlink_traversal_time_in_ms(dlink_traversal_time_in_ms),
	first_dlink_of_forbidden_maneuver(first_dlink_of_forbidden_maneuver),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink),
	first_link_of_way(first_link_of_way),
	node_osm_id(node_osm_id),
	way_osm_id(way_osm_id),
	forbidden_maneuver_osm_id(forbidden_maneuver_osm_id){}

inline Span<uint32_t> VecOSMCarRoads::link_head_as_ref()noexcept{
	return {&link_head[0], &link_head[0]+link_head.size()};
}

inline Span<uint32_t> VecOSMCarRoads::link_tail_as_ref()noexcept{
	return {&link_tail[0], &link_tail[0]+link_tail.size()};
}

inline Span<uint32_t> VecOSMCarRoads::link_length_in_cm_as_ref()noexcept{
	return {&link_length_in_cm[0], &link_length_in_cm[0]+link_length_in_cm.size()};
}

inline Span<LatLon> VecOSMCarRoads::node_pos_as_ref()noexcept{
	return {&node_pos[0], &node_pos[0]+node_pos.size()};
}

inline Span<uint32_t> VecOSMCarRoads::first_shape_pos_of_link_as_ref()noexcept{
	return {&first_shape_pos_of_link[0], &first_shape_pos_of_link[0]+first_shape_pos_of_link.size()};
}

inline Span<LatLon> VecOSMCarRoads::shape_pos_as_ref()noexcept{
	return {&shape_pos[0], &shape_pos[0]+shape_pos.size()};
}

inline Span<uint32_t> VecOSMCarRoads::dlink_traversal_time_in_ms_as_ref()noexcept{
	return {&dlink_traversal_time_in_ms[0], &dlink_traversal_time_in_ms[0]+dlink_traversal_time_in_ms.size()};
}

inline Span<uint32_t> VecOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()noexcept{
	return {&first_dlink_of_forbidden_maneuver[0], &first_dlink_of_forbidden_maneuver[0]+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<uint32_t> VecOSMCarRoads::forbidden_maneuver_dlink_as_ref()noexcept{
	return {&forbidden_maneuver_dlink[0], &forbidden_maneuver_dlink[0]+forbidden_maneuver_dlink.size()};
}

inline Span<uint32_t> VecOSMCarRoads::first_link_of_way_as_ref()noexcept{
	return {&first_link_of_way[0], &first_link_of_way[0]+first_link_of_way.size()};
}

inline Span<uint64_t> VecOSMCarRoads::node_osm_id_as_ref()noexcept{
	return {&node_osm_id[0], &node_osm_id[0]+node_osm_id.size()};
}

inline Span<uint64_t> VecOSMCarRoads::way_osm_id_as_ref()noexcept{
	return {&way_osm_id[0], &way_osm_id[0]+way_osm_id.size()};
}

inline Span<uint64_t> VecOSMCarRoads::forbidden_maneuver_osm_id_as_ref()noexcept{
	return {&forbidden_maneuver_osm_id[0], &forbidden_maneuver_osm_id[0]+forbidden_maneuver_osm_id.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> VecOSMCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> VecOSMCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::first_link_of_way_as_ref()const noexcept{
	return first_link_of_way_as_cref();
}

inline Span<const uint64_t> VecOSMCarRoads::node_osm_id_as_ref()const noexcept{
	return node_osm_id_as_cref();
}

inline Span<const uint64_t> VecOSMCarRoads::way_osm_id_as_ref()const noexcept{
	return way_osm_id_as_cref();
}

inline Span<const uint64_t> VecOSMCarRoads::forbidden_maneuver_osm_id_as_ref()const noexcept{
	return forbidden_maneuver_osm_id_as_cref();
}

inline Span<const uint32_t> VecOSMCarRoads::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> VecOSMCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> VecOSMCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms.data(), dlink_traversal_time_in_ms.data()+dlink_traversal_time_in_ms.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver.data(), first_dlink_of_forbidden_maneuver.data()+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink.data(), forbidden_maneuver_dlink.data()+forbidden_maneuver_dlink.size()};
}

inline Span<const uint32_t> VecOSMCarRoads::first_link_of_way_as_cref()const noexcept{
	return {first_link_of_way.data(), first_link_of_way.data()+first_link_of_way.size()};
}

inline Span<const uint64_t> VecOSMCarRoads::node_osm_id_as_cref()const noexcept{
	return {node_osm_id.data(), node_osm_id.data()+node_osm_id.size()};
}

inline Span<const uint64_t> VecOSMCarRoads::way_osm_id_as_cref()const noexcept{
	return {way_osm_id.data(), way_osm_id.data()+way_osm_id.size()};
}

inline Span<const uint64_t> VecOSMCarRoads::forbidden_maneuver_osm_id_as_cref()const noexcept{
	return {forbidden_maneuver_osm_id.data(), forbidden_maneuver_osm_id.data()+forbidden_maneuver_osm_id.size()};
}


inline VecOSMCarRoads::VecOSMCarRoads(){}

inline VecOSMCarRoads::VecOSMCarRoads(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	size_t way_count
):
	node_count(node_count),
	link_count(link_count),
	shape_pos_count(shape_pos_count),
	forbidden_maneuver_count(forbidden_maneuver_count),
	forbidden_maneuver_dlink_count(forbidden_maneuver_dlink_count),
	way_count(way_count),
	link_head(link_count),
	link_tail(link_count),
	link_length_in_cm(link_count),
	node_pos(node_count),
	first_shape_pos_of_link(link_count + 1),
	shape_pos(shape_pos_count),
	dlink_traversal_time_in_ms(2*link_count),
	first_dlink_of_forbidden_maneuver(forbidden_maneuver_count + 1),
	forbidden_maneuver_dlink(forbidden_maneuver_dlink_count),
	first_link_of_way(way_count + 1),
	node_osm_id(node_count),
	way_osm_id(way_count),
	forbidden_maneuver_osm_id(forbidden_maneuver_count){}

inline void VecOSMCarRoads::resize(
	size_t node_count,
	size_t link_count,
	size_t shape_pos_count,
	size_t forbidden_maneuver_count,
	size_t forbidden_maneuver_dlink_count,
	size_t way_count
){
	this->node_count = node_count;
	this->link_count = link_count;
	this->shape_pos_count = shape_pos_count;
	this->forbidden_maneuver_count = forbidden_maneuver_count;
	this->forbidden_maneuver_dlink_count = forbidden_maneuver_dlink_count;
	this->way_count = way_count;
	link_head.resize(link_count);
	link_tail.resize(link_count);
	link_length_in_cm.resize(link_count);
	node_pos.resize(node_count);
	first_shape_pos_of_link.resize(link_count + 1);
	shape_pos.resize(shape_pos_count);
	dlink_traversal_time_in_ms.resize(2*link_count);
	first_dlink_of_forbidden_maneuver.resize(forbidden_maneuver_count + 1);
	forbidden_maneuver_dlink.resize(forbidden_maneuver_dlink_count);
	first_link_of_way.resize(way_count + 1);
	node_osm_id.resize(node_count);
	way_osm_id.resize(way_count);
	forbidden_maneuver_osm_id.resize(forbidden_maneuver_count);
}

inline void VecOSMCarRoads::throw_if_wrong_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(dlink_traversal_time_in_ms.size() != 2*link_count)
		 err += ("Column dlink_traversal_time_in_ms has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(dlink_traversal_time_in_ms.size())+"\n");
	if(first_dlink_of_forbidden_maneuver.size() != forbidden_maneuver_count + 1)
		 err += ("Column first_dlink_of_forbidden_maneuver has wrong size. Expected: "+std::to_string(forbidden_maneuver_count + 1)+" Actual: "+std::to_string(first_dlink_of_forbidden_maneuver.size())+"\n");
	if(forbidden_maneuver_dlink.size() != forbidden_maneuver_dlink_count)
		 err += ("Column forbidden_maneuver_dlink has wrong size. Expected: "+std::to_string(forbidden_maneuver_dlink_count)+" Actual: "+std::to_string(forbidden_maneuver_dlink.size())+"\n");
	if(first_link_of_way.size() != way_count + 1)
		 err += ("Column first_link_of_way has wrong size. Expected: "+std::to_string(way_count + 1)+" Actual: "+std::to_string(first_link_of_way.size())+"\n");
	if(node_osm_id.size() != node_count)
		 err += ("Column node_osm_id has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_osm_id.size())+"\n");
	if(way_osm_id.size() != way_count)
		 err += ("Column way_osm_id has wrong size. Expected: "+std::to_string(way_count)+" Actual: "+std::to_string(way_osm_id.size())+"\n");
	if(forbidden_maneuver_osm_id.size() != forbidden_maneuver_count)
		 err += ("Column forbidden_maneuver_osm_id has wrong size. Expected: "+std::to_string(forbidden_maneuver_count)+" Actual: "+std::to_string(forbidden_maneuver_osm_id.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline void VecOSMCarRoads::assert_correct_size()const noexcept{
	assert(link_head.size() == link_count);
	assert(link_tail.size() == link_count);
	assert(link_length_in_cm.size() == link_count);
	assert(node_pos.size() == node_count);
	assert(first_shape_pos_of_link.size() == link_count + 1);
	assert(shape_pos.size() == shape_pos_count);
	assert(dlink_traversal_time_in_ms.size() == 2*link_count);
	assert(first_dlink_of_forbidden_maneuver.size() == forbidden_maneuver_count + 1);
	assert(forbidden_maneuver_dlink.size() == forbidden_maneuver_dlink_count);
	assert(first_link_of_way.size() == way_count + 1);
	assert(node_osm_id.size() == node_count);
	assert(way_osm_id.size() == way_count);
	assert(forbidden_maneuver_osm_id.size() == forbidden_maneuver_count);
}

inline void VecOSMCarRoads::shrink_to_fit(){
	link_head.shrink_to_fit();
	link_tail.shrink_to_fit();
	link_length_in_cm.shrink_to_fit();
	node_pos.shrink_to_fit();
	first_shape_pos_of_link.shrink_to_fit();
	shape_pos.shrink_to_fit();
	dlink_traversal_time_in_ms.shrink_to_fit();
	first_dlink_of_forbidden_maneuver.shrink_to_fit();
	forbidden_maneuver_dlink.shrink_to_fit();
	first_link_of_way.shrink_to_fit();
	node_osm_id.shrink_to_fit();
	way_osm_id.shrink_to_fit();
	forbidden_maneuver_osm_id.shrink_to_fit();
}

inline ConstRefOSMCarRoads VecOSMCarRoads::as_ref()const noexcept{
	return as_cref();
}

inline RefOSMCarRoads VecOSMCarRoads::as_ref()noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		way_count,
		&link_head[0],
		&link_tail[0],
		&link_length_in_cm[0],
		&node_pos[0],
		&first_shape_pos_of_link[0],
		&shape_pos[0],
		&dlink_traversal_time_in_ms[0],
		&first_dlink_of_forbidden_maneuver[0],
		&forbidden_maneuver_dlink[0],
		&first_link_of_way[0],
		&node_osm_id[0],
		&way_osm_id[0],
		&forbidden_maneuver_osm_id[0]
	};
}

inline ConstRefOSMCarRoads VecOSMCarRoads::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		way_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data(),
		dlink_traversal_time_in_ms.data(),
		first_dlink_of_forbidden_maneuver.data(),
		forbidden_maneuver_dlink.data(),
		first_link_of_way.data(),
		node_osm_id.data(),
		way_osm_id.data(),
		forbidden_maneuver_osm_id.data()
	};
}

inline DirOSMCarRoads::DirOSMCarRoads(){}

inline DirOSMCarRoads::DirOSMCarRoads(std::string dir){
	append_dir_slash_if_needed(dir);
	{
		FileDataSource src(dir+"node_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&node_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"link_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&link_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"shape_pos_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&shape_pos_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"forbidden_maneuver_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&forbidden_maneuver_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"forbidden_maneuver_dlink_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&forbidden_maneuver_dlink_count, sizeof(size_t));
	}
	{
		FileDataSource src(dir+"way_count");
		read_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&way_count, sizeof(size_t));
	}
	link_head = FileArray<uint32_t>(dir+"link_head");
	link_tail = FileArray<uint32_t>(dir+"link_tail");
	link_length_in_cm = FileArray<uint32_t>(dir+"link_length_in_cm");
	node_pos = FileArray<LatLon>(dir+"node_pos");
	first_shape_pos_of_link = FileArray<uint32_t>(dir+"first_shape_pos_of_link");
	shape_pos = FileArray<LatLon>(dir+"shape_pos");
	dlink_traversal_time_in_ms = FileArray<uint32_t>(dir+"dlink_traversal_time_in_ms");
	first_dlink_of_forbidden_maneuver = FileArray<uint32_t>(dir+"first_dlink_of_forbidden_maneuver");
	forbidden_maneuver_dlink = FileArray<uint32_t>(dir+"forbidden_maneuver_dlink");
	first_link_of_way = FileArray<uint32_t>(dir+"first_link_of_way");
	node_osm_id = FileArray<uint64_t>(dir+"node_osm_id");
	way_osm_id = FileArray<uint64_t>(dir+"way_osm_id");
	forbidden_maneuver_osm_id = FileArray<uint64_t>(dir+"forbidden_maneuver_osm_id");
	throw_if_wrong_col_size();
}

inline void DirOSMCarRoads::open(std::string dir){
	*this = DirOSMCarRoads(std::move(dir));
}

inline void DirOSMCarRoads::close(){
	*this = DirOSMCarRoads();
}

inline void DirOSMCarRoads::throw_if_wrong_col_size()const{
	std::string err;
	if(link_head.size() != link_count)
		 err += ("Column link_head has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_head.size())+"\n");
	if(link_tail.size() != link_count)
		 err += ("Column link_tail has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_tail.size())+"\n");
	if(link_length_in_cm.size() != link_count)
		 err += ("Column link_length_in_cm has wrong size. Expected: "+std::to_string(link_count)+" Actual: "+std::to_string(link_length_in_cm.size())+"\n");
	if(node_pos.size() != node_count)
		 err += ("Column node_pos has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_pos.size())+"\n");
	if(first_shape_pos_of_link.size() != link_count + 1)
		 err += ("Column first_shape_pos_of_link has wrong size. Expected: "+std::to_string(link_count + 1)+" Actual: "+std::to_string(first_shape_pos_of_link.size())+"\n");
	if(shape_pos.size() != shape_pos_count)
		 err += ("Column shape_pos has wrong size. Expected: "+std::to_string(shape_pos_count)+" Actual: "+std::to_string(shape_pos.size())+"\n");
	if(dlink_traversal_time_in_ms.size() != 2*link_count)
		 err += ("Column dlink_traversal_time_in_ms has wrong size. Expected: "+std::to_string(2*link_count)+" Actual: "+std::to_string(dlink_traversal_time_in_ms.size())+"\n");
	if(first_dlink_of_forbidden_maneuver.size() != forbidden_maneuver_count + 1)
		 err += ("Column first_dlink_of_forbidden_maneuver has wrong size. Expected: "+std::to_string(forbidden_maneuver_count + 1)+" Actual: "+std::to_string(first_dlink_of_forbidden_maneuver.size())+"\n");
	if(forbidden_maneuver_dlink.size() != forbidden_maneuver_dlink_count)
		 err += ("Column forbidden_maneuver_dlink has wrong size. Expected: "+std::to_string(forbidden_maneuver_dlink_count)+" Actual: "+std::to_string(forbidden_maneuver_dlink.size())+"\n");
	if(first_link_of_way.size() != way_count + 1)
		 err += ("Column first_link_of_way has wrong size. Expected: "+std::to_string(way_count + 1)+" Actual: "+std::to_string(first_link_of_way.size())+"\n");
	if(node_osm_id.size() != node_count)
		 err += ("Column node_osm_id has wrong size. Expected: "+std::to_string(node_count)+" Actual: "+std::to_string(node_osm_id.size())+"\n");
	if(way_osm_id.size() != way_count)
		 err += ("Column way_osm_id has wrong size. Expected: "+std::to_string(way_count)+" Actual: "+std::to_string(way_osm_id.size())+"\n");
	if(forbidden_maneuver_osm_id.size() != forbidden_maneuver_count)
		 err += ("Column forbidden_maneuver_osm_id has wrong size. Expected: "+std::to_string(forbidden_maneuver_count)+" Actual: "+std::to_string(forbidden_maneuver_osm_id.size())+"\n");
	if(!err.empty())
		throw std::runtime_error(err);
}

inline ConstRefOSMCarRoads DirOSMCarRoads::as_cref()const noexcept{
	return {
		node_count,
		link_count,
		shape_pos_count,
		forbidden_maneuver_count,
		forbidden_maneuver_dlink_count,
		way_count,
		link_head.data(),
		link_tail.data(),
		link_length_in_cm.data(),
		node_pos.data(),
		first_shape_pos_of_link.data(),
		shape_pos.data(),
		dlink_traversal_time_in_ms.data(),
		first_dlink_of_forbidden_maneuver.data(),
		forbidden_maneuver_dlink.data(),
		first_link_of_way.data(),
		node_osm_id.data(),
		way_osm_id.data(),
		forbidden_maneuver_osm_id.data()
	};
}

inline ConstRefOSMCarRoads DirOSMCarRoads::as_ref()const noexcept{
	return as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::link_head_as_ref()const noexcept{
	return link_head_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::link_tail_as_ref()const noexcept{
	return link_tail_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::link_length_in_cm_as_ref()const noexcept{
	return link_length_in_cm_as_cref();
}

inline Span<const LatLon> DirOSMCarRoads::node_pos_as_ref()const noexcept{
	return node_pos_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::first_shape_pos_of_link_as_ref()const noexcept{
	return first_shape_pos_of_link_as_cref();
}

inline Span<const LatLon> DirOSMCarRoads::shape_pos_as_ref()const noexcept{
	return shape_pos_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::dlink_traversal_time_in_ms_as_ref()const noexcept{
	return dlink_traversal_time_in_ms_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::first_dlink_of_forbidden_maneuver_as_ref()const noexcept{
	return first_dlink_of_forbidden_maneuver_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::forbidden_maneuver_dlink_as_ref()const noexcept{
	return forbidden_maneuver_dlink_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::first_link_of_way_as_ref()const noexcept{
	return first_link_of_way_as_cref();
}

inline Span<const uint64_t> DirOSMCarRoads::node_osm_id_as_ref()const noexcept{
	return node_osm_id_as_cref();
}

inline Span<const uint64_t> DirOSMCarRoads::way_osm_id_as_ref()const noexcept{
	return way_osm_id_as_cref();
}

inline Span<const uint64_t> DirOSMCarRoads::forbidden_maneuver_osm_id_as_ref()const noexcept{
	return forbidden_maneuver_osm_id_as_cref();
}

inline Span<const uint32_t> DirOSMCarRoads::link_head_as_cref()const noexcept{
	return {link_head.data(), link_head.data()+link_head.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::link_tail_as_cref()const noexcept{
	return {link_tail.data(), link_tail.data()+link_tail.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::link_length_in_cm_as_cref()const noexcept{
	return {link_length_in_cm.data(), link_length_in_cm.data()+link_length_in_cm.size()};
}

inline Span<const LatLon> DirOSMCarRoads::node_pos_as_cref()const noexcept{
	return {node_pos.data(), node_pos.data()+node_pos.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::first_shape_pos_of_link_as_cref()const noexcept{
	return {first_shape_pos_of_link.data(), first_shape_pos_of_link.data()+first_shape_pos_of_link.size()};
}

inline Span<const LatLon> DirOSMCarRoads::shape_pos_as_cref()const noexcept{
	return {shape_pos.data(), shape_pos.data()+shape_pos.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::dlink_traversal_time_in_ms_as_cref()const noexcept{
	return {dlink_traversal_time_in_ms.data(), dlink_traversal_time_in_ms.data()+dlink_traversal_time_in_ms.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::first_dlink_of_forbidden_maneuver_as_cref()const noexcept{
	return {first_dlink_of_forbidden_maneuver.data(), first_dlink_of_forbidden_maneuver.data()+first_dlink_of_forbidden_maneuver.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::forbidden_maneuver_dlink_as_cref()const noexcept{
	return {forbidden_maneuver_dlink.data(), forbidden_maneuver_dlink.data()+forbidden_maneuver_dlink.size()};
}

inline Span<const uint32_t> DirOSMCarRoads::first_link_of_way_as_cref()const noexcept{
	return {first_link_of_way.data(), first_link_of_way.data()+first_link_of_way.size()};
}

inline Span<const uint64_t> DirOSMCarRoads::node_osm_id_as_cref()const noexcept{
	return {node_osm_id.data(), node_osm_id.data()+node_osm_id.size()};
}

inline Span<const uint64_t> DirOSMCarRoads::way_osm_id_as_cref()const noexcept{
	return {way_osm_id.data(), way_osm_id.data()+way_osm_id.size()};
}

inline Span<const uint64_t> DirOSMCarRoads::forbidden_maneuver_osm_id_as_cref()const noexcept{
	return {forbidden_maneuver_osm_id.data(), forbidden_maneuver_osm_id.data()+forbidden_maneuver_osm_id.size()};
}


inline void dump_into_dir(std::string dir, RefOSMCarRoads data){
	dump_into_dir(std::move(dir), ConstRefOSMCarRoads(data));
}

inline void dump_into_dir(std::string dir, ConstRefOSMCarRoads data){
	append_dir_slash_if_needed(dir);
	FileDataSink(dir+"node_count")((const uint8_t*)&data.node_count, sizeof(size_t));
	FileDataSink(dir+"link_count")((const uint8_t*)&data.link_count, sizeof(size_t));
	FileDataSink(dir+"shape_pos_count")((const uint8_t*)&data.shape_pos_count, sizeof(size_t));
	FileDataSink(dir+"forbidden_maneuver_count")((const uint8_t*)&data.forbidden_maneuver_count, sizeof(size_t));
	FileDataSink(dir+"forbidden_maneuver_dlink_count")((const uint8_t*)&data.forbidden_maneuver_dlink_count, sizeof(size_t));
	FileDataSink(dir+"way_count")((const uint8_t*)&data.way_count, sizeof(size_t));
	FileDataSink(dir+"link_head")((const uint8_t*)data.link_head, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_tail")((const uint8_t*)data.link_tail, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"link_length_in_cm")((const uint8_t*)data.link_length_in_cm, (data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"node_pos")((const uint8_t*)data.node_pos, (data.node_count)*sizeof(LatLon));
	FileDataSink(dir+"first_shape_pos_of_link")((const uint8_t*)data.first_shape_pos_of_link, (data.link_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"shape_pos")((const uint8_t*)data.shape_pos, (data.shape_pos_count)*sizeof(LatLon));
	FileDataSink(dir+"dlink_traversal_time_in_ms")((const uint8_t*)data.dlink_traversal_time_in_ms, (2*data.link_count)*sizeof(uint32_t));
	FileDataSink(dir+"first_dlink_of_forbidden_maneuver")((const uint8_t*)data.first_dlink_of_forbidden_maneuver, (data.forbidden_maneuver_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"forbidden_maneuver_dlink")((const uint8_t*)data.forbidden_maneuver_dlink, (data.forbidden_maneuver_dlink_count)*sizeof(uint32_t));
	FileDataSink(dir+"first_link_of_way")((const uint8_t*)data.first_link_of_way, (data.way_count + 1)*sizeof(uint32_t));
	FileDataSink(dir+"node_osm_id")((const uint8_t*)data.node_osm_id, (data.node_count)*sizeof(uint64_t));
	FileDataSink(dir+"way_osm_id")((const uint8_t*)data.way_osm_id, (data.way_count)*sizeof(uint64_t));
	FileDataSink(dir+"forbidden_maneuver_osm_id")((const uint8_t*)data.forbidden_maneuver_osm_id, (data.forbidden_maneuver_count)*sizeof(uint64_t));
}

} // namespace RoutingKit2

#endif
