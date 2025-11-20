#include "../includes/BinaryStream.h"

#pragma region Constructors

BinaryStream& BinaryStream::operator=(const BinaryStream& rhs)
{
	if (this == &rhs) return *this;
	fileSize = rhs.fileSize;
	pos = rhs.pos;
	isFileOpen = rhs.isFileOpen;
	delete[] buffer;
	buffer = new char[fileSize];
	std::copy(buffer, buffer + fileSize, rhs.buffer);
	return *this;
}

BinaryStream& BinaryStream::operator=(BinaryStream&& rhs)
{
	if (this == &rhs) return *this;
	fileSize = rhs.fileSize;
	pos = rhs.pos;
	isFileOpen = rhs.isFileOpen;
	std::swap(buffer, rhs.buffer);
	return *this;
}

BinaryStream::BinaryStream(const std::string& fileName) {
	isFileOpen = open_file(fileName);
	pos = 0;
	if (!isFileOpen) {
		throw "Failed to Open File\n";
	}
}

#pragma endregion

void BinaryStream::clear()
{
	pos = 0;
	fileSize = 0;
	if (buffer = nullptr) delete[] buffer;
}

bool BinaryStream::open_file(const std::string& filename)
{
	HANDLE hFile = CreateFile(
		filename.c_str(),		// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		nullptr,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		nullptr					// no attr. template
	);
	clear();
	if (hFile == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to Open File\n";
		return false;
	}
	fileSize = GetFileSize(hFile, nullptr);
	if (fileSize == 0) {
		std::cout << "Failed to read file. File is Empty?\n";
		return false;
	}
	buffer = new char[fileSize];
	unsigned long dwBytesRead = 0;
	if (ReadFile(hFile, buffer, fileSize, &dwBytesRead, nullptr) == FALSE || dwBytesRead != fileSize) {
		std::cout << "Failed to copy file into memory\n";
		fileSize = 0;
		delete[] buffer;
		buffer = nullptr;
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return true;
}

#pragma region DataGet

std::string BinaryStream::convert_int16(const short& num) {
	std::string result(2, 0);
	for (int i = 0; i != 2; ++i) {
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_int32(const int& num) {
	std::string result(4, 0);
	for (int i = 0; i != 4; ++i) {
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_int64(const long& num) {
	std::string result(8, 0);
	for (int i = 0; i != 8; ++i) {
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_float(const float& num) {
	std::string result(4, 0);
	union {
		float f;
		uint32_t u;
	} converter;
	converter.f = num;
	for (int i = 0; i != 4; ++i) {
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_double(const double& num) {
	std::string result(8, 0);
	union {
		double f;
		uint64_t u;
	} converter;
	converter.f = num;
	for (int i = 0; i != 8; ++i) {
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);
	}
	return result;
}

char BinaryStream::convert_char(const int& num) {
	return static_cast<char>(num & 0xFF);
}

std::string BinaryStream::convert_string(const int& size, const std::string& str) {
	std::string result(str);
	result.resize(size, '\0');
	return result;
}

#pragma endregion

bool BinaryStream::jump(const int& jump) {
	if (jump > fileSize || jump < 0) {
		return false;
	}
	pos = jump;
	return true;
}

bool BinaryStream::forward(const int& num) {
	return jump(pos + num);
}

bool BinaryStream::back(const int& num) {
	return jump(pos - num);
}

#pragma region FileRead
std::string BinaryStream::read_str(const int& size, const bool& withTrans) {
	if (withTrans) {
		forward(size);
	}
	const char* start = &buffer[pos - size];
	return std::string(start, start + size);;
}

std::string BinaryStream::read_str_wz(const int& size, const bool& withTrans) {
	if (withTrans) {
		forward(size);
	}
	const char* start = &buffer[pos - size];
	const char* end = std::find(start, start + size, '\0');
	return std::string(start, end);
}

float BinaryStream::read_float(const bool& withTrans) {
	if (withTrans) {
		forward(4);
	}
	return *(float*)&buffer[pos - 4];
}

double BinaryStream::read_double(const bool& withTrans) {
	if (withTrans) {
		forward(8);
	}
	return *(double*)&buffer[pos - 8];
}

char BinaryStream::read_char(const bool& withTrans) {
	if (withTrans) {
		forward(1);
	}
	return *(char*)&buffer[pos - 1];
}

int8_t BinaryStream::read_int8(const bool& withTrans) {
	return read_char(withTrans);
}

uint64_t BinaryStream::read_uint64(const bool& withTrans) {
	if (withTrans) {
		forward(8);
	}
	return *(uint64_t*)&buffer[pos - 8];
}

uint32_t BinaryStream::read_uint32(const bool& withTrans) {
	if (withTrans) {
		forward(4);
	}
	return *(uint32_t*)&buffer[pos - 4];
}

uint16_t BinaryStream::read_uint16(const bool& withTrans) {
	if (withTrans) {
		forward(2);
	}
	return *(uint16_t*)&buffer[pos - 2];
}

int64_t BinaryStream::read_int64(const bool& withTrans) {
	if (withTrans) {
		forward(8);
	}
	return *(int64_t*)&buffer[pos - 8];
}

int32_t BinaryStream::read_int32(const bool& withTrans) {
	if (withTrans) {
		forward(4);
	}
	return *(int32_t*)&buffer[pos - 4];
}

int16_t BinaryStream::read_int16(const bool& withTrans) {
	if (withTrans) {
		forward(2);
	}
	return *(int16_t*)&buffer[pos - 2];
}

uint8_t BinaryStream::read_uint8(const bool& withTrans) {
	if (withTrans) {
		forward(1);
	}
	return *(uint8_t*)&buffer[pos - 1];
}
#pragma endregion

std::string BinaryStream::get_filename(const std::string& str) {
	size_t found;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found < str.size()) {
		strt = str.substr(found + 1, -1);
		found = strt.find(".");
		if (found < strt.size()) {
			strt = strt.substr(0, found);
		}
	}
	else {
		strt = str;
	}
	size_t lastdot = strt.find_last_of(".");
	if (lastdot == std::string::npos) {
		return strt;
	}
	return strt.substr(0, lastdot);
}

std::string BinaryStream::get_filename_path(const std::string& str) {
	size_t found;
	found = str.find_last_of("/\\");
	return found != std::string::npos ? (str.substr(0, found) + "\\") : "";
}

/// <summary>
/// Only for console.
/// Print HEX table
/// </summary>
void BinaryStream::print_file(const uint8_t& size) const {
	print_file(this->buffer, 0, this->fileSize, size);
}

void BinaryStream::print_file(const int& start, const size_t& fileSize, const uint8_t& size) const {
	std::printf("Printing file from %d position to %d", start, fileSize);
	print_file(this->buffer, start, fileSize, size);
}

void BinaryStream::print_file(const char* buffer, const int& start, const size_t& fileSize, const uint8_t& size) {
	if (buffer == nullptr || fileSize == 0 || start > fileSize) {
		throw "File buffer is empty or size equals zero!";
	}
	if (size % 4 != 0) {
		std::cout << "Incorrect table size.";
		return;
	}
	std::cout << "\nOffeset (h) | ";
	for (int i = 0; i < size; ++i) {
		std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << i << " ";
	}
	std::cout << "| Decoded Text\n" << std::string(14 + (int)size * 3 + 2 + 16, '-') << '\n';
	int linesCount = (fileSize - start) % size == 0 ? (fileSize - start) / size : (fileSize - start) / size + 1;
	for (int i = 0; i != linesCount; ++i) {
		std::cout << " 0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << i * size << " | ";
		for (uint8_t j = 0; j != size; ++j) {
			if (i * size + j >= (fileSize - start)) {
				std::cout << std::string((size - j) * 3, ' ');
				break;
			}
			std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)(uint8_t)buffer[start + i * size + j] << " ";
		}
		std::cout << "| ";
		for (uint8_t j = 0; j != size; ++j) {
			if (i * size + j >= (fileSize - start)) {
				break;
			}
			std::cout << (buffer[start + i * size + j] >= 0 && buffer[start + i * size + j] <= 31 ? '.' : buffer[start + i * size + j]);
		}
		std::cout << std::endl;
	}
}

void BinaryStream::print_file(const BinaryStream& bstream, const uint8_t& size) {
	print_file(bstream.get_buffer(), 0, bstream.get_file_size(), size);
}

void BinaryStream::write_file(const std::string& fileName) {
	write_file(fileName, buffer, fileSize);
}

#include <fstream>
void BinaryStream::write_file(const std::string& filePath, char* data, const size_t& size) {
	std::ofstream file(filePath, std::ios::binary);
	file.write(data, size);
	file.close();
}
