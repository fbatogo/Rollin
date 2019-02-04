#include "base32coder.h"

#include <cstring>
#include <stdint.h>
#include <logger.h>

const char base32chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";


Base32Coder::Base32Coder()
{

}

/**
 * @brief Base32Coder::encode - Given a binary array of bytes, encode them using base32
 *      encoding.
 *
 * @param toEncode - The byte array to encode.
 * @param toEncodeSize - The length of the byte array.
 *
 * @return std::string containing the base32 encoded string.
 */
std::string Base32Coder::encode(unsigned char *toEncode, size_t toEncodeSize)
{
    std::string result;
    size_t blocks;
    size_t offset;
    unsigned char b;

    // If we have nothing to encode, return an empty string.
    if (toEncodeSize == 0) {
        return "";
    }

    result.clear();

    blocks = (toEncodeSize / 5);

    if (blocks == 0) {
        blocks = 1;
    } else {
        if ((toEncodeSize % 5) != 0) {
            blocks++;
        }
    }

    for (size_t i = 0; i < blocks; i++) {
        // Calculate the 32bit offset in to the byte string.
        offset = (i * 5);

        // Pull out the values that we need to look up.

        // Take the first 5 bits.
        result += base32chars[(toEncode[offset] >> 3)];

        // Take the last 3 bits in the first byte, and the first 2 of the 2nd byte.
        b = ((toEncode[offset] & 0x07)  << 2);
        if ((offset + 1) < toEncodeSize) {
            b |= (toEncode[offset+1] >> 6);
        }
        result += base32chars[b];

        if ((offset + 1) >= toEncodeSize) {
            result += "======";
        } else {
            // Then, the 5 bits in the middle of the 2nd byte.
            result += base32chars[(((toEncode[offset+1] & 0x3f) >> 1))];

            // Then, the last bit of the 2nd byte, and the high nibble of the 3rd.
            b = ((toEncode[offset+1] & 0x01) << 4);
            if ((offset + 2) < toEncodeSize) {
                b |= (toEncode[offset+2] >> 4);
            }
            result += base32chars[b];

            if ((offset + 2) >= toEncodeSize) {
                result += "====";
            } else {
                // Then, the low nibble of the 3rd byte, and 1 bit from the 4th.
                b = ((toEncode[offset+2] & 0x0f) << 1);
                if ((offset + 3) < toEncodeSize) {
                    b |= (toEncode[offset+3] >> 7);
                }
                result += base32chars[b];

                if ((offset + 3) >= toEncodeSize) {
                    result += "===";
                } else {
                    // Then the 5 middle-ish bits from the 4th byte.
                    result += base32chars[(((toEncode[offset+3] & 0x7f) >> 2))];

                    // Then the low 2 bits of the 4th byte and the high 3 of the 5th.
                    b = ((toEncode[offset+3] & 0x03) << 3);
                    if ((offset + 4) < toEncodeSize) {
                        b |= (toEncode[offset+4] >> 5);
                    }
                    result += base32chars[b];

                    if ((offset + 4) >= toEncodeSize) {
                        result += "=";
                    } else {
                        // Then, the last 5 bits of the 5th byte.
                        result += base32chars[(toEncode[offset+4] & 0x1f)];
                    }
                }
            }
        }
    }

    return result;
}

/**
 * @brief Base32Coder::decode - Decode a base 32 encoded string.
 *
 * @param toDecode - The string that we want to decode.
 * @param decodedSize - The size of the data pointed to by the returned pointer.
 *
 * @return unsigned char pointer to the decoded data.
 */
unsigned char *Base32Coder::decode(std::string toDecode, size_t &decodedSize)
{
    unsigned char *result;
    size_t blocks;
    unsigned char *bytesToDecode;

    decodedSize = 0;

    if (toDecode.empty()) {
        // Return an empty string.
        return reinterpret_cast<unsigned char *>(strdup(""));
    }

    // It should be evenly divisible by 8, if not, it isn't a valid string.
    if ((toDecode.length() % 8) != 0) {
        // Return a null pointer.
        return nullptr;
    }

    // Allocate plenty of space. (More than we should need.)
    result = reinterpret_cast<unsigned char *>(calloc(1, toDecode.length()));
    if (result == nullptr) {
        LOG_ERROR("Failed to allocate memory for base32 decoded value!");
        return nullptr;
    }

    // Figure out how many blocks we have.
    blocks = toDecode.length() / 8;

    // Get the text in to a bytes to decode buffer.
    bytesToDecode = reinterpret_cast<unsigned char *>(strdup(toDecode.c_str()));
    decodedSize = 0;

    // Iterate each block, converting it.
    for (size_t i = 0; i < blocks; i++) {
        if (decode8Chars(bytesToDecode, (i * 8), result, decodedSize) == false) {
            LOG_ERROR("Failed to decode 8 bytes!");

            // Free the memory we used.
            free(bytesToDecode);
            bytesToDecode = nullptr;

            return nullptr;
        }
    }

    // Free the memory we used.
    free(bytesToDecode);
    bytesToDecode = nullptr;

    return result;
}

bool Base32Coder::decode8Chars(unsigned char *data, size_t dataOffset, unsigned char *target, size_t &decodedSize)
{
    if ((data == nullptr) || (target == nullptr)) {
        LOG_ERROR("Invalid inputs to decode8Chars!");
        return false;
    }

    // Decode the first byte.
    target[decodedSize++] = static_cast<unsigned char>((decodeChar(data[dataOffset]) << 3) | ((decodeChar(data[dataOffset + 1]) >> 2)));

    if (data[dataOffset + 2] != '=') {
        // Then the next.
        target[decodedSize++] = static_cast<unsigned char>(((decodeChar(data[dataOffset + 1]) << 6) | ((decodeChar(data[dataOffset + 2]) << 1) | ((decodeChar(data[dataOffset + 3]) >> 4)))));
    }

    if (data[dataOffset + 4] != '=') {
        target[decodedSize++] = static_cast<unsigned char>(((decodeChar(data[dataOffset + 3]) << 4) | ((decodeChar(data[dataOffset + 4]) >> 1))));
    }

    if (data[dataOffset + 5] != '=') {
        target[decodedSize++] = static_cast<unsigned char>(((decodeChar(data[dataOffset + 4]) << 7) | ((decodeChar(data[dataOffset + 5]) << 2) | ((decodeChar(data[dataOffset + 6]) >> 3)))));
    }

    if (data[dataOffset + 7] != '=') {
        target[decodedSize++] = static_cast<unsigned char>(((decodeChar(data[dataOffset + 6]) << 5) | ((decodeChar(data[dataOffset + 7])))));
    }

    return true;
}

/**
 * @brief Base32Coder::decodeChar - Convert a character to a 5 bit value.
 *
 * @param toDecode - The character to decode.
 *
 * @return unsigned char containing the 5 bit value.
 */
unsigned char Base32Coder::decodeChar(unsigned char toDecode)
{
    for (unsigned char i = 0; i < strlen(base32chars); i++) {
        if (toDecode == base32chars[i]) {
            return i;
        }
    }

    // Didn't find it.
    return 0xff;
}