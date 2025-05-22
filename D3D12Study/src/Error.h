#pragma once
#ifndef DXUT_ERROR_H
#define DXUT_ERROR_H

#include <windows.h>
#include <string>
using std::string;

class Error {

private:
    HRESULT hrCode;
    string funcName;
    string fileName;
    int lineNum;

public:
    Error();
    Error(HRESULT hr, const string& func, const string& file, int line);
    string ToString() const;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                               \
{                                                                      \
    HRESULT hr = (x);                                                  \
    if(FAILED(hr)) { throw Error(hr, __func__, __FILE__, __LINE__); }  \
}
#endif


#endif
