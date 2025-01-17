package com.ufsc.ine5645.hadoop.reducers;

import java.io.IOException;

import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

import com.ufsc.ine5645.hadoop.DoubleFormatHelper;

public class ParliamentarySpendingReducer extends Reducer<Text, DoubleWritable, Text, Text>  {
	
	@Override
	public void reduce(Text key, Iterable<DoubleWritable> values, Context context)
			throws IOException, InterruptedException {
		double averageMax = Double.MIN_VALUE;
		double averageMin = Double.MAX_VALUE;
		double averageSpend = 0.0;
		double total = 0.0;
		
		int occurences = 0;
		for (DoubleWritable value : values) {
			averageMax = Math.max(averageMax, value.get());
			averageMin = Math.min(averageMin, value.get());
			total += value.get();
			occurences++;
		}
		
		averageSpend = total / occurences;
		
//		System.out.println("Average Maximum - " + averageMax);
//		System.out.println("Average Minimum - " + averageMin);
//		System.out.println("Average Spend - " + averageSpend);
//		System.out.println("Total - " + total);

		context.write(key, new Text("Total - " + DoubleFormatHelper.parseToString(total)));
		context.write(key, new Text("Max - " + DoubleFormatHelper.parseToString(averageMax)));
		context.write(key, new Text("Min - " + DoubleFormatHelper.parseToString(averageMin)));
		context.write(key, new Text("Spend - " + DoubleFormatHelper.parseToString(averageSpend)));
	}
}