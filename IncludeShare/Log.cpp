#include "pch.h"
#include "Log.h"
#include <iostream>
#include <fstream>

void Log::WriteStart()
{
    m_LogFile.open("image_blur.log", std::ios::app);
}

void Log::WriteClose()
{
    if (m_LogFile.is_open())
        m_LogFile.close();
}

// 생성자 정의
Log::Log() 
{
}

// 소멸자 정의
Log::~Log() 
{
    if (m_LogFile.is_open())
        m_LogFile.close();
}