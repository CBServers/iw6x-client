#pragma once

namespace gsc
{
	unsigned int scr_get_object(unsigned int index);
	unsigned int scr_get_const_string(unsigned int index);
	unsigned int scr_get_const_istring(unsigned int index);
	void scr_get_vector(unsigned int index, float* vector_value);
	int scr_get_int(unsigned int index);
	float scr_get_float(unsigned int index);
	int scr_get_pointer_type(unsigned int index);
	int scr_get_type(unsigned int index);
	const char* scr_get_type_name(unsigned int index);

	std::optional<std::pair<std::string, std::string>> find_function(const char* pos);
}
