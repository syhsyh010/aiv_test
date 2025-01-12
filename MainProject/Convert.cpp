#include "pch.h"
#include "Convert.h"
#include "Resource.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <chrono>
#include "../IncludeShare/Log.h"
#include "MonitoringTool.h"
#include "MainProject.h"

using namespace std;

// 4GB
#define BLOCKBYTE 4294967296

string Convert::GetExecutablePath()
{
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	std::string exePath = buffer;

	// 경로에서 실행 파일 이름 제거
	size_t lastSlash = exePath.find_last_of("\\/");
	if (lastSlash != std::string::npos) {
		exePath = exePath.substr(0, lastSlash);
	}

	return exePath;
}

bool Convert::Start(DlgProgress* progressDlg, bool isCustom, vector<string> imgFilePaths, string outputPath, int kernelSize)
{
	if (isCustom)
		m_OptionStr = "Custom";
	else
		m_OptionStr = "OpenCV";

	m_DllPath = GetExecutablePath() + "/" + m_OptionStr + "DLL.dll";

	//m_DllPath = "../x64/" + m_BuildConf + "/" + m_OptionStr + "DLL.dll";
	HMODULE hDLL = LoadLibraryA(m_DllPath.c_str());
	vector<pair<string, ImageObject*>> fileNameAndImgs;

	try
	{
		if (hDLL != NULL)
		{
			CreateImageProcess = (CreateImageProcessFunc)GetProcAddress(hDLL, "CreateImageProcess");
			DestroyImageProcess = (DestroyImageProcessFunc)GetProcAddress(hDLL, "DestroyImageProcess");
		}
		else
		{
			throw runtime_error("DLL 로드 실패.");
		}

		int i;
		long long bytesSize = 0;
		string outputPathName;
		CString progressText = L"Processing " + CString(m_OptionStr.c_str()) + L"...";

		progressDlg->Init(progressText, 0, (int)imgFilePaths.size());
		bytesSize = 0;
		i = 0;

		MonitoringTool monitoringTool;
		monitoringTool.StartMonitoring(100);

		while (i < imgFilePaths.size())
		{
			ImageObject* img = new ImageObject(imgFilePaths[i].c_str());

			if (img->GetLoadError())
				throw runtime_error("이미지 로드 실패.");

			bytesSize += img->GetSizeInBytes();
			if (bytesSize > BLOCKBYTE)
			{
				ImageBundleProcess(fileNameAndImgs, kernelSize, outputPath, progressDlg);
				bytesSize = img->GetSizeInBytes();
			}
			fileNameAndImgs.push_back({ imgFilePaths[i], img });
			i++;
		}
		if (fileNameAndImgs.empty() == false)
		{
			ImageBundleProcess(fileNameAndImgs, kernelSize, outputPath, progressDlg);
		}

		monitoringTool.StopMonitoring();

		Log::GetInstance().WriteStart();
		Log::GetInstance() << m_OptionStr << endl;
		Log::GetInstance() << "실행 시간 : " << monitoringTool.MeasureExecutionTime() << " ms" << endl;
		Log::GetInstance() << "평균 메모리 사용량 : " << monitoringTool.GetAverageMemoryUsage() << " MB" << endl;
		Log::GetInstance() << "최대 메모리 사용량 : " << monitoringTool.GetPeakMemoryUsage() << " MB" << endl << endl;
		Log::GetInstance().WriteClose();

		progressDlg->PostMessage(WM_THREAD_DONE, IDOK, 0);
		FreeLibrary(hDLL);
		return 0;
	}
	catch (const std::runtime_error& e)
	{
		Log::GetInstance().WriteStart();
		Log::GetInstance() << e.what() << endl;
		Log::GetInstance().WriteClose();
		fileNameAndImgs.clear();
		progressDlg->PostMessage(WM_THREAD_DONE, IDOK, 0);
		if(hDLL != NULL) 
			FreeLibrary(hDLL);
		return 1;
	}
}


void Convert::ProcessImage(ImageObject* src, int kernelSize, const string& outputPathName, DlgProgress* progressDlg)
{
	ImageObject* dst = new ImageObject(src->GetWidth(), src->GetHeight());
	function<void(ImageProcess*)> deleter = [this](ImageProcess* ptr) {
		if (ptr) {
			DestroyImageProcess(ptr);
		}
	};
	unique_ptr<ImageProcess, decltype(deleter)> blurOpenCV(CreateImageProcess(), deleter);


	if (blurOpenCV->ImageBlur(src, dst, kernelSize) == false || dst == NULL)
	{
		AfxMessageBox(L"이미지 변환을 실패하였습니다.", MB_ICONERROR);
		return;
	}

	cv::imwrite(outputPathName, static_cast<cv::Mat>(*dst));
	//DestroyImageProcess(blurOpenCV);
	progressDlg->PostMessage(WM_UPDATE_PROGRESS, 1, 0);
	delete dst;
	delete src;
}

void Convert::ImageBundleProcess(vector<pair<string, ImageObject*>>& fileNameAndImgs, int kernelSize, string& outputPath, DlgProgress* progressDlg) {

	string outputPathName;
	vector<thread> threads;
	for (int j = 0; j < fileNameAndImgs.size(); j++)
	{
		std::filesystem::path curPath(fileNameAndImgs[j].first);
		outputPathName = outputPath + "\\" + m_OptionStr + "_" + curPath.filename().string();
		threads.emplace_back(thread(&Convert::ProcessImage,
			this, fileNameAndImgs[j].second, kernelSize, outputPathName, progressDlg));
	}
	for (auto& t : threads)
	{
		t.join();
	}
	fileNameAndImgs.clear();
}