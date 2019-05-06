#include "bytearraytests.h"

#include "container/bytearray.h"
#include <QtTest>

void ByteArrayTests::toStringTests()
{
    ByteArray testByteArray("This is a test char array.");

    // See if that, converted to a string, matches the original value.
    QCOMPARE(std::string("This is a test char array."), testByteArray.toString());

    // Clear it out and check the string.
    testByteArray.clear();

    QCOMPARE(std::string(""), testByteArray.toString());
}

void ByteArrayTests::emptyStringTests()
{
    ByteArray testByteArray;

    QCOMPARE(std::string(""), testByteArray.toString());
}

void ByteArrayTests::atTests()
{
    ByteArray testByteArray("This is a test char array.");

    QCOMPARE((size_t)26, testByteArray.size());
    QCOMPARE((unsigned char)'T', testByteArray.at(0));
    QCOMPARE((unsigned char)'h', testByteArray.at(1));
    QCOMPARE((unsigned char)'.', testByteArray.at(25));
    QCOMPARE((unsigned char)0x00, testByteArray.at(100));
}

void ByteArrayTests::equalityTests()
{
    ByteArray testByteArray1("This is a test char array.");
    ByteArray testByteArray2;
    ByteArray *testByteArray3;

    // The two objects should NOT match to start with.
    QVERIFY(testByteArray1 != testByteArray2);

    // Copy the data with the = operator.
    testByteArray2 = testByteArray1;

    // They should now match.
    QVERIFY(testByteArray1 == testByteArray2);

    // Clear the 2nd object.
    testByteArray2.clear();

    // The two objects should NOT match again.
    QVERIFY(testByteArray1 != testByteArray2);

    // Populate the 2nd one from a string.
    testByteArray2 = std::string("This is a test char array.");

    // They should match again.
    QVERIFY(testByteArray1 == testByteArray2);

    // Same length string, but slightly different text.
    testByteArray2 = std::string("This is a test char array!");

    // They should note match.
    QVERIFY(testByteArray1 != testByteArray2);

    // Copy the data with the copy ctor.
    testByteArray3 = new ByteArray(testByteArray1);

    // They should now match again.
    QVERIFY(testByteArray1 == (*testByteArray3));

    // Clean up.
    delete testByteArray3;
}

void ByteArrayTests::fromOthersTests()
{
    ByteArray testByteArray(std::string("This is a test char array."));
    char *cString;

    // Change the value using a string.
    testByteArray.fromStdString(std::string("This is a different test char array."));

    // Make sure it matches.
    QCOMPARE(std::string("This is a different test char array."), testByteArray.toString());

    // Create a C style string to  assign to the byte array.
    cString = strdup("This is a test C style string!");
    Q_ASSERT(nullptr != cString);

    // Update testByteArray with the C string.
    testByteArray.fromCharArray(cString);

    // Make sure testByteArray has the data we expect.
    QCOMPARE(std::string("This is a test C style string!"), testByteArray.toString());

    // Clear out the testByteArray.
    testByteArray.clear();

    QCOMPARE(std::string(""), testByteArray.toString());

    // Then, add the string back in, this time as an unsigned char array.
    testByteArray.fromUCharArray(reinterpret_cast<unsigned char *>(cString));

    // And make sure it is what we expect.
    QCOMPARE(std::string("This is a test C style string!"), testByteArray.toString());

    // Then, clean up the memory we used.
    free(cString);
}

void ByteArrayTests::assignmentTests()
{
    ByteArray ba1;
    ByteArray ba2;

    // ba1 should be empty and null.
    QCOMPARE(std::string(""), ba1.toString());
    QCOMPARE(nullptr, ba1.toCharArrayPtr());
    QCOMPARE(nullptr, ba1.toUCharArrayPtr());

    // Assign a string to it.
    ba1 = std::string("This is a test string!");

    // Make sure it is what we expect.
    QCOMPARE(std::string("This is a test string!"), ba1.toString());

    // ba2 should be empty.
    QCOMPARE(std::string(""), ba2.toString());
    QCOMPARE(nullptr, ba2.toCharArrayPtr());
    QCOMPARE(nullptr, ba2.toUCharArrayPtr());

    // Assign the data in ba1 to ba2.
    ba2 = ba1;

    // Make sure it is what we expect.
    QCOMPARE(std::string("This is a test string!"), ba2.toString());

    // Clear out ba1 and then check again, to be sure that the data was copied, and
    // not just a pointer to the data was copied.
    ba1.clear();

    // ba1 should be empty now..
    QCOMPARE(std::string(""), ba1.toString());
    QCOMPARE(nullptr, ba1.toCharArrayPtr());
    QCOMPARE(nullptr, ba1.toUCharArrayPtr());

    // But, ba2 should still have our data.
    QCOMPARE(std::string("This is a test string!"), ba2.toString());
}

void ByteArrayTests::appendTests()
{
    ByteArray testByteArray("This is some initial text.");
    ByteArray toAppendByteArray("This is text to append!");
    char *cString;

    // Tell the object to zero the array when freeing memory.
    testByteArray.setZeroOnFree(true);

    // Append an std::string to the end.
    testByteArray.append(std::string(" So is this!"));

    // Make sure we got the appended data back.
    QCOMPARE(std::string("This is some initial text. So is this!"), testByteArray.toString());

    // Clear the string, and append to an empty object.
    testByteArray.clear();
    testByteArray.append(std::string("So is this!"));
    QCOMPARE(std::string("So is this!"), testByteArray.toString());

    // Allocate a C style string to append.
    cString = strdup("C style string!");

    testByteArray.append(cString);
    QCOMPARE(std::string("So is this!C style string!"), testByteArray.toString());

    // Append it as an unsigned char array.
    testByteArray.append(reinterpret_cast<unsigned char *>(cString));
    QCOMPARE(std::string("So is this!C style string!C style string!"), testByteArray.toString());

    // Clean up.
    free(cString);

    // Append a single character.
    testByteArray.append('A');
    QCOMPARE(std::string("So is this!C style string!C style string!A"), testByteArray.toString());

    // Append another ByteArray object.
    testByteArray.append(toAppendByteArray);
    QCOMPARE(std::string("So is this!C style string!C style string!AThis is text to append!"), testByteArray.toString());
}

void ByteArrayTests::expandedBufferAppendTests()
{
    ByteArray testByteArray("This is some initial text.");
    char *cString;

    // Tell the object to allocate a bunch of extra space. (On the next allocation.)
    testByteArray.setExtraAllocation(512);

    // Append an std::string to the end.  (This should cause a new allocation, with the extra space.)
    testByteArray.append(std::string(" So is this!"));

    // Make sure we got the appended data back.
    QCOMPARE(std::string("This is some initial text. So is this!"), testByteArray.toString());

    // Clear the string, and append to an empty object.  (This should also cause a new allocation, because we do a clear().)
    testByteArray.clear();
    testByteArray.append(std::string("So is this!"));
    QCOMPARE(std::string("So is this!"), testByteArray.toString());

    // Allocate a C style string to append.
    cString = strdup("C style string!");

    // Append data to the existing byte array.  (This should only cause a copy, since the buffer should be large enough to hold it all.)
    testByteArray.append(cString);
    QCOMPARE(std::string("So is this!C style string!"), testByteArray.toString());

    // Append it as an unsigned char array.  (This should also only cause a copy.)
    testByteArray.append(reinterpret_cast<unsigned char *>(cString));
    QCOMPARE(std::string("So is this!C style string!C style string!"), testByteArray.toString());

    // Append a single character.  (This should also only cause a copy.)
    testByteArray.append('A');
    QCOMPARE(std::string("So is this!C style string!C style string!A"), testByteArray.toString());

    // Clean up.
    free(cString);
}

