#ifndef RESULT_OK_ERRO_HPP
#define RESULT_OK_ERRO_HPP

#include <variant>
#include <optional>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <string>

namespace rcpp
{

#if __cplusplus >= 202002L
    template<class T>
    concept IsNotVoid = !std::is_void_v<T>;

    template<class T, class U>
    concept SameType = std::is_same_v<T, U>;
#endif

    /**
     * @brief Ok型またはErr型のいずれかを保持するResultクラス。
     * @tparam Ok 成功時の型
     * @tparam Err エラー時の型
     */
    template<class Ok, class Err>
    class Result
    {
    public:
        /**
         * @brief Ok型の結果を初期化する。
         * @param ok 成功時の値
         */
        constexpr Result(const Ok& ok) noexcept
            : value_{ ok }
        {}

        /**
         * @brief Ok型の結果を右辺値参照で初期化する。
         * @param ok 成功時の値
         */
        constexpr Result(Ok&& ok) noexcept
            : value_{ std::move(ok) }
        {}

        /**
         * @brief Err型の結果を初期化する。
         * @param err エラー時の値
         */
        constexpr Result(const Err& err) noexcept
            : value_{ err }
        {}

        /**
         * @brief Err型の結果を右辺値参照で初期化する。
         * @param err エラー時の値
         */
        constexpr Result(Err&& err) noexcept
            : value_{ std::move(err) }
        {}

        /**
         * @brief 成功かどうかを判定する。
         * @return 成功ならtrue、そうでなければfalse
         */
        [[nodiscard]]
        constexpr bool is_ok() const noexcept
        {
            return std::holds_alternative<Ok>(value_);
        }

        /**
         * @brief エラーかどうかを判定する。
         * @return エラーならtrue、そうでなければfalse
         */
        [[nodiscard]]
        constexpr bool is_err() const noexcept
        {
            return std::holds_alternative<Err>(value_);
        }

        /**
         * @brief Ok型の値を返す。エラーの場合は例外を投げる。
         * @return 成功時のOk型の値
         * @throws std::runtime_error エラーの場合
         */
        constexpr Ok unwrap() const
        {
            if (is_ok())
            {
                return std::get<Ok>(value_);
            }
            else
            {
                throw std::runtime_error{ "Called unwrap on an Err value" };
            }
        }

        /**
         * @brief Err型の値を返す。成功の場合は例外を投げる。
         * @return エラー時のErr型の値
         * @throws std::runtime_error 成功の場合
         */
        constexpr Err unwrap_err() const
        {
            if (is_err())
            {
                return std::get<Err>(value_);
            }
            else
            {
                throw std::runtime_error{ "Called unwrap_err on an Ok value" };
            }
        }

        /**
         * @brief 成功時の値を変換する関数を適用する。
         * @tparam Func 変換関数の型
         * @param func 成功時の値を変換する関数
         * @return 変換後のResultオブジェクト
         */
        template<class Func>
#if __cplusplus >= 202002L
            requires IsNotVoid<decltype(std::declval<Func>()(std::declval<Ok>()))>
#endif
        constexpr auto map(Func&& func) const -> Result<decltype(func(std::declval<Ok>())), Err>
        {
            using NewOk = decltype(func(std::declval<Ok>()));
            if (is_ok())
            {
                return Result<NewOk, Err>(func(std::get<Ok>(value_)));
            }
            else
            {
                return Result<NewOk, Err>(std::get<Err>(value_));
            }
        }

        /**
         * @brief エラー時の値を変換する関数を適用する。
         * @tparam Func 変換関数の型
         * @param func エラー時の値を変換する関数
         * @return 変換後のResultオブジェクト
         */
        template<class Func>
#if __cplusplus >= 202002L
            requires IsNotVoid<decltype(std::declval<Func>()(std::declval<Err>()))>
#endif
        constexpr auto map_err(Func&& func) const -> Result<Ok, decltype(func(std::declval<Err>()))>
        {
            using NewErr = decltype(func(std::declval<Err>()));
            if (is_err())
            {
                return Result<Ok, NewErr>(func(std::get<Err>(value_)));
            }
            else
            {
                return Result<Ok, NewErr>(std::get<Ok>(value_));
            }
        }

        /**
         * @brief 成功時に別のResultを返す関数を適用する。
         * @tparam Func 適用する関数の型
         * @param func 成功時に適用する関数
         * @return 関数の結果またはエラーのResult
         */
        template<class Func>
#if __cplusplus >= 202002L
            requires IsNotVoid<decltype(std::declval<Func>()(std::declval<Ok>()))>
#endif
        constexpr auto and_then(Func&& func) const -> decltype(func(std::declval<Ok>()))
        {
            if (is_ok())
            {
                return func(std::get<Ok>(value_));
            }
            else
            {
                return Result<Ok, Err>(std::get<Err>(value_));
            }
        }

        /**
         * @brief エラー時に別のResultを返す関数を適用する。
         * @tparam Func 適用する関数の型
         * @param func エラー時に適用する関数
         * @return 関数の結果または成功のResult
         */
        template<class Func>
#if __cplusplus >= 202002L
            requires IsNotVoid<decltype(std::declval<Func>()(std::declval<Err>()))>
#endif
        constexpr auto or_else(Func&& func) const -> decltype(func(std::declval<Err>()))
        {
            if (is_err())
            {
                return func(std::get<Err>(value_));
            }
            else
            {
                return Result<Ok, Err>(std::get<Ok>(value_));
            }
        }

        /**
         * @brief 成功時の値を返す。エラーの場合は指定したメッセージで例外を投げる。
         * @param msg 例外メッセージ
         * @return 成功時のOk型の値
         * @throws std::runtime_error エラーの場合、指定したメッセージで例外を投げる
         */
        constexpr Ok expect(const std::string& msg) const
        {
            if (is_ok())
            {
                return std::get<Ok>(value_);
            }
            else
            {
                throw std::runtime_error(msg);
            }
        }

    private:
        std::variant<Ok, Err> value_;
    };

    /**
     * @brief 成功結果を生成するヘルパー関数。
     * @tparam Ok 成功時の型
     * @tparam Err エラー時の型
     * @param ok 成功時の値
     * @return Ok型とErr型のResultオブジェクト
     */
    template<class Ok, class Err>
    constexpr Result<Ok, Err> make_ok(Ok&& ok) noexcept
    {
        return Result<Ok, Err>{ std::forward<Ok>(ok) };
    }

    /**
     * @brief エラー結果を生成するヘルパー関数。
     * @tparam Ok 成功時の型
     * @tparam Err エラー時の型
     * @param err エラー時の値
     * @return Ok型とErr型のResultオブジェクト
     */
    template<class Ok, class Err>
    constexpr Result<Ok, Err> make_err(Err&& err) noexcept
    {
        return Result<Ok, Err>{ std::forward<Err>(err) };
    }

    /**
     * @brief 成功結果を生成するヘルパー関数（エラー型がstd::monostateの場合）。
     * @tparam Ok 成功時の型
     * @param ok 成功時の値
     * @return Ok型とstd::monostate型のResultオブジェクト
     */
    template<class Ok>
    constexpr Result<Ok, std::monostate> make_ok(Ok&& ok) noexcept
    {
        return Result<Ok, std::monostate>{ std::forward<Ok>(ok) };
    }

    /**
     * @brief エラー結果を生成するヘルパー関数（成功型がstd::monostateの場合）。
     * @tparam Err エラー時の型
     * @param err エラー時の値
     * @return std::monostate型とErr型のResultオブジェクト
     */
    template<class Err>
    constexpr Result<std::monostate, Err> make_err(Err&& err) noexcept
    {
        return Result<std::monostate, Err>{ std::forward<Err>(err) };
    }

} // namespace rcpp

#endif // RESULT_OK_ERRO_HPP
