#include "http_server.h"
#include "map.h"
#include "gpoly.h"
#include "geo_index.h"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>

using namespace RoutingKit2;
using namespace std;

struct ServerData{
	ConstRefLinkShapes link_shapes;
	ConstRefGeoIndex link_geo_index;
};

struct WorkerData{
    LinkGeoIndexFindWithinRadiusQuery link_geo_query;

    WorkerData(int worker_id, int worker_count, const ServerData&server_data):
        link_geo_query(server_data.link_shapes, server_data.link_geo_index){
        (void)worker_id;
        (void)worker_count;
        (void)server_data;
    }
};


void serve_index(LatLon center, http::Response&res){

	res.body = R"XXX(
<!DOCTYPE html>
<html>
<head>
	<meta charset="utf-8" />
	<title>RoutingKit2</title>

	<meta name="viewport" content="width=device-width, initial-scale=1.0">

    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.5.1/dist/leaflet.css" integrity="sha512-xwE/Az9zrjBIphAcBb3F6JVqxf46+CDLwfLMHloNu6KEQCAWi6HcDUbeOfBIptF7tcCzusKFjFw2yuvEpDL9wQ==" crossorigin=""></link>
    <script src="https://unpkg.com/leaflet@1.5.1/dist/leaflet.js" integrity="sha512-GffPMF3RvMeYyc1LWMHtK8EbPv0iNZ8/oTtHPx9/cc2ILxQ+u905qIwdpULaqDkyBKgOaB57QTMg7ztg8Jm2Og==" crossorigin=""></script>
    <script src="https://unpkg.com/polyline-encoded@0.0.8/Polyline.encoded.js" integrity="sha512-IxcXX9OwJ72ucNMR833ngaxl3HIXfrm1ZdnHJFpXOhJeLNLfkM/q0iL6lGVt8Xt4yl124ybQn+F/6L+ZmH57kg==" crossorigin=""></script>

    <style>
        body {
            padding: 0;
            margin: 0;
        }
        html, body, #map {
            height: 100%;
            width: 100%;
        }
    </style>

</head>
<body>
<div id="map"></div>
<script>
	var map = L.map('map');

	L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
		maxZoom: 18,
		attribution: 'Map data &copy; <a href="https://www.openstreetmap.org/">OpenStreetMap</a> contributors'
	}).addTo(map);

)XXX";
	res.body += "\tlet center = L.latLng("+std::to_string(center.lat())+","+std::to_string(center.lon())+");\n";
	res.body += R"XXX(
	map.setView(center, 13);
	let drag_marker = L.marker(center, {draggable: 'true'});
	drag_marker.addTo(map);

	let current_group = null;

	let last_request_start = 0;
	let last_request_end = 0;

	function request_polylines(pos, radius){
		let now = Date.now();

		if(
			(now - last_request_end > 0.1 && last_request_start < last_request_end) ||
			now - last_request_start > 1
		){
			last_request_start = now;

			let r = new XMLHttpRequest();
			r.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {

					let gpoly_list = r.responseText.split('\n');
					let new_group = L.layerGroup();

					for(let i=0; i<gpoly_list.length; ++i){
						let a = gpoly_list[i].split(' ');
						let line = L.Polyline.fromEncoded(a[0]);
						line.bindPopup("link id "+a[1]);
						new_group.addLayer(line);
					}
					map.addLayer(new_group);
					if(current_group != null)
						map.removeLayer(current_group);
					current_group = new_group;
					let now = Date.now();
					last_request_end = now;
				}
			};
			r.open("POST", "/find_links_close_to", true);
			r.setRequestHeader("Content-type", "text/plain");
			r.send(pos.lat + " " + pos.lng + " " + radius);
		}
	}

	drag_marker.on('drag', function(event) {
		request_polylines(drag_marker.getLatLng(), 100000);
	});
	request_polylines(center, 100000);
</script>
</body>
</html>

)XXX";
	res.mime_type = "text/html";

}

void serve_links_close_to(ConstRefLinkShapes shapes, LinkGeoIndexFindWithinRadiusQuery&query, const string&body, http::Response&res){
	double lat, lon, radius_in_cm;
	{
		istringstream in(body);
		in >> lat >> lon >> radius_in_cm;
	}

	GeoPos p(LatLon::from_lat_lon(lat, lon));

	ostringstream out;

	query.start(p, radius_in_cm);
	while(auto next = query.next()){
		out << encode_gpoly_from_dlink(shapes, link_to_forward_dlink(*next)) << " " << *next << endl;
	}

	res.body = out.str();
	res.mime_type = "text/plain";
}

LatLon compute_map_center(ConstRefLinkShapes shapes){
	int64_t lat_sum = 0;
	int64_t lon_sum = 0;

	for(uint32_t i=0; i<shapes.node_count; ++i){
		lat_sum += shapes.node_pos[i].lat_in_decamicrodeg;
		lon_sum += shapes.node_pos[i].lon_in_decamicrodeg;
	}
	return LatLon::from_lat_lon_in_decamicrodeg(lat_sum / shapes.node_count, lon_sum / shapes.node_count);

}

void run(int port, ConstRefLinkShapes shapes){
	http::Config config;
	config.port = port;
	config.print_exception_message_in_answer = true;

	LatLon center = compute_map_center(shapes);
	VecGeoIndex link_geo_index = build_link_geo_index(shapes);

	auto answer = [&](
		int, int,
		const ServerData&server_data, WorkerData&worker_data,
		const http::Request&req, http::Response&res
	){
		cout << "Request to "<<req.resource << endl;
		if(req.resource == "/index.html"){
			serve_index(center, res);
		}else if(req.resource == "/find_links_close_to"){
			serve_links_close_to(server_data.link_shapes, worker_data.link_geo_query, req.body, res);
		}else{
			throw runtime_error("Unknown resource");
		}
	};

	ServerData server_data;
	server_data.link_shapes = shapes;
	server_data.link_geo_index = link_geo_index.as_cref();

	http::run_with_worker_data<WorkerData>(config, server_data, answer,  []{cerr << "Server is running.\n";});
}

int main(int argc, char*argv[]){

	if(argc != 3){
		cout << "usage: "<<argv[0] << " port link_shapes" << endl;
		return 1;
	}

	int port = atoi(argv[1]);
	DirLinkShapes link_shapes(argv[2]);

	run(port, link_shapes.as_cref());
}
