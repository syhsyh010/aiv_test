#include "pch.h"
#include "ImageProcessCustom.h"


bool ImageProcessCustom::ImageBlur(const ImageObject* src, ImageObject* dst, const int kernelSize)
{
    if (!src || !dst) return false;

    // BORDER_DEFAULT 방식 = BORDER_REFLECT_101 방식
    // 커널범위가 이미지범위를 벗어나면 그 벗어난부분은 이미지 반사된 것을 쓰는 것
    int width = src->GetWidth();
    int height = src->GetHeight();

    // 사실 1채널 고정이지만, 여러 채널이 들어와도 계산 가능하게 구현
    int channels = src->GetChannels();

    vector<uint8_t> inputData = src->GetData();
    vector<uint8_t>& outputData = dst->GetData();



    int halfKernel = kernelSize / 2;
    // 1차원배열로 처리하는 것이 2차원 배열로 처리하는 것 보다 빠르게 때문에 전부 1차원 배열로 처리
    int paddedWidth = width + 2 * halfKernel;
    int paddedHeight = height + 2 * halfKernel;
    // 바깥쪽 반사된 배열은(-halfKernel ~ width + halfKernel), (-halfKeranl ~ height + halfKernel) 크기로 확장해야함
    vector<uint8_t> paddedData(paddedWidth * paddedHeight * channels, 0);
    // 누적합 배열은 0,0이아니라 1,1부터 시작해야 편함
    vector<double> sumPaddedData((paddedWidth + 1) * (paddedHeight + 1) * channels, 0);
    int nx, ny;

    // x, y : 경계 확장한 픽셀 위치
    // nx, ny : 원본 픽셀 위치
	for (int y = 0; y < paddedHeight; y++) 
    {
        ny = y - halfKernel;
        if (ny < 0) ny = -ny;
        else if (ny >= height) ny = 2 * height - ny - 2;

		for (int x = 0; x < paddedWidth; x++) 
        {
			for (int c = 0; c < channels; c++) 
            {
				nx = x - halfKernel;

				if (nx < 0) nx = -nx;
				else if (nx >= width) nx = 2 * width - nx - 2;

				paddedData[(y * paddedWidth + x) * channels + c] = inputData[(ny * width + nx) * channels + c];
			}
		}
	}

    double divCnt = (kernelSize * kernelSize);

    // 누적 합 데이터
    // 2차원배열로 생각하면 sumPaddedData[y][x]는 paddedData[0][0] ~ paddedData[y][x]를 전부 더한 값이다.
    // 즉, sumPaddedData[y][x] = sumPaddedData[y - 1][x] + sumPaddedData[y][x - 1] - sumPaddedData[y - 1][x - 1] + paddedData[y][x]
    double t1, t2, t3, t4;
    for (int y = 1; y <= paddedHeight; y++) 
    {
        for (int x = 1; x <= paddedWidth; x++) 
        {
            for (int c = 0; c < channels; c++) 
            {
                t1 = sumPaddedData[((y - 1) * (paddedWidth + 1) + x) * channels + c];
                t2 = sumPaddedData[(y * (paddedWidth + 1) + (x - 1)) * channels + c];
                t3 = sumPaddedData[((y - 1) * (paddedWidth + 1) + (x - 1)) * channels + c];

                sumPaddedData[(y * (paddedWidth + 1) + x) * channels + c]
                    = t1 + t2 - t3 + paddedData[((y - 1) * paddedWidth + (x - 1)) * channels + c];
            }
        }
    }


    // x, y : output 픽셀 위치
    // right, bottom : 경계 확장한 픽셀 위치의 오른쪽 아래 
    // left, top : 경계 확장한 픽셀 위치의 왼쪽 위
    // left ~ right , top ~ bottom : 커널 범위

    double kernelSum;
    int top, bottom, left, right;
    //double normalizedValue;

    int rounded;
    uchar res;

    //anchor = ksize / 2; 로 되어있음

    for (int y = 0; y < height; y++)
    {
        top = y;
        bottom = y + kernelSize - 1;

        for (int x = 0; x < width; x++)
        {
            left = x;
            right = x + kernelSize - 1;
            for (int c = 0; c < channels; c++)
            {
                // 전체
                t1 = sumPaddedData[((bottom + 1) * (paddedWidth + 1) + (right + 1)) * channels + c]; 

                // 오른쪽 위
                t2 = sumPaddedData[(top * (paddedWidth + 1) + (right + 1)) * channels + c];

                // 왼쪽 아래
                t3 = sumPaddedData[((bottom + 1) * (paddedWidth + 1) + left) * channels + c];

                // 왼쪽 위 (두번 겹치는 부분)
                t4 = sumPaddedData[(top * (paddedWidth + 1) + left) * channels + c];

                kernelSum = t1 - t2 - t3 + t4;

                
                rounded = static_cast<int>(round(kernelSum / divCnt));
                res = clamp(rounded, 0, 255);
                outputData[(y * width + x) * channels + c] = res;                
                
                // 이렇게 cv::saturate_cast<uchar> 를 사용하면 시간도 더 빠르고 이미지 결과물도 일치하게 된다.
                //normalizedValue = kernelSum / divCnt;
                //outputData[(y * width + x) * channels + c] = cv::saturate_cast<uchar>(normalizedValue);                
            }
        }
    }

    return true;


}

extern "C" __declspec(dllexport) ImageProcess * CreateImageProcess() {
    return new ImageProcessCustom();
}

extern "C" __declspec(dllexport) void DestroyImageProcess(ImageProcess * processor) {
    delete processor;
}