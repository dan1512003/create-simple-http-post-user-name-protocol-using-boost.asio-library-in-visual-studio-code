#include "app.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <iostream>

using boost::asio::ip::tcp;

app::app() {}
app::~app() {}

void app::use(const std::string& basePath, const Router& router) {
    routes.emplace_back(basePath, router);
}

void app::listen(int port) {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port));

        std::cout << "[app] Server is running on port " << port << "...\n";

       
        do_accept(acceptor, io_context);

     
        const int num_threads = std::thread::hardware_concurrency();
        std::vector<std::thread> thread_pool;

        for (int i = 0; i < num_threads; ++i) {
            thread_pool.emplace_back([&io_context]() {
                io_context.run();
            });
        }

        for (auto& t : thread_pool) {
            if (t.joinable()) t.join();
        }

    } catch (std::exception& e) {
        std::cerr << "[app] Exception: " << e.what() << "\n";
    }
}

void app::do_accept(tcp::acceptor& acceptor, boost::asio::io_context& io_context) {
    auto socket = std::make_shared<tcp::socket>(io_context);

    acceptor.async_accept(*socket, [this, &acceptor, &io_context, socket](boost::system::error_code ec) {
        if (!ec) {
           
            for (auto& [prefix, router] : routes) {
                router.handleRequest(std::move(*socket),prefix);
                return;
            }

        } else {
            std::cerr << "[app] Accept error: " << ec.message() << "\n";
        }

        do_accept(acceptor, io_context); 
    });
}