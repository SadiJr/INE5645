package com.ufsc.ine5645.hadoop;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class PartyReducer extends Reducer<Text, DoubleWritable, Text, Text> {
	
	@Override
	public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
			throws IOException, InterruptedException {
		double total = 0.0;
		
		for (DoubleWritable value : values) {
			total += value.get();
		}
		
//		System.out.println("Total in year " + key + " is " + total);

		context.write(key, new Text("Total - " + total));
	}
}