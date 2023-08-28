#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>

#include "Server.hpp"
#include "Session.hpp"

using boost::asio::ip::tcp;

int main(int argc, const char* argv[]) {
    std::string port = "4000";
    boost::asio::io_context io_context;
    tcp::endpoint endpoint(tcp::v4(), stoi(port));
    try {
        auto server = std::make_shared<Server>(io_context, endpoint);

        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception in main " << e.what() << std::endl;
    }

    return 0;
}
