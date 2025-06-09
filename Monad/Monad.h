#ifndef MONAD_H
#define MONAD_H

//Our inculdes
#include "Result.h"

//Qt includes
#include <QFuture>

//Std includes
#include <type_traits>

namespace Monad {

/**
 * Tests if T is a QFuture<T>, for all other types it's std::false_type
 */
template <typename Future> struct is_qfuture : std::false_type {};
template <typename T> struct is_qfuture<QFuture<T>> : std::true_type { };

/**
  Creates a contained_type_t that the T in QFuture<T>
 */
template <typename T>
struct qfuture {
    static_assert(is_qfuture<T>(), "T must be a QFuture<>");
    using contained_type_t = decltype(T().result());
};

/**
  Monad for ResultBase handling
 */
template<typename T, typename Func>
auto mbind(T value, Func f) {

    auto result = [value]() {
        if constexpr(is_qfuture<T>()) {
            //Will strip the QFuture from value
            return value.result();
        } else {
            return value;
        }
    }();

    if constexpr(is_qfuture<T>()) {
        using FutureT = typename qfuture<T>::contained_type_t;
        using R = typename std::invoke_result<Func, FutureT>::type;

        //This is copy pasta from above
        if(result.hasError()) {
            if constexpr(is_qfuture<R>()) {
                using FutureR = typename qfuture<R>::contained_type_t;
                return QtFuture::makeReadyValueFuture(FutureR(result.errorMessage(), result.errorCode()));
            } else {
                return R(result.errorMessage(), result.errorCode());
            }
        } else {
            return f(result);
        }
    } else {
        using R = typename std::invoke_result<Func, T>::type;

        //This is copy pasta from above
        if(result.hasError()) {
            if constexpr(is_qfuture<R>()) {
                using FutureR = typename qfuture<R>::contained_type_t;
                return QtFuture::makeReadyValueFuture(FutureR(result.errorMessage(), result.errorCode()));
            } else {
                return R(result.errorMessage(), result.errorCode());
            }
        } else {
            return f(result);
        }
    }

    // using ResultType = decltype(result);

    // if(result.hasError()) {
    //     // if constexpr(is_pfuture<R>()) {
    //     //     using FutureT = typename pfuture<R>::contained_type_t;
    //     //     if constexpr(is_pfuture<T>()) {
    //     //         return value.ready(FutureT(result.errorMessage(), result.errorCode()));
    //     //     } else {
    //     //         return PFuture(QtFuture::makeReadyFuture<FutureT>(FutureT(result.errorMessage(), result.errorCode())));
    //     //     }
    //     // } else
    //     using R = typename std::invoke_result<Func, T>::type;
    //     if constexpr(is_qfuture<R>()) {
    //         using FutureT = typename qfuture<R>::contained_type_t;
    //         return QtFuture::makeReadyValueFuture(FutureT(result.errorMessage(), result.errorCode()));
    //     } else {
    //         return ResultType(result.errorMessage(), result.errorCode());
    //     }
    // } else {
    //     if constexpr (std::is_invocable_v<Func, T>) {
    //         return f(value);
    //     } else if constexpr (std::is_invocable_v<Func, ResultType>) {
    //         return f(result);
    //     } else {
    //         Q_ASSERT(false);
    //         //static_assert(false, "f must accept either T or the result type of T as its argument");
    //     }
    // }
}

///**
// Monad for error checking ProcessResults
// */
//template<typename Func>
//auto mbind(const LocalClient::ProcessResult& result, Func f) {
//    using R = typename std::invoke_result<Func, LocalClient::ProcessData>::type;
//    if(result.hasError()) {
//        return R(result.errorMessage(), result.errorCode());
//    }
//    if(result.value().exitCode != 0) {
//        QString errorMessage = QStringLiteral("%4 exited with %1 with out:%2 err:%3")
//                .arg(result.value().exitCode)
//                .arg(result.value().standardOut.constData(),
//                     result.value().standardError.constData(),
//                     result.value().command);
//        return R(errorMessage);
//    } else {
//        return f(result);
//    }
//}

/**
 Monad for handling try and catch blocks
 */
template<typename F>
auto mtry(F f) {
    using R = typename std::invoke_result<F>::type;
    try {
        return f();
    } catch( std::runtime_error error) {
        return R(QString::fromLocal8Bit(error.what()).trimmed());
    }
}

/**
 * Monad composition
 */
template<typename F, typename G>
auto mcompose(F f, G g) {
    return [=](auto value) {
        return mbind(f(value), g);
    };
}

/**
 * Monad composition
 *
 * This is needed if other functions like QFuture.then() can't figure out what T is.
 */
template<typename T, typename F, typename G>
auto mcompose(F f, G g) {
    return [=](T value) {
        return mbind(f(value), g);
    };
}

template<typename T>
auto passthrough(T result) {
    return std::forward<T>(result);
}

template<typename T, typename F>
auto pbind(F f) {
    return mcompose<T>(passthrough<T>, f);
}

template<typename T>
auto makeSuccessFrom(T) {
    return ResultBase();
}
};

#endif // MONAD_H
