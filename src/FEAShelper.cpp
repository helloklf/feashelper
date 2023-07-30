#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

#include "common.h"
#include "Androidutils_feas.h"
#include "S3profile.h"
#include "FreqandGov.h"
#include "StartupConfig.h"
#include "Logging.h"

int startServer() {
	LOGI("Use profile: %s", startupConfig.configFile);
	LOGI("Use ioctl: %d", startupConfig.useIoctl);

	listProfile profile(startupConfig.configFile);
	LOGI("profile start");
	if (!profile.Readsuccess) {
		// read error
		LOGE("Profile error");
		return ENOENT;
	}

	// read done
	LOGI("Read profile done.");
	profile.List();

	// update-profile daemon
	profile.startProfilemonitor(5);
	LOGI("startProfilemonitor");

	// creat device
	AndroidDeviceFEAS device;

	// test feas support
	if (!device.HasFEAS()) {
		LOGE("Unsupported device, or you are using an outdated version of FEASHelper.");
		// not supported
		return ENODEV;
	}
	LOGI("Device is supported.");
	LOGI("Device category: %s", device.getTypeCStr());

	// start Topappmonitor
	device.startTopappmonitor(3);
	LOGI("startTopappmonitor.");

	while (true) {
		if (profile.Inlist(device.getToppkg())) { // is a game in config
			// open feas
			device.FEASon(profile.fps);

			// Additional configuration
			// addutils(device.getToppkg());

			// set governor
			if (profile.performance_governor && device.getType() != TYPE_QCOM_OLD) {
				setGov("performance");
			}
		} else {
			// close feas
			device.FEASoff();
			setGov_normal(device);
			restore(device);
		}
		// package name update in fastest 1s
		sleep(1);
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int ret = 0;
	std::ostream::sync_with_stdio(false);
	std::cout << std::unitbuf;
	std::cout << argv[0] << ": Start running\n";

	for (int i = 0; i < argc; i++) {
		char *arg = argv[i];
		if (strncmp(arg, "-c", 2) == 0) {
			string _arg = argv[i + 1];
			if (!_arg.empty()) {
				startupConfig.configFile = strdup(_arg.c_str());
				LOGI("Config file: %s", _arg.c_str());
			}
		} else if (strncmp(arg, "-ioctl", 6) == 0) {
			startupConfig.useIoctl = true;
			LOGI("Use ioctl!\n");
			break;
		}
	}

	if (fileExists(startupConfig.configFile)) {
//		setCpuAffinity(0, "0,1,2,3");
		signal(SIGHUP, SIG_IGN);
		if (daemon(0, 0) == -1) {
			return ENOEXEC;
		}
		while (true) {
			int status = 0;
			pid_t pid = -1;

			switch (pid = fork()) {
				case -1:
					ret = ENOEXEC;
					break;
				case 0:
					ret = startServer();
					exit(ret);
				default:
					ret = waitpid(pid, &status, 0);
			}
			sleep(1);
		}
	} else {
		std::cout << argv[0] << ": Profile not exists!\n";
		ret = ENOENT;
	}

	return ret;
}
