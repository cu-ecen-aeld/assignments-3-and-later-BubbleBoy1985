#!/bin/bash

echo -n >result.txt
tail -f result.txt | nc localhost 9000 

echo -n -e "1111" >> result.txt
echo -n -e "2222" >> result.txt
echo -n -e "333333\n" >> result.txt

echo -n -e "3323423444\n" >> result.txt

echo -n -e "12222" >> result.txt
echo -n -e "232222" >> result.txt
echo -n -e "442222\n" >> result.txt
# ps -ef | grep -w nc | grep -v grep | awk '{print $2}' | xargs kill
# echo -n -e 
# echo -n "123" >> result.txt
# echo -n
