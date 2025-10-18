#pragma once

#include "imports.hpp"


namespace shrekrooms::rng {


using RandomEngine = std::default_random_engine;

template <typename _Val>
class RandVal {
public:
    static_assert((std::is_integral_v<_Val> || std::is_floating_point_v<_Val>), "shrekrooms::rng::RandVal::_Val has to be either integral or floating point");

    using Val = _Val;

    inline RandVal(RandomEngine &engine, _Val lo, _Val hi) :
        m_engine(engine), m_distribution(lo, hi) { }

    inline _Val get() {
        return m_distribution(m_engine);
    }

protected:
    using M_Distribution = std::conditional_t<
        std::is_integral_v<_Val>,
        std::uniform_int_distribution<_Val>,
        std::uniform_real_distribution<_Val>
    >;

    RandomEngine &m_engine;
    M_Distribution m_distribution;

};

using RandInt = RandVal<int>;
using RandFloat = RandVal<float>;


class Random {
public:
    Random();

    RandomEngine &getEngine();

    RandInt getRandInt(RandInt::Val lo, RandInt::Val hi);
    RandFloat getRandFloat(RandFloat::Val lo, RandFloat::Val hi);

protected:
    std::random_device m_device;
    RandomEngine m_engine;

};


}; // namespace shrekrooms::rng
