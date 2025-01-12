#include "../IncludeShare/ImageProcess.h"
#include "../IncludeShare/ImageObject.h"
#include "DlgProgress.h"


typedef ImageProcess* (*CreateImageProcessFunc)();
typedef void (*DestroyImageProcessFunc)(ImageProcess*);

#ifdef _DEBUG
#define BUILD_CONFIG "Debug"
#else
#define BUILD_CONFIG "Release"
#endif

class Convert
{
private:
    static Convert* instance;

    Convert() 
    {
        m_BuildConf = BUILD_CONFIG;
    }

public:

    static Convert& getInstance() {
        static Convert instance;
        return instance;
    }
    string GetExecutablePath();
    bool Start(DlgProgress* progressDlg, bool isCustom, vector<string> imgFilePaths, string outputPath, int kernelSize);
private:
	void ProcessImage(ImageObject* src, int kernelSize, const std::string &outputPathName, DlgProgress* progressDlg);
    void ImageBundleProcess(vector<pair<string, ImageObject*>> &fileNameAndImgs, int kernelSize, string& outputPath, DlgProgress* progressDlg);
    
    CreateImageProcessFunc  CreateImageProcess;
    DestroyImageProcessFunc DestroyImageProcess;

    string                  m_OptionStr;
    string                  m_BuildConf;
    string                  m_DllPath;

    // 복사 생성자 막기
    Convert(const Convert&) = delete;
    // 대입 연산자 막기
    Convert& operator=(const Convert&) = delete;
};