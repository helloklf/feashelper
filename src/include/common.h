#ifndef _COMMON_HEADER_
#define _COMMON_HEADER_

#include <string>
#include <vector>

using namespace std;

bool fileExists(const string &filePath);

vector<string> splitString(const string &str, const string &delimiter, bool removeEmpty);

void setCpuAffinity(pid_t pid, const char *cpuSet);

#endif  // _COMMON_HEADER_ end
