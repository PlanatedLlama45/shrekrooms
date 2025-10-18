#pragma once

#include "imports.hpp"


namespace shrekrooms::rng {


template <typename _ValT>
class RandVal {
public:
    static_assert((std::is_integral_v<_ValT> || std::is_floating_point_v<_ValT>), "shrekrooms::rng::RandVal::_ValT has to be either integral or floating point");

    using ValT = _ValT;

    inline RandVal(std::default_random_engine &engine, _ValT lo, _ValT hi) :
        m_engine(engine), m_distribution(lo, hi) { }

    inline _ValT get() {
        return m_distribution(m_engine);
    }

protected:
    using M_DistributionT = std::conditional_t<
        std::is_integral_v<_ValT>,
        std::uniform_int_distribution<_ValT>,
        std::uniform_real_distribution<_ValT>
    >;

    std::default_random_engine &m_engine;
    M_DistributionT m_distribution;

};

using RandInt = RandVal<int>;
using RandFloat = RandVal<float>;


class Random {
public:
    using EngineT = std::default_random_engine;

    Random();

    EngineT &getEngine();

    RandInt getRandInt(RandInt::ValT lo, RandInt::ValT hi);
    RandFloat getRandFloat(RandFloat::ValT lo, RandFloat::ValT hi);

protected:
    std::random_device m_device;
    EngineT m_engine;

};


}; // namespace shrekrooms::rng
