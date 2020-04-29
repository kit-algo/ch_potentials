#ifndef ROUTING_KIT2_DIR_H
#define ROUTING_KIT2_DIR_H

#include <string>

namespace RoutingKit2{

inline void append_dir_slash_if_needed(std::string&dir){
	if(!dir.empty() && dir.back() != '/')
		dir.push_back('/');
}

} // namespace RoutingKit2

#endif
