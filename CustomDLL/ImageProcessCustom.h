#pragma once

#ifdef IMAGE_PROCESS_CUSTOM_EXPORTS
#define IMAGE_PROCESS_CUSTOM __declspec(dllexport)
#else
#define IMAGE_PROCESS_CUSTOM __declspec(dllimport)
#endif

#include "../IncludeShare/ImageProcess.h"
#include "../IncludeShare/ImageObject.h"
class IMAGE_PROCESS_CUSTOM ImageProcessCustom : public ImageProcess
{
public:
    ImageProcessCustom()
    {

    }
    bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize);
};


#ifdef IMAGE_PROCESS_CUSTOM_EXPORTS
extern "C" __declspec(dllexport) ImageProcess * CreateImageProcess();
extern "C" __declspec(dllexport) void DestroyImageProcess(ImageProcess * processor);
#else
extern "C" __declspec(dllimport) ImageProcess * CreateImageProcess();
extern "C" __declspec(dllimport) void DestroyImageProcess(ImageProcess * processor);
#endif