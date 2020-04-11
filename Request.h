#pragma once

#include <vector>
#include "Methods.h"
#include "Header.h"

namespace DeltaHttp {
	class Request {
	public:
		std::string URI;
		std::string QueryString;
		HttpMethod Method;
		Header Headers[64];

		size_t ContentLength;
		std::vector<unsigned char> Body;

		Request() {
			Method = (HttpMethod)0;
			ContentLength = 0;
			Body.reserve(1024);
		}

		std::string GetHeader(const std::string& key) const {
			for (auto& i : Headers)
				if (iequals(key, i.Key))
					return i.Value;
			return "";
		}

		static Request* Parse(const std::string& httpHeader, const char* payload_start) {
			auto request = new Request();

			unsigned int hstart = httpHeader.find(' ');
			if (hstart == -1)
				return request;

			request->Method = FromString(httpHeader.substr(0, hstart++));
			auto path = httpHeader.substr(hstart, httpHeader.find(' ', hstart) - hstart);

			unsigned int qsplit = path.find('?');
			if (qsplit != -1)
				request->QueryString = path.substr(qsplit);
			request->URI = path.substr(0, qsplit);
			for (auto& i : request->Headers) {
				unsigned int split = httpHeader.find(": ", hstart);
				if (split == -1) break; // Invalid Header Line
				i.Key = httpHeader.substr(hstart, split - hstart);
				i.Value = httpHeader.substr(split + 2, (hstart = httpHeader.find("\r\n", hstart) + 2) - split - 4);
				if (hstart == 1) break;
			}

			if (request->Method != HttpMethod::GET) {
				try {
					request->ContentLength = std::stoll(request->GetHeader("Content-Length"));
				}
				catch (...) {
					request->ContentLength = 0;
				}

				if (request->ContentLength) {
					request->Body.resize(request->ContentLength);
					memcpy(&request->Body[0], payload_start + 4, request->ContentLength);
				}

			}

			return request;
		}
	};
}