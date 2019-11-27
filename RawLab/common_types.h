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

using histogramvec = std::vector<unsigned int>;

struct RgbBuff
{
	unsigned char* m_buff;
	int m_width;
	int m_height;
	unsigned char m_bits;	// только кратный 8
	unsigned char m_colors;

	histogramvec m_histogram[3];

	RgbBuff() : m_buff(nullptr), m_width(0), m_height(0), m_bits(8), m_colors(3) {}
	~RgbBuff() { delete[] m_buff; }
	void alloc_histogram()
	{
		for (int i = 0; i < 3; ++i)
			m_histogram[i].resize(256);

	}
};

using RgbBuffPtr = std::unique_ptr<RgbBuff>;

#endif