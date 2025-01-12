#include "pch.h"
#include "ImageObject.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include "Log.h"


ImageObject::ImageObject(int width, int height)
    : width(width), height(height), channels(1), data(width* height* channels, 0), loadError(false)
{
}

ImageObject::ImageObject(const char* fileName) :channels(1), loadError(false)
{    
    cv::Mat image = cv::imread(fileName, cv::IMREAD_UNCHANGED);
  
    try
    {
        if (image.empty())
            throw runtime_error("이미지가 비어 있음");
        // 이미지가 흑백이어야 함
        if (image.channels() != 1)
            throw runtime_error("이미지가 흑백이 아님");
        

        // 이미지 크기
        width = image.cols;
        height = image.rows;

        if (image.isContinuous())        
            data.assign(image.data, image.data + image.total() * channels);
        // 연속적이지 않은 이미지
        else
            throw runtime_error("이미지가 연속적이지 않음");        
    }
    catch (const std::runtime_error& e)
    {
        Log::GetInstance().WriteStart();
        Log::GetInstance() << e.what() << endl;
        Log::GetInstance().WriteClose();
        loadError = true;
    }
}
ImageObject::ImageObject(const cv::Mat& mat) : width(mat.cols), height(mat.rows), channels(1), data(mat.total()* mat.elemSize()), loadError(false)
{
    if (mat.empty() || mat.channels() != 1 || mat.isContinuous() == false)
    {
        loadError = true;
        return;
    }

    // 데이터 복사
    std::memcpy(data.data(), mat.data, data.size());
}

ImageObject::~ImageObject()
{
    
}

const bool ImageObject::GetLoadError()
{
    return loadError;
}
const size_t ImageObject::GetSizeInBytes() const
{
    return data.size();
}

const int ImageObject::GetWidth() const
{
    return width; 
}
const int ImageObject::GetHeight() const
{
    return height; 
}
const int ImageObject::GetChannels() const
{
    return channels; 
}


vector<uint8_t>& ImageObject::GetData() 
{
    return data;
}

const vector<uint8_t>& ImageObject::GetData() const 
{
    return data;
}