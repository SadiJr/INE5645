#!/bin/bash

if [ ! -f "memory" ]; then
       	mpicc -o memory mpi_memory_priority.c
fi

if [ ! -d "process" ]; then
	mpicc -o process mpi_process_priority.c
fi

for h in {1..10}; do
	for i in "memory" "process"; do
		for j in {1..8}; do
			for k in 10 100 1000 10000 100000 1000000 10000000 100000000 1000000000 2147483647; do
				mpirun --use-hwthread-cpus -np "$j" "$i" "$k" &>> "$i"_results.txt 
			done
		done
	done
done
