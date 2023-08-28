#include "Server.hpp"

#include "Room.hpp"
#include "Session.hpp"

Server::Server(boost::asio::io_context& io_context, tcp::endpoint& endpoint)
    : acceptor_(io_context, endpoint) {
    room_ = std::make_shared<Room>();

    co_spawn(io_context, accept(), detached);
}

boost::asio::awaitable<void> Server::accept() {
    for (;;) {
        tcp::socket socket =
            co_await acceptor_.async_accept(boost::asio::use_awaitable);

        try {
            std::cout << "client accepted! " << std::endl;
            std::make_shared<Session>(std::move(socket), room_)->start();
        } catch (std::exception& e) {
            std::cerr << "error in acception" << e.what() << std::endl;
        }
    }
}
