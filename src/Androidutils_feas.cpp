#include "Androidutils_feas.h"
#include "fpsgo.h"
#include "StartupConfig.h"

AndroidDeviceFEAS::AndroidDeviceFEAS() {
	isFEASSupported = checkFEASType();
	if (isFEASSupported)
		FEASoff();
}

bool AndroidDeviceFEAS::getmtkFEASsupport() {
	return Testfile("/sys/module/mtk_fpsgo/parameters/perfmgr_enable");
}

bool AndroidDeviceFEAS::getqcomFEASsupport() {
	return Testfile("/sys/module/perfmgr/parameters/perfmgr_enable");
}

bool AndroidDeviceFEAS::getoldqcomFEASsupport() {
	return Testfile("/sys/module/perfmgr_policy/parameters/perfmgr_enable");
}

bool AndroidDeviceFEAS::getBocchiFASsupport() {
	return Testfile("/sys/module/bocchi_perfmgr/parameters/perfmgr_enable");
}

bool AndroidDeviceFEAS::checkFEASType() {
	if (getBocchiFASsupport()) {
		type = TYPE_BOCCHI;
		return true;
	}
	if (getqcomFEASsupport()) {
		type = TYPE_QCOM;
		return true;
	}
	if (getmtkFEASsupport()) {
		type = TYPE_MTK;
		return true;
	}
	if (getoldqcomFEASsupport()) {
		type = TYPE_QCOM_OLD;
		return true;
	}
	return false;
}

static void target_fps_helper_mtk(unsigned int &fps) {
	if (fps > 100) {
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_121", 1);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_91", 0);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", 0);
	} else if (fps < 100 && fps > 70) {
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_121", 0);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_91", 1);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", 0);
	} else {
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_121", 0);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_91", 0);
		Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", 1);
	}
}

static void target_fps_helper_qcom(unsigned int &fps) {
	if (fps > 100) {
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_121", 1);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_91", 0);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_61", 0);
	} else if (fps < 100 && fps > 70) {
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_121", 0);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_91", 1);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_61", 0);
	} else {
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_121", 0);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_91", 0);
		Lockvalue("/sys/module/perfmgr/parameters/target_fps_61", 1);
	}
}

bool AndroidDeviceFEAS::FEASon(unsigned int &fps) {
	switch (type) {
		case TYPE_BOCCHI:
			Lockvalue("/sys/module/bocchi_perfmgr/parameters/perfmgr_enable", 1);
			Lockvalue("/sys/module/bocchi_perfmgr/parameters/fixed_target_fps", fps);
			break;
		case TYPE_QCOM:
			if (startupConfig.useIoctl) {
				fpsgo_ioctl(1, 0, 0, 1, 1);
				fpsgo_ioctl(2, 0, 0, 1, 1);
			}
			if (!Lockvalue("/sys/module/perfmgr/parameters/perfmgr_enable", 1))
				return false;
			Lockvalue("/sys/module/perfmgr/parameters/fixed_target_fps", fps);
			target_fps_helper_qcom(fps);
			break;
		case TYPE_MTK:
			if (!Lockvalue("/sys/module/mtk_fpsgo/parameters/perfmgr_enable", 1))
				return false;
			Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps", fps);
			target_fps_helper_mtk(fps);
			break;
		case TYPE_QCOM_OLD:
			if (!Lockvalue("/sys/module/perfmgr_policy/parameters/perfmgr_enable", 1))
				return false;
			Lockvalue("/sys/module/perfmgr_policy/parameters/fixed_target_fps", fps);
			break;
	}
	return true;
}

bool AndroidDeviceFEAS::FEASoff() const {
	switch (type) {
		case TYPE_BOCCHI:
			Lockvalue("/sys/module/bocchi_perfmgr/parameters/perfmgr_enable", 0);
			Lockvalue("/sys/module/bocchi_perfmgr/parameters/fixed_target_fps", 0);
			break;
		case TYPE_QCOM:
			if (startupConfig.useIoctl) {
				fpsgo_ioctl(3, 0, 1, 1, 1);
			}
			if (!Lockvalue("/sys/module/perfmgr/parameters/perfmgr_enable", 0))
				return false;
			Lockvalue("/sys/module/perfmgr/parameters/fixed_target_fps", 0);
			Lockvalue("/sys/module/perfmgr/parameters/target_fps_61", 0);
			Lockvalue("/sys/module/perfmgr/parameters/target_fps_91", 0);
			Lockvalue("/sys/module/perfmgr/parameters/target_fps_121", 0);
			break;
		case TYPE_MTK:
			if (!Lockvalue("/sys/module/mtk_fpsgo/parameters/perfmgr_enable", 0))
				return false;
			Lockvalue("/sys/module/mtk_fpsgo/parameters/fixed_target_fps", 0);
			Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_121", 0);
			Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_91", 0);
			Lockvalue("/sys/module/mtk_fpsgo/parameters/target_fps_61", 0);
			break;
		case TYPE_QCOM_OLD:
			if (!Lockvalue("/sys/module/perfmgr_policy/parameters/perfmgr_enable", 0))
				return false;
			Lockvalue("/sys/module/perfmgr_policy/parameters/fixed_target_fps", 0);
			break;
	}
	return true;
}

bool AndroidDeviceFEAS::HasFEAS() const {
	return isFEASSupported;
}

int AndroidDeviceFEAS::getType() const {
	return type;
}

const char *AndroidDeviceFEAS::getTypeCStr() const{
	switch (type) {
		case TYPE_BOCCHI:
			return "BOCCHI";
		case TYPE_QCOM:
			return "QCOM";
		case TYPE_MTK:
			return "MTK";
		case TYPE_QCOM_OLD:
			return "QCOM_OLD";
	}
	return "UNKNOWN";
}
