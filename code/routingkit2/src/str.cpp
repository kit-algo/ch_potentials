#include "str.h"
#include <string.h>

namespace RoutingKit2{
	bool str_eq(const char*l, const char*r)noexcept{
		return !strcmp(l, r);
	}

	bool str_wild_char_eq(const char*l, const char*r)noexcept{
		while(*l != '\0' && *r != '\0'){
			if(*l != '?' && *r != '?' && *l != *r)
				return false;
			++l;
			++r;
		}
		return *l == '\0' && *r == '\0';
	}

	bool starts_with(const char*prefix, const char*str)noexcept{
		while(*prefix != '\0' && *str == *prefix){
			++prefix;
			++str;
		}
		return *prefix == '\0';
	}

	void copy_str_and_make_lower_case(const char*in, char*out, unsigned out_size)noexcept{
		char*out_end = out + out_size-1;
		while(*in && out != out_end){
			if('A' <= *in && *in <= 'Z')
				*out = *in - 'A' + 'a';
			else
				*out = *in;
			++in;
			++out;
		}
		*out = '\0';
	}
}
