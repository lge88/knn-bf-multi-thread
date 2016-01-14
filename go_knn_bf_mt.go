package main

import (
	"os"
	"fmt"
	"bufio"
	"math"
	"strconv"
  "sync"
	"github.com/petar/GoLLRB/llrb"
)

type Candidate struct {
	record, distance float64
	index uint64
}

func (self Candidate) Less(other llrb.Item) bool {
	if (self.distance == other.(Candidate).distance) {
		return self.index < other.(Candidate).index;
	}
	return self.distance < other.(Candidate).distance
}

type CandidateSet struct {
	rbTree *llrb.LLRB
}

func createCandidateSet() *CandidateSet {
	return &CandidateSet{llrb.New()}
}

func (s *CandidateSet) Len() uint8 {
	return uint8(s.rbTree.Len())
}

func (s *CandidateSet) Insert(c Candidate) {
	s.rbTree.ReplaceOrInsert(c)
}

func (s *CandidateSet) RemoveLast() Candidate {
	return s.rbTree.DeleteMax().(Candidate)
}

func (s * CandidateSet) ToSlice() []Candidate {
	candidates := []Candidate{}
	t := s.rbTree
	first := t.Min()
	t.AscendGreaterOrEqual(first, func(item llrb.Item) bool {
		candidate := item.(Candidate)
		candidates = append(candidates, candidate)
		return true
	})
	return candidates
}

func (s * CandidateSet) Print() {
	vec := s.ToSlice()
	for _, candidate := range vec {
		fmt.Println(candidate.record, "\t", candidate.distance)
	}
}

func readNumbers(filename string) []float64 {
	src, _ := os.Open(filename)
	defer src.Close()

	nums := []float64{}
	scanner := bufio.NewScanner(src)
	for scanner.Scan() {
		num, _ := strconv.ParseFloat(scanner.Text(), 64)
		nums = append(nums, num)
	}
	return nums
}

func writeNumbers(nums []float64, writer *bufio.Writer) {
	for _, num := range nums { fmt.Fprintln(writer, num) }
}

func knnBFSearchRoutine(
	query float64,
	records []float64,
	topK uint8,
  startIndex uint64,
  endIndex uint64) []Candidate {
	s := createCandidateSet()
	for i, record := range records[startIndex:endIndex+1] {
		distance := math.Abs(record - query)
		s.Insert(Candidate{record, distance, startIndex + uint64(i)})
		if s.Len() > topK { s.RemoveLast() }
	}
  candidates := s.ToSlice();
  return candidates
}

func knnBFSearch(
	query float64,
	records []float64,
	topK uint8) []Candidate {
  numGoRoutines := 8

  var wg sync.WaitGroup
  wg.Add(numGoRoutines)

  numRecords := len(records)
  base := numRecords / numGoRoutines
  remains := numRecords % numGoRoutines
  resultChan := make(chan []Candidate)
  set := createCandidateSet()
  startIndex := 0
  for i := 1; i <= numGoRoutines; i++ {
    endIndex := startIndex + base - 1
    if remains > 0 {
      endIndex += 1
      remains -= 1
    }

    go func(s int, e int) {
      defer wg.Done()
      candidates := knnBFSearchRoutine(
        query,
        records,
        topK,
        uint64(s),
        uint64(e))
      resultChan <- candidates
    }(startIndex, endIndex)

    startIndex = endIndex + 1
  }

  var wg2 sync.WaitGroup
  wg2.Add(1)
  go func() {
    defer wg2.Done()
    for candidates := range resultChan {
      for _, candidate := range candidates {
        set.Insert(candidate)
        if set.Len() > topK { set.RemoveLast() }
      }
    }
  }()

  wg.Wait()
  close(resultChan);

  wg2.Wait()

  return set.ToSlice()
}

func printUsageAndExit() {
	fmt.Println("Usage: go_knn_bf_mt <datasetFile> <queriesFile> <topK>")
	fmt.Println("	 Foreach query in <queriesFile> print <topK> nearest neighbor")
	fmt.Println("	 in <datasetFile> and print to stdout.")
	os.Exit(1);
}

func main() {
	argc := len(os.Args)
	if argc < 4 {
		printUsageAndExit()
	}

	records := readNumbers(os.Args[1])
	queries := readNumbers(os.Args[2])
	topK, _ := strconv.ParseUint(os.Args[3], 10, 8)

	var count uint64 = 0
	for _, query := range queries {
		count += 1
		fmt.Fprintln(os.Stderr, "processing", count, ":", query)

		candidates := knnBFSearch(query, records, uint8(topK))

		fmt.Printf("%.6f:", query)
		for i, candidate := range candidates {
			if i > 0 { fmt.Print(",") }
			fmt.Print(candidate.index)
		}
		fmt.Println()
	}
}
