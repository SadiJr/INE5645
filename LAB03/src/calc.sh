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
            if [ "$i" == "memory_results.txt" ]; then
                echo "The average execution with $j parallel processes operating on a $k position array and having memory management is: $result"
            else
                echo "The average execution with $j parallel processes operating on a $k position array and not having memory management is: $result"
            fi

            calc=0.0	
            total=`cat "$i" | grep 'Total sequential processing time using '$j' buckets operating on a array of size '$k':' | wc -l`
            for time in $(cat "$i" | grep 'Total sequential processing time using '$j' buckets operating on a array of size '$k':' | cut -d ":" -f 2 | cut -d " " -f 2); do
                calc=$(echo "$time+$calc" | bc)
            done
            result=`echo "scale=10; $calc/$total" | bc`
            if [ "$i" == "memory_results.txt" ]; then
                echo "The average execution with $j sequential processes operating on a $k position array and having memory management is: $result"
            else
                echo "The average execution with $j sequential processes operating on a $k position array and not having memory management is: $result"
            fi

            calc=0.0
            total=`cat "$i" | grep 'In an environment with '$j' processes operating over a array of size '$k',' | wc -l`
            for speedup in $(cat "$i" | grep 'In an environment with '$j' processes operating over a array of size '$k',' | cut -d ":" -f 2); do
                calc=$(echo "scale=5; $speedup+$calc" | bc)
            done
            result=`echo "scale=10; $calc/$total" | bc`
            if [ "$i" == "memory_results.txt" ]; then
                echo "The average speedup having $j processes or buckets operating over a $k position array and having memory management was: $result"
            else
                echo "The average speedup having $j processes or buckets operating over a $k position array and not having memory management is: $result"
            fi
            echo -e "\n\n"
        done
    done
done
