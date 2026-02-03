#include "../includes/BinaryStream.h"

#pragma region Constructors

BinaryStream& BinaryStream::operator=(const BinaryStream& rhs)
{
	if (this == &rhs)
		return *this;

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
	if (this == &rhs)
		return *this;

	fileSize = rhs.fileSize;
	pos = rhs.pos;
	isFileOpen = rhs.isFileOpen;
	std::swap(buffer, rhs.buffer);

	return *this;
}

BinaryStream::BinaryStream(const std::string& fileName)
{
	isFileOpen = OpenFile(fileName);
	pos = 0;

	if (!isFileOpen)
		throw "Failed to Open File\n";
}

#pragma endregion

void BinaryStream::Clear()
{
	pos = 0;
	fileSize = 0;

	if (buffer)
		delete[] buffer;
}

bool BinaryStream::OpenFile(const std::string& filename)
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

	Clear();
	if (hFile == INVALID_HANDLE_VALUE)
	{
		std::cout << "Failed to Open File\n";
		return false;
	}

	fileSize = GetFileSize(hFile, nullptr);
	if (fileSize == 0)
	{
		std::cout << "Failed to read file. File is Empty?\n";
		return false;
	}

	buffer = new char[fileSize];
	unsigned long dwBytesRead = 0;
	if (!ReadFile(hFile, buffer, fileSize, &dwBytesRead, nullptr) ||
		dwBytesRead != fileSize)
	{
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

std::string BinaryStream::ConvertInt16(const short& num)
{
	std::string result(2, 0);
	for (int i = 0; i != 2; ++i)
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);

	return result;
}

std::string BinaryStream::ConvertInt32(const int& num)
{
	std::string result(4, 0);
	for (int i = 0; i != 4; ++i)
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);

	return result;
}

std::string BinaryStream::ConvertInt64(const long& num)
{
	std::string result(8, 0);
	for (int i = 0; i != 8; ++i)
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);

	return result;
}

std::string BinaryStream::ConvertFloat(const float& num)
{
	std::string result(4, 0);
	union
	{
		float f;
		uint32_t u;
	} converter;

	converter.f = num;
	for (int i = 0; i != 4; ++i)
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);

	return result;
}

std::string BinaryStream::ConvertDouble(const double& num)
{
	std::string result(8, 0);
	union
	{
		double d;
		uint64_t u;
	} converter;
	
	converter.d = num;
	for (int i = 0; i != 8; ++i) 
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);

	return result;
}

char BinaryStream::ConvertInt8(const uint8_t& num)
{
	return ConvertChar(num);
}

char BinaryStream::ConvertChar(const int& num)
{
	return static_cast<char>(num & 0xFF);
}

std::string BinaryStream::ConvertString(const int& size, const std::string& str)
{
	std::string result(str);
	result.resize(size, '\0');

	return result;
}

#pragma endregion

bool BinaryStream::Jump(const int& jump)
{
	if (jump > fileSize || jump < 0)
		return false;

	pos = jump;
	return true;
}

bool BinaryStream::Forward(const int& num)
{
	return Jump(pos + num);
}

bool BinaryStream::Back(const int& num)
{
	return Jump(pos - num);
}

#pragma region FileRead
std::string BinaryStream::ReadString(const int& size, const bool& withTrans)
{
	if (withTrans)
		Forward(size);

	const char* start = &buffer[pos - size];
	return std::string(start, start + size);;
}

std::string BinaryStream::ReadStringRaw(const int& size, const bool& withTrans)
{
	if (withTrans)
		Forward(size);

	const char* start = &buffer[pos - size];
	const char* end = std::find(start, start + size, '\0');
	return std::string(start, end);
}

float BinaryStream::ReadFloat(const bool& withTrans)
{
	if (withTrans)
		Forward(4);

	return *reinterpret_cast<float*>(&buffer[pos - 4]);
}

double BinaryStream::ReadDouble(const bool& withTrans)
{
	if (withTrans)
		Forward(8);

	return *reinterpret_cast<double*>(&buffer[pos - 8]);
}

char BinaryStream::ReadChar(const bool& withTrans)
{
	if (withTrans)
		Forward(1);

	return *reinterpret_cast<char*>(&buffer[pos - 1]);
}

int8_t BinaryStream::ReadInt8(const bool& withTrans)
{
	return ReadChar(withTrans);
}

uint8_t BinaryStream::ReadUInt8(const bool& withTrans)
{
	if (withTrans)
		Forward(1);

	return *reinterpret_cast<uint8_t*>(&buffer[pos - 1]);
}

uint64_t BinaryStream::ReadUInt64(const bool& withTrans)
{
	if (withTrans)
		Forward(8);

	return *reinterpret_cast<uint64_t*>(&buffer[pos - 8]);
}

uint32_t BinaryStream::ReadUInt32(const bool& withTrans)
{
	if (withTrans)
		Forward(4);

	return *reinterpret_cast<uint32_t*>(&buffer[pos - 4]);
}

uint16_t BinaryStream::ReadUInt16(const bool& withTrans)
{
	if (withTrans)
		Forward(2);

	return *reinterpret_cast<uint16_t*>(&buffer[pos - 2]);
}

int64_t BinaryStream::ReadInt64(const bool& withTrans)
{
	if (withTrans)
		Forward(8);

	return *reinterpret_cast<int64_t*>(&buffer[pos - 8]);
}

int32_t BinaryStream::ReadInt32(const bool& withTrans)
{
	if (withTrans)
		Forward(4);

	return *reinterpret_cast<int32_t*>(&buffer[pos - 4]);
}

int16_t BinaryStream::ReadInt16(const bool& withTrans)
{
	if (withTrans)
		Forward(2);

	return *reinterpret_cast<int16_t*>(&buffer[pos - 2]);
}
#pragma endregion

std::string BinaryStream::GetFileNameWithExt(const std::string& str)
{
	auto found = str.find_last_of("/\\");
	const auto strt = found < str.size() ? str.substr(found + 1, -1) : str;
	
	return strt;
}

std::string BinaryStream::GetFileName(const std::string& str)
{
	std::string strt = GetFileNameWithExt(str);
	
	size_t lastdot = strt.find_last_of(".");
	if (lastdot == std::string::npos)
		return strt;

	return strt.substr(0, lastdot);
}

std::string BinaryStream::GetFileExtention(const std::string& str)
{
	const auto lastDot = str.find_last_of(".");
	if (lastDot == std::string::npos)
		return str;

	return str.substr(lastDot, str.length());
}

std::string BinaryStream::GetFilePath(const std::string& str)
{
	const auto found = str.find_last_of("/\\");
	return found != std::string::npos ? (str.substr(0, found) + "\\") : "";
}

/// <summary>
/// Only for console.
/// Print HEX table
/// </summary>
void BinaryStream::PrintFile(const uint8_t& size) const
{
	PrintFile(this->buffer, 0, this->fileSize, size);
}

void BinaryStream::PrintFile(const int& start, const size_t& fileSize, const uint8_t& size) const
{
	std::printf("Printing file from %d position to %d", start, fileSize);
	PrintFile(this->buffer, start, fileSize, size);
}

void BinaryStream::PrintFile(const char* buffer, const int& start, const size_t& fileSize, const uint8_t& size)
{
	if (buffer == nullptr || fileSize == 0 || start > fileSize)
		throw "File buffer is empty or size equals zero!";

	if (size % 4 != 0)
		throw "Incorrect table size.";

	std::cout << "\nOffeset (h) | ";
	for (int i = 0; i < size; ++i)
		std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << i << " ";
		
	std::cout << "| Decoded Text\n" << std::string(14 + (int)size * 3 + 2 + 16, '-') << '\n';
	int linesCount = (fileSize - start) % size == 0 ? (fileSize - start) / size : (fileSize - start) / size + 1;
	for (int i = 0; i != linesCount; ++i)
	{
		std::cout << " 0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << i * size << " | ";
		for (uint8_t j = 0; j != size; ++j)
		{
			if (i * size + j >= (fileSize - start))
			{
				std::cout << std::string((size - j) * 3, ' ');
				break;
			}

			std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)(uint8_t)buffer[start + i * size + j] << " ";
		}

		std::cout << "| ";
		for (uint8_t j = 0; j != size; ++j)
		{
			if (i * size + j >= (fileSize - start))
				break;

			std::cout << (buffer[start + i * size + j] >= 0 && buffer[start + i * size + j] <= 31 ? '.' : buffer[start + i * size + j]);
		}

		std::cout << std::endl;
	}
}

void BinaryStream::PrintFile(const BinaryStream& bstream, const uint8_t& size)
{
	PrintFile(bstream.GetBuffer(), 0, bstream.GetSize(), size);
}

void BinaryStream::WriteFile(const std::string& fileName)
{
	WriteFile(fileName, buffer, fileSize);
}

#include <fstream>
void BinaryStream::WriteFile(const std::string& filePath, char* data, const size_t& size)
{
	std::ofstream file(filePath, std::ios::binary);
	file.write(data, size);
	file.close();
}
