#ifndef Room_hpp
#define Room_hpp

#include <boost/asio.hpp>
#include <cstdlib>
#include <iostream>
#include <set>

using boost::asio::ip::tcp;

class Session;
class ChatMessage;

class Room {
public:
	Room();
	void join(std::shared_ptr<Session> session);
	void leave(std::shared_ptr<Session> session);
	void deliver(const ChatMessage& msg);
private:
	std::set<std::shared_ptr<Session>> sessions_;
};

#endif /* Room_hpp */
