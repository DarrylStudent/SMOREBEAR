#pragma once

#include "ProcessManager.h"
#include "ControlState.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

void spawnProcess(ControlState &cState, ProcessManager &processManager);
void coreWorker(Process *proc, int coreId, ProcessManager &pm, ControlState &cState);

void schedulerLoop(ControlState &cState, ProcessManager &processManager)
{
    std::vector<std::thread> coreThreads(cState.num_cpu);
    std::vector<Process *> coreAssignments(cState.num_cpu, nullptr);

    while (true)
    {
        for (int core = 0; core < cState.num_cpu; ++core)
        {
            // Clean up finished threads
            if (coreAssignments[core] && coreAssignments[core]->isFinished())
            {
                if (coreThreads[core].joinable())
                    coreThreads[core].join();
                coreThreads[core] = std::thread(); // Reset thread object
                coreAssignments[core] = nullptr;
            }

            // Assign new process if core is free
            if (!coreAssignments[core])
            {
                // Lock the ready queue before checking and popping
                auto &readyQ = processManager.getReadyQueue();
                if (!readyQ.empty())
                {
                    Process *proc = readyQ.front();
                    readyQ.pop();
                    processManager.moveToRunning(proc);
                    coreAssignments[core] = proc;

                    // Ensure the thread isn't joinable before assigning
                    if (coreThreads[core].joinable())
                        coreThreads[core].join();

                    coreThreads[core] = std::thread(coreWorker, proc, core, std::ref(processManager), std::ref(cState));
                }
            }
        }

        // Other logic for batch process creation, etc.
        cState.cpuTicks++;
        if (cState.batchRunning && cState.batch_process_freq > 0 && cState.cpuTicks % cState.batch_process_freq == 0)
        {
            spawnProcess(cState, processManager);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Join any remaining threads before exiting
    for (int core = 0; core < cState.num_cpu; ++core)
    {
        if (coreThreads[core].joinable())
            coreThreads[core].join();
    }
}

void coreWorker(Process *proc, int coreId, ProcessManager &pm, ControlState &cState)
{
    proc->setCurrentCore(coreId);
    int lastTick = cState.cpuTicks.load();
    while (!proc->isFinished())
    {
        while (cState.cpuTicks.load() == lastTick)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        lastTick = cState.cpuTicks.load();
        proc->executeStep();
    }
    pm.moveToFinished(proc);
}

void spawnProcess(ControlState &cState, ProcessManager &processManager)
{
    // Find the higheste id among existing screens (from allScreens)
    const auto &allProcesses = processManager.getAllProcesses();

    int last_id = 0;
    for (const auto &process : allProcesses)
    {
        if (process->getId() > last_id)
        {
            last_id = process->getId();
        }
    }

    // New process name: "p" + (last_id + 1)
    std::string name = "p" + std::to_string(last_id + 1);
    processManager.createProcess(name, cState);
}