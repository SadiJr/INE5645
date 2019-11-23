package com.ufsc.ine5645.hadoop;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class ProductParliamentarySpendingReducer extends Reducer<Text, DoubleWritable, Text, Text> {
	
	@Override
	public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
			throws IOException, InterruptedException {
		double average = 0.0;
		double total = 0.0;
		int occurrences = 0;
		
		for (DoubleWritable value : values) {
			total += value.get();
			occurrences++;
		}
		
		average = total / occurrences;
//		System.out.println("Total of " + key + " cost is " + total + " and average is " + average);

		context.write(key, new Text("Total - " + total));
		context.write(key, new Text("Average - " + average));
	}

}