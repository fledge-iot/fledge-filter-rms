/*
 * FogLAMP "RMS" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */
#include <filter.h>
#include <reading_set.h>
#include <string>
#include <map>

class RMSFilter : public FogLampFilter {
	public:
		RMSFilter(const std::string& filterName,
			ConfigCategory& filterConfig,
			OUTPUT_HANDLE *outHandle,
			OUTPUT_STREAM out);
		void	setSampleSize(unsigned int size)
		{
			m_sampleSize = size;
		};
		void	addValue(const std::string& asset, const std::string& dpname, long value);
		void	addValue(const std::string& asset, const std::string& dpname, double value);
		void	outputData(ReadingSet&);
		bool	sendRawData();
	private:
		class RMSData {
			public:
				RMSData() : samples(0), cumulative(0.0)
				{
				};
				unsigned int 	samples;
				double	      	cumulative;
				double		peak_max;
				double		peak_min;
		};
		unsigned int	m_sampleSize;
		bool		m_sendRawData;
		bool		m_sendPeak;
		std::string	m_assetName;
		std::map<std::pair<std::string, std::string>, RMSData *>
				m_values;
};
