/*
 * FogLAMP "RMS" filter plugin.
 *
 * Copyright (c) 2018 Dianomic Systems
 *
 * Released under the Apache 2.0 Licence
 *
 * Author: Mark Riddoch
 */

#include <plugin_api.h>
#include <config_category.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string>
#include <iostream>
#include <filter_plugin.h>
#include <filter.h>
#include <reading_set.h>
#include <logger.h>
#include <rms.h>

#define FILTER_NAME "rms"
#define SAMPLE_SIZE "10"
#define DEFAULT_CONFIG "{\"plugin\" : { \"description\" : \"RMS filter plugin\", " \
                       		"\"type\" : \"string\", " \
				"\"default\" : \"" FILTER_NAME "\" }, " \
			 "\"rawData\": {\"description\": \"Switch to control the inclusion of the raw data in the output\", " \
				"\"type\": \"boolean\", " \
				"\"default\": \"false\" }, " \
			 "\"peak\": {\"description\": \"Include peak to peak values in readings\", " \
				"\"type\": \"boolean\", " \
				"\"default\": \"false\" }, " \
			 "\"assetName\": {\"description\": \"Name of the output asset for the RMS data\", " \
				"\"type\": \"string\", " \
				"\"default\": \"%%a RMS\" }, " \
			 "\"enable\": {\"description\": \"A switch that can be used to enable or disable execution of " \
					 "the RMS filter.\", " \
				"\"type\": \"boolean\", " \
				"\"default\": \"false\" }, " \
			"\"samples\" : {\"description\" : \"Number of samples per calculation.\", " \
				"\"type\": \"integer\", " \
				"\"default\": \"" SAMPLE_SIZE "\"} }"
using namespace std;

/**
 * The Filter plugin interface
 */
extern "C" {

/**
 * The plugin information structure
 */
static PLUGIN_INFORMATION info = {
        FILTER_NAME,              // Name
        "1.0.0",                  // Version
        0,                        // Flags
        PLUGIN_TYPE_FILTER,       // Type
        "1.0.0",                  // Interface version
	DEFAULT_CONFIG	          // Default plugin configuration
};

/**
 * Return the information about this plugin
 */
PLUGIN_INFORMATION *plugin_info()
{
	return &info;
}

/**
 * Initialise the plugin, called to get the plugin handle and setup the
 * output handle that will be passed to the output stream. The output stream
 * is merely a function pointer that is called with the output handle and
 * the new set of readings generated by the plugin.
 *     (*output)(outHandle, readings);
 * Note that the plugin may not call the output stream if the result of
 * the filtering is that no readings are to be sent onwards in the chain.
 * This allows the plugin to discard data or to buffer it for aggregation
 * with data that follows in subsequent calls
 *
 * @param config	The configuration category for the filter
 * @param outHandle	A handle that will be passed to the output stream
 * @param output	The output stream (function pointer) to which data is passed
 * @return		An opaque handle that is used in all subsequent calls to the plugin
 */
PLUGIN_HANDLE plugin_init(ConfigCategory* config,
			  OUTPUT_HANDLE *outHandle,
			  OUTPUT_STREAM output)
{
	RMSFilter *handle = new RMSFilter(FILTER_NAME,
						  *config,
						  outHandle,
						  output);

	return (PLUGIN_HANDLE)handle;
}

/**
 * Ingest a set of readings into the plugin for processing
 *
 * @param handle	The plugin handle returned from plugin_init
 * @param readingSet	The readings to process
 */
void plugin_ingest(PLUGIN_HANDLE *handle,
		   READINGSET *readingSet)
{
	RMSFilter *filter = (RMSFilter *)handle;
	if (!filter->isEnabled())
	{
		// Current filter is not active: just pass the readings set
		filter->m_func(filter->m_data, readingSet);
		return;
	}

	vector<Reading *>out;
	filter->ingest(readingSet->getAllReadingsPtr(), out);
	delete (ReadingSet *)readingSet;

	ReadingSet *newReadingSet = new ReadingSet(&out);
	filter->m_func(filter->m_data, newReadingSet);
}


/**
 * Call the shutdown method in the plugin
 */
void plugin_shutdown(PLUGIN_HANDLE *handle)
{
	RMSFilter *data = (RMSFilter *)handle;
	delete data;
}

// End of extern "C"
};
