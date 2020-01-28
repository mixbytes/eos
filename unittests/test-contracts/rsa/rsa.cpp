/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <rsa.hpp>

using namespace eosio;

static int global_variable = 45;

void rsa::procassert( int8_t condition, std::string message ) {
   check( condition != 0, message );
}

void rsa::provereset() {
   check( global_variable == 45, "Global Variable Initialized poorly" );
   global_variable = 100;
}
