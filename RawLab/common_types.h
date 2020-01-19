#ifndef RAWLAB_COMMON_TYPES_H
#define RAWLAB_COMMON_TYPES_H

#define SIZEOFDWORD 4
#define DECIMAL4 4
#define DECIMAL3 3
#define DECIMAL2 2
#define DECIMAL0 0

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

using histogram_channel = std::array<unsigned int, 256>;


struct RgbBuff
{
	unsigned char* m_buff;
	size_t m_width;
	size_t m_height;
	int m_bits;	// только кратный 8
	int m_colors;
	void* m_profile;

	std::array<histogram_channel, 3> m_histogram = { };

	RgbBuff() : m_buff(nullptr), m_width(0), m_height(0), m_bits(8), m_colors(3), m_profile(nullptr) {}
	~RgbBuff() { delete[] m_buff; delete[] m_profile; }

	std::unique_ptr<RgbBuff> copy()
	{
		std::unique_ptr<RgbBuff> mycopy = std::make_unique<RgbBuff>();
		mycopy->m_width = m_width;
		mycopy->m_height = m_height;
		mycopy->m_bits = m_bits;
		mycopy->m_colors = m_colors;
		if (m_profile)
		{
			size_t len = ntohl(*static_cast<unsigned int*>(m_profile));
			mycopy->m_profile = new unsigned char[len];
			memcpy(mycopy->m_profile, m_profile, len);
		}

		size_t stride;
		stride = m_width * static_cast<size_t>(m_colors * m_bits / 8);
		if (stride & 3) stride += SIZEOFDWORD - stride & 3; // DWORD aligned
		mycopy->m_buff = new unsigned char[stride * m_height];
		memcpy(mycopy->m_buff, m_buff, stride * m_height);

		return std::move(mycopy);
	}
};

using RgbBuffPtr = std::unique_ptr<RgbBuff>;

#endif