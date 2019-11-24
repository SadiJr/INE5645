package com.ufsc.ine5645.hadoop;

import java.io.IOException;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.jobcontrol.ControlledJob;
import org.apache.hadoop.mapreduce.lib.jobcontrol.JobControl;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import com.ufsc.ine5645.hadoop.mappers.ParliamentarySpendingMapper;
import com.ufsc.ine5645.hadoop.mappers.PartyMapper;
import com.ufsc.ine5645.hadoop.mappers.ProductParliamentarySpendingMapper;
import com.ufsc.ine5645.hadoop.mappers.SalesMapper;
import com.ufsc.ine5645.hadoop.reducers.ParliamentarySpendingReducer;
import com.ufsc.ine5645.hadoop.reducers.PartyReducer;
import com.ufsc.ine5645.hadoop.reducers.ProductParliamentarySpendingReducer;
import com.ufsc.ine5645.hadoop.reducers.SalesReducer;

public class App {
	public static void main(String args[]) {
		try {

			if (args.length != 2) {
				System.err.println("Usage: jar <input path> <output path>");
				System.exit(-1);
			}

//			JobControl jobControl = new JobControl("ctrl");
			Path output = new Path(args[1]);
			Configuration config1 = new Configuration();

			FileSystem hdfs = FileSystem.get(config1);
			if (hdfs.exists(output))
				hdfs.delete(output, true);

			// Job1
			System.out.println("Starting job1");
			Job job = Job.getInstance(config1);
			job.setJarByClass(App.class);
			job.setJobName("employees-cost");

			FileInputFormat.addInputPath(job, new Path(args[0]));
			FileOutputFormat.setOutputPath(job, new Path(args[1], "employees-cost"));

			job.setMapperClass(ParliamentarySpendingMapper.class);
			job.setReducerClass(ParliamentarySpendingReducer.class);
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(DoubleWritable.class);
			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(DoubleWritable.class);
			job.setNumReduceTasks(1);
			job.waitForCompletion(true);

			// Job2
			System.out.println("Starting job2");
			job = Job.getInstance(config1);
			job.setJarByClass(App.class);
			job.setJobName("employee-products");

			FileInputFormat.addInputPath(job, new Path(args[0]));
			FileOutputFormat.setOutputPath(job, new Path(args[1], "employees-products"));

			job.setMapperClass(ProductParliamentarySpendingMapper.class);
			job.setReducerClass(ProductParliamentarySpendingReducer.class);
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(DoubleWritable.class);
			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(DoubleWritable.class);
			job.setNumReduceTasks(1);
			job.waitForCompletion(true);

			// Job3
			System.out.println("Starting job3");
			job = Job.getInstance(config1);
			job.setJarByClass(App.class);
			job.setJobName("party");

			FileInputFormat.addInputPath(job, new Path(args[0]));
			FileOutputFormat.setOutputPath(job, new Path(args[1], "party"));

			job.setMapperClass(PartyMapper.class);
			job.setReducerClass(PartyReducer.class);
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(DoubleWritable.class);
			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(DoubleWritable.class);
			job.setNumReduceTasks(1);
			job.waitForCompletion(true);

			// Job4
			System.out.println("Starting job4");
			job = Job.getInstance(config1);
			job.setJarByClass(App.class);
			job.setJobName("products");

			FileInputFormat.addInputPath(job, new Path(args[0]));
			FileOutputFormat.setOutputPath(job, new Path(args[1], "products"));

			job.setMapperClass(SalesMapper.class);
			job.setReducerClass(SalesReducer.class);
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(DoubleWritable.class);
			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(DoubleWritable.class);
			job.setNumReduceTasks(1);
			job.waitForCompletion(true);

			// Job5
			System.out.println("Starting job5");
			job = Job.getInstance(config1);
			job.setJarByClass(App.class);
			job.setJobName("per-period");

			FileInputFormat.addInputPath(job, new Path(args[0]));
			FileOutputFormat.setOutputPath(job, new Path(args[1], "per-period"));

			job.setMapperClass(SalesMapper.class);
			job.setReducerClass(SalesReducer.class);
			job.setOutputKeyClass(Text.class);
			job.setOutputValueClass(DoubleWritable.class);
			job.setInputFormatClass(TextInputFormat.class);
			job.setOutputFormatClass(TextOutputFormat.class);
			job.setMapOutputKeyClass(Text.class);
			job.setMapOutputValueClass(DoubleWritable.class);
			job.setNumReduceTasks(1);
			job.waitForCompletion(true);

//			job1(jobControl, job, config1, args);
//			job2(jobControl, args);
//			job3(jobControl, args);
//			job4(jobControl, args);
//			job5(jobControl, args);

//			new Thread(jobControl).start();
//			while(!jobControl.allFinished()) {
//			
//			}
//			jobControl.run();
			System.exit(0);
		} catch (Exception e) {
			System.err.println(e.getMessage());
		}
		System.exit(-1);
	}

	private static void job5(JobControl jobControl, String[] args) throws IOException {
		Configuration config = new Configuration();

		Job job = Job.getInstance(config);
		job.setJarByClass(App.class);
		job.setJobName("per-period");

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1], "per-period"));

		job.setMapperClass(SalesMapper.class);
		job.setReducerClass(SalesReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(DoubleWritable.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(DoubleWritable.class);
		job.setNumReduceTasks(2);

		ControlledJob controlledJob = new ControlledJob(config);
		controlledJob.setJob(job);
		jobControl.addJob(controlledJob);
	}

	private static void job4(JobControl jobControl, String[] args) throws IOException {
		Configuration config = new Configuration();

		Job job = Job.getInstance(config);
		job.setJarByClass(App.class);
		job.setJobName("products");

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1], "products"));

		job.setMapperClass(SalesMapper.class);
		job.setReducerClass(SalesReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(DoubleWritable.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(DoubleWritable.class);
		job.setNumReduceTasks(2);

		ControlledJob controlledJob = new ControlledJob(config);
		controlledJob.setJob(job);
		jobControl.addJob(controlledJob);
	}

	private static void job3(JobControl jobControl, String[] args) throws IOException {
		Configuration config = new Configuration();

		Job job = Job.getInstance(config);
		job.setJarByClass(App.class);
		job.setJobName("party");

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1], "party"));

		job.setMapperClass(PartyMapper.class);
		job.setReducerClass(PartyReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(DoubleWritable.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(DoubleWritable.class);
		job.setNumReduceTasks(2);

		ControlledJob controlledJob = new ControlledJob(config);
		controlledJob.setJob(job);
		jobControl.addJob(controlledJob);
	}

	private static void job2(JobControl jobControl, String[] args) throws IOException {
		Configuration config = new Configuration();

		Job job = Job.getInstance(config);
		job.setJarByClass(App.class);
		job.setJobName("employee-products");

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1], "employees-products"));

		job.setMapperClass(ProductParliamentarySpendingMapper.class);
		job.setReducerClass(ProductParliamentarySpendingReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(DoubleWritable.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(DoubleWritable.class);
		job.setNumReduceTasks(2);

		ControlledJob controlledJob = new ControlledJob(config);
		controlledJob.setJob(job);
		jobControl.addJob(controlledJob);
	}

	private static void job1(JobControl jobControl, Job job, Configuration config1, String[] args) throws IOException {
		job.setJarByClass(App.class);
		job.setJobName("employees-cost");

		FileInputFormat.addInputPath(job, new Path(args[0]));
		FileOutputFormat.setOutputPath(job, new Path(args[1], "employees-cost"));

		job.setMapperClass(ParliamentarySpendingMapper.class);
		job.setReducerClass(ParliamentarySpendingReducer.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(DoubleWritable.class);
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(DoubleWritable.class);
		job.setNumReduceTasks(2);

		ControlledJob controlledJob = new ControlledJob(config1);
		controlledJob.setJob(job);
		jobControl.addJob(controlledJob);
	}
}