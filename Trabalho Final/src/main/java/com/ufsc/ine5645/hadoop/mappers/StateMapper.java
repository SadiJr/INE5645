package com.ufsc.ine5645.hadoop.mappers;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class StateMapper extends Mapper<LongWritable, Text, Text, DoubleWritable> {
	@Override
	public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
		String[] lines = value.toString().split(",");
		String state = lines[16];
		String year = lines[8];
		
		Double cost = Double.valueOf(lines[27]);
		String k = state + " - " + year;
//		System.out.println("Key is " + k + " and values are: Cost - " + cost);
		context.write(new Text(k), new DoubleWritable(cost));
	}
}