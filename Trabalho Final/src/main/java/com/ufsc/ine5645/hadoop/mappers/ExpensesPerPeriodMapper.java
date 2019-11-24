package com.ufsc.ine5645.hadoop.mappers;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class ExpensesPerPeriodMapper extends Mapper<LongWritable, Text, Text, DoubleWritable> {
	@Override
	public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
		String[] lines = value.toString().split(",");
		String parliamentary = lines[17];
		String identifier = lines[18];
		String year = lines[8];
		String month = lines[11];
		
		Double cost = Double.valueOf(lines[27]);
		
		String k = identifier + " - " + parliamentary + " - " + year + "/" + month;
//		System.out.println("Key is " + k + " and values are: Cost - " + cost);
		
		context.write(new Text(k), new DoubleWritable(cost));
	}
}
