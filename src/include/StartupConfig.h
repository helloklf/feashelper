#ifndef _FEASHELPER_STARTUPCONFIG_H
#define _FEASHELPER_STARTUPCONFIG_H

class StartupConfig {
	public:
		bool useIoctl;
		const char *configFile;
};

static StartupConfig startupConfig = {};

#endif //_FEASHELPER_STARTUPCONFIG_H
