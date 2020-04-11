#pragma once

#include "Header.h"

#undef DELETE
namespace DeltaHttp {
	/// <summary>
	/// HTTP Methods
	/// </summary>
	enum HttpMethod {
		GET = 1,
		POST = 2,
		PATCH = 4,
		PUT = 8,
		DELETE = 16,
		OPTIONS = 32,

		All = GET | POST | PATCH | PUT | DELETE | OPTIONS
	};

	constexpr HttpMethod operator ""_Method(const char* str, size_t) {
		return iequals(str, "GET") ? HttpMethod::GET :
			iequals(str, "POST") ? HttpMethod::POST :
			iequals(str, "PATCH") ? HttpMethod::PATCH :
			iequals(str, "PUT") ? HttpMethod::PUT :
			iequals(str, "DELETE") ? HttpMethod::DELETE :
			iequals(str, "OPTIONS") ? HttpMethod::OPTIONS :
			(HttpMethod)0;
	}

	inline HttpMethod FromString(const std::string& str) {
		return iequals(str, "GET") ? HttpMethod::GET :
			iequals(str, "POST") ? HttpMethod::POST :
			iequals(str, "PATCH") ? HttpMethod::PATCH :
			iequals(str, "PUT") ? HttpMethod::PUT :
			iequals(str, "DELETE") ? HttpMethod::DELETE :
			iequals(str, "OPTIONS") ? HttpMethod::OPTIONS :
			(HttpMethod)0;
	}

	HttpMethod operator | (const HttpMethod& a, const HttpMethod& b) {
		return (HttpMethod)((int)a | (int)b);
	}
}