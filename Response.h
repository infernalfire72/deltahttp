#pragma once

#include <cstring>
#include <string>
#include <vector>
#include <sstream>

#include "Header.h"

#if _WIN32
#include <WinSock2.h>
#endif

namespace DeltaHttp {

	class Response {
	public:
		Response(SOCKET cfd) {
			clientfd = cfd;
			data.reserve(512);
			written = false;
		}

		inline void Write(const char* value) {
			Write(std::string(value));
		}

		void Write(const std::string& value)
		{
			data.resize(data.size() + value.size());
			memcpy(&data[data.size() - value.size()], value.c_str(), value.size());
		}

		template<typename T>
		void Write(const T& v) {
			data.resize(data.size() + sizeof(v));
			memcpy(&data[data.size() - sizeof(v)], &v, sizeof(v));
		}

		template<typename T>
		void Response::Write(const std::vector<T>& values)
		{
			data.resize(data.size() + values.size());
			memcpy(&data[data.size() - values.size()], &values[0], values.size());
		}

		void EndResponse(int code) {
			if (written) return;
			written = 1;
			std::ostringstream strs;

			switch (code) {
			case 200:
				strs << "HTTP/1.1 200 OK\r\n";
				break;
			case 400:
				strs << "HTTP/1.1 400 Bad Request\r\n";
				break;
			case 403:
				strs << "HTTP/1.1 403 Forbidden\r\n";
				break;
			case 404:
				strs << "HTTP/1.1 404 Not Found\r\n";
				break;
			case 405:
				strs << "HTTP/1.1 405 Method Not Allowed\r\n";
				break;
			case 408:
				strs << "HTTP/1.1 408 Request Timeout\r\n";
				break;
			case 500:
				strs << "HTTP/1.1 500 Internal Server Error\r\n";
				break;
			default:
				strs << "HTTP/1.1 " << code << " Unknown Statuscode\r\n";
				break;
			}

			for (auto& i : Headers)
				if (i.Key.length() && i.Value.length())
					strs << i.Key << ": " << i.Value << "\r\n";

			strs << "Content-Length: " << data.size() << "\r\n\r\n";

			auto headers = strs.str();
			send(clientfd, headers.c_str(), headers.size(), 0);

			if (data.size())
				send(clientfd, (const char*)&data[0], data.size(), 0);
		}

		void SetHeader(const std::string& Key, const std::string& Value) {
			for (auto& i : Headers) {
				if (!i.Key.size()) {
					i = { Key, Value };
					break;
				}
				else if (iequals(i.Key, Key)) {
					i.Value = Value;
					break;
				}
			}
		}

	private:
		SOCKET clientfd;
		bool written;
		Header Headers[64];
		std::vector<unsigned char> data;
	};
}