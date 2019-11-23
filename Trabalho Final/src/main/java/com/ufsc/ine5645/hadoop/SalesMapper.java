package com.ufsc.ine5645.hadoop;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class SalesMapper extends Mapper<LongWritable, Text, Text, DoubleWritable> {
			
	@Override
	public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
		String[] lines = value.toString().split(",");
		String product = lines[19];
		Double cost = Double.valueOf(lines[27]);
//		System.out.println("Key is " + product + " and values are: Cost - " + cost);
		context.write(new Text(product), new DoubleWritable(cost));
	}
}