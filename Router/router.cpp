#include "router.hpp"
#include <iostream>
Router::Router() {}
Router::~Router() {}
void Router::post(const std::string& path, Handler handler) {
    post_routes[path] = handler;
}

void Router::handleRequest(tcp::socket&& socket, const std::string& basePath) {
    auto buffer = std::make_shared<std::array<char, 4096>>();
    auto sock_ptr = std::make_shared<tcp::socket>(std::move(socket));

    sock_ptr->async_read_some(boost::asio::buffer(*buffer),
        [buffer, sock_ptr, this, basePath](boost::system::error_code ec, std::size_t len) mutable {
            if (!ec) {
                std::string data(buffer->data(), len);
                HttpRequest request(data);
                HttpResponse response;

                std::string fullPath = request.path;
                std::string routePath;

          
                if (fullPath.size() >= basePath.size()) {
                    routePath = fullPath.substr(basePath.size());
                } else {
                  
                    response.status_code = 400;
                    response.status_message = "Bad Request";
                    response.json(R"({"error":"Invalid route path"})");

                    std::string res = response.toString();
                    boost::asio::async_write(*sock_ptr, boost::asio::buffer(res),
                        [sock_ptr](boost::system::error_code write_ec, std::size_t) {
                            if (write_ec) {
                                std::cerr << "[Router] Write error: " << write_ec.message() << "\n";
                            }
                        }
                    );
                    return;
                }

                if (request.method == "POST") {
                    auto it = post_routes.find(routePath);
                    if (it != post_routes.end()) {
                        it->second(request, response);  

                        std::string res = response.toString();
                        boost::asio::async_write(*sock_ptr, boost::asio::buffer(res),
                            [sock_ptr](boost::system::error_code write_ec, std::size_t) {
                                if (write_ec) {
                                    std::cerr << "[Router] Write error: " << write_ec.message() << "\n";
                                }
                            }
                        );
                        return;
                    }
                }

             
                response.status_code = 404;
                response.status_message = "Not Found";
                response.json(R"({"error":"Route not found"})");

                std::string res = response.toString();
                boost::asio::async_write(*sock_ptr, boost::asio::buffer(res),
                    [sock_ptr](boost::system::error_code write_ec, std::size_t) {
                        if (write_ec) {
                            std::cerr << "[Router] Write error: " << write_ec.message() << "\n";
                        }
                    }
                );
            } else {
                std::cerr << "[Router] Read error: " << ec.message() << "\n";
            }
        }
    );
}
