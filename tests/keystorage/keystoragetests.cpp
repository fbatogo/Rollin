#include "keystoragetests.h"

#include <QtTest>

#include "settingshandler.h"
#include "tests/testutils.h"
#include "keystorage/keystorage.h"

void KeyStorageTests::e2eTests()
{
    KeyStorage storageTest;
    QString dbPath;
    KeyEntry kEntry;
    KeyEntry newEntry;
    QList<KeyEntry> allKeys;

    // Try to delete an entry when the database isn't open.
    QVERIFY(!storageTest.deleteKeyByIdentifier("Test Key"));

    // If we have an old database file hanging around, delete it.
    dbPath = SettingsHandler::getInstance()->dataPath() + "keydatabase.db";
    if (dbPath.isEmpty()) {
        QFAIL("The database path was empty!");
    }

    // See if the database exists.
    if (TestUtils::fileExists(dbPath.toStdString())) {
        // Delete it.
        QVERIFY(TestUtils::deleteFile(dbPath.toStdString()));
    }

    // Init the key storage.
    QVERIFY(storageTest.initStorage());

    // Build a key entry to write to the database.
    kEntry.clear();
    kEntry.setIdentifier("Test Key");
    kEntry.setIssuer("Test Issuer");
    kEntry.setSecret("secret");
    kEntry.setKeyType(1);
    kEntry.setOtpType(1);
    kEntry.setTimeStep(30);
    kEntry.setAlgorithm(1);
    kEntry.setCodeValid(true);
    kEntry.setStartTime(123);
    kEntry.setTimeOffset(456);
    kEntry.setCurrentCode("1234567");
    kEntry.setHotpCounter(9);
    kEntry.setOutNumberCount(8);
    kEntry.setPrintableCurrentCode("123 4567");

    // Write it to the database.
    QVERIFY(storageTest.addKey(kEntry));

    // Clear the key entry so we can load it back.
    kEntry.clear();

    // Find the key entry by identifier.
    QVERIFY(storageTest.keyByIdentifier("Test Key", kEntry));

    // Make sure the secret is what we expect, which proves the data was written to the
    // database, and read back.
    QCOMPARE(std::string("secret"), kEntry.secret().toString());

    // Attempt to get all the keys in the database.
    QVERIFY(storageTest.getAllKeys(allKeys));

    // Make sure there is only a single entry.
    QCOMPARE((size_t)1, allKeys.size());

    // Try to re-add the key (should fail).
    QVERIFY(!storageTest.addKey(kEntry));

    // Change the secret, and update the database record.
    newEntry = kEntry;
    newEntry.setSecret("updatedsecret");
    QVERIFY(storageTest.updateKey(kEntry, newEntry));

    // Clear both key entries, and read back the updated value.
    newEntry.clear();
    kEntry.clear();

    QVERIFY(storageTest.keyByIdentifier("Test Key", kEntry));

    // Make sure the secret is the updated value.
    QCOMPARE(std::string("updatedsecret"), kEntry.secret().toString());

    // Verify that all of the other values in the key entry are what we expect.
    QCOMPARE(std::string("Test Key"), kEntry.identifier());
    QCOMPARE(std::string("Test Issuer"), kEntry.issuer());
    QCOMPARE((size_t)1, kEntry.keyType());
    QCOMPARE((size_t)1, kEntry.otpType());
    QCOMPARE((size_t)30, kEntry.timeStep());
    QCOMPARE((size_t)1, kEntry.algorithm());
    QVERIFY(!kEntry.codeValid());
    QCOMPARE((size_t)0, kEntry.startTime());
    QCOMPARE((size_t)456, kEntry.timeOffset());
    QVERIFY(kEntry.currentCode().isEmpty());        // Should be empty.  Not calculated yet.
    QCOMPARE((size_t)9, kEntry.hotpCounter());
    QVERIFY(kEntry.invalidReason().isEmpty());      // Should be empty.  Data is all valid.
    QCOMPARE((size_t)8, kEntry.outNumberCount());
    QVERIFY(kEntry.printableCurrentCode().isEmpty());   // Should be empty.  Not calculated.

    // Then, attempt to delete the entry.
    QVERIFY(storageTest.deleteKeyByIdentifier("Test Key"));

    // Attempt to find it again, which should fail.
    kEntry.clear();
    QVERIFY(!storageTest.keyByIdentifier("Test Key", kEntry));

    // Get all of the keys again.  The count should be 0.
    QVERIFY(storageTest.getAllKeys(allKeys));
    QCOMPARE((size_t)0, allKeys.size());
}
