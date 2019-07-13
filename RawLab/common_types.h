#ifndef RAWLAB_COMMON_TYPES_H
#define RAWLAB_COMMON_TYPES_H

#define SIZEOFDWORD 4
#define DECIMAL4 4
#define DECIMAL2 2

namespace RAWLAB {
	enum WBSTATE { WBDAYLIGHT, WBASSHOT, WBAUTO, WBUSER, WBUNKNOWN };
}

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

struct ImgBuff
{
	unsigned char* m_buff;
	int m_width;
	int m_height;

	int* m_histogram;//[4][LIBRAW_HISTOGRAM_SIZE]

	ImgBuff() : m_buff(nullptr), m_width(0), m_height(0), m_histogram(nullptr){}
	~ImgBuff() { delete[] m_buff; delete[] m_histogram; }
	void alloc_histogram()
	{
		m_histogram = new int[LIBRAW_HISTOGRAM_SIZE * 4]();
	}
};

struct RgbBuff: public ImgBuff
{
};

struct BmpBuff : public ImgBuff
{
	unsigned char m_bits;	// только кратный 8
	unsigned char m_colors;
	BmpBuff() : m_bits(8), m_colors(3) {}
};

#endif