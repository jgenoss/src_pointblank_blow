#ifndef BOOST_RANDOM_HPP
#define BOOST_RANDOM_HPP

#include <boost/random/additive_combine.hpp>
#include <boost/random/shuffle_output.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/linear_feedback_shift.hpp>
#include <boost/random/xor_combine.hpp>

namespace boost {

  typedef random::xor_combine<random::xor_combine<random::linear_feedback_shift<uint32_t, 32, 31, 13, 12, 0>, 0,
    random::linear_feedback_shift<uint32_t, 32, 29, 2, 4, 0>, 0, 0>, 0,
                      random::linear_feedback_shift<uint32_t, 32, 28, 3, 17, 0>, 0, 0> taus88;
}
#include <boost/random/random_number_generator.hpp>


#endif
