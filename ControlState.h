#ifndef CONTROLSTATE_H
#define CONTROLSTATE_H

#include <string>
#include <atomic>

struct ControlState
{
    int num_cpu;
    std::string scheduler;
    int quantum_cycles;
    int batch_process_freq;
    int min_ins;
    int max_ins;
    int delay_per_exec;

    std::atomic<int> cpuTicks{0}; // Make atomic and initialize to 0
    std::atomic<bool> batchRunning{false};
    bool initialized = false;
};

#endif // CONFIG_H