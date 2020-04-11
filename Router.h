#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <thread>

#ifdef _WIN32
#include <WinSock2.h>
#elif defined(__linux__) || defined(__CYGWIN__)
#include "Linux.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "Methods.h"
#include "Route.h"
#include "Request.h"
#include "Response.h"

namespace DeltaHttp {
	class Router {
	public:
		Router() {
			memset(&ssockaddr, 0, sizeof(sockaddr_in));
			ssockaddr.sin_family = AF_INET;
			ssockaddr.sin_addr.s_addr = INADDR_ANY;

			threads.resize(1);
		}

		void HandleRoute(const std::string& path, void (handlerFunc)(const Request&, Response&), const HttpMethod& AllowedMethods = HttpMethod::All) {
			auto r = new Route{ path, AllowedMethods };
			routes.emplace(r, handlerFunc);
		}

		void Listen(const unsigned short& port) {
#ifdef _WIN32
			int wserr = WSAStartup(2, &wsa);
			if (wserr != 0) {
				printf("Failed to start WSocket\nError Code: %d\n", wserr);
				return;
			}
#endif
			ssockaddr.sin_port = htons(port);

			SOCKET sockfd = socket(AF_INET, SOCK_STREAM, 0);

			if (bind(sockfd, (sockaddr*)&ssockaddr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
				std::cerr << "Couldn't bind to :" << port << std::endl;
				return;
			}

			if (listen(sockfd, 10) == SOCKET_ERROR) {
				std::cerr << "Failed to listen" << std::endl;
				return;
			}

			std::cout << "Listening on :" << port << " with " << threads.size() << " Threads" << std::endl;

			for (auto& i : threads) {
				i = std::thread(
					[&] {
						while (1) {
							SOCKET clientfd = accept(sockfd, NULL, NULL);

							if (clientfd == INVALID_SOCKET) {
								std::cerr << "Invalid Socket" << std::endl;
								return;
							}
							char* buffer = (char*)malloc(4096);
							int rcvd = recv(clientfd, buffer, 4096, 0);
							/*int rcvd = 0, i = 1;
							int a;

							do {
								if (i == 1)
									++i;
								else {
									char* nbuffer = (char*)malloc(4096 * i);
									memcpy(&nbuffer, &buffer, 4096 * i - 4096);
									free(buffer);
									buffer = nbuffer;
								}

								a = recv(clientfd, buffer, 4096 * i, 0);
								if (!a) break; // EOF

								rcvd += a;
								i++;
							} while (a == 4096);*/

							if (buffer == NULL) return;

							if (!rcvd) {
								std::cerr << "Socket Read Error" << std::endl;
								return;
							}

							char* payload_start = strstr(buffer, "\r\n\r\n");
							std::string httpHeader(buffer, payload_start ? (size_t)(payload_start - buffer) : rcvd);
							auto r = Request::Parse(httpHeader, payload_start);
							free(buffer);
							Response res(clientfd);
							for (auto& i : routes) {
								if (i.first->Path != r->URI) continue;
								if (i.first->AllowedMethods & r->Method) {
									i.second(*r, res);
									res.EndResponse(200);
									break;
								}
							}

							res.Write("Method Not Allowed");
							res.EndResponse(405);
							closesocket(clientfd);
							delete r;
						}
					}
				);
			}

			for (auto& i : threads) {
				i.join();
			}
		}

		// Multithreading VWERGFWVJDC
		Router& operator * (const size_t& nThreads) {
			threads.resize(nThreads);
			return *this;
		}

		void Stop() {

		}
	private:
		std::map<Route*, void(*)(const Request&, Response&)> routes;
		std::vector<std::thread> threads;

#ifdef _WIN32
		WSADATA wsa; // Windows Sockets Startup Data
#endif
		SOCKET sockfd;
		sockaddr_in ssockaddr;
	};
}
