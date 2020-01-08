#include "ClQueue.hpp"

ClQueue::ClQueue()
    : queue_(ClContext::Get().context, ClContext::Get().deviceDefault, CL_QUEUE_PROFILING_ENABLE) {
}

cl::CommandQueue &ClQueue::getQueue() {
    return queue_;
}

void ClQueue::addEvent(std::string const &name) {
    eventMapping_.insert(std::pair<std::string, unsigned int>(name, event_.size()));
    event_.push_back(cl::Event());
}

cl::Event &ClQueue::getEvent(std::string const &name) {
    return event_[eventMapping_.at(name)];
}

std::vector<cl::Event> &ClQueue::getEventBuffer() {
    return event_;
}
std::map<std::string, unsigned int> &ClQueue::getEventMap() {
    return eventMapping_;
}