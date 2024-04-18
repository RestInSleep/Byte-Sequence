#!/bin/bash

rm large.txt
for i in {1..100000}
do
  echo "Hello World $i" >> large.txt
done