#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <boost/asio.hpp>
#include "../http/HttpTransaction.hpp"

using boost::asio::ip::tcp;
using Handler = std::function<void(HttpRequest&, HttpResponse&)>;

class Router {
public:
    void post(const std::string& path, Handler handler);
    void handleRequest(tcp::socket&& socket, const std::string& basePath = "");
      Router(/* args */);
    ~Router();
private:
    std::unordered_map<std::string, Handler> post_routes;
};
