#include "XFtpUSER.h"
#include <iostream>
using namespace std;

void XFtpUSER::Parse(std::string type,std::string msg)
{
    ResCMD("230 Login successful.\r\n");
}

XFtpUSER::XFtpUSER()
{
}

XFtpUSER::~XFtpUSER()
{
}