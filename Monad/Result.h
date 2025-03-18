#ifndef RESULT_H
#define RESULT_H

//Qt includes
#include <QString>
#include <QMetaType>
#include <QFuture>


//Std includes
#include <utility>

namespace Monad {

class ResultBase {
public:
    enum ErrorCode {
        NoError,
        Warning,
        Unknown,
        CustomError = 1024
    };

    ResultBase() = default;

    explicit ResultBase(const QString& errorMessage, int errorCode = Unknown) :
        mErrorMessage(std::move(errorMessage)),
        mErrorCode(errorCode)
    {}

    QString errorMessage() const { return mErrorMessage; }

    int errorCode() const { return mErrorCode; }

    template<typename T>
    T errorCodeTo() const {
        return static_cast<T>(mErrorCode);
    }

    bool hasError() const { return mErrorCode != NoError || mErrorCode != Warning; }

    // Conversion operator to bool
    explicit operator bool() const {
        return !hasError();
    }

private:
    QString mErrorMessage;
    int mErrorCode = NoError;
};

template<typename T>
class Result : public ResultBase {
public:
    Result() = default;

    explicit Result(const QString& errorMessage, int errorCode = Unknown) :
        ResultBase(errorMessage, errorCode)
    {}

    Result(T value):
        ResultBase(),
        mValue(std::move(value))
    { }

    Result(T value, const QString& warningMessage):
        ResultBase(warningMessage, Warning),
        mValue(std::move(value))
    { }

    T value() const {
        return mValue;
    }

protected:
    T mValue;
};

class ResultString : public Result<QString> {
public:
    ResultString() { }

    ResultString(const QString& errorMessage, int errorCode) :
        Result<QString>(errorMessage, errorCode) {}

    ResultString(const QString& value) :
        Result<QString>(QString(), NoError)
    {
        mValue = value;
    }
    ResultString(const QString& value, const QString& warningMessage) :
        Result<QString>(warningMessage, Warning)
    {
        mValue = value;
    }
};
}

Q_DECLARE_METATYPE(QFuture<Monad::ResultBase>)
Q_DECLARE_METATYPE(QFuture<Monad::ResultString>)
Q_DECLARE_METATYPE(Monad::ResultBase)
Q_DECLARE_METATYPE(Monad::ResultString)


#endif
