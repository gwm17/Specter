#ifndef SPECTRUM_SERIALIZER_H
#define SPECTRUM_SERIALIZER_H

namespace Navigator {

	class NAV_API SpectrumSerializer
	{
	public:
		SpectrumSerializer(const std::string& filepath);
		~SpectrumSerializer();

		void SerializeData();
		void DeserializeData();

		inline const std::string& GetFilename() { return m_filename; }

	private:
		std::string m_filename;
	};

}

#endif