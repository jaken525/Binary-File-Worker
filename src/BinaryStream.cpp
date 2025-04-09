#include "../includes/BinaryStream.h"

#pragma region Constructors

BinaryStream::BinaryStream() {
	buffer = NULL;
	fileSize = 0;
	pos = 0;
	isFileOpen = false;
}

BinaryStream::BinaryStream(const std::string fileName) {
	isFileOpen = open_file(fileName);
	if (!isFileOpen) {
		throw "Failed to Open File\n";
	}
}

#pragma endregion

void BinaryStream::clear()
{
	pos = 0;
	fileSize = 0;
	if (buffer = NULL) delete[] buffer;
}

bool BinaryStream::open_file(const std::string filename)
{
	// If you have an error, just remove (LPCWSTR)
	HANDLE hFile = CreateFile(
		(LPCWSTR)filename.c_str(),		// file to open
		GENERIC_READ,					// open for reading
		FILE_SHARE_READ,				// share for reading
		NULL,							// default security
		OPEN_EXISTING,					// existing file only
		FILE_ATTRIBUTE_NORMAL,			// normal file
		NULL							// no attr. template
	);
	clear();
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to Open File\n";
		return false;
	}
	this->fileSize = GetFileSize(hFile, NULL);
	if (this->fileSize == 0)
	{
		std::cout << "Failed to read file. File is Empty?\n";
		return false;
	}
	this->buffer = new char[this->fileSize];
	unsigned long dwBytesRead = 0;
	if (ReadFile(hFile, this->buffer, this->fileSize, &dwBytesRead, NULL) == FALSE || dwBytesRead != this->fileSize)
	{
		std::cout << "Failed to copy file into memory\n";
		this->fileSize = 0;
		delete[] this->buffer;
		this->buffer = NULL;
		CloseHandle(hFile);
		return false;
	}
	CloseHandle(hFile);
	return true;
}

#pragma region DataGet

std::string BinaryStream::convert_long(int num) {
	std::string result(4, 0);
	for (int i = 0; i < 4; ++i) {
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_long_long(int num) {
	std::string result(8, 0);
	for (int i = 0; i < 8; ++i) {
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_float(float num) {
	std::string result(4, 0);
	union {
		float f;
		uint32_t u;
	} converter;
	converter.f = num;
	for (int i = 0; i < 4; ++i) {
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);
	}
	return result;
}

std::string BinaryStream::convert_double(double num) {
	std::string result(8, 0);
	union {
		double f;
		uint64_t u;
	} converter;
	converter.f = num;
	for (int i = 0; i < 8; ++i) {
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);
	}
	return result;
}

char BinaryStream::convert_symbol(int num) {
	return static_cast<char>(num & 0xFF);
}

std::string BinaryStream::convert_string(int size, const std::string str) {
	std::string result(str);
	result.resize(size, '\0');
	return result;
}

#pragma endregion

bool BinaryStream::check_jump(int jump) const {
	if (pos + jump > fileSize) {
		throw "Position out of file buffer.\n";
	}
	return true;
}

bool BinaryStream::jump(int jump) {
	check_jump(jump);
	pos += jump;
	return true;
}

#pragma region FileRead
std::string BinaryStream::read_str(int size) {
	check_jump(size);
	std::string result = "";
	for (int i = 0; i < size; i++) {
		result += this->buffer[pos + i];
	}
	jump(size);
	return result;
}

std::string BinaryStream::read_str_wz(int size) {
	check_jump(size);
	std::string result = "";
	for (int i = 0; i < size; i++) {
		if (this->buffer[pos] == 0)
			break;
		result += this->buffer[pos + i];
	}
	jump(size);
	return result;
}

float BinaryStream::read_float() {
	jump(4);
	return *(float*)&this->buffer[pos - 4];
}

double BinaryStream::read_double() {
	jump(8);
	return *(double*)&this->buffer[pos - 8];
}

unsigned long long BinaryStream::read_long_long() {
	jump(8);
	return *(unsigned long long*)&this->buffer[pos - 8];
}

unsigned long BinaryStream::read_long() {
	jump(4);
	return *(unsigned long*)&this->buffer[pos - 4];
}

int BinaryStream::read_short_short() {
	jump(1);
	return *(uint8_t*)&this->buffer[pos - 1];
}

unsigned short BinaryStream::read_short() {
	jump(2);
	return *(unsigned short*)&this->buffer[pos - 2];
}
#pragma endregion

std::string BinaryStream::get_filename(const std::string str) const {
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

std::string BinaryStream::get_filename_path(const std::string str) const {
	size_t found;
	found = str.find_last_of("/\\");
	return found != std::string::npos ? (str.substr(0, found) + "\\") : "";
}

/// <summary>
/// Only for console.
/// Print HEX table
/// </summary>
void BinaryStream::print_file(uint8_t size) const {
	if (buffer == NULL) {
		throw "File buffer is empty!";
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

	int linesCount = fileSize % size == 0 ? fileSize / size : fileSize / size + 1;
	for (int i = 0; i < linesCount; ++i) {
		std::cout << " 0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << i * size << " | ";
		for (uint8_t j = 0; j < size; ++j) {
			if (i * size + j >= this->fileSize) {
				std::cout << std::string((size - j) * 3, ' ');
				break;
			}
			std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)this->buffer[i * size + j] << " ";
		}
		std::cout << "| ";
		for (uint8_t j = 0; j < size; ++j) {
			if (i * size + j >= this->fileSize) {
				break;
			}
			std::cout << (this->buffer[i * size + j] >= 0 && this->buffer[i * size + j] <= 31 ? '.' : this->buffer[i * size + j]);
		}
		std::cout << std::endl;
	}
}