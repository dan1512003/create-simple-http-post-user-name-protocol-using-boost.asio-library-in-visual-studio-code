#pragma once
#include <string>
#include <vector>
#include <utility>
#include <boost/asio.hpp>
#include "../Router/router.hpp"

class app
{
private:
      std::vector<std::pair<std::string, Router>> routes;
      void do_accept(boost::asio::ip::tcp::acceptor& acceptor, boost::asio::io_context& io_context);
public:
    app();
    ~app();
       void use(const std::string& basePath, const Router& router);
    void listen(int port);
};

