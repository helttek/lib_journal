#pragma once

#include <csignal>
#include "ThreadSafeQueue.hpp"

extern int threads_count;
extern ThreadSafeQueue<std::unique_ptr<std::string>> queue;

extern "C" void signal_handler(int signal_number);

void setup_sigint_handler();