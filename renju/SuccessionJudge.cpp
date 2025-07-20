#include "SuccessionJudge.hpp"
#include <array>
#include <initializer_list>

namespace renju {

    /**
     * 模式匹配器模板
     * @tparam Pattern 要匹配的模式
     * @tparam MaxShift 最大移位次数
     * @tparam FixedShifts 固定移位位置
     */
    template <UINT_64 Pattern, int MaxShift, int... FixedShifts>
    struct PatternMatcher {
    private:
        // 生成常规移位掩码数组
        static constexpr auto generate_shift_masks() {
            std::array<UINT_64, MaxShift + 1> masks{};
            for (int i = 0; i <= MaxShift; ++i) {
                masks[i] = Pattern << (8 * i); // 每次左移8位(1字节)
            }
            return masks;
        }

        // 生成固定移位掩码数组
        static constexpr auto generate_fixed_masks() {
            if (sizeof...(FixedShifts) > 0) {
                return std::array<UINT_64, sizeof...(FixedShifts)>{ (Pattern << (8 * FixedShifts))... };
            } else {
                return std::array<UINT_64, 0>{};
            }
        }

    public:
        // 预计算的移位掩码
        static constexpr auto shift_masks = generate_shift_masks();
        // 预计算的固定移位掩码
        static constexpr auto fixed_masks = generate_fixed_masks();

        /**
         * 执行模式匹配
         * @param input 输入数据
         * @return 匹配成功返回1，否则返回0
         */
        static UINT_8 match(const UINT_64& input) {
            // 检查常规移位模式
            for (const auto& mask : shift_masks) {
                if ((input & mask) == mask) return 1;
            }

            // 检查固定移位模式
            for (const auto& mask : fixed_masks) {
                if ((input & mask) == mask) return 1;
            }

            return 0;
        }
    };

    /**
     * 复合模式匹配器（支持多模式OR逻辑）
     * @tparam Matchers 多个模式匹配器类型
     */
    template <typename... Matchers>
    struct MultiPatternMatcher {
        /**
         * 执行多模式匹配
         * @param input 输入数据
         * @return 任一模式匹配成功返回1，否则返回0
         */
        static UINT_8 match(const UINT_64& input) {
            return (Matchers::match(input) || ...); // 使用折叠表达式实现OR逻辑
        }
    };

    /**
     * 创建连珠模式
     * @param stones 棋子类型列表
     * @return 生成的模式
     */
    constexpr UINT_64 MakeSuccession(const std::initializer_list<StoneType> stones) {
        UINT_64 result = 0;
        int shift = 7; // 从最高位开始(64位中的第56-63位)
        for (auto stone : stones) {
            result |= static_cast<UINT_64>(stone) << (8 * shift--);
        }
        return result;
    }

    // 定义各种五子棋模式
    namespace Patterns {
        // 五连或长连模式: 任意位置+四个己方棋子
        constexpr UINT_64 FiveOrMore = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::own, StoneType::own, StoneType::own});

        // 活四模式: 任意位置+空位+三个己方棋子+空位
        constexpr UINT_64 LiveFour = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::own, StoneType::own, StoneType::none});

        // 冲四模式1: 空位+三个己方棋子
        constexpr UINT_64 RushFour1 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::none, StoneType::own, StoneType::own, StoneType::own});

        // 冲四模式2: 己方棋子+空位+两个己方棋子
        constexpr UINT_64 RushFour2 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::none, StoneType::own, StoneType::own});

        // 冲四模式3: 两个己方棋子+空位+己方棋子
        constexpr UINT_64 RushFour3 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::own, StoneType::none, StoneType::own});

        // 冲四模式4: 三个己方棋子+空位
        constexpr UINT_64 RushFour4 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::own, StoneType::own, StoneType::none});

        // 冲四模式5: 对手棋子+三个己方棋子+空位
        constexpr UINT_64 RushFour5 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::own, StoneType::own, StoneType::own, StoneType::none});

        // 冲四模式6: 空位+三个己方棋子+对手棋子
        constexpr UINT_64 RushFour6 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::own, StoneType::own, StoneType::opponent});

        // 活三模式1: 空位+空位+两个己方棋子+空位
        constexpr UINT_64 LiveThree1 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::own, StoneType::own, StoneType::none});

        // 活三模式2: 空位+两个己方棋子+空位+空位
        constexpr UINT_64 LiveThree2 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::own, StoneType::none, StoneType::none});

        // 活三模式3: 空位+己方棋子+空位+己方棋子+空位
        constexpr UINT_64 LiveThree3 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::none, StoneType::own, StoneType::none});

        // 眠三模式1: 对手棋子+两个己方棋子+空位+空位
        constexpr UINT_64 SleepThree1 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::own, StoneType::own, StoneType::none, StoneType::none});

        // 眠三模式2: 空位+空位+两个己方棋子+对手棋子
        constexpr UINT_64 SleepThree2 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::own, StoneType::own, StoneType::opponent});

        // 眠三模式3: 对手棋子+己方棋子+空位+己方棋子+空位
        constexpr UINT_64 SleepThree3 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::own, StoneType::none, StoneType::own, StoneType::none});

        // 眠三模式4: 空位+己方棋子+空位+己方棋子+对手棋子
        constexpr UINT_64 SleepThree4 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::none, StoneType::own, StoneType::opponent});

        // 眠三模式5: 对手棋子+空位+两个己方棋子+空位
        constexpr UINT_64 SleepThree5 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::none, StoneType::own, StoneType::own, StoneType::none});

        // 眠三模式6: 空位+两个己方棋子+空位+对手棋子
        constexpr UINT_64 SleepThree6 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::own, StoneType::none, StoneType::opponent});

        // 眠三模式7: 空位+空位+两个己方棋子
        constexpr UINT_64 SleepThree7 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::none, StoneType::none, StoneType::own, StoneType::own});

        // 眠三模式8: 己方棋子+空位+空位+己方棋子
        constexpr UINT_64 SleepThree8 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::none, StoneType::none, StoneType::own});

        // 眠三模式9: 两个己方棋子+空位+空位
        constexpr UINT_64 SleepThree9 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::own, StoneType::none, StoneType::none});

        // 眠三模式10: 空位+己方棋子+空位+己方棋子
        constexpr UINT_64 SleepThree10 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::none, StoneType::own, StoneType::none, StoneType::own});

        // 眠三模式11: 己方棋子+空位+己方棋子+空位
        constexpr UINT_64 SleepThree11 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::none, StoneType::own, StoneType::none});

        // 眠三模式12: 对手棋子+空位+两个己方棋子+空位+对手棋子
        constexpr UINT_64 SleepThree12 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::opponent, StoneType::none,
            StoneType::own, StoneType::own, StoneType::none, StoneType::opponent});

        // 活二模式1: 空位+空位+空位+己方棋子+空位
        constexpr UINT_64 LiveTwo1 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::none, StoneType::own, StoneType::none});

        // 活二模式2: 空位+空位+己方棋子+空位+空位
        constexpr UINT_64 LiveTwo2 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::own, StoneType::none, StoneType::none});

        // 活二模式3: 空位+己方棋子+空位+空位+空位
        constexpr UINT_64 LiveTwo3 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::none, StoneType::none, StoneType::none});

        // 眠二模式1: 对手棋子+空位+己方棋子+空位+空位+对手棋子
        constexpr UINT_64 SleepTwo1 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::opponent, StoneType::none,
            StoneType::own, StoneType::none, StoneType::none, StoneType::opponent});

        // 眠二模式2: 对手棋子+空位+空位+己方棋子+空位+对手棋子
        constexpr UINT_64 SleepTwo2 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::opponent, StoneType::none,
            StoneType::none, StoneType::own, StoneType::none, StoneType::opponent});

        // 眠二模式3: 对手棋子+空位+己方棋子+空位+空位
        constexpr UINT_64 SleepTwo3 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::none, StoneType::own, StoneType::none, StoneType::none});

        // 眠二模式4: 空位+空位+空位+己方棋子+对手棋子
        constexpr UINT_64 SleepTwo4 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::none, StoneType::own, StoneType::opponent});

        // 眠二模式5: 空位+空位+己方棋子+空位+对手棋子
        constexpr UINT_64 SleepTwo5 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::none, StoneType::own, StoneType::none, StoneType::opponent});

        // 眠二模式6: 对手棋子+己方棋子+空位+空位+空位
        constexpr UINT_64 SleepTwo6 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::own, StoneType::none, StoneType::none, StoneType::none});

        // 眠二模式7: 对手棋子+空位+空位+己方棋子+空位
        constexpr UINT_64 SleepTwo7 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::opponent,
            StoneType::none, StoneType::none, StoneType::own, StoneType::none});

        // 眠二模式8: 空位+己方棋子+空位+空位+对手棋子
        constexpr UINT_64 SleepTwo8 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::none,
            StoneType::own, StoneType::none, StoneType::none, StoneType::opponent});

        // 眠二模式9: 空位+空位+空位+己方棋子
        constexpr UINT_64 SleepTwo9 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::none, StoneType::none, StoneType::none, StoneType::own});

        // 眠二模式10: 己方棋子+空位+空位+空位
        constexpr UINT_64 SleepTwo10 = MakeSuccession({
            StoneType::any, StoneType::any, StoneType::any, StoneType::any,
            StoneType::own, StoneType::none, StoneType::none, StoneType::none});
    }

    /**
     * 判断是否五连或长连
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionFiveOrMore(const UINT_64 &input) {
        return PatternMatcher<Patterns::FiveOrMore, 4>::match(input);
    }

    /**
     * 判断是否活四
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionLiveFour(const UINT_64 &input) {
        return PatternMatcher<Patterns::LiveFour, 3>::match(input);
    }

    /**
     * 判断是否冲四
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionRushFour(const UINT_64 &input) {
        return MultiPatternMatcher<
            PatternMatcher<Patterns::RushFour1, 4>,
            PatternMatcher<Patterns::RushFour2, 4>,
            PatternMatcher<Patterns::RushFour3, 4>,
            PatternMatcher<Patterns::RushFour4, 4>,
            PatternMatcher<Patterns::RushFour5, 3>,
            PatternMatcher<Patterns::RushFour6, 3>
        >::match(input);
    }

    /**
     * 判断是否活三
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionLiveThree(const UINT_64 &input) {
        return MultiPatternMatcher<
            PatternMatcher<Patterns::LiveThree1, 3>,
            PatternMatcher<Patterns::LiveThree2, 3>,
            PatternMatcher<Patterns::LiveThree3, 3>
        >::match(input);
    }

    /**
     * 判断是否眠三
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionSleepThree(const UINT_64 &input) {
        return MultiPatternMatcher<
            PatternMatcher<Patterns::SleepThree1, 3>,
            PatternMatcher<Patterns::SleepThree2, 3>,
            PatternMatcher<Patterns::SleepThree3, 3>,
            PatternMatcher<Patterns::SleepThree4, 3>,
            PatternMatcher<Patterns::SleepThree5, 2>,
            PatternMatcher<Patterns::SleepThree6, 3>,
            PatternMatcher<Patterns::SleepThree7, 4>,
            PatternMatcher<Patterns::SleepThree8, 4>,
            PatternMatcher<Patterns::SleepThree9, 4>,
            PatternMatcher<Patterns::SleepThree10, 4>,
            PatternMatcher<Patterns::SleepThree11, 4>,
            PatternMatcher<Patterns::SleepThree12, 2>
        >::match(input);
    }

    /**
     * 判断是否活二
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionLiveTwo(const UINT_64 &input) {
        return MultiPatternMatcher<
            PatternMatcher<Patterns::LiveTwo1, 3>,
            PatternMatcher<Patterns::LiveTwo2, 3>,
            PatternMatcher<Patterns::LiveTwo3, 3>
        >::match(input);
    }

    /**
     * 判断是否眠二
     * @param input 输入数据
     * @return 匹配成功返回1，否则返回0
     */
    UINT_8 IsSuccessionSleepTwo(const UINT_64 &input) {
        return MultiPatternMatcher<
            PatternMatcher<Patterns::SleepTwo1, 2>,
            PatternMatcher<Patterns::SleepTwo2, 2>,
            PatternMatcher<Patterns::SleepTwo3, 3>,
            PatternMatcher<Patterns::SleepTwo4, 3>,
            PatternMatcher<Patterns::SleepTwo5, 3>,
            PatternMatcher<Patterns::SleepTwo6, 3>,
            PatternMatcher<Patterns::SleepTwo7, 3>,
            PatternMatcher<Patterns::SleepTwo8, 3>,
            PatternMatcher<Patterns::SleepTwo9, 4>,
            PatternMatcher<Patterns::SleepTwo10, 4>
        >::match(input);
    }
}
