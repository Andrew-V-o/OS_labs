#ifndef BUFFERED_CHANNEL_H_
#define BUFFERED_CHANNEL_H_

#include <queue>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <stdexcept>

using namespace std;

template <class T>
class BufferedChannel
{
public:
    explicit BufferedChannel(int size)
        : buffer_size_(size), closed_(false)
    {
        if (size < 0)
        {
            throw std::invalid_argument("Buffer size must be non-negative");
        }
    }

    void Send(T value)
    {
        unique_lock<mutex> lock(mutex_);

        if (closed_)
        {
            throw runtime_error("Attempt to send to a closed channel");
        }

        send_cv_.wait(lock, [this]
                      { return queue_.size() < buffer_size_ || closed_; });

        if (closed_)
        {
            throw runtime_error("Attempt to send to a closed channel");
        }

        queue_.push(move(value));
        recv_cv_.notify_one();
    }

    pair<T, bool> Recv()
    {
        unique_lock<mutex> lock(mutex_);

        recv_cv_.wait(lock, [this]
                      { return !queue_.empty() || closed_; });

        if (queue_.empty() && closed_)
        {
            return {T{}, false};
        }

        T value = move(queue_.front());
        queue_.pop();
        send_cv_.notify_one();

        return {move(value), true};
    }

    void Close()
    {
        unique_lock<mutex> lock(mutex_);

        closed_ = true;
        send_cv_.notify_all();
        recv_cv_.notify_all();
    }

private:
    queue<T> queue_;
    mutex mutex_;
    condition_variable send_cv_;
    condition_variable recv_cv_;
    int buffer_size_;
    bool closed_;
};

#endif