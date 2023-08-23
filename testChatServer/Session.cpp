#include "Session.hpp"

#include "Room.hpp"

Session::Session(tcp::socket socket, std::shared_ptr<Room> room)
    : socket_(std::move(socket)), buff_(BUFF_SIZE), room_(room) {}
void Session::start() {
    auto self(shared_from_this());
    boost::asio::async_read_until(
        socket_, buff_, '\n',
        [this, self](boost::system::error_code ec, size_t bytesRead) {
            if (!ec) {
                std::istream is(&buff_);
                std::getline(is, nickname_);

                room_->join(shared_from_this());
                read();

            } else if (ec == boost::asio::error::eof) {
                room_->leave(shared_from_this());
            } else {
                std::cerr << "error in reading " << ec.message() << std::endl;
                socket_.close();
            }
        });
}

void Session::read() {
    auto self(shared_from_this());
    boost::asio::async_read_until(
        socket_, buff_, '\n',
        [this, self](boost::system::error_code ec, size_t bytesRead) {
            if (!ec) {
                std::istream is(&buff_);
                std::string line;
                std::getline(is, line);
                std::string sendLine = nickname() + ": " + line + "\n";

                room_->deliver(sendLine);
                read();
            } else if (ec == boost::asio::error::eof) {
                room_->leave(shared_from_this());
            } else {
                std::cerr << "error in reading " << ec.message() << std::endl;
                socket_.close();
            }
        });
}

void Session::deliver(const std::string& msg) {
    bool writeInProgress = !writeMsgs_.empty();
    writeMsgs_.push_front(msg);
    if (!writeInProgress) {
        write();
    }
}

void Session::write() {
    auto self(shared_from_this());
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(writeMsgs_.front(), writeMsgs_.front().size()),
        [this, self](boost::system::error_code ec, size_t byteTransferred) {
            if (!ec) {
                writeMsgs_.pop_front();
                if (!writeMsgs_.empty()) {
                    write();
                }
            } else {
                std::cerr << "error in writing" << std::endl;
                socket_.close();
            }
        });
}

std::string Session::nickname() { return nickname_; }
