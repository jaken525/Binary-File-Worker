#include "../includes/BinaryStream.h"

void BinaryStream::Clear()
{
	this->pos = 0;
	this->fileSize = 0;
	if (buffer = NULL) delete[] this->buffer;
}

bool BinaryStream::OpenFile(std::string filename)
{
	HANDLE hFile = CreateFile(
		(LPCWSTR)filename.c_str(),		// file to open
		GENERIC_READ,			// open for reading
		FILE_SHARE_READ,		// share for reading
		NULL,					// default security
		OPEN_EXISTING,			// existing file only
		FILE_ATTRIBUTE_NORMAL,	// normal file
		NULL					// no attr. template
	);
	Clear();
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

std::string BinaryStream::WriteLong(int num)
{
	std::string result(4, 0);
	for (int i = 0; i < 4; ++i)
		result[i] = static_cast<char>((num >> (i * 8)) & 0xFF);
	return result;
}

std::string BinaryStream::WriteFloat(float num)
{
	std::string result(4, 0);
	union
	{
		float f;
		uint32_t u;
	} converter;
	converter.f = num;
	for (int i = 0; i < 4; ++i)
		result[i] = static_cast<char>((converter.u >> (i * 8)) & 0xFF);
	return result;
}

char BinaryStream::WriteShort(int num)
{
	return static_cast<char>(num & 0xFF);
}

std::string BinaryStream::WriteString(int size, std::string str)
{
	std::string result(str);
	result.resize(size, '\0');
	return result;
}

bool BinaryStream::Jump(int jump)
{
	if (pos + jump > fileSize)
		throw "Position out of file buffer.\n";
	pos += jump;
	return true;
}

std::string BinaryStream::ReadString(int size)
{
	if (pos + size > fileSize)
		throw "Position out of file buffer.\n";
	std::string result = "";
	for (int i = 0; i < size; i++)
		result += this->buffer[pos];
	pos += size;
	return result;
}

std::string BinaryStream::ReadStringWithoutZeroes(int size)
{
	if (pos + size > fileSize)
		throw "Position out of file buffer.\n";
	std::string result = "";
	for (int i = 0; i < size; i++)
		if (this->buffer[pos] != 0)
			result += this->buffer[pos];
		else
			break;
	pos += size;
	return result;
}

float BinaryStream::ReadFloat()
{
	if (pos + 4 > fileSize)
		throw "Position out of file buffer.\n";
	float result = 0.0;
	unsigned long b = ReadLong();
	memcpy(&result, &b, 4);
	return result;
}

unsigned long BinaryStream::ReadLong()
{
	if (pos + 4 > fileSize)
		throw "Position out of file buffer.\n";
	unsigned long result = {
		(unsigned int)((uint8_t)this->buffer[pos] * 0x00000001) + \
		(unsigned int)((uint8_t)this->buffer[pos + 1] * 0x00000100) + \
		(unsigned int)((uint8_t)this->buffer[pos + 2] * 0x00010000) + \
		(unsigned int)((uint8_t)this->buffer[pos + 3] * 0x01000000)
	};
	pos += 4;
	return result;
}

unsigned int BinaryStream::ReadShortShort()
{
	if (pos + 1 > fileSize)
		throw "Position out of file buffer.\n";
	pos += 1;
	return (unsigned int)((uint8_t)this->buffer[this->pos] * 0x00000001);
}

unsigned short BinaryStream::ReadShort()
{
	if (pos + 2 > fileSize)
		throw "Position out of file buffer.\n";
	unsigned short result = {
		(unsigned int)((uint8_t)this->buffer[this->pos] * 0x00000001) + \
		(unsigned int)((uint8_t)this->buffer[this->pos + 1] * 0x00000100)
	};
	pos += 2;
	return result;
}

std::string BinaryStream::GetFilenameFile(const std::string str)
{
	size_t found;
	std::string strt;
	found = str.find_last_of("/\\");
	if (found < str.size())
	{
		strt = str.substr(found + 1, -1);
		found = strt.find(".");
		if (found < strt.size())
			strt = strt.substr(0, found);
	}
	else
		strt = str;
	size_t lastdot = strt.find_last_of(".");
	if (lastdot == std::string::npos)
		return strt;
	return strt.substr(0, lastdot);
}

std::string BinaryStream::GetFileNamePath(const std::string str)
{
	size_t found;
	found = str.find_last_of("/\\");
	return found != std::string::npos ? (str.substr(0, found) + "\\") : "";
}

/// <summary>
/// Only for console.
/// Print HEX table
/// </summary>
void BinaryStream::PrintFile(uint8_t size)
{
	if (buffer = NULL)
		throw "File buffer is empty!";

	if (size % 4 != 0)
	{
		std::cout << "Incorrect table size.";
		return;
	}

	std::cout << "Offeset (h) | ";
	for (int i = 0; i < size; ++i)
		std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << i << " ";
	std::cout << "| Decoded Text\n" << std::string(14 + (int)size * 3 + 2 + 16, '-') << '\n';

	int linesCount = fileSize % size == 0 ? fileSize / size : fileSize / size + 1;
	for (int i = 0; i < linesCount; ++i)
	{
		std::cout << " 0x" << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << i * size << " | ";
		for (uint8_t j = 0; j < size; ++j)
		{
			if (i * size + j >= this->fileSize)
			{
				std::cout << std::string((size - j) * 3, ' ');
				break;
			}
			std::cout << std::setfill('0') << std::setw(sizeof(uint8_t) * 2) << std::hex << (int)this->buffer[i * size + j] << " ";
		}
		std::cout << "| ";
		for (uint8_t j = 0; j < size; ++j)
		{
			if (i * size + j >= this->fileSize)
				break;
			std::cout << (this->buffer[i * size + j] >= 0 && this->buffer[i * size + j] <= 31 ? '.' : this->buffer[i * size + j]);
		}
		std::cout << std::endl;
	}
}