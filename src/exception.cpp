/*
 * exception.cpp
 *
 *  Created on: 24.02.2015
 *      Author: stas
 */

#include "exception.h"
#include <sstream>

namespace dboost
{

namespace
{

std::string construct_message(const std::string& msg1, const std::string& msg2, const char* file, int line)
{
    std::stringstream ss;
    ss << msg1 << msg2 << " in file: " << file << " ,line: " << line;
    return ss.str();
}

}

exception::exception(const std::string& msg1, const std::string& msg2, const char* file, int line)
    : std::runtime_error(construct_message(msg1, msg2, file, line))
{
}

}



