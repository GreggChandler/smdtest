#!/bin/bash
echo core 0
./smdtest -testcore  0  $@   >test00.txt
echo core 1
./smdtest -testcore  1  $@   >test01.txt
echo core 2
./smdtest -testcore  2  $@   >test02.txt
echo core 3
./smdtest -testcore  3  $@   >test03.txt
echo core 4
./smdtest -testcore  4  $@   >test04.txt
echo core 5
./smdtest -testcore  5  $@   >test05.txt
echo core 6
./smdtest -testcore  6  $@   >test06.txt
echo core 7
./smdtest -testcore  7  $@   >test07.txt
echo core 8
./smdtest -testcore  8  $@   >test08.txt
echo core 9
./smdtest -testcore  9  $@   >test09.txt
echo core 10
./smdtest -testcore 10  $@   >test10.txt
echo core 11
./smdtest -testcore 11  $@   >test11.txt
echo core 12
./smdtest -testcore 12  $@   >test12.txt
echo core 13
./smdtest -testcore 13  $@   >test13.txt
echo core 14
./smdtest -testcore 14  $@   >test14.txt
echo core 15
./smdtest -testcore 15  $@   >test15.txt
