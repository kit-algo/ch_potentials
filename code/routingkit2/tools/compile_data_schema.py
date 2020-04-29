#!/usr/bin/env python3
import sys
import itertools
from collections import namedtuple

Dim = namedtuple('Dim', 'name')
Col = namedtuple('Col', 'type name dim dim_offset dim_factor')
Database = namedtuple('Database', 'name dim_list col_list')
Schema = namedtuple('Schema', 'database_list namespace include_guard include_list')

def generate_database_code(database, convertible_from):
	def format_offset(o):
		if o < 0:
			return " - "+str(-o)
		elif o > 0:
			return " + "+str(o)
		else:
			return ""

	def push(s):
		lines.extend(s.split("\n"))

	push____= push

	def push_gbl(s):
		push____(s.format(
			N=database.name,
			E="explicit " if len(database.dim_list)==1 else ""
		))

	def push_col(s):
		for col in database.col_list:
			push(s.format(
				N=database.name,
				T=col.type,
				D=col.dim,
				C=col.name,
				P=format_offset(col.dim_offset),
				M=format_offset(-col.dim_offset),
				F="" if col.dim_factor == 1 else str(col.dim_factor)+"*"
			))

	def push_dim(s):
		for dim in database.dim_list:
			push(s.format(
				N=database.name,
				D=dim.name
			))

	def remove_last_chars(n):
		lines[-1] = lines[-1][:-n]

	def replace_last_chars(n, s):
		lines[-1] = lines[-1][:-n]+s

	intro_lines = []
	main_lines = []
	finish_lines = []


	lines = intro_lines
	push_gbl("struct Ref{N};")
	push_gbl("struct ConstRef{N};")

	lines = main_lines
	push____("")
	push_gbl("struct Ref{N}{{")
	push_dim("\tsize_t {D};")
	push_col("\t{T}* __restrict__ {C}; // size = {F}{D}{P}")
	push____("")
	push_col("\tSpan<{T}> {C}_as_ref()noexcept;")
	push_col("\tSpan<const {T}> {C}_as_ref()const noexcept;")
	push_col("\tSpan<const {T}> {C}_as_cref()const noexcept;")

	lines = finish_lines
	push____("")
	push_col("inline Span<{T}> Ref{N}::{C}_as_ref()noexcept{{\n\treturn {{{C}, {C}+{F}{D}{P}}};\n}}\n")
	push_col("inline Span<const {T}> Ref{N}::{C}_as_ref()const noexcept{{\n\treturn {C}_as_cref();\n}}\n")
	push_col("inline Span<const {T}> Ref{N}::{C}_as_cref()const noexcept{{\n\treturn {{{C}, {C}+{F}{D}{P}}};\n}}\n")

	lines = main_lines
	push____("")
	push_gbl("\tRef{N}();")

	for other_database in convertible_from:
		lines = main_lines
		push("\tRef"+database.name+"(Ref"+other_database.name+");")

		lines = finish_lines
		push____("")
		push____("inline Ref"+database.name+"::Ref"+database.name+"(Ref"+other_database.name+" o):")
		push_dim("\t{D}(o.{D}),")
		push_col("\t{C}(o.{C}),")
		replace_last_chars(1, "{}")

	lines = main_lines
	push_gbl("\tRef{N}(")
	push_dim("\t\tsize_t {D},")
	push_col("\t\t{T}* __restrict__ {C},")
	remove_last_chars(1)
	push____("\t);")

	lines = finish_lines
	push____("")
	push_gbl("inline Ref{N}::Ref{N}(){{}}")
	push____("")
	push_gbl("inline Ref{N}::Ref{N}(")
	push_dim("\tsize_t {D},")
	push_col("\t{T}* __restrict__ {C},")
	remove_last_chars(1)
	push____("):")
	push_dim("\t{D}({D}),")
	push_col("\t{C}({C}),")
	replace_last_chars(1, "{}")

	lines = main_lines
	push____("};")
	push____("")
	push_gbl("struct ConstRef{N}{{")
	push_dim("\tsize_t {D};")
	push_col("\tconst {T}* __restrict__ {C}; // size = {F}{D}{P}")
	push____("")
	push_col("\tSpan<const {T}> {C}_as_ref()const noexcept;")
	push_col("\tSpan<const {T}> {C}_as_cref()const noexcept;")

	lines = finish_lines
	push____("")
	push_col("inline Span<const {T}> ConstRef{N}::{C}_as_ref()const noexcept{{\n\treturn {C}_as_cref();\n}}\n")
	push_col("inline Span<const {T}> ConstRef{N}::{C}_as_cref()const noexcept{{\n\treturn {{{C}, {C}+{F}{D}{P}}};\n}}\n")

	lines = main_lines
	push____("")
	push_gbl("\tConstRef{N}();")
	push_gbl("\tConstRef{N}(Ref{N});")
	for other_database in convertible_from:
		lines = main_lines
		push("\tConstRef"+database.name+"(Ref"+other_database.name+");")
		push("\tConstRef"+database.name+"(ConstRef"+other_database.name+");")

		lines = finish_lines
		push____("")
		push____("inline ConstRef"+database.name+"::ConstRef"+database.name+"(Ref"+other_database.name+" o):")
		push_dim("\t{D}(o.{D}),")
		push_col("\t{C}(o.{C}),")
		replace_last_chars(1, "{}")

		push____("")
		push____("inline ConstRef"+database.name+"::ConstRef"+database.name+"(ConstRef"+other_database.name+" o):")
		push_dim("\t{D}(o.{D}),")
		push_col("\t{C}(o.{C}),")
		replace_last_chars(1, "{}")

	lines = main_lines
	push_gbl("\tConstRef{N}(")
	push_dim("\t\tsize_t {D},")
	push_col("\t\tconst {T}* __restrict__ {C},")
	remove_last_chars(1)
	push____("\t);")


	lines = finish_lines

	push____("")
	push_gbl("inline ConstRef{N}::ConstRef{N}() {{}}")
	push____("")
	push_gbl("inline ConstRef{N}::ConstRef{N}(Ref{N} o):")
	push_dim("\t{D}(o.{D}),")
	push_col("\t{C}(o.{C}),")
	replace_last_chars(1, "{}")
	push____("")
	push_gbl("inline ConstRef{N}::ConstRef{N}(")
	push_dim("\tsize_t {D},")
	push_col("\tconst {T}* __restrict__ {C},")
	remove_last_chars(1)
	push____("):")
	push_dim("\t{D}({D}),")
	push_col("\t{C}({C}),")
	replace_last_chars(1, "{}")

	lines = main_lines
	push____("};")
	push____("")

	push_gbl("struct Vec{N}{{")
	push_dim("\tsize_t {D};")
	push_col("\tstd::vector<{T}>{C};")
	push____("")
	push_gbl("\tVec{N}();")
	push_gbl("\t{E}Vec{N}(")
	push_dim("\t\tsize_t {D},")
	remove_last_chars(1)
	push____("\t);")
	push_gbl("\tvoid resize(")
	push_dim("\t\tsize_t {D},")
	remove_last_chars(1)
	push____("\t);")

	push____("\tvoid throw_if_wrong_size()const;")
	push____("\tvoid assert_correct_size()const noexcept;")
	push____("\tvoid shrink_to_fit();")
	push_gbl("\tConstRef{N} as_ref()const noexcept;")
	push_gbl("\tRef{N} as_ref()noexcept;")
	push_gbl("\tConstRef{N} as_cref()const noexcept;")

	push____("")
	push_col("\tSpan<{T}> {C}_as_ref()noexcept;")
	push_col("\tSpan<const {T}> {C}_as_ref()const noexcept;")
	push_col("\tSpan<const {T}> {C}_as_cref()const noexcept;")

	lines = finish_lines
	push____("")
	push_col("inline Span<{T}> Vec{N}::{C}_as_ref()noexcept{{\n\treturn {{&{C}[0], &{C}[0]+{C}.size()}};\n}}\n")
	push_col("inline Span<const {T}> Vec{N}::{C}_as_ref()const noexcept{{\n\treturn {C}_as_cref();\n}}\n")
	push_col("inline Span<const {T}> Vec{N}::{C}_as_cref()const noexcept{{\n\treturn {{{C}.data(), {C}.data()+{C}.size()}};\n}}\n")

	lines = main_lines

	push____("};")
	push____("")

	lines = finish_lines

	push____("")
	push_gbl("inline Vec{N}::Vec{N}(){{}}")
	push____("")
	push_gbl("inline Vec{N}::Vec{N}(")
	push_dim("\tsize_t {D},")
	remove_last_chars(1)
	push____("):")
	push_dim("\t{D}({D}),")
	push_col("\t{C}({F}{D}{P}),")
	replace_last_chars(1, "{}")
	push____("")
	push_gbl("inline void Vec{N}::resize(")
	push_dim("\tsize_t {D},")
	remove_last_chars(1)
	push____("){")
	push_dim("\tthis->{D} = {D};")
	push_col("\t{C}.resize({F}{D}{P});")
	push____("}")

	push____("")
	push_gbl("inline void Vec{N}::throw_if_wrong_size()const{{")
	push____("\tstd::string err;")
	push_col("\tif({C}.size() != {F}{D}{P})\n\t\t err += (\"Column {C} has wrong size. Expected: \"+std::to_string({F}{D}{P})+\" Actual: \"+std::to_string({C}.size())+\"\\n\");")
	push____("\tif(!err.empty())")
	push____("\t\tthrow std::runtime_error(err);")
	push____("}")
	push____("")
	push_gbl("inline void Vec{N}::assert_correct_size()const noexcept{{")
	push_col("\tassert({C}.size() == {F}{D}{P});")
	push____("}")
	push____("")
	push_gbl("inline void Vec{N}::shrink_to_fit(){{")
	push_col("\t{C}.shrink_to_fit();")
	push____("}")
	push____("")
	push_gbl("inline ConstRef{N} Vec{N}::as_ref()const noexcept{{")
	push____("\treturn as_cref();")
	push____("}")
	push____("")
	push_gbl("inline Ref{N} Vec{N}::as_ref()noexcept{{")
	push____("\treturn {")
	push_dim("\t\t{D},")
	push_col("\t\t&{C}[0],")
	remove_last_chars(1)
	push____("\t};")
	push____("}")
	push____("")
	push_gbl("inline ConstRef{N} Vec{N}::as_cref()const noexcept{{")
	push____("\treturn {")
	push_dim("\t\t{D},")
	push_col("\t\t{C}.data(),")
	remove_last_chars(1)
	push____("\t};")
	push____("}")

	lines = main_lines
	push_gbl("struct Dir{N}{{")
	push_dim("\tsize_t {D};")
	push_col("\tFileArray<{T}>{C};")
	push____("")
	push_gbl("\tDir{N}();")
	push_gbl("\texplicit Dir{N}(std::string dir);")

	lines = finish_lines
	push____("")
	push_gbl("inline Dir{N}::Dir{N}(){{}}")
	push____("")
	push_gbl("inline Dir{N}::Dir{N}(std::string dir){{")
	push____("\tappend_dir_slash_if_needed(dir);")
	push_dim("\t{{\n\t\tFileDataSource src(dir+\"{D}\");\n\t\tread_full_buffer_from_data_source(src.as_ref(), (uint8_t*)&{D}, sizeof(size_t));\n\t}}")
	push_col("\t{C} = FileArray<{T}>(dir+\"{C}\");")
	push____("\tthrow_if_wrong_col_size();")
	push____("}")

	lines = main_lines
	push____("\tvoid open(std::string dir);")
	push____("\tvoid close();")
	push____("\tvoid throw_if_wrong_col_size()const;")

	lines = finish_lines
	push____("")
	push_gbl("inline void Dir{N}::open(std::string dir){{")
	push_gbl("\t*this = Dir{N}(std::move(dir));")
	push____("}")
	push____("")
	push_gbl("inline void Dir{N}::close(){{")
	push_gbl("\t*this = Dir{N}();")
	push____("}")
	push____("")
	push_gbl("inline void Dir{N}::throw_if_wrong_col_size()const{{")
	push____("\tstd::string err;")
	push_col("\tif({C}.size() != {F}{D}{P})\n\t\t err += (\"Column {C} has wrong size. Expected: \"+std::to_string({F}{D}{P})+\" Actual: \"+std::to_string({C}.size())+\"\\n\");")
	push____("\tif(!err.empty())")
	push____("\t\tthrow std::runtime_error(err);")
	push____("}")
	lines = main_lines

	push_gbl("\tConstRef{N} as_cref()const noexcept;")
	push_gbl("\tConstRef{N} as_ref()const noexcept;")

	lines = finish_lines
	push____("")
	push_gbl("inline ConstRef{N} Dir{N}::as_cref()const noexcept{{")
	push____("\treturn {")
	push_dim("\t\t{D},")
	push_col("\t\t{C}.data(),")
	remove_last_chars(1)
	push____("\t};")
	push____("}")
	push____("")
	push_gbl("inline ConstRef{N} Dir{N}::as_ref()const noexcept{{")
	push____("\treturn as_cref();")
	push____("}")

	lines = main_lines
	push____("")
	push_col("\tSpan<const {T}> {C}_as_ref()const noexcept;")
	push_col("\tSpan<const {T}> {C}_as_cref()const noexcept;")

	lines = finish_lines
	push____("")
	push_col("inline Span<const {T}> Dir{N}::{C}_as_ref()const noexcept{{\n\treturn {C}_as_cref();\n}}\n")
	push_col("inline Span<const {T}> Dir{N}::{C}_as_cref()const noexcept{{\n\treturn {{{C}.data(), {C}.data()+{C}.size()}};\n}}\n")

	lines = main_lines
	push____("};")
	push____("")
	push_gbl("inline void dump_into_dir(std::string dir, Ref{N} data);")
	push_gbl("inline void dump_into_dir(std::string dir, ConstRef{N} data);")


	lines = finish_lines

	push____("")
	push_gbl("inline void dump_into_dir(std::string dir, Ref{N} data){{")
	push_gbl("\tdump_into_dir(std::move(dir), ConstRef{N}(data));")

	push____("}")
	push____("")
	push_gbl("inline void dump_into_dir(std::string dir, ConstRef{N} data){{")
	push_gbl("\tappend_dir_slash_if_needed(dir);")
	push_dim("\tFileDataSink(dir+\"{D}\")((const uint8_t*)&data.{D}, sizeof(size_t));");
	push_col("\tFileDataSink(dir+\"{C}\")((const uint8_t*)data.{C}, ({F}data.{D}{P})*sizeof({T}));");
	push____("}")


	return intro_lines, main_lines, finish_lines




def parse_schema(in_stream):
	database_list = []

	namespace = None
	include_guard = None
	include_list = []

	database_name = None
	col_list = []
	dim_list = []
	dim_set = set()

	def database_finish():
		database_list.append(
			Database(
				name = database_name,
				dim_list = dim_list,
				col_list = col_list
			)
		)

	for line in in_stream:
		line = line.strip()
		if len(line) == 0:
			continue
		if line[0] == '=':
			if database_name != None:
				database_finish()
			database_name = line.strip("= \t")
			col_list = []
			dim_list = []
			dim_set = set()

		elif line[0] == '!':
			cmd = [x for x in line[1:].strip().split(" ") if x!=""]
			if cmd[0] == "namespace":
				namespace = cmd[1]
			elif cmd[0] == "include_guard":
				include_guard = cmd[1]
			elif cmd[0] == "include":
				include_list.append(cmd[1])

		else:
			sep1 = line.find("@")
			sep2 = line.rfind(" ")
			if sep1 == -1 or sep2 == -1 or sep2 < sep1:
				dim_name = line.strip()
				if not dim_name in dim_set:
					dim_set.add(dim_name)
					dim_list.append(Dim(name = dim_name))
			else:
				type_name = line[:sep1].strip()
				dim_name = line[sep1+1:sep2].strip()
				col_name = line[sep2+1:].strip()
				dim_offset = 0
				dim_factor = 1

				mul = dim_name.find("*")
				if mul != -1:
					dim_factor = int(dim_name[:mul].strip())
					dim_name = dim_name[mul+1:].strip()

				m = dim_name.rfind("-")
				p = dim_name.rfind("+")
				if m < p and p != -1:
					dim_offset = int(dim_name[p+1:].strip())
					dim_name = dim_name[:p].strip()
				elif p < m and m != -1:
					dim_offset = -int(dim_name[m+1:].strip())
					dim_name = dim_name[:m].strip()

				if not dim_name in dim_set:
					dim_set.add(dim_name)
					dim_list.append(Dim(name = dim_name))

				col_list.append(Col(type = type_name, name = col_name, dim = dim_name, dim_offset = dim_offset, dim_factor=dim_factor))
	database_finish()

	return Schema(
		database_list = database_list, namespace = namespace,
		include_list = include_list, include_guard = include_guard
	)

schema = parse_schema(sys.stdin)


def is_database_subset_of(database_a, database_b):

	dim_list_set_b = set((dim.name for dim in database_b.dim_list))
	for dim in database_a.dim_list:
		if not dim.name in dim_list_set_b:
			return False
	del dim_list_set_b

	col_list_set_b = set((col.name for col in database_b.col_list))
	for col in database_a.col_list:
		if not col.name in col_list_set_b:
			return False
	del col_list_set_b

	return True

lines = []

if schema.include_guard != None:
	lines.append("#ifndef "+schema.include_guard)
	lines.append("#define "+schema.include_guard)
	lines.append("")
	lines.append("#include \"span.h\"")
	lines.append("#include \"dir.h\"")
	lines.append("#include \"file_array.h\"")
	lines.append("#include \"data_sink.h\"")
	lines.append("#include \"data_source.h\"")

	for include in schema.include_list:
		lines.append("#include "+include)
	lines.append("#include <vector>")
	lines.append("#include <string>")
	lines.append("#include <stdexcept>")
	lines.append("#include <stdlib.h>")
	lines.append("#include <stdint.h>")
	lines.append("#include <assert.h>")
	lines.append("")
if schema.namespace != None:
	lines.append("namespace "+schema.namespace+" {")
	lines.append("")

def breakup_into_max_len_lines(s, max):
	lines = ["//"]
	for word in s.replace("\n", " ").split(" "):
		if len(word) != 0:
			if len(lines[-1]) + len(word) + 1 <= max:
				lines[-1] = lines[-1] + " " + word
			else:
				lines.append("// "+word)
	return lines

	str.split("\n")

# lines.extend(breakup_into_max_len_lines("""
# This header defines types that represent databases of POD types.
# Every databases consists of a fixed number of columns and dimensions.
# A column is an array of a POD type.
# The length of the array is one of the dimensions plus possibly a constant offset.
# All columns of a database with the same dimension can be viewed as being part of a database.

# Suppose a database is called MyData. In this case, there are four types:

# RefMyData,
# ConstRefMyData,
# VecMyData,
# DirMyData.

# The first two types are structs of sizes and pointers.
# They are PODs.
# They do not own the referenced data and do not cleanup the memory upon destruction.
# MyDataRef can mutate the data in the rows.
# MyDataConstRef references read-only data.
# DynMyData is a struct of vectors.
# For every column there is a vector.
# You can construct a MyDataRef or a MyDataConstRef by calling as_ref or as_cref.
# DirMyData represent views into memory mapped read-only files.
# There is one file per column and the name of the column is the name of the file.

# Files can be written into a directory using the dump_to_dir function.

# """, 75))

lines.append("")

main_lines = []
finish_lines = []

for database in schema.database_list:
	convertible_from_database = []
	for other_database in schema.database_list:
		if other_database != database:
			if is_database_subset_of(database, other_database):
				convertible_from_database.append(other_database)

	intro_code, main_code, finish_code = generate_database_code(database, convertible_from=convertible_from_database)

	lines.extend(intro_code)
	main_lines.extend(main_code)
	finish_lines.extend(finish_code)

lines.extend(main_lines)
lines.extend(finish_lines)


if schema.namespace != None:
	lines.append("")
	lines.append("} // namespace "+schema.namespace)

if schema.include_guard != None:
	lines.append("")
	lines.append("#endif")

sys.stdout.write("\n".join(lines)+"\n")
