package com.ufsc.ine5645.hadoop;

import java.text.DecimalFormat;

public abstract class DoubleFormatHelper {
	private static DecimalFormat df = new DecimalFormat("0.00");
	
	public static double parseToDouble(double value) {
		return Double.valueOf(df.format(value));
	}
	
	public static String parseToString(double value) {
		return df.format(value);
	}
}