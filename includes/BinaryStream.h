#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <iomanip>
#include <windows.h>
#include <shlwapi.h>

class BinaryStream
{
private:
	char* buffer = NULL;
	size_t fileSize = 0;
	size_t pos = 0;

	bool isFileOpen;

	bool check_jump(int) const;

public:
	static BinaryStream* init();
	static BinaryStream* init(const std::string);

	BinaryStream();
	BinaryStream(const std::string);
	~BinaryStream() { clear(); }

	unsigned int read_short_short();
	unsigned short read_short();
	unsigned long read_long();
	unsigned long long read_long_long();
	float read_float();
	std::string read_str_wz(int);
	std::string read_str(int);

	bool jump(int);

	// Functions which convert values into bytes for writting into the file.
	static char convert_symbol(int);
	static std::string convert_string(int, const std::string);
	static std::string convert_float(float);
	static std::string convert_double(double);
	static std::string convert_long(int);
	static std::string convert_long_long(int);

	bool open_file(const std::string);
	void clear();
	std::string get_filename_path(const std::string) const;
	std::string get_filename(const std::string) const;

	char* get_buffer() const { return buffer; }

	void print_file(uint8_t size = 16) const;
	size_t get_position() const { return pos; }
};

#endif
