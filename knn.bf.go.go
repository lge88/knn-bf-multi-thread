package main

import "fmt"
import "os"
import "bufio"
import "strconv"

// single routine search
func readNumbers(filename string) []float64 {
	file, _ := os.Open(filename)
	defer file.Close()

	scanner := bufio.NewScanner(file)
	nums := make([]float64, 0)

	for scanner.Scan() {
		line := scanner.Text()
		num, _ := strconv.ParseFloat(line, 64)
		nums = append(nums, num)
	}
	return nums
}

func printUsageAndExit() {
	fmt.Println("Usage node knn.bf.js <datasetFile> <queriesFile> <topK>")
	fmt.Println("  Foreach query in <queriesFile> print <topK> nearest neighbor")
	fmt.Println("  in <datasetFile> and print to stdout.")
	os.Exit(1);
}

struct Candidate {
	record, distance float64
	index uint64
}

func knnBFSearchThread() {

}

// knnBFSearch ...
func knnBFSearch(query float64) []Candidate {

}

func main() {
	args := os.Args[1:]
	if len(args) < 3 {
		printUsageAndExit();
	}

	// recordsFile := args[0]
	queriesFile := args[1]
	// topK, _ := strconv.ParseInt(args[2], 10, 64)

	// records := readNumbers(recordsFile)
	queries := readNumbers(queriesFile)

	count := 0
	for _, query := range queries {
		count += 1
		fmt.Fprintln(os.Stderr, "processing ", count, ": ", query)


		// fmt.Println("count: ", count, i, query);
	}

	// fmt.Println("records: ", records)
}
