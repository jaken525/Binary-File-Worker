#pragma once

#include <iostream>
#include <cstring>
#include <iomanip>

#ifdef _WIN32
#include <shlwapi.h>
#endif

class BinaryStream final
{
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

	BinaryStream& operator=(const BinaryStream& rhs);
	BinaryStream& operator=(BinaryStream&& rhs);
	operator const int() { return fileSize; }
	char* operator->() { return buffer; };
	const char* operator->() const { return buffer; };
	char& operator[](const int& x) const { return buffer[x]; }

	char ReadChar(const bool& withTrans = true);
	int8_t ReadInt8(const bool& withTrans = true);
	int16_t ReadInt16(const bool& withTrans = true);
	int32_t ReadInt32(const bool& withTrans = true);
	int64_t ReadInt64(const bool& withTrans = true);
	uint8_t ReadUInt8(const bool& withTrans = true);
	uint16_t ReadUInt16(const bool& withTrans = true);
	uint32_t ReadUInt32(const bool& withTrans = true);
	uint64_t ReadUInt64(const bool& withTrans = true);
	float ReadFloat(const bool& withTrans = true);
	double ReadDouble(const bool& withTrans = true);
	std::string ReadStringRaw(const int&, const bool& withTrans = true);
	std::string ReadString(const int&, const bool& withTrans = true);
	template <typename T> T ReadData(const int& size = -1, const bool& withTrans = true);

	bool Jump(const int&);
	bool Forward(const int&);
	bool Back(const int&);

	// Functions which convert values into bytes for writting into the file.

	static char ConvertChar(const int&);
	static std::string ConvertString(const int&, const std::string&);
	static std::string ConvertFloat(const float&);
	static std::string ConvertDouble(const double&);
	static char ConvertInt8(const uint8_t&);
	static std::string ConvertInt16(const short&);
	static std::string ConvertInt32(const int&);
	static std::string ConvertInt64(const long&);

	bool OpenFile(const std::string&);
	void Clear();
	static std::string GetFileNameWithExt(const std::string&);
	static std::string GetFileName(const std::string&);
	static std::string GetFileExtention(const std::string&);
	static std::string GetFilePath(const std::string&);

	char* GetBuffer() const { return buffer; }
	uint8_t* GetBufferUInt8() const { return reinterpret_cast<uint8_t*>(buffer); }
	size_t GetSize() const { return fileSize; }
	size_t GetPosition() const { return pos; }
	bool IsOpen() const { return isFileOpen; }
	static char* GetBuffer(const BinaryStream* bstream) { return bstream->buffer; }
	static size_t GetSize(const BinaryStream* bstream) { return bstream->fileSize; }
	static size_t GetPosition(const BinaryStream* bstream) { return bstream->pos; }
	static bool IsOpen(const BinaryStream* bstream) { return bstream->isFileOpen; }

	void PrintFile(const uint8_t& size = 16) const;
	void PrintFile(const int&, const size_t&, const uint8_t& size = 16) const;
	static void PrintFile(const char*, const int&, const size_t&, const uint8_t& size = 16);
	static void PrintFile(const BinaryStream&, const uint8_t& size = 16);

	void WriteFile(const std::string&);
	static void WriteFile(const std::string&, char*, const size_t&);

private:
	size_t fileSize;
	size_t pos;
	char* buffer;
	bool isFileOpen;
};

/// <summary>
/// Use carefully
/// </summary>
template <typename T> T BinaryStream::ReadData(const int& size, const bool& withTrans)
{
	try
	{
		int dataSize = size == -1 ? sizeof(T) : size;
		if (withTrans)
			Forward(dataSize);

		return *reinterpret_cast<T*>(&buffer[pos - dataSize]);
	}
	catch (...)
	{
		std::cout << "Can't read this data. Please try another type of data.\n";
	}
}
