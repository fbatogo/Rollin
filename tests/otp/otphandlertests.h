#ifndef OTPHANDLERTESTS_H
#define OTPHANDLERTESTS_H

#include <QObject>

class OtpHandlerTests : public QObject
{
    Q_OBJECT

private slots:
    void calculateOtpForKeyEntryTest();
    void calculateOtpForKeyEntryTest2();
};

#endif // OTPHANDLERTESTS_H