#ifndef HEXDECODERTESTS_H
#define HEXDECODERTESTS_H

#include <QObject>
#include "autotest.h"

class HexDecoderTests : public QObject
{
    Q_OBJECT

private slots:
    void hexDecoderTest1();
    void isHexEncodedTest();
};

DECLARE_TEST(HexDecoderTests)

#endif // HEXDECODERTESTS_H
