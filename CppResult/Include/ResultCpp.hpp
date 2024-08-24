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
     * @brief Ok�^�܂���Err�^�̂����ꂩ��ێ�����Result�N���X�B
     * @tparam Ok �������̌^
     * @tparam Err �G���[���̌^
     */
    template<class Ok, class Err>
    class Result
    {
    public:
        /**
         * @brief Ok�^�̌��ʂ�����������B
         * @param ok �������̒l
         */
        constexpr Result(const Ok& ok) noexcept
            : value_{ ok }
        {}

        /**
         * @brief Ok�^�̌��ʂ��E�Ӓl�Q�Ƃŏ���������B
         * @param ok �������̒l
         */
        constexpr Result(Ok&& ok) noexcept
            : value_{ std::move(ok) }
        {}

        /**
         * @brief Err�^�̌��ʂ�����������B
         * @param err �G���[���̒l
         */
        constexpr Result(const Err& err) noexcept
            : value_{ err }
        {}

        /**
         * @brief Err�^�̌��ʂ��E�Ӓl�Q�Ƃŏ���������B
         * @param err �G���[���̒l
         */
        constexpr Result(Err&& err) noexcept
            : value_{ std::move(err) }
        {}

        /**
         * @brief �������ǂ����𔻒肷��B
         * @return �����Ȃ�true�A�����łȂ����false
         */
        [[nodiscard]]
        constexpr bool is_ok() const noexcept
        {
            return std::holds_alternative<Ok>(value_);
        }

        /**
         * @brief �G���[���ǂ����𔻒肷��B
         * @return �G���[�Ȃ�true�A�����łȂ����false
         */
        [[nodiscard]]
        constexpr bool is_err() const noexcept
        {
            return std::holds_alternative<Err>(value_);
        }

        /**
         * @brief Ok�^�̒l��Ԃ��B�G���[�̏ꍇ�͗�O�𓊂���B
         * @return ��������Ok�^�̒l
         * @throws std::runtime_error �G���[�̏ꍇ
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
         * @brief Err�^�̒l��Ԃ��B�����̏ꍇ�͗�O�𓊂���B
         * @return �G���[����Err�^�̒l
         * @throws std::runtime_error �����̏ꍇ
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
         * @brief �������̒l��ϊ�����֐���K�p����B
         * @tparam Func �ϊ��֐��̌^
         * @param func �������̒l��ϊ�����֐�
         * @return �ϊ����Result�I�u�W�F�N�g
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
         * @brief �G���[���̒l��ϊ�����֐���K�p����B
         * @tparam Func �ϊ��֐��̌^
         * @param func �G���[���̒l��ϊ�����֐�
         * @return �ϊ����Result�I�u�W�F�N�g
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
         * @brief �������ɕʂ�Result��Ԃ��֐���K�p����B
         * @tparam Func �K�p����֐��̌^
         * @param func �������ɓK�p����֐�
         * @return �֐��̌��ʂ܂��̓G���[��Result
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
         * @brief �G���[���ɕʂ�Result��Ԃ��֐���K�p����B
         * @tparam Func �K�p����֐��̌^
         * @param func �G���[���ɓK�p����֐�
         * @return �֐��̌��ʂ܂��͐�����Result
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
         * @brief �������̒l��Ԃ��B�G���[�̏ꍇ�͎w�肵�����b�Z�[�W�ŗ�O�𓊂���B
         * @param msg ��O���b�Z�[�W
         * @return ��������Ok�^�̒l
         * @throws std::runtime_error �G���[�̏ꍇ�A�w�肵�����b�Z�[�W�ŗ�O�𓊂���
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
     * @brief �������ʂ𐶐�����w���p�[�֐��B
     * @tparam Ok �������̌^
     * @tparam Err �G���[���̌^
     * @param ok �������̒l
     * @return Ok�^��Err�^��Result�I�u�W�F�N�g
     */
    template<class Ok, class Err>
    constexpr Result<Ok, Err> make_ok(Ok&& ok) noexcept
    {
        return Result<Ok, Err>{ std::forward<Ok>(ok) };
    }

    /**
     * @brief �G���[���ʂ𐶐�����w���p�[�֐��B
     * @tparam Ok �������̌^
     * @tparam Err �G���[���̌^
     * @param err �G���[���̒l
     * @return Ok�^��Err�^��Result�I�u�W�F�N�g
     */
    template<class Ok, class Err>
    constexpr Result<Ok, Err> make_err(Err&& err) noexcept
    {
        return Result<Ok, Err>{ std::forward<Err>(err) };
    }

    /**
     * @brief �������ʂ𐶐�����w���p�[�֐��i�G���[�^��std::monostate�̏ꍇ�j�B
     * @tparam Ok �������̌^
     * @param ok �������̒l
     * @return Ok�^��std::monostate�^��Result�I�u�W�F�N�g
     */
    template<class Ok>
    constexpr Result<Ok, std::monostate> make_ok(Ok&& ok) noexcept
    {
        return Result<Ok, std::monostate>{ std::forward<Ok>(ok) };
    }

    /**
     * @brief �G���[���ʂ𐶐�����w���p�[�֐��i�����^��std::monostate�̏ꍇ�j�B
     * @tparam Err �G���[���̌^
     * @param err �G���[���̒l
     * @return std::monostate�^��Err�^��Result�I�u�W�F�N�g
     */
    template<class Err>
    constexpr Result<std::monostate, Err> make_err(Err&& err) noexcept
    {
        return Result<std::monostate, Err>{ std::forward<Err>(err) };
    }

} // namespace rcpp

#endif // RESULT_OK_ERRO_HPP
