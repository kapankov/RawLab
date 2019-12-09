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

struct CmsParams
{
	int m_iColorSpace = 1; // 0 - Undefined(RAW), 1 - sRGB, 2 - AdobeRGB, 3 - WideRGB, 4 - ProPhoto, 5 - XYZ, 6 - ACES
	float cam_xyz[4][3] = {}; // Libraw imgdata.color.cam_xyz
	double gamm[6] = {}; // Libraw imgdata.params.gamm
	std::string output_profile; // LibRaw imgdata.params.output_profile

};

struct RgbBuff
{
	unsigned char* m_buff;
	size_t m_width;
	size_t m_height;
	int m_bits;	// только кратный 8
	int m_colors;
	std::unique_ptr<CmsParams> m_params;

	histogramvec m_histogram[3];

	RgbBuff() : m_buff(nullptr), m_width(0), m_height(0), m_bits(8), m_colors(3) {}
	~RgbBuff() { delete[] m_buff; }

	void alloc_histogram()
	{
		for (int i = 0; i < 3; ++i)
			m_histogram[i].resize(256);
	}

	std::unique_ptr<RgbBuff> copy()
	{
		std::unique_ptr<RgbBuff> mycopy = std::make_unique<RgbBuff>();
		mycopy->m_width = m_width;
		mycopy->m_height = m_height;
		mycopy->m_bits = m_bits;
		mycopy->m_colors = m_colors;
		if (m_params)
		{
			CmsParams& params= *(mycopy->m_params = std::make_unique<CmsParams>()).get();
			params.m_iColorSpace = m_params->m_iColorSpace;
			memcpy(params.gamm, m_params->gamm, sizeof(m_params->gamm));
			memcpy(params.cam_xyz, m_params->cam_xyz, sizeof(m_params->cam_xyz));
			params.output_profile = m_params->output_profile;
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