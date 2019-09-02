import java.util.Scanner;

public class Main {
	private static final String ERROR_MESSAGE = "Please type only numbers, idiot!\n";

	private static int[][] arr;
	private static int maxThreads;

	public static void main(String[] args) {
		Scanner scan = new Scanner(System.in);
		int rows = treatUserInput("Expected numbers of rows: ", ERROR_MESSAGE, scan);
		int columns = treatUserInput("Expected size of columns: ", ERROR_MESSAGE, scan);
		int threads = treatUserInput("Expected number of threads: ", ERROR_MESSAGE, scan);

		/*
		int processors = Runtime.getRuntime().availableProcessors();
		if (threads > processors) {
			LOGGER.info("The max numbers of processors in system (" + processors
					+ ") is letter then your number of threads!\n"
					+ "Changing the number of threads to max number of processors in this system...");
			threads = processors;
		}
		*/
		maxThreads = threads;

		System.out.println("\nGenerating random matrix...");

		arr = createRandom2dArray(rows, columns);
		int[][] clone2dArray = clone2DArray(arr);

		System.out.println("The random matrix generated is:\n");
		printMatrix();

		long start = 0;
		long end = 0;
		System.out.println("\nExecuting Bubble Sort Algorithm...");
		start = System.nanoTime();
		for (int i = 0; i < threads; i++) {
			Thread t = new Thread(createRunnableBubble(i));
			t.start();
			try {
				t.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		end = System.nanoTime();
		System.out.println("Time to proccess matrix using Bubble Sort: " + (end - start) / 1_000_000_000.0 + " seconds");
		System.out.println("\nThe ordened matrix is:\n");
		printMatrix();
		System.out.println();

		arr = clone2dArray;
		System.out.println("Restoring original matrix... Restored matrix is: \n");
		printMatrix();
		
		System.out.println("\nExecuting Merge Sort Algorithm...");
		start = System.nanoTime();
		for (int i = 0; i < threads; i++) {
			Thread t = new Thread(createRunnableMerge(i));
			t.start();
			try {
				t.join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		end = System.nanoTime();
		System.out.println("Time to proccess matrix using merge sort: " + (end - start) / 1_000_000_000.0 + " seconds");
		System.out.println("\nThe ordened matrix is:\n");
		printMatrix();
	}

	private static int[][] createRandom2dArray(int rows, int columns) {
		int[][] arr = new int[rows][columns];

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < columns; j++) {
				arr[i][j] = (int) (Math.random() * 1000 + 1);
			}
		}
		return arr;
	}

	private static Runnable createRunnableBubble(int row) {
		return () -> bubble(row);
	}
	
	private static Runnable createRunnableMerge(int row) {
		return () -> merge(row);
	}

	private static void bubble(int row) {
		boolean running = true;

		do {
			if (row >= arr.length) {
				running = false;
				continue;
			}
			// System.out.println("Thread " + Thread.currentThread().getId() + " processing row " + row);

			for (int j = 0; j < arr[0].length; j++) {
				for (int k = 0; k < arr[0].length - 1; k++) {
					if (arr[row][k] > arr[row][k + 1]) {
						int temp = arr[row][k];
						arr[row][k] = arr[row][k + 1];
						arr[row][k + 1] = temp;
					}
				}
			}
			row += maxThreads;
		} while (running);
	}

	private static void merge(int row) {
		boolean running = true;
		do {
			if (row >= arr.length) {
				running = false;
				continue;
			}
			// System.out.println("Thread " + Thread.currentThread().getId() + " processing row " + row);
			
			mergeSort(row, 0, arr[0].length - 1);
			row += maxThreads;
		} while (running);
	}

	private static void mergeSort(int row, int start, int end) {
		if(row >= arr.length)
			return;

		int mid = (start + end) / 2;

		if (start < end) {
			mergeSort(row, start, mid);
			mergeSort(row, mid + 1, end);
		}

		int i = 0;
		int first = start;
		int last = mid + 1;
		int[] tmp = new int[end - start + 1];

		while (first <= mid && last <= end) {
			tmp[i++] = arr[row][first] < arr[row][last] ? arr[row][first++] : arr[row][last++];
		}
		while (first <= mid) {
			tmp[i++] = arr[row][first++];
		}
		while (last <= end) {
			tmp[i++] = arr[row][last++];
		}
		i = 0;
		while (start <= end) {
			arr[row][start++] = tmp[i++];
		}
	}

	private static int treatUserInput(String inputMessage, String errorMessage, Scanner scan) {
		boolean finishInput = false;
		int input = 0;
		do {
			try {
				System.out.print(inputMessage);
				input = scan.nextInt();
				finishInput = true;
			} catch (Exception e) {
				System.out.println(errorMessage);
				scan.nextLine();
			}

		} while (!finishInput);
		return input;
	}

	private static void printMatrix() {
		for (int i = 0; i < arr.length; i++) {
			for (int j = 0; j < arr[0].length; j++) {
				System.out.print(arr[i][j] + " ");
			}
			System.out.println();
		}
	}

	private static int[][] clone2DArray(int[][] array) {
		int rows = array.length;

		int[][] newArray = array.clone();
		for (int row = 0; row < rows; row++) {
			newArray[row] = array[row].clone();
		}
		return newArray;
	}
}