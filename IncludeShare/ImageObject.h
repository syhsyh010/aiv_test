#ifndef IMAGEOBJECT_H
#define IMAGEOBJECT_H

#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
class ImageObject {
private:
	int width;
	int height;
	const int channels;
	vector<uint8_t> data;
	bool loadError;

public:
	ImageObject(int width, int height);
	ImageObject(const char* fileName);
	ImageObject(const cv::Mat& mat);
	~ImageObject();
	const bool GetLoadError();

	vector<uint8_t>& GetData();
	const vector<uint8_t>& GetData() const;

	const size_t GetSizeInBytes() const;

	operator cv::Mat() const {
		return cv::Mat(height, width, CV_8UC1, const_cast<uint8_t*>(data.data()));
	}

	const int GetWidth() const;
	const int GetHeight() const;
	const int GetChannels() const;

};

#endif // IMAGEOBJECT_H