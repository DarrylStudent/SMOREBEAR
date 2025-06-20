#ifndef HANDLER_H
#define HANDLER_H

#include <atomic>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ControlState.h"
#include "ProcessManager.h"

class Handler
{
private:
    ControlState &cState;
    ProcessManager processManager;

public:
    Handler(ControlState &config);
    bool handleCommand(const std::string &command);
    void loadConfig(const std::string &filename);
    void viewProcess(Process *process);

    ProcessManager &getProcessManager() { return processManager; }
};

#endif // HANDLER_H