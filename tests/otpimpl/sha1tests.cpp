#include <testsuitebase.h>

#include <cstring>
#include "otpimpl/sha1hash.h"
#include "logger.h"

#include <QDebug>
#include <iomanip>
#include <sstream>
#include "testutils.h"

EMPTY_TEST_SUITE(Sha1Tests);

// We only run two test vectors here, because the base SHA1 code is already NIST certified.
TEST_F(Sha1Tests, Sha1Tests1)
{
    unsigned char emptyStringResult[20] = {0xda, 0x39, 0xa3, 0xee, 0x5e, 0x6b, 0x4b, 0x0d, 0x32, 0x55, 0xbf, 0xef, 0x95, 0x60, 0x18, 0x90, 0xaf, 0xd8, 0x07, 0x09};
    ByteArray result;
    Sha1Hash hashObj;
    ByteArray toHash;

    // Calculate the empty string test.
    EXPECT_TRUE(toHash.fromStdString(""));
    result = hashObj.hash(toHash);
    EXPECT_TRUE(memcmp((void *)result.toUCharArrayPtr(), (void *)&emptyStringResult, 20) == 0);
}

TEST_F(Sha1Tests, Sha1Tests2)
{
    unsigned char string896bits[20] = {0xa4, 0x9b, 0x24, 0x46, 0xa0, 0x2c, 0x64, 0x5b, 0xf4, 0x19, 0xf9, 0x95, 0xb6, 0x70, 0x91, 0x25, 0x3a, 0x04, 0xa2, 0x59};
    ByteArray result;
    Sha1Hash hashObj;
    ByteArray toHash;

    // Calculate 896 bit string test.
    EXPECT_TRUE(toHash.fromStdString("abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"));
    result = hashObj.hash(toHash);
    qDebug("Calculated hash : %s", TestUtils::binaryToString(result.toUCharArrayPtr(), 20).c_str());
    qDebug("Expected hash   : %s", TestUtils::binaryToString(string896bits, 20).c_str());
    EXPECT_TRUE(memcmp((void *)result.toUCharArrayPtr(), (void *)&string896bits, 20) == 0);
}
