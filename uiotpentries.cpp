#include "uiotpentries.h"

#include "logger.h"

UiOtpEntries::UiOtpEntries(QObject *parent) :
    QObject(parent)
{

}

UiOtpEntries::~UiOtpEntries()
{
    // Clean up the memory used by our list.
    for (int i = 0; i < mEntryList.count(); i++) {
        if (mEntryList.at(i) != nullptr) {
            delete mEntryList.at(i);
        }
    }
}

/**
 * @brief UiOtpEntries::populateEntries - Take a list of KeyEntry objects, use them
 *      to calculate the OTP, and return an object that contains a list of OtpEntry
 *      objects.
 *
 * @param toCalculate - A list of KeyEntry objects that we want to calculate the OTP
 *      for.
 *
 * @return true if the KeyEntry objects were processed.  false on error.
 */
bool UiOtpEntries::populateEntries(QList<KeyEntry> &toCalculate)
{
    OtpEntry *temp;

    // Make sure the target list starts out clear.
    mEntryList.clear();

    for (int i = 0; i < toCalculate.size(); i++) {
        temp = calculateEntry(toCalculate.at(i));
        if (temp == nullptr) {
            LOG_ERROR("Unable to calculate the OTP data for identifier : " + toCalculate.at(i).identifier());
            return false;
        }

        // Save it to our internal list.
        mEntryList.push_back(temp);
    }

    return true;
}

/**
 * @brief UiOtpEntries::count - Return the number of entries in the UiOtpEntries list.
 *
 * @return int containing the number of entries in the UiOtpEntries list.
 */
int UiOtpEntries::count()
{
    return mEntryList.count();
}

/**
 * @brief UiOtpEntries::at - Return an OtpEntry object in the list at the specified
 *      index.
 *
 * @param i - The index of the OtpEntry object to get.
 *
 * @return OtpEntry pointer to requested object.  On error, nullptr will be returned.
 */
OtpEntry *UiOtpEntries::at(int i)
{
    if (i >= mEntryList.count()) {
        return nullptr;
    }

    return mEntryList.at(i);
}

/**
 * @brief UiOtpEntries::calculateEntry - Calculate the OTP and other useful parameters
 *      from the provided KeyEntry.
 *
 * @param calc - The KeyEntry to calculate values for.
 *
 * @return OtpEntry pointer with the calculated entry.  On failure, nullptr is returned.
 */
OtpEntry *UiOtpEntries::calculateEntry(const KeyEntry &calc)
{

}
