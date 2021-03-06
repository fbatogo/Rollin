#include "hmac.h"

#include <cstring>
#include <iostream>
#include "../logger.h"

Hmac::Hmac()
{
    mHashType = nullptr;
    mHashResult = nullptr;
}

Hmac::Hmac(std::shared_ptr<HashTypeBase> hashType) :
    mHashType(hashType)
{
    mHashResult = nullptr;
}

Hmac::Hmac(Hmac &toCopy)
{
    copy(toCopy);
}

/**
 * @brief Hmac::setHashType - Set the hash algorithm to use to create an HMAC.  This
 *      is an alternative to using the ctor to set this.
 *
 * @param hashType - A HashTypeBase object that implements the hash algorithm to be used
 *      to create an HMAC.
 */
void Hmac::setHashType(const std::shared_ptr<HashTypeBase> &hashType)
{
    // Set the new values.
    mHashType = hashType;
}

/**
 * @brief Hmac::calculate - Generate an HMAC of the provided data, using the has method provided when
 *      this object was created.
 *
 * @param key - The key data that should be used to generate the HMAC.
 * @param data - The data that we want to get the HMAC for.
 *
 * @return unsigned char * containing the calculated HMAC value.  On error, nullptr will be
 *      returned.
 */
std::shared_ptr<ByteArray> Hmac::calculate(const ByteArray &key, const ByteArray &data)
{
    ByteArray keyIpad;
    ByteArray keyOpad;
    ByteArray iPadHashed;
    ByteArray keyToUse;

    // Validate inputs.
    if (key.empty() || data.empty()) {
        // Nothing we can do.
        LOG_ERROR("Either the key or data block provided to HMAC was NULL!");
        return nullptr;
    }

    // Make sure we are properly configured to do the hashing.
    if (nullptr == mHashType) {
        // Need to have the object set before we can calculate the HMAC.
        LOG_ERROR("No hash object was configured when attempting to create an HMAC value!");
        return nullptr;
    }

    // Make a copy of our key data.
    keyToUse = key;

    // Calculate the HMAC over the 'data' by using the formula :
    //    Hash(key XOR opad, Hash(key XOR ipad, data))

    // If the key length is larger than one block, we need to hash the key to come up
    // with a key of decreased size.
    if (keyToUse.size() > mHashType->hashBlockLength()) {
        // Create a hash of the key to use.
        keyToUse = mHashType->hash(keyToUse);
    }

    // Copy the data from our keyToUse to the Ipad and Opad.
    keyIpad = keyToUse;
    keyOpad = keyToUse;

    // Pre-allocate a full block on the next reallocation.
    keyIpad.setExtraAllocation(mHashType->hashBlockLength());
    keyOpad.setExtraAllocation(mHashType->hashBlockLength());

    // Padd the ipad and opad to the proper block size.
    for (size_t i = keyIpad.size(); i < mHashType->hashBlockLength(); i++) {
        if (!keyIpad.append(static_cast<char>(0x00))) {
            LOG_ERROR("Failed to append a zero byte to the key ipad!");
            return nullptr;
        }
    }

    for (size_t i = keyOpad.size(); i < mHashType->hashBlockLength(); i++) {
        if (!keyOpad.append(static_cast<char>(0x00))) {
            LOG_ERROR("Failed to append a zero byte to the key opad!");
            return nullptr;
        }
    }

    // Iterate the two values, XORing them with 0x36 for the ipad, and
    // 0x5c for the opad.
    for (size_t i = 0; i < mHashType->hashBlockLength(); i++) {
        if (!keyIpad.setAt(i, (keyIpad.at(i) ^ 0x36))) {
            LOG_ERROR("Unable to set the ipad value at index " + QString::number(i) + "!");
            return nullptr;
        }

        if (!keyOpad.setAt(i, (keyOpad.at(i) ^ 0x5c))) {
            LOG_ERROR("Unable to set the opad value at index " + QString::number(i) + "!");
            return nullptr;
        }
    }

    // Then, concatenate the data on to the inner hash value.
    keyIpad.append(data);

    // Then, perform the inner hash on the data provided.
    iPadHashed = mHashType->hash(keyIpad);

    // Then, concatenate the iPadHashed value to the keyOpad value.
    keyOpad.append(iPadHashed);

    // And hash that.
    mHashResult.reset(new ByteArray(mHashType->hash(keyOpad)));

    // And, return the result.
    return mHashResult;
}

Hmac &Hmac::operator=(const Hmac &toCopy)
{
    if (this == &toCopy) {
        return (*this);
    }

    copy(toCopy);

    return (*this);
}

/**
 * @brief Hmac::copy - Copy the members of the \c toCopy object in to the current
 *      object.
 *
 * @param toCopy - The object to copy from.
 */
void Hmac::copy(const Hmac &toCopy)
{
    mHashType = toCopy.mHashType;
    mHashResult = toCopy.mHashResult;
}
