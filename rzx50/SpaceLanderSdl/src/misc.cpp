//
// This source file is a part of Space Lander game.
// 
// Copyright (C) 2011-2012 by Dmitry Simakov <dosimakov@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "misc.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>

void util_current_time_to_string(std::string& str)
{
    // Use standard C library to get the local time
    time_t t = time(0);
    tm* now = localtime(&t);

    std::ostringstream oss;
    oss << std::setw(4) << std::setfill('0') << (now->tm_year + 1900) << "_"
        << std::setw(2) << std::setfill('0') << (now->tm_mon + 1) << "_"
        << std::setw(2) << std::setfill('0') << now->tm_mday << "_"
        << std::setw(2) << std::setfill('0') << now->tm_hour 
        << std::setw(2) << std::setfill('0') << now->tm_min 
        << std::setw(2) << std::setfill('0') << now->tm_sec;
    
    str = oss.str();
}

bool util_check_filename(const std::string& filename)
{   
    std::ifstream file(filename.c_str());
    return file.good();
}
