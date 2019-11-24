package com.ufsc.ine5645.hadoop.mappers;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class ProductParliamentarySpendingMapper extends Mapper<LongWritable, Text, Text, DoubleWritable> {

	@Override
	public void map(LongWritable key, Text value, Context context) throws IOException, InterruptedException {
		String[] lines = value.toString().split(",");
		String parliamentary = lines[17];
		String identifier = lines[18];
		String product = lines[19];
		
		String keyValue = identifier + " - " + parliamentary + " - " + product;
		Double cost = Double.valueOf(lines[27]);
		
//		System.out.println("Key is " + keyValue + " and values are: Cost - " + cost);
		context.write(new Text(keyValue), new DoubleWritable(cost));
	}

}
