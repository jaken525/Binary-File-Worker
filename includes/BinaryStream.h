#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <iostream>
#include <cstring>
#include <iomanip>
#include <shlwapi.h>

class BinaryStream final {
public:
	BinaryStream() : buffer(nullptr), fileSize(0), pos(0), isFileOpen(false) {};
	BinaryStream(char* buffer, size_t fileSize, size_t pos = 0) :
		buffer(buffer),
		fileSize(fileSize),
		pos(pos),
		isFileOpen(true) {};
	// Constructor with path
	BinaryStream(const std::string&);
	BinaryStream(const BinaryStream& rhs) :
		fileSize(rhs.fileSize),
		buffer(new char{ *rhs.buffer }),
		pos(0),
		isFileOpen(true) {};
	BinaryStream(BinaryStream&& rhs) :
		fileSize(rhs.fileSize),
		buffer(rhs.buffer),
		pos(0),
		isFileOpen(true) {
		rhs.buffer = nullptr;
	};
	~BinaryStream() { delete[] buffer; }
	static BinaryStream* create_instance() { return new BinaryStream(); }
	static BinaryStream* create_instance(const std::string& file_name) { return new BinaryStream(file_name); }

	BinaryStream& operator=(const BinaryStream& rhs);
	BinaryStream& operator=(BinaryStream&& rhs);
	operator const int() { return fileSize; }
	char* operator->() { return buffer; };
	const char* operator->() const { return buffer; };
	char& operator[](const int& x) const { return buffer[x]; }

	char read_char(const bool& withTrans = true);
	int8_t read_int8(const bool& withTrans = true);
	int16_t read_int16(const bool& withTrans = true);
	int32_t read_int32(const bool& withTrans = true);
	int64_t read_int64(const bool& withTrans = true);
	uint8_t read_uint8(const bool& withTrans = true);
	uint16_t read_uint16(const bool& withTrans = true);
	uint32_t read_uint32(const bool& withTrans = true);
	uint64_t read_uint64(const bool& withTrans = true);
	float read_float(const bool& withTrans = true);
	double read_double(const bool& withTrans = true);
	std::string read_str_wz(const int&, const bool& withTrans = true);
	std::string read_str(const int&, const bool& withTrans = true);
	template <typename T> T read_data(const int& size = 0, const bool& withTrans = true);

	bool jump(const int&);
	bool forward(const int&);
	bool back(const int&);

	// Functions which convert values into bytes for writting into the file.

	static char convert_char(const int&);
	static std::string convert_string(const int&, const std::string&);
	static std::string convert_float(const float&);
	static std::string convert_double(const double&);
	static std::string convert_int16(const short&);
	static std::string convert_int32(const int&);
	static std::string convert_int64(const long&);

	bool open_file(const std::string&);
	void clear();
	static std::string get_filename_path(const std::string&);
	static std::string get_filename(const std::string&);
	static std::string get_filext(const std::string&);

	char* get_bufferr() const { return buffer; }
	const char* get_buffer() const { return buffer; }
	uint8_t* get_buffer_uint8() const { return reinterpret_cast<uint8_t*>(buffer); }
	size_t get_file_size() const { return fileSize; }
	size_t get_position() const { return pos; }
	bool is_open() const { return isFileOpen; }
	static char* get_buffer(const BinaryStream* bstream) { return bstream->buffer; }
	static size_t get_file_size(const BinaryStream* bstream) { return bstream->fileSize; }
	static size_t get_position(const BinaryStream* bstream) { return bstream->pos; }
	static bool is_open(const BinaryStream* bstream) { return bstream->isFileOpen; }

	void print_file(const uint8_t& size = 16) const;
	void print_file(const int&, const size_t&, const uint8_t& size = 16) const;
	static void print_file(const char*, const int&, const size_t&, const uint8_t& size = 16);
	static void print_file(const BinaryStream&, const uint8_t& size = 16);

	void write_file(const std::string&);
	static void write_file(const std::string&, char*, const size_t&);

private:
	size_t fileSize;
	size_t pos;
	char* buffer;
	bool isFileOpen;
};

/// <summary>
/// Use carefully
/// </summary>
template <typename T> T BinaryStream::read_data(const int& size, const bool& withTrans) {
	try {
		int data_size = size == 0 ? sizeof(T) : size;
		if (withTrans) {
			forward(data_size);
		}
		return *(T*)&buffer[pos - data_size];
	}
	catch (...) {
		std::cout << "Can't read this data. Please try another type of data.\n";
	}
}

#endif
