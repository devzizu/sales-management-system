> input_10k_add_rm

for i in `seq 1 10000`
do
	echo "1 1" >> input_10k_add_rm
done

for i in `seq 1 10000`
do
	echo "1 -1" >> input_10k_add_rm
done
