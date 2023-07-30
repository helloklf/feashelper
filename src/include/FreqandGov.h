#pragma once

#include "Androidutils_feas.h"

std::string getGov();

void restore(const AndroidDeviceFEAS &device);

void setGov(const std::string &governor);

void setGov_normal(const AndroidDeviceFEAS &device);
