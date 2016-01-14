N_RECORDS=100000
N_QUERIES=100
TOPK=100
LOW=0
HIGH=100

MAKEFILE_PATH=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))
INPUT_PATH=$(MAKEFILE_PATH)input
RECORDS=$(INPUT_PATH)/records_$(N_RECORDS).txt
QUERIES=$(INPUT_PATH)/queries_$(N_QUERIES).txt

JS_ST=js_knn_bf_st
CPP_ST=cpp_knn_bf_st
CPP_MT=cpp_knn_bf_mt
GO_ST=go_knn_bf_st
GO_MT=go_knn_bf_mt

OUTPUT_PATH=$(MAKEFILE_PATH)output
OUT_JS_ST=$(OUTPUT_PATH)/$(JS_ST)_r$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
OUT_CPP_ST=$(OUTPUT_PATH)/$(CPP_ST)_r$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
OUT_CPP_MT=$(OUTPUT_PATH)/$(CPP_MT)_r$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
OUT_GO_ST=$(OUTPUT_PATH)/$(GO_ST)_r$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt
OUT_GO_MT=$(OUTPUT_PATH)/$(GO_MT)_r$(N_RECORDS)_q$(N_QUERIES)_k$(TOPK).txt

EXE_PATH=$(MAKEFILE_PATH)exes
EXE_JS_ST=$(EXE_PATH)/$(JS_ST)
EXE_CPP_ST=$(EXE_PATH)/$(CPP_ST)
EXE_CPP_MT=$(EXE_PATH)/$(CPP_MT)
EXE_GO_ST=$(EXE_PATH)/$(GO_ST)
EXE_GO_MT=$(EXE_PATH)/$(GO_MT)

COPY=cp
CHMOD=chmod
RANDOM_VECTOR=$(MAKEFILE_PATH)bin/random_vectors
CC=g++
CPPFLAGS=-std=c++0x -O3 -pthread
GO=go
GOFLAGS=

default: exes

debug-path:
	echo $(MAKEFILE_PATH)

$(RECORDS):
	$(RANDOM_VECTOR) $(N_RECORDS) $(LOW) $(HIGH) > $@

$(QUERIES):
	$(RANDOM_VECTOR) $(N_QUERIES) $(LOW) $(HIGH) > $@

$(EXE_JS_ST): $(JS_ST).js
	$(COPY) $^ $@
	$(CHMOD) +x $@

$(EXE_CPP_ST): $(CPP_ST).cpp
	$(CC) $(CPPFLAGS) -o $@ $^

$(EXE_CPP_MT): $(CPP_MT).cpp
	$(CC) $(CPPFLAGS) -o $@ $^

$(EXE_GO_ST): $(GO_ST).go
	$(GO) build $(GOFLAGS) -o $@ $^

$(EXE_GO_MT): $(GO_MT).go
	$(GO) build $(GOFLAGS) -o $@ $^

$(OUT_JS_ST): $(INPUT_DATA) $(EXE_JS_ST)
	time $(EXE_JS_ST) $(RECORDS) $(QUERIES) $(TOPK) > $@

$(OUT_CPP_ST): $(INPUT_DATA) $(EXE_CPP_ST)
	time $(EXE_CPP_ST) $(RECORDS) $(QUERIES) $(TOPK) > $@

$(OUT_CPP_MT): $(INPUT_DATA) $(EXE_CPP_MT)
	time $(EXE_CPP_MT) $(RECORDS) $(QUERIES) $(TOPK) > $@

$(OUT_GO_ST): $(INPUT_DATA) $(EXE_GO_ST)
	time $(EXE_GO_ST) $(RECORDS) $(QUERIES) $(TOPK) > $@

$(OUT_GO_MT): $(INPUT_DATA) $(EXE_GO_MT)
	time $(EXE_GO_MT) $(RECORDS) $(QUERIES) $(TOPK) > $@

INPUT_DATA+=$(RECORDS)
INPUT_DATA+=$(QUERIES)
input: $(INPUT_DATA)

EXES+=$(EXE_JS_ST)
EXES+=$(EXE_CPP_ST)
EXES+=$(EXE_CPP_MT)
EXES+=$(EXE_GO_ST)
EXES+=$(EXE_GO_MT)
exes: $(EXES)

OUTPUT_DATA+=$(OUT_JS_ST)
OUTPUT_DATA+=$(OUT_CPP_ST)
OUTPUT_DATA+=$(OUT_CPP_MT)
OUTPUT_DATA+=$(OUT_GO_ST)
OUTPUT_DATA+=$(OUT_GO_MT)
output: $(OUTPUT_DATA)
out-js-st: $(OUT_JS_ST)
out-cpp-st: $(OUT_CPP_ST)
out-cpp-mt: $(OUT_CPP_MT)
out-go-st: $(OUT_GO_ST)
out-go-mt: $(OUT_GO_MT)

check-js-st: $(OUT_JS_ST) $(OUT_CPP_ST)
	diff $(OUT_JS_ST) $(OUT_CPP_ST)

check-cpp-mt: $(OUT_CPP_MT) $(OUT_CPP_ST)
	diff $(OUT_CPP_MT) $(OUT_CPP_ST)

check-go-st: $(OUT_GO_ST) $(OUT_CPP_ST)
	diff $(OUT_GO_ST) $(OUT_CPP_ST)

check-go-mt: $(OUT_GO_ST) $(OUT_CPP_MT)
	diff $(OUT_GO_MT) $(OUT_CPP_ST)

check-all: check-js-st check-cpp-mt check-go-st check-go-mt

clean-exes:
	rm -fr $(EXE_PATH)/*

clean-input:
	rm -fr $(INPUT_PATH)/*

clean-output:
	rm -fr $(OUTPUT_PATH)/*

clean-data: clean-input clean-output

clean: clean-exes clean-data

.PHONY: input exes output check-all clean
