/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#pragma once

#include <eosio/eosio.hpp>

class [[eosio::contract]] rsa : public eosio::contract {
public:
   using eosio::contract::contract;

   [[eosio::action]]
   void procassert( int8_t condition, std::string message );

   [[eosio::action]]
   void provereset();
};
