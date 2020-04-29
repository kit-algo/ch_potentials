#ifndef ROUTINGKIT_STR_H
#define ROUTINGKIT_STR_H

namespace RoutingKit2{

	bool str_eq(const char*l, const char*r)noexcept;
	bool str_wild_char_eq(const char*l, const char*r)noexcept;
	bool starts_with(const char*prefix, const char*str)noexcept;
	void copy_str_and_make_lower_case(const char*in, char*out, unsigned out_size)noexcept;

	// Splits the string at some separators such as ; and calls f(str) for each part.
	// The ; is replaced by a '\0'. Leading spaces are removed
	template<class F>
	void split_str_at_osm_value_separators(char*in, const F&f){
		while(*in == ' ')
			++in;
		const char*value_begin = in;
		for(;;){
			while(*in != '\0' && *in != ';')
				++in;
			if(*in == '\0'){
				f(value_begin);
				return;
			}else{
				*in = '\0';
				f(value_begin);
				++in;
				while(*in == ' ')
					++in;
				value_begin = in;
			}
		}
	}
}

#endif
