#ifndef KEYENTRIESSINGLETON_H
#define KEYENTRIESSINGLETON_H

#include <QObject>
#include <QTimer>
#include <QQmlEngine>
#include <QJSEngine>

#include "keystorage/keyentry.h"
#include "keystorage/keystorage.h"

class KeyEntriesSingleton : public QObject
{
    Q_OBJECT

public:
    ~KeyEntriesSingleton();                 //NOSONAR

    static KeyEntriesSingleton *getInstance();
    static QObject *getQmlSingleton(QQmlEngine *engine, QJSEngine *scriptEngine);

    void clear();

    bool open();
    bool isOpen();
    bool close();

    bool calculateEntries();

    Q_INVOKABLE bool addKeyEntry(const QString &identifier, const QString &issuer, const QString &secret, unsigned int keyType, unsigned int otpType, unsigned int numberCount, unsigned int algorithm, unsigned int period, unsigned int offset);
    bool addKeyEntry(const KeyEntry &toAdd);

    Q_INVOKABLE bool updateKeyEntry(KeyEntry *currentEntry, QString identifier, QString secret, unsigned int keyType, unsigned int otpType, unsigned int numberCount, unsigned int algorithm, unsigned int period, unsigned int offset);
    bool updateKeyEntry(const KeyEntry &original, const KeyEntry &updated);

    Q_INVOKABLE bool deleteKeyEntry(const QString &toDelete);

    Q_INVOKABLE bool incrementHotpCounter(const QString &identifier);

    Q_INVOKABLE int count();
    Q_INVOKABLE KeyEntry *at(int i);
    Q_INVOKABLE KeyEntry *fromIdentifier(const QString &identifier);

signals:                                //NOSONAR
    void fullRefresh();

private slots:
    void slotUpdateOtpValues();

private:                                //NOSONAR
    explicit KeyEntriesSingleton(QObject *parent = nullptr);

    bool entryParametersAreValid(const QString &addUpdate, QString identifier, QString secret, unsigned int keyType, unsigned int otpType, unsigned int numberCount, unsigned int algorithm);
    bool populateEntries();
    int indexFromIdentifierInMemory(const QString &identifier);
    KeyEntry *fromIdentifierInMemory(const QString &identifier);
    KeyEntry *fromIdentifierInKeyStorage(const QString &identifier);
    bool deleteKeyEntryFromMemory(const QString &toDelete);
    bool updateKeyEntryInMemory(const KeyEntry &original, const KeyEntry &updated);
    bool addKeyEntryInMemory(const KeyEntry &toAdd);

    bool updateTimer();
    unsigned int shortestUpdatePeriod();

    QList<KeyEntry *> mEntryList;

    QTimer mUpdateTimer;

    KeyStorage mKeyStorage;
};

#endif // KEYENTRIESSINGLETON_H
