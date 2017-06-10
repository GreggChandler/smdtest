#!/bin/bash
echo core 0
./smdtest -othercore  0 0  $@   >test00.txt
echo core 1
./smdtest -othercore  0 1  $@   >test01.txt
echo core 2
./smdtest -othercore  0 2  $@   >test02.txt
echo core 3
./smdtest -othercore  0 3  $@   >test03.txt
echo core 4
./smdtest -othercore  1 0  $@   >test04.txt
echo core 5
./smdtest -othercore  1 1  $@   >test05.txt
echo core 6
./smdtest -othercore  1 2  $@   >test06.txt
echo core 7
./smdtest -othercore  1 3  $@   >test07.txt
echo core 8
./smdtest -othercore  2 0  $@   >test08.txt
echo core 9
./smdtest -othercore  2 1  $@   >test09.txt
echo core 10
./smdtest -othercore  2 2  $@   >test10.txt
echo core 11
./smdtest -othercore  2 3  $@   >test11.txt
echo core 12
./smdtest -othercore  3 0  $@   >test12.txt
echo core 13
./smdtest -othercore  3 1  $@   >test13.txt
echo core 14
./smdtest -othercore  3 2  $@   >test14.txt
echo core 15
./smdtest -othercore  3 3  $@   >test15.txt
