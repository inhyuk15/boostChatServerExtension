#ifndef Session_hpp
#define Session_hpp

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <queue>

using boost::asio::ip::tcp;

class Room;

class Session : public std::enable_shared_from_this<Session> {
	 public:
	Session(tcp::socket socket, std::shared_ptr<Room> room);
	void start();
	void read();
	
	void deliver(const std::string& msg);
	void write();
	
	std::string nickname();
	
	 private:
		tcp::socket socket_;
		boost::asio::streambuf buff_;
		static constexpr int BUFF_SIZE = 4096;
	std::shared_ptr<Room> room_;
	
	std::deque<std::string> writeMsgs_;
	std::string nickname_;
};

#endif /* Session_hpp */
