
#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <mutex>

using namespace std;

class Log {
public:
    static Log& GetInstance() {
        static Log instance;
        return instance;
    }

    template <typename T>
    Log& operator<<(const T& value) 
    {
        lock_guard<mutex> lock(m_LogMutex);
        m_Buffer << value;
        return *this;
    }

    Log& operator<<(ostream& (*manip)(ostream&)) {
        lock_guard<mutex> lock(m_LogMutex);
        manip(m_Buffer);
        // endl이면
        if (manip == static_cast<ostream & (*)(ostream&)>(endl)) 
        {
            // 버퍼 내용을 로그 파일에 기록
            m_LogFile << m_Buffer.str();
            m_LogFile.flush();
            m_Buffer.str("");
            m_Buffer.clear();
        }
        return *this;
    }

    void WriteStart();
    void WriteClose();

private:
    ofstream            m_LogFile;
    ostringstream       m_Buffer;
    mutex               m_LogMutex;

    Log();
    ~Log();

    // 복사 생성자 막기
    Log(const Log&) = delete;
    //대입 연산자 막기
    Log& operator=(const Log&) = delete;
};

#endif // LOG_H