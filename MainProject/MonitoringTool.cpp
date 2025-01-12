#include "pch.h"
#include "MonitoringTool.h"
#include <iostream>
#include <functional>


MonitoringTool::~MonitoringTool()
{
	if (monitoringThread.joinable())
		StopMonitoring();
}

// 모니터링 시작
void MonitoringTool::StartMonitoring(int intervalMs)
{
	start = chrono::high_resolution_clock::now();
	stopMonitoring = false;
	measurements.clear();

	monitoringThread = std::thread([this, intervalMs]()
	{
		while (!stopMonitoring) 
		{
			PROCESS_MEMORY_COUNTERS memInfo;
			// KB단위 
			if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) 
				measurements.push_back(memInfo.WorkingSetSize / 1024);
			
			this_thread::sleep_for(chrono::milliseconds(intervalMs));
		}
	});
}

// 모니터링 중지
void MonitoringTool::StopMonitoring() 
{
	stopMonitoring = true;
	end = chrono::high_resolution_clock::now();
	if (monitoringThread.joinable()) 
		monitoringThread.join();
}

// 평균 메모리 사용량 반환
double MonitoringTool::GetAverageMemoryUsage() const 
{
	if (measurements.empty()) return 0.0;
	double sum = 0.0;
	for (size_t i = 0; i < measurements.size(); ++i) {
		sum += measurements[i];
	}
	return sum / measurements.size() / 1024.0;
}

// 피크 메모리 사용량 반환
double MonitoringTool::GetPeakMemoryUsage() const 
{
	if (measurements.empty()) return 0;
	return *std::max_element(measurements.begin(), measurements.end()) / 1024.0;
}

long long MonitoringTool::MeasureExecutionTime()
{
	return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}
