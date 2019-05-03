#include "testutils.h"

#include <sstream>
#include <iomanip>

std::string TestUtils::binaryToString(const unsigned char *bytes, size_t bytesLength)
{
    std::stringstream result;

    result.clear();

    for (size_t i = 0; i < bytesLength; i++) {
        result << std::setfill('0') << std::setw(2) << std::hex << (int)bytes[i] << " ";
    }

    return result.str();
}

std::string TestUtils::binaryToString(const ByteArray &bytes)
{
    return binaryToString(bytes.toUCharArrayPtr(), bytes.size());
}

std::string TestUtils::binaryToString(const std::shared_ptr<ByteArray> &bytes)
{
    return binaryToString((*bytes.get()));
}
