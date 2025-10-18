#include "rng.hpp"

using namespace shrekrooms::rng;


/*
 * class shrekrooms::rng::Random
*/

Random::Random() :
    m_device(), m_engine(m_device()) { }

RandInt shrekrooms::rng::Random::getRandInt(RandInt::ValT lo, RandInt::ValT hi) {
    return { m_engine, lo, hi };
}

RandFloat shrekrooms::rng::Random::getRandFloat(RandFloat::ValT lo, RandFloat::ValT hi) {
    return { m_engine, lo, hi };
}
