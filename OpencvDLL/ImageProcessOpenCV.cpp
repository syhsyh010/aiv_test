#include "pch.h"
#include "ImageProcessOpenCV.h"
bool ImageProcessCustom::ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize)
{ 
    if (!src || !dst) return false;

    cv::Mat srcMat = *src;
    cv::Mat dstMat = *dst;

    // anchor, delta, borderType은 default로
    cv::blur(srcMat, dstMat, cv::Size(kernelSize, kernelSize));

    if (dstMat.empty())
        return false;

    return true;
}

extern "C" __declspec(dllexport) ImageProcess * CreateImageProcess() {
    return new ImageProcessCustom();
}

extern "C" __declspec(dllexport) void DestroyImageProcess(ImageProcess * processor) {
    delete processor;
}