#ifndef Server_hpp
#define Server_hpp

#include <boost/asio.hpp>

#include <cstdlib>
#include <iostream>
#include <set>


using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

class Room;

class Server : public std::enable_shared_from_this<Server> {
	 public:
	Server(boost::asio::io_context& io_context, tcp::endpoint& endpoint);
	boost::asio::awaitable<void> accept();

	 private:
		tcp::acceptor acceptor_;
	std::shared_ptr<Room> room_;
	
};

#endif /* Server_hpp */
