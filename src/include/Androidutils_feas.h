#pragma once

#include "Androidutils.h"

enum DEVICE_TYPE_ID {
	TYPE_UNKNOWN = 0,
	TYPE_BOCCHI,
	TYPE_QCOM,
	TYPE_MTK,
	TYPE_QCOM_OLD,
};

// Feas Android device
class AndroidDeviceFEAS : public AndroidDevice {
		bool isFEASSupported = false;
		int type = TYPE_UNKNOWN;
	public:
		AndroidDeviceFEAS();

		AndroidDeviceFEAS(const AndroidDeviceFEAS &others) = delete; // dont't copy this

		static bool getmtkFEASsupport();

		static bool getqcomFEASsupport();

		static bool getoldqcomFEASsupport();

		static bool getBocchiFASsupport();

		bool checkFEASType();

		bool FEASon(unsigned int &fps);

		bool FEASoff() const;

		bool HasFEAS() const;

		int getType() const;

		const char *getTypeCStr() const;
};
