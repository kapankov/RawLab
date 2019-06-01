#ifndef RAWLAB_COMMON_TYPES_H
#define RAWLAB_COMMON_TYPES_H

class RawLabException : public std::exception
{
	const std::string m_msg;
public:
	RawLabException(const std::string& msg) : m_msg(msg) { }

	~RawLabException() { }

	virtual const char* what() const noexcept
	{
		return m_msg.c_str();
	}
};

struct RgbBuff
{
	unsigned char* m_buff;
	int m_width;
	int m_height;
	RgbBuff() : m_buff(nullptr), m_width(0), m_height(0) {}
	~RgbBuff() { delete[] m_buff; }
};

struct BmpBuff
{
	unsigned char* m_buff;
	int m_width;
	int m_height;
	unsigned char m_bits;	// только кратный 8
	unsigned char m_colors;
	BmpBuff() : m_buff(nullptr), m_width(0), m_height(0), m_bits(8), m_colors(3) {}
};

#endif