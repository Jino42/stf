#pragma once

#include "ClContext.hpp"
#include <NTL.hpp>
#include <map>

class ClQueue {
  public:
    ClQueue();
    ~ClQueue() = default;
    ClQueue(ClQueue const &) = delete;


    cl::CommandQueue &getQueue();

    void addEvent(std::string const &name);

    cl::Event &getEvent(std::string const &name);

    std::vector<cl::Event> &getEventBuffer();
    std::map<std::string, unsigned int> &getEventMap();

  private:
    cl::CommandQueue queue_;
    std::vector<cl::Event> event_;
    std::map<std::string, unsigned int> eventMapping_;
};
