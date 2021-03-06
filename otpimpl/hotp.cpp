#include "hotp.h"

#include "hmac.h"
#include "sha1hash.h"
#include "../logger.h"

Hotp::Hotp()
{
    mHmacToUse = nullptr;
}

Hotp::Hotp(std::shared_ptr<Hmac> &hmacToUse) :
    mHmacToUse(hmacToUse)
{
}

/**
 * @brief Hotp::setHmac - Set the HMAC object to use when calculating the HOTP value.
 *
 * @param hmacToUse - The HMAC object that should be used to calculate the HOTP value.
 * @param takeOwnership - If true, this object will take ownership of the HMAC object, and
 *      will delete it in the dtor.  If false, the caller is responsible for freeing the
 *      HMAC object when it knows it is no longer in use.
 */
void Hotp::setHmac(std::shared_ptr<Hmac> &hmacToUse)
{
    // Set the new values to use.
    mHmacToUse = hmacToUse;
}

/**
 * @brief Hotp::calculate - Calculate and return an HOTP value.
 *
 * @param key - The DECODED key to use.  (20 byte string, *NOT* base32 encoded)
 * @param counter - The counter value to use.
 * @param digits - The number of digits to calculate.
 * @param addChecksum - true to include a checksum in the HOTP calculation.
 *      false otherwise.
 * @param truncationOffset - The offset in to the HOTP that we should truncate to
 *      come up with the HOTP value.
 *
 * @return std::string containing the HOTP value.  On error, this string will be
 *      empty.
 */
std::string Hotp::calculate(const ByteArray &key, uint64_t counter, size_t digits, bool addChecksum, int truncationOffset)
{
    unsigned char number[8];
    ByteArray baNumber;
    unsigned char inverse;
    std::shared_ptr<ByteArray> calcResult;

    // Validate inputs.
    if (key.empty()) {
        // We can't work with this!
        LOG_ERROR("A null key was provided for the HOTP calculation!");
        return "";
    }

    // Make sure we are configured properly.
    if (mHmacToUse == nullptr) {
        LOG_ERROR("No HMAC object was set when attempting to calculate an HOTP value!");
        return "";
    }

    if ((digits < 6) || (digits > 8)) {
        LOG_ERROR("The number of digits provided must be 6, 7, or 8!");
        return "";
    }

    // Convert the value of the counter to an unsigned char *, without worrying about endianness.
    inverse = 7;
    for (size_t i = 0; i < 8; i++) {        // A 64 bit number is 8 bytes.
        number[i] = ((counter >> (inverse * 8)) & 0xff);
        inverse--;
    }

    baNumber.fromCharArray(reinterpret_cast<char *>(number), sizeof(number));

    // Calculate the SHA hash of the key and counter.
    calcResult = mHmacToUse->calculate(key, baNumber);
    if (calcResult->toCharArrayPtr() == nullptr) {
        LOG_ERROR("Failed to caculate HMAC-SHA1 portion of the HOTP!");
        return "";
    }

    return calculateHotpFromHmac((*calcResult.get()), digits, addChecksum, truncationOffset);
}

/**
 * @brief Hotp::calculateHotpFromHmac - Calculate the HOTP from the HMAC that was calculated against the
 *      key and counter that were originally passed in.
 *
 * @param hmac - The HMAC that was calculated using one of the hashing algorithms.
 * @param digits - The number of digits that the HOTP should have. (6, 7, or 8)
 * @param addChecksum - If true, we will add checksum digits to the HOTP value.
 * @param truncationOffset - A value of 0..(hmacSize - 4) that indicates the truncation value we should use.  If this
 *      value is anything but 0..15, dynamic truncation will be used.
 *
 * @return std::string containing the calculated HOTP value. On error, the string will be empty.
 */
std::string Hotp::calculateHotpFromHmac(const ByteArray &hmac, size_t digits, bool addChecksum, int truncationOffset)
{
    int64_t DIGITS_POWER[9] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
    ByteArray truncValue;
    size_t calcDigits;
    int32_t truncData;
    int64_t otp;
    std::string result;

    // Validate inputs.
    if (hmac.empty()) {
        LOG_ERROR("No HMAC value was provided to calculate the HOTP!");
        return "";
    }

    if ((digits < 6) || (digits > 8)) {
        LOG_ERROR("An invalid number of digits was requested!  It must be 6, 7, or 8!");
        return "";
    }

    // Figure out how many digits to calculate.
    calcDigits = addChecksum ? (digits + 1) : digits;

    // Do the dynamic truncation on it.
    truncValue = dynamicTruncate(hmac, truncationOffset);
    if (truncValue.empty()) {
        LOG_ERROR("Unable to execute the dynamic truncation of the HMAC-SHA1 value generated!");
        return "";
    }

    // Convert the truncated value to a 32 bit number.
    truncData = (((truncValue.at(0) & 0x7f) << 24) | ((truncValue.at(1) & 0xff) << 16) | ((truncValue.at(2) & 0xff) << 8) | (truncValue.at(3) & 0xff));

    // Then, calculate the otp.
    otp = truncData % DIGITS_POWER[calcDigits];

    // If we need a checksum, calculate that.
    if (addChecksum) {
        otp = (otp * 10) + calcChecksum(otp, calcDigits);
    }

    // Convert the otp number to a string.
    result = std::to_string(otp);

    // Prepend 0s if needed.
    while (result.length() < digits) {
        result = "0" + result;
    }

    return result;
}

/**
 * @brief Hotp::calcChecksum - Calculate the checksum for the OTP.
 *
 * @param otp - The OTP without the checksum.
 * @param digits - The number of digits the OTP should be when done.
 *
 * @return int containing the checksum.
 */
int64_t Hotp::calcChecksum(int64_t otp, size_t digits)
{
    int doubleDigits[10] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };
    bool doubleDigit = true;
    int total = 0;
    int digit;
    int result;

    while (0 < digits--) {
        digit = (otp % 10);

        otp /= 10;

        if (doubleDigit) {
            digit = doubleDigits[digit];
        }

        total += digit;
        doubleDigit = !doubleDigit;
    }

    result = total % 10;
    if (result > 0) {
        result = 10 - result;
    }

    return result;
}

/**
 * @brief Hotp::dynamicTruncate - Given an HMAC calculation, execute the dynamic truncation
 *      algorithm, and return that result.
 *
 * @param hmac - The HMAC generated to be used for the HOTP value.
 * @param truncateOffset - A value of 0..(hmacSize - 4) to be used for the truncationOffset.  If it is
 *      any other number, dynamic truncation will be used.
 *
 * @return ByteArray containing the dynamically truncated value.
 */
ByteArray Hotp::dynamicTruncate(const ByteArray &hmac, size_t truncateOffset)
{
    size_t offset;
    ByteArray result;

    if (hmac.empty()) {
        LOG_ERROR("No HMAC value provided while attempting dynamic truncation!");
        return ByteArray();
    }

    if (truncateOffset < (hmac.size() - 4)) {
        // Use the provided truncation value.
        offset = truncateOffset;
    } else {
        // Get the offset bits from the last byte of the HMAC.
        offset = (hmac.at(hmac.size() - 1) & 0x0f);
    }

    // Allocate 3 extra bytes on our first allocation (resulting in a total allocation of 4 bytes)
    result.setExtraAllocation(3);

    // Copy the 4 bytes at the offset.
    for (size_t i = 0; i < 4; i++) {
        result.append(hmac.at(offset + i));
    }

    return result;
}
