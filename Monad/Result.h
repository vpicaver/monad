#pragma once

//Qt includes
#include <QString>
#include <QMetaType>
#include <QFuture>

//Std includes
#include <utility>

namespace Monad {

//Forward declare mbind
template<typename T, typename Func>
auto mbind(T value, Func f);

class ResultBase {
    Q_GADGET
    Q_PROPERTY(int errorCode READ errorCode CONSTANT FINAL)
    Q_PROPERTY(QString errorMessage READ errorMessage CONSTANT FINAL)
    Q_PROPERTY(bool hasError READ hasError CONSTANT FINAL)

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

    bool hasError() const { return mErrorCode != NoError && mErrorCode != Warning; }

    // Conversion operator to bool
    explicit operator bool() const {
        return !hasError();
    }

    template<typename F>
    auto then(F f) const {
        return mbind(*this, f);
    }

    ResultBase replaceIfErrorWith(int error) const {
        if(hasError()) {
            return ResultBase(mErrorMessage, error);
        } else {
            return *this;
        }
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

    template<typename F>
    auto then(F f) const {
        return mbind(*this, f);
    }

    Result<T> replaceIfErrorWith(int error) const {
        if(hasError()) {
            return Result<T>(errorMessage(), error);
        } else {
            return *this;
        }
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
