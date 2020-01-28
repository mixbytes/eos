# BUILD
 - Install EOSIO.CDT v1.6.1
 - ```EOSIO_CDT_CMAKE=<PATH_TO_EOSIO_CDT_CMAKE_MODULE_DIR> ./build.sh```

`PATH_TO_EOSIO_CDT_CMAKE_MODULE_DIR` is path to `eosio.cdt` cmake module on your host, usually located at `/usr/lib/cmake/eosio.cdt` after install EOSIO.CDT from package manager.


# NOTE
`test_ram_limit` contract was compiled with eosio.cdt v1.4.1 and build.sh doesn't rebuild it.
That contract can be compiled with eosio.cdt v1.5.0, but the test that uses it is very sensitive to stdlib/eosiolib changes, compilation flags and linker flags.
