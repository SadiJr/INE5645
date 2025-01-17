package com.ufsc.ine5645.hadoop.reducers;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class StateReducer extends Reducer<Text, DoubleWritable, Text, Text> {
	
	@Override
	public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
			throws IOException, InterruptedException {
		double total = 0.0;
		
		for (DoubleWritable value : values) {
			total += value.get();
		}


		context.write(key, new Text("Total per state - " + total));
	}
}