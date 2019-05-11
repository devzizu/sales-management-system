> input_100k_add_rm

for i in `seq 1 50000`
do
	echo "1 1" >> input_100k_add_rm
done

for i in `seq 1 50000`
do
	echo "1 -1" >> input_100k_add_rm
done
