#!/bin/sh
wget https://waf.io/waf-1.8.9 -O waf
chmod +x waf
wget https://raw.githubusercontent.com/philsquared/Catch/develop/single_include/catch.hpp -O tests/test_support/catch.hpp