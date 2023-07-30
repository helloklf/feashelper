#include "common.h"

#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

bool fileExists(const string &filePath) {
	struct stat st = {};

	if (stat(filePath.c_str(), &st) == 0) {
		return !S_ISDIR(st.st_mode);
	}
	return false;
}

vector<string> splitString(const string &str, const string &delimiter, bool removeEmpty) {
	vector<std::string> res;
	size_t idx = 0, idx_last = 0;

	while (idx < str.size()) {
		idx = str.find_first_of(delimiter, idx_last);
		if (idx == string::npos)
			idx = str.size();

		if (idx - idx_last != 0 || !removeEmpty)
			res.push_back(str.substr(idx_last, idx - idx_last));

		idx_last = idx + delimiter.size();
	}

	return res;
}

void setCpuAffinity(pid_t pid, const char *cpuSet) {
	cpu_set_t mask;
	if (pid < 0) return;
	long cpuNum = sysconf(_SC_NPROCESSORS_CONF);
	vector<string> cpus = splitString(cpuSet, ",", true);
	auto cpuSetSize = cpus.size();
	if (cpuSetSize > 0) {
		CPU_ZERO(&mask);
		for (int i = 0; i < cpuSetSize; i++) {
			int cpu = atoi(cpus[i].c_str());
			if (cpu >= cpuNum) continue;
			CPU_SET(cpu, &mask);
			//printf("skkk: cpunum=%d cpuset=%d\n", cpuNum, cpu);
		}
		sched_setaffinity(pid, sizeof(mask), &mask);
	}
}
