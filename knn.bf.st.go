package main

import (
  "os"
	"fmt"
  "bufio"
  "math"
  "strconv"
	"github.com/petar/GoLLRB/llrb"
)

type Candidate struct {
	record, distance float64
  index uint64
}

func (self Candidate) Less(other llrb.Item) bool {
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
  for _, num := range nums {
    // fmt.Fprintf(writer, "%d\t%e\n", i, num)
    fmt.Fprintln(writer, num)
    // writer.WriteString(fmt.Sprintf("%d\t%e\n", i, num))
  }
}

func writeResultLine(candidates []Candidate, writer *bufio.Writer) {
  for i, candidate := range candidates {
    if i > 0 { fmt.Fprint(writer, " ") }
    fmt.Fprint(writer, candidate.index)
  }
}

func knnBFSearch(
  query float64,
  records []float64,
  topK uint8) []Candidate {
  s := createCandidateSet()
  for index, record := range records {
    distance := math.Abs(record - query)
    s.Insert(Candidate{record, distance, uint64(index)})
    if s.Len() > topK { s.RemoveLast() }
  }
  return s.ToSlice()
}

func main() {
  queries := readNumbers("./data/input/queries_5.txt")
  records := readNumbers("./data/input/records_100.txt")
  var topK uint8 = 2

  for _, query := range queries {
    candidates := knnBFSearch(query, records, topK)

    fmt.Print(query)
    fmt.Print(" ")
    for i, candidate := range candidates {
      if i > 0 { fmt.Print(" ") }
      // fmt.Print(candidate.index, candidate.record, candidate.distance)
      fmt.Print(candidate.index)
    }
    fmt.Println()
  }
}
