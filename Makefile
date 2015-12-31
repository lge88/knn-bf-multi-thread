N_RECORDS=1000000
N_QUERIES=100
TOPK=100

INPUT_PATH=data/input
RECORDS=$(INPUT_PATH)/records_$(N_RECORDS).txt
QUERIES=$(INPUT_PATH)/queries_$(N_QUERIES).txt
INPUT_DATA=$(RECORDS) $(QUERIES)

OUTPUT_PATH=data/output
REF=$(OUTPUT_PATH)/ref_d$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
CPP_ST=$(OUTPUT_PATH)/cpp_st_d$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
CPP_MT=$(OUTPUT_PATH)/cpp_mt_d$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt

CC=g++
CPPFLAGS=-std=c++0x -O3 -pthread

default: check

$(RECORDS):
	./random.vectors.js $(N_RECORDS) 0 100 > $@

$(QUERIES):
	./random.vectors.js $(N_QUERIES) 0 100 > $@

input: $(INPUT_DATA)

knn.bf: knn.bf.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

knn.bf.mt: knn.bf.mt.cpp
	$(CC) $(CPPFLAGS) $^ -o $@

$(CPP_ST): $(INPUT_DATA) knn.bf
	time ./knn.bf $(RECORDS) $(QUERIES) $(TOPK) > $@

$(CPP_MT): $(INPUT_DATA) knn.bf.mt
	time ./knn.bf.mt $(RECORDS) $(QUERIES) $(TOPK) > $@

check: $(CPP_ST) $(CPP_MT)
	diff $(CPP_ST) $(CPP_MT)

clean-exe:
	rm -fr knn.bf knn.bf.mt

clean-data:
	rm -fr $(INPUT_PATH)/* $(OUTPUT_PATH)/*

clean: clean-exe clean-data

$(REF): $(INPUT_DATA) knn.bf.js
	time ./knn.bf.js $(RECORDS) $(QUERIES) $(TOPK) > $@

ref: $(REF)

st: $(CPP_ST)
mt: $(CPP_MT)

.PHONY: check clean ref mt st $(CPP_ST) $(CPP_MT)
