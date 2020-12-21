# CSIDH-UPKE
This is a fork of the [CSI-FiSh codebase](https://github.com/KULeuven-COSIC/CSI-FiSh) to demonstrate updatable publick key encryption with CSIDH. 

## implementation 

The implementation depends on GMP for the Babai nearest plane step

All dependencies:
* make
* xsltproc
* gcc
* libgmp-dev
* libssl-dev

## running

First run "make keccaklib"
Build with "make upketest"

Run with "./upketest"

It has been tested on Ubuntu 20.04
