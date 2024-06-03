#ifndef KINT_CHANNEL_HPP
#define KINT_CHANNEL_HPP

#include <memory>
#include <vector>
#include <queue>
#include <optional>
#include <condition_variable>

template<typename MessageType>
class Channel {
private:
    std::queue<MessageType> mQueue;
    std::mutex mQueueMutex;
    std::condition_variable mCv;
    bool open = true;
    uint32_t max_capacity = 0;

public:
    Channel() = default;
    explicit Channel(uint32_t capacity) : max_capacity(capacity) {}

    ~Channel() = default;

    Channel(const Channel&) = delete;
    Channel& operator=(const Channel&) = delete;

    std::optional<MessageType> WaitForNextMessage();
    std::optional<MessageType> NextMessage();
    bool IsEmpty();
    void QueueMessage(const MessageType&& message);
    void QueueMessage(const MessageType& message);
    void Close();
};

#include "channel.cpp"

#endif //KINT_CHANNEL_HPP
