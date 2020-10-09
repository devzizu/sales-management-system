#!/bin/bash

> input_ma

for i in `seq 1 10000`
do
	echo "i A$i 1" >> input_ma
done

echo "ma script created"
