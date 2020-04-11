#pragma once

#include <string>

namespace DeltaHttp {
	bool iequals(const char* a, const char* b) {
		for (int i = 0; a[i] && b[i]; i++) {
			if (a[i] > b[i] && tolower(a[i]) != b[i])
				return false;
			else if (a[i] < b[i] && tolower(b[i]) != a[i])
				return false;
		}
		return true;
	}

	bool iequals(const std::string& a, const std::string& b) {
		if (a.size() != b.size()) return false;
		for (size_t i = 0; i < a.size(); i++)
			if (tolower(a[i]) != tolower(b[i])) return false;
		return true;
	}

	struct Header {
	public:
		std::string Key;
		std::string Value;
	};
}