#include "Session.hpp"

#include "Room.hpp"

Session::Session(tcp::socket socket, std::shared_ptr<Room> room)
    : socket_(std::move(socket)), room_(room), timer_(socket_.get_executor()) {
    timer_.expires_at(std::chrono::steady_clock::time_point::max());
}

void Session::start() {
    co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()]() { return self->startRead(); }, detached);
    co_spawn(
        socket_.get_executor(),
        [self = shared_from_this()] { return self->write(); }, detached);
}

boost::asio::awaitable<void> Session::readNickname() {
    try {
        std::string readMsg;
        std::size_t n = co_await boost::asio::async_read_until(
            socket_, boost::asio::dynamic_buffer(readMsg, 1024), "\n",
            use_awaitable);
        nickname_ = readMsg.substr(0, n - 1);
    } catch (std::exception&) {
        stop();
    }
}
boost::asio::awaitable<void> Session::readMsg() {
    try {
        for (std::string readMsg;;) {
            std::size_t n = co_await boost::asio::async_read_until(
                socket_, boost::asio::dynamic_buffer(readMsg, 1024), "\n",
                use_awaitable);
            std::string sendLine = nickname() + ": " + readMsg;
            room_->deliver(sendLine);
            readMsg.erase(0, n);
        }
    } catch (std::exception&) {
        stop();
    }
}

boost::asio::awaitable<void> Session::startRead() {
    try {
        co_await readNickname();
        room_->join(shared_from_this());
        co_await readMsg();
    } catch (std::exception&) {
        stop();
    }
}

void Session::deliver(const std::string& msg) {
    std::cout << "msg " << msg << std::endl;
    writeMsgs_.push_back(msg);
    timer_.cancel_one();
}

boost::asio::awaitable<void> Session::write() {
    try {
        while (socket_.is_open()) {
            if (writeMsgs_.empty()) {
                boost::system::error_code ec;
                co_await timer_.async_wait(redirect_error(use_awaitable, ec));
            } else {
                co_await boost::asio::async_write(
                    socket_, boost::asio::buffer(writeMsgs_.front()),
                    use_awaitable);
                writeMsgs_.pop_front();
            }
        }
    } catch (std::exception& e) {
        stop();
    }
}

void Session::stop() {
    room_->leave(shared_from_this());
    socket_.close();
    timer_.cancel();
}

std::string Session::nickname() { return nickname_; }
