#include "channel.hpp"

template<typename MessageType>
std::optional<MessageType> Channel<MessageType>::WaitForNextMessage() {
    std::unique_lock l(mQueueMutex);

get_message:
    if (!mQueue.empty()) {
        MessageType m = mQueue.front();
        mQueue.pop();

        return { std::move(m) };
    }

    if (!open) return {};

    mCv.wait(l, [this]{ return !mQueue.empty() || !open; });
    goto get_message;
}

template<typename MessageType>
std::optional<MessageType> Channel<MessageType>::NextMessage() {
    std::unique_lock l(mQueueMutex);

    if (!mQueue.empty()) {
        MessageType m = mQueue.front();
        mQueue.pop();

        return { std::move(m) };
    }

    return {};
}

template<typename MessageType>
bool Channel<MessageType>::IsEmpty() {
    return mQueue.empty();
}

template<typename MessageType>
void Channel<MessageType>::QueueMessage(const MessageType& message) {
    std::unique_lock l(mQueueMutex);
    if (!open) return;

    if (max_capacity > 0 && mQueue.size() == max_capacity) {
        mQueue.pop();
    }

    mQueue.push(message);
    l.unlock();
    mCv.notify_all();
}

template<typename MessageType>
void Channel<MessageType>::QueueMessage(const MessageType&& message) {
    QueueMessage(message);
}

template<typename MessageType>
void Channel<MessageType>::Close() {
    std::unique_lock l(mQueueMutex);
    open = false;
    l.unlock();
    mCv.notify_all();
}
