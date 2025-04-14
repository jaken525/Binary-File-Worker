#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <iostream>
#include <cstring>
#include <iomanip>
#include <shlwapi.h>

class BinaryStream
{
private:
	char* buffer = nullptr;
	size_t fileSize = 0;
	size_t pos = 0;

	bool isFileOpen;

	bool check_jump(const int&) const;

public:

	BinaryStream();
	BinaryStream(const std::string&);
	~BinaryStream() { clear(); }

	char read_short_short();
	unsigned short read_short();
	unsigned long read_long();
	unsigned long long read_long_long();
	float read_float();
	double read_double();
	std::string read_str_wz(const int&);
	std::string read_str(const int&);
	template <typename T> T read_data(const int& size = 0);

	bool jump(const int&);

	// Functions which convert values into bytes for writting into the file.
	static char convert_symbol(const int&);
	static std::string convert_string(const int&, const std::string&);
	static std::string convert_float(const float&);
	static std::string convert_double(const double&);
	static std::string convert_long(const int&);
	static std::string convert_long_long(const int&);

	bool open_file(const std::string&);
	void clear();
	std::string get_filename_path(const std::string&) const;
	std::string get_filename(const std::string&) const;

	char* get_buffer() const { return buffer; }

	void print_file(const uint8_t& size = 16) const;
	size_t get_position() const { return pos; }
};

/// <summary>
/// Use carefully
/// </summary>
template <typename T> T BinaryStream::read_data(const int& size) {
	try
	{
		int data_size = size == 0 ? sizeof(T) : size;
		jump(data_size);
		return *(T*)&buffer[pos - data_size];
	}
	catch (...)
	{
		std::cout << "Can't read this data. Please try another data's type.\n";
	}
}

#endif
