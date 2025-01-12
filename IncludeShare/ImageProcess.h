#pragma once
#include "ImageObject.h"

class ImageObject;

class ImageProcess {
public:

    virtual ~ImageProcess() = default;
    virtual bool ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize) = 0; 
};

// 팩토리 함수
extern "C" __declspec(dllexport) ImageProcess * CreateImageProcess();
extern "C" __declspec(dllexport) void DestroyImageProcess(ImageProcess * instance);