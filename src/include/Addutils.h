#pragma once

enum model_list {
	UNKNOWN,
	QCOM,      // K50U, 12T Pro
	MTK,  // K50, N11TP, K50P, 12PD, ...
	OLD // k40s
};

void addutils(const std::string &Frontpkgname);
