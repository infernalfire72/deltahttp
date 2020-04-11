#pragma once

#include <string>

#include "Methods.h"

namespace DeltaHttp {
	struct Route {
	public:
		std::string Path;
		HttpMethod AllowedMethods;
	};
}