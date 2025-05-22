#include "Error.h"            
#include <comdef.h>             
#include <sstream>              
using std::stringstream;        



Error::Error()
    : hrCode(S_OK), lineNum(-1)
{
    
}



Error::Error(HRESULT hr, const string& func, const string& file, int line)
    : hrCode(hr), funcName(func), lineNum(line)
{
    
    size_t pos = file.find_last_of('\\');

    
    if (pos != string::npos)
    {
        
        fileName = file.substr(pos + 1);
    }
}



string Error::ToString() const
{
    
    _com_error err(hrCode);

    stringstream text;
    text << funcName
        << " falhou em " << fileName
        << ", linha " << std::to_string(lineNum)
        << ":\n" << err.ErrorMessage();

    return text.str();
}


