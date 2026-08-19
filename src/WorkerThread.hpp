#pragma once

#include "libjournal.hpp"
#include "ThreadSafeQueue.hpp"
#include <memory>
#include <string>

void worker_thread(Journal &journal, ThreadSafeQueue<std::unique_ptr<std::string>> &queue);