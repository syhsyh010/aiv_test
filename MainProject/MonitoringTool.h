// MonitoringTool.h
#ifndef MONITORING_TOOL_H
#define MONITORING_TOOL_H

#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <windows.h>
#include <psapi.h>

using namespace std;

class MonitoringTool {
public:
    MonitoringTool() : stopMonitoring(false) {}
    void StartMonitoring(int intervalMs = 100);
    void StopMonitoring();

    double GetAverageMemoryUsage() const;
    double GetPeakMemoryUsage() const;

    long long MeasureExecutionTime();
    ~MonitoringTool();

    

private:
    vector<size_t>                      measurements;
    atomic<bool>                        stopMonitoring = false;
    thread                              monitoringThread;
    chrono::steady_clock::time_point    start;
    chrono::steady_clock::time_point    end;
};

#endif
