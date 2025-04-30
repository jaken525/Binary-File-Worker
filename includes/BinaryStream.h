#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <iostream>
#include <cstring>
#include <iomanip>
#include <shlwapi.h>

class BinaryStream
{
private:
	size_t fileSize;
	size_t pos;
	char* buffer;
	bool isFileOpen;

	bool check_jump(const int) const;

public:
	BinaryStream() : buffer(nullptr), fileSize(0), pos(0), isFileOpen(false) {};
	BinaryStream(const std::string&);
	BinaryStream(const BinaryStream& rhs) : fileSize(rhs.fileSize), buffer(new char{ *rhs.buffer }), pos(0), isFileOpen(false) {};
	BinaryStream(BinaryStream&& rhs) : fileSize(rhs.fileSize), buffer(rhs.buffer), pos(0), isFileOpen(false) {
		rhs.buffer = nullptr;
	};
	~BinaryStream() { delete[] buffer; }
	static BinaryStream create_instance() { return BinaryStream(); }
	static BinaryStream create_instance(const std::string& file_name) { return BinaryStream(file_name); }

	BinaryStream& operator=(const BinaryStream& rhs);
	BinaryStream& operator=(BinaryStream&& rhs);
	operator const int() { return fileSize; }
	char* operator->() { return buffer; };
	const char* operator->() const { return buffer; };
	char& operator[](int x) { return buffer[x]; }

	char read_short_short();
	unsigned short read_short();
	unsigned long read_long();
	unsigned long long read_long_long();
	float read_float();
	double read_double();
	std::string read_str_wz(const int&);
	std::string read_str(const int&);
	template <typename T> T read_data(const int& size = 0);

	bool jump(const int);

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
	size_t get_file_size() const { return fileSize; }
	size_t get_position() const { return pos; }
	static char* get_buffer(const BinaryStream* bstream) { return bstream->buffer; }
	static size_t get_file_size(const BinaryStream* bstream) { return bstream->fileSize; }
	static size_t get_position(const BinaryStream* bstream) { return bstream->pos; }

	void print_file(const uint8_t& size = 16) const;
	void print_file(const int&, const size_t&, const uint8_t& size = 16) const;
	static void print_file(const char*, const int&, const size_t&, const uint8_t& size = 16);
	static void print_file(const BinaryStream&, const uint8_t& size = 16);
};

/// <summary>
/// Use carefully
/// </summary>
template <typename T> T BinaryStream::read_data(const int& size) {
	try {
		int data_size = size == 0 ? sizeof(T) : size;
		jump(data_size);
		return *(T*)&buffer[pos - data_size];
	}
	catch (...) {
		std::cout << "Can't read this data. Please try another type of data.\n";
	}
}

#endif
