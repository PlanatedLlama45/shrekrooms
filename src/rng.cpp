#include "rng.hpp"

using namespace shrekrooms::rng;


/*
 * class shrekrooms::rng::Random
*/

Random::Random() :
    m_device(), m_engine(m_device()) { }

RandomEngine &shrekrooms::rng::Random::getEngine() {
    return m_engine;
}

RandInt shrekrooms::rng::Random::getRandInt(RandInt::Val lo, RandInt::Val hi) {
    return { m_engine, lo, hi };
}

RandFloat shrekrooms::rng::Random::getRandFloat(RandFloat::Val lo, RandFloat::Val hi) {
    return { m_engine, lo, hi };
}
