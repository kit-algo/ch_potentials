#include <routingkit/vector_io.h>
#include <routingkit/geo_position_to_node.h>
#include <routingkit/geo_dist.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <limits>
#include <iostream>
#include <cassert>
using namespace std;
using namespace RoutingKit;

struct Color{
	unsigned char red,green,blue;
};

unsigned img_width, img_height;
std::vector<Color>img;

std::string img_file_name;
std::string node_seq_file_name;

double min_lat, max_lat, min_lon, max_lon;
double scale;

std::vector<unsigned>first_out;
std::vector<unsigned>head;
std::vector<unsigned>tail;
std::vector<float>latitude;
std::vector<float>longitude;
std::vector<unsigned>first_modelling_node;
std::vector<float>modelling_node_latitude;
std::vector<float>modelling_node_longitude;
std::vector<unsigned>geo_distance;

void load(){
	first_out = load_vector<unsigned>("first_out");
	head = load_vector<unsigned>("head");
	tail = load_vector<unsigned>("tail");
	latitude = load_vector<float>("latitude");
	longitude = load_vector<float>("longitude");
	first_modelling_node = load_vector<unsigned>("first_modelling_node");
	modelling_node_latitude = load_vector<float>("modelling_node_latitude");
	modelling_node_longitude = load_vector<float>("modelling_node_longitude");
	geo_distance = load_vector<unsigned>("geo_distance");
}

GeoPositionToNode node_geo_index;

std::vector<float>arc_geo_index_id;
GeoPositionToNode arc_geo_index;

double blend_double(double f, double a, double b){
	return (1.0-f)*a + f*b;
}

void setup_geo_index(){
	node_geo_index = GeoPositionToNode(latitude, longitude);
}

std::vector<unsigned>long_arcs;

const double long_arc_threshold = 4000;

void find_long_arcs(){
	for(unsigned a=0; a<head.size(); ++a)
		if(geo_distance[a] > long_arc_threshold)
			long_arcs.push_back(a);
	cout << "found "<<long_arcs.size() << " long arcs" << endl;
}

std::vector<float>node_color;

void load_node_seq(){
	node_color.resize(first_out.size()-1, -1);

	std::vector<unsigned> node_seq = load_vector<unsigned>(node_seq_file_name);

	for(unsigned i=0; i<node_seq.size(); ++i){
		node_color[node_seq[i]] = float(i) / (node_seq.size()-1);
	}
}

void save_ppm_image(){
	FILE *fp = fopen(img_file_name.c_str(), "wb");
	fprintf(fp, "P6\n%d %d\n255\n", img_width, img_height);
	for (unsigned y = 0; y < img_height; ++y)
	{
		for (unsigned x = 0; x < img_width; ++x)
		{
			unsigned char color[3];
			color[0] = img[y*img_width + x].red;
			color[1] = img[y*img_width + x].green;
			color[2] = img[y*img_width + x].blue;
			fwrite(color, 1, 3, fp);
		}
	}
	fclose(fp);
}

struct GeoPos{
	double lat, lon;
};

struct ImgPos{
	double x, y;
};

double sqr(double x){
	return x*x;
}

double compute_sqr_distance(ImgPos p, ImgPos q){
	return sqr(p.x-q.x) + sqr(p.y-q.y);
}

double compute_distance(ImgPos p, ImgPos q){
	return sqrt(compute_sqr_distance(p, q));
}

ImgPos project_point_onto_segment(ImgPos p, ImgPos v, ImgPos w){
	ImgPos proj;
	double l2 = compute_sqr_distance(v, w);
	if (l2 == 0.0) {
		proj = v;
	}else{
		double dot = (p.x - v.x)*(w.x - v.x)+(p.y - v.y)*(w.y - v.y);
		double t = dot / l2;
		if(t < 0.0)
			t = 0.0;
		else if(t > 1.0)
			t = 1.0;
		proj.x = v.x + t * (w.x - v.x);
		proj.y = v.y + t * (w.y - v.y);
	}
	return proj;
}

ImgPos to_img_pos(GeoPos p){
	ImgPos ret;
	ret.y = (p.lat - min_lat) * (img_height-1) / (max_lat-min_lat);
	ret.x = (p.lon - min_lon) * (img_width-1) / (max_lon-min_lon);
	ret.y = img_height - 1 - ret.y;
	return ret;
}

GeoPos to_geo_pos(ImgPos p){
	GeoPos ret;
	p.y = img_height - 1 - p.y;
	ret.lat = min_lat + (max_lat-min_lat) * p.y / (img_height-1);
	ret.lon = min_lon + (max_lon-min_lon) * p.x / (img_width-1);
	return ret;
}

struct DoubleColor{
	double red, green, blue; // between 0 and 255
};

DoubleColor clip_rounding_error(DoubleColor ret){
	assert(-0.5 <= ret.red && ret.red <= 255.5);
	assert(-0.5 <= ret.green && ret.green <= 255.5);
	assert(-0.5 <= ret.blue && ret.blue <= 255.5);
	if(ret.red < 0.0)
		ret.red = 0.0;
	if(ret.green < 0.0)
		ret.green = 0.0;
	if(ret.blue < 0.0)
		ret.blue = 0.0;
	if(ret.red > 255.0)
		ret.red = 255.0;
	if(ret.green > 255.0)
		ret.green = 255.0;
	if(ret.blue > 255.0)
		ret.blue = 255.0;
	return ret;
}

double sin01(double x){
	double ret = (sin((x*M_PI-M_PI/2.0))+1.0)/2.0;
	if(ret < 0.0)
		ret = 0.0;
	if(ret > 1.0)
		ret = 1.0;
	return ret;
};

DoubleColor blend_color(double f, DoubleColor a, DoubleColor b){
	f = sin01(f);
	DoubleColor r;
	r.red = blend_double(f, a.red, b.red);
	r.green = blend_double(f, a.green, b.green);
	r.blue = blend_double(f, a.blue, b.blue);
	return r;
}

DoubleColor compute_node_color(unsigned node_id){
	/*Disco mode

	return DoubleColor{
		(111*node_id)%255,
		(127*node_id)%255,
		(201*node_id)%255
	};*/

	assert(0 <= node_color[node_id] && node_color[node_id] <= 1);

	return blend_color(node_color[node_id], DoubleColor{0,0,255}, DoubleColor{255,0,0});
}

bool has_node_color(unsigned node_id){
	return node_color[node_id] >= 0;
}

double compute_distance_to_node(ImgPos img_query_pos, unsigned node_id){
	GeoPos geo_node_pos = {latitude[node_id], longitude[node_id]};
	ImgPos img_node_pos = to_img_pos(geo_node_pos);
	return compute_distance(img_query_pos, img_node_pos);
}

struct ArcDistanceResult{
	double distance;
	double pos_along_arc; // between 0 and 1
};

ArcDistanceResult compute_distance_to_arc(ImgPos img_query_pos, unsigned arc_id){
	unsigned begin = first_modelling_node[arc_id], end = first_modelling_node[arc_id+1];

	auto get_node_img_pos = [&](unsigned node_id){
		GeoPos p;
		p.lat = latitude[node_id];
		p.lon = longitude[node_id];
		return to_img_pos(p);
	};

	auto get_modelling_img_pos = [&](unsigned node_id){
		GeoPos p;
		p.lat = modelling_node_latitude[node_id];
		p.lon = modelling_node_longitude[node_id];
		return to_img_pos(p);
	};

	double min_distance_to_arc = std::numeric_limits<double>::infinity();
	double total_arc_distance = 0;
	double distance_along_arc;

	auto check_seg = [&](ImgPos a, ImgPos b){
		ImgPos proj = project_point_onto_segment(img_query_pos, a, b);

		double proj_distance = compute_distance(proj, img_query_pos);
		if(proj_distance < min_distance_to_arc){
			min_distance_to_arc = proj_distance;
			distance_along_arc = total_arc_distance + compute_distance(a, proj);
		}
		total_arc_distance += compute_distance(a,b);
	};

	if(begin == end){
		check_seg(get_node_img_pos(tail[arc_id]), get_node_img_pos(head[arc_id]));
	}else{
		check_seg(get_node_img_pos(tail[arc_id]), get_modelling_img_pos(begin));
		// FIXME: This loop is slow as fuck
		for(unsigned i=begin+1; i<end; ++i){
			check_seg(get_modelling_img_pos(i-1), get_modelling_img_pos(i));
		}
		check_seg(get_modelling_img_pos(end-1), get_node_img_pos(head[arc_id]));
	}

	return ArcDistanceResult{min_distance_to_arc, distance_along_arc/total_arc_distance};
}

double strength_from_distance(double dist, double inner_radius, double outer_radius){
	assert(0<=dist);
	assert(0 < inner_radius);
	assert(inner_radius < outer_radius);

	dist *= scale;

	double ret;
	if(dist <= inner_radius)
		ret = 1.0;
	else if(dist >= outer_radius)
		ret = 0.0;
	else
		ret = sin01(1.0-((dist - inner_radius) / (outer_radius - inner_radius)));
	assert(0.0 <= ret && ret <= 1.0);
	return ret;
}


struct ColorMixer{

	void add(DoubleColor new_color, double strength){
		assert(0.0 <= new_color.red && new_color.red <= 255.0);
		assert(0.0 <= new_color.green && new_color.green <= 255.0);
		assert(0.0 <= new_color.blue && new_color.blue <= 255.0);
		total_strength += strength;
		color.red += strength*new_color.red;
		color.green += strength*new_color.green;
		color.blue += strength*new_color.blue;
	}

	DoubleColor get()const{
		double f = 1.0/total_strength;
		DoubleColor ret = color;
		ret.red *= f;
		ret.green *= f;
		ret.blue *= f;

		return clip_rounding_error(ret);
	}

	DoubleColor color{0,0,0};
	double total_strength = 0;
};

Color compute_geo_pos_color(ImgPos img_query_pos){
	GeoPos geo_query_pos = to_geo_pos(img_query_pos);

	constexpr int layer_count = 2;

	ColorMixer layer_color_mixer[layer_count];
	double layer_max_strength[layer_count];

	for(int i=0; i<layer_count; ++i)
		layer_max_strength[i] = 0;

	auto add_color = [&](int layer, DoubleColor col, double strength){
		if(strength > 0){
			layer_color_mixer[layer].add(col, strength);
			if(strength > layer_max_strength[layer]){
				layer_max_strength[layer] = strength;
			}
		}
	};

	auto handle_node = [&](unsigned node_id){
		double node_dist = compute_distance_to_node(img_query_pos, node_id);
		if(has_node_color(node_id)){
			add_color(1, compute_node_color(node_id), strength_from_distance(node_dist, 5, 7));
		}else{
			add_color(0, DoubleColor{0,0,0}, strength_from_distance(node_dist, 3, 5));
		}
	};

	auto handle_arc = [&](unsigned arc){
		unsigned t = tail[arc];
		unsigned h = head[arc];

		auto d = compute_distance_to_arc(img_query_pos, arc);
		bool has_arc_color = has_node_color(t) && has_node_color(h);
		if(has_arc_color){
			DoubleColor arc_color = clip_rounding_error(blend_color(d.pos_along_arc, compute_node_color(t), compute_node_color(h)));
			double arc_color_strength = strength_from_distance(d.distance, 2, 4);
			add_color(1, arc_color, arc_color_strength);
		}else{
			double arc_color_strength = strength_from_distance(d.distance, 1, 3);
			add_color(0, DoubleColor{0,0,0}, arc_color_strength);
		}
	};

	for(auto node : node_geo_index.find_all_nodes_within_radius(geo_query_pos.lat, geo_query_pos.lon, long_arc_threshold)){
		if(node.id == invalid_id)
			continue;

		handle_node(node.id);

		for(unsigned arc = first_out[node.id]; arc < first_out[node.id+1]; ++arc){
			handle_arc(arc);
		}
	}

	// FIXME: This loop is slow as fuck
	for(auto arc:long_arcs){
		handle_arc(arc);
	}

	DoubleColor final_col = {255,255,255};

	for(int i=0; i<layer_count; ++i)
		if(layer_max_strength[i] > 0)
			final_col = blend_color(layer_max_strength[i], final_col, layer_color_mixer[i].get());

	Color ret;
	ret.red = static_cast<unsigned char>(final_col.red);
	ret.green = static_cast<unsigned char>(final_col.green);
	ret.blue = static_cast<unsigned char>(final_col.blue);
	return ret;
}

void render(){
	#pragma omp parallel for schedule(dynamic)
	for(unsigned y=0; y<img_height; ++y){
		for(unsigned x=0; x<img_width; ++x){
			img[y*img_width + x] = compute_geo_pos_color(ImgPos{(double)x, (double)y});
		}
		cout << "row " << y << " finished" << endl;
	}
}

int main(int argc, char*argv[]){

	if(argc != 10){
		cerr << "usage "<<argv[0] << " min_lat max_lat min_lon max_lon img_width img_height element_scale node_sequence out.ppm" <<endl;
		cerr << "Renders a road graph and a node sequence, The road graph is read from the current working directory. ";
		cerr << "The output image is written to the file out.ppm . This can be converted to a more widely used image format using convert as follows:";
		cerr << "convert out.ppm out.png" << endl;
		return 1;
	}

	min_lat = atof(argv[1]);
	max_lat = atof(argv[2]);
	min_lon = atof(argv[3]);
	max_lon = atof(argv[4]);

	img_width = atof(argv[5]);
	img_height = atof(argv[6]);
	scale = 1.0/atof(argv[7]);

	node_seq_file_name = argv[8];
	img_file_name = argv[9];

	load();
	find_long_arcs();

	setup_geo_index();

	load_node_seq();

	img.resize(img_width * img_height);

	render();

	save_ppm_image();
}
