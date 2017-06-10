#!/bin/bash
echo core 0
./smdtest -core  0  $@   >test.log
echo core 1
./smdtest -core  1  $@  >>test.log
echo core 2
./smdtest -core  2  $@  >>test.log
echo core 3
./smdtest -core  3  $@  >>test.log
echo core 4
./smdtest -core  4  $@  >>test.log
echo core 5
./smdtest -core  5  $@  >>test.log
echo core 6
./smdtest -core  6  $@  >>test.log
echo core 7
./smdtest -core  7  $@  >>test.log
echo core 8
./smdtest -core  8  $@  >>test.log
echo core 9
./smdtest -core  9  $@  >>test.log
echo core 10
./smdtest -core 10  $@  >>test.log
echo core 11
./smdtest -core 11  $@  >>test.log
echo core 12
./smdtest -core 12  $@  >>test.log
echo core 13
./smdtest -core 13  $@  >>test.log
echo core 14
./smdtest -core 14  $@  >>test.log
echo core 15
./smdtest -core 15  $@  >>test.log
