#!/bin/bash

for i in "memory_results.txt" "process_results.txt"; do
	for j in {1..8}; do
		calc=0.0
		for k in 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000 2147483647; do
			total=`cat "$i" | grep 'Total parallel processing time using '$j' processes operating on a array of size '$k':' | wc -l`	
			for time in $(cat "$i" | grep 'Total parallel processing time using '$j' processes operating on a array of size '$k':' | cut -d ":" -f 2 | cut -d " " -f 2); do
				calc=$(echo "$calc + $time" | bc)
			done				
			result=`echo "scale=10; $calc/$total" | bc`
			echo -e "Parallel data: Total $total\nCalc $calc\nThe result is $result"
			echo "$result" >> "$i"_"$j"_"$k"_parallel

			calc=0.0	
			total=`cat "$i" | grep 'Total sequential processing time using '$j' buckets operating on a array of size '$k':' | wc -l`
			for time in $(cat "$i" | grep 'Total sequential processing time using '$j' buckets operating on a array of size '$k':' | cut -d ":" -f 2 | cut -d " " -f 2); do
				calc=$(echo "$time+$calc" | bc)
			done
			result=`echo "scale=10; $calc/$total" | bc`
			echo -e "Sequential data: Total $total\nCalc $calc\nThe result is $result"
			echo "$result" >> "$i"_"$j"_"$k"_sequential
			
			calc=0.0
			total=`cat "$i" | grep 'In an environment with '$j' processes operating over a array of size '$k',' | wc -l`
			for speedup in $(cat "$i" | grep 'In an environment with '$j' processes operating over a array of size '$k',' | cut -d ":" -f 2); do
				calc=$(echo "scale=5; $speedup+$calc" | bc)
			done
			result=`echo "scale=10; $calc/$total" | bc`
			echo -e "Speedup data: Total $total\nCalc $calc\nThe result is $result"
			echo "$result" >> "$i"_"$j"_"$k"_speedup
		done
	done
done
