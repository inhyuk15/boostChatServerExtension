#ifndef Session_hpp
#define Session_hpp

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <queue>

using boost::asio::ip::tcp;
using boost::asio::awaitable;
using boost::asio::co_spawn;
using boost::asio::detached;
using boost::asio::use_awaitable;

class Room;

class Session : public std::enable_shared_from_this<Session> {
	 public:
	Session(tcp::socket socket, std::shared_ptr<Room> room);
	void start();
	boost::asio::awaitable<void> readNickname();
	boost::asio::awaitable<void> readMsg();
	boost::asio::awaitable<void> startRead();
	
	void deliver(const std::string& msg);
	boost::asio::awaitable<void> write();
	void stop();
	
	std::string nickname();
	
	 private:
		tcp::socket socket_;
	std::shared_ptr<Room> room_;
	boost::asio::steady_timer timer_;
	
	std::deque<std::string> writeMsgs_;
	std::string nickname_;
};

#endif /* Session_hpp */
