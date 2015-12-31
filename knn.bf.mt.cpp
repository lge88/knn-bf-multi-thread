#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

vector<double> readNumbers(const string& filename) {
  ifstream ifs(filename);
  vector<double> nums;

  double num;
  while (ifs >> num) nums.push_back(num);
  return nums;
}

void writeNumbers(const vector<double>& nums, ostream& out) {
  for (auto num : nums) out << num << endl;
}

void printUsageAndExit() {
  cout << "Usage: knn.bf.mt <datasetFile> <queriesFile> <topK>" << endl
       << "  Foreach query in <queriesFile> print <topK> nearest neighbor" << endl
       << "  in <datasetFile> and print to stdout." << endl;
  exit(0);
}

struct Candidate {
  double record;
  double distance;
  size_t index;
};

struct CandidateComp {
  bool operator() (const Candidate& lhs, const Candidate& rhs) const {
    return lhs.distance < rhs.distance;
  }
};

namespace {

mutex mtx;
vector<bool> threadReady;
vector<thread> threads;
condition_variable cvThreadReady;
condition_variable cvDone;

struct Request {
  enum Type {
    SHUTDOWN = 0,
    SEARCH
  };
  Type type;

  double query;
  const vector<double>* records;
  size_t topK;

  set<Candidate, CandidateComp> candidates;
  Request() :
      type(SEARCH),
      query(0.0),
      records(NULL),
      topK(1) {}
};
Request req;

bool isThreadReady(size_t index) {
  return threadReady[index];
}

bool isDone() {
  for (size_t i = 0, len = threads.size(); i < len; ++i) {
    if (threadReady[i]) return false;
  }
  return true;
}

} // namespace

void doKnnBFSearch(
    size_t threadIndex,
    size_t start,
    size_t end) {
  double query = req.query;
  const vector<double>& records = *(req.records);
  size_t topK = req.topK;

  set<Candidate, CandidateComp> candidates;
  for (size_t index = start; index <= end; ++index) {
    const double record = records[index];
    const double distance = abs(record - query);
    Candidate item;
    item.record = record;
    item.distance = distance;
    item.index = index;

    candidates.insert(item);
    if (candidates.size() > topK) {
      set<Candidate, CandidateComp>::iterator it = candidates.end();
      --it;
      candidates.erase(it);
    }
  }

  {
    lock_guard<mutex> lk(mtx);
    for (auto candidate : candidates) {
      req.candidates.insert(candidate);
      if (req.candidates.size() > topK) {
        set<Candidate, CandidateComp>::iterator it = req.candidates.end();
        --it;
        req.candidates.erase(it);
      }
    }
    threadReady[threadIndex] = false;
  }
  cvDone.notify_all();
}

void knnBFThreadSearch(
    size_t threadIndex,
    size_t start,
    size_t end) {
  while (true) {
    unique_lock<mutex> lk(mtx);
    cvThreadReady.wait(
        lk, bind(&isThreadReady, threadIndex));
    lk.unlock();

    if (req.type == Request::Type::SHUTDOWN) {
      return;
    } else if (req.type == Request::Type::SEARCH) {
      doKnnBFSearch(threadIndex, start, end);
    }
  }
}

void threadInit(size_t nThreads, size_t nRecords) {
  threadReady = vector<bool>(nThreads, false);
  req.type = Request::Type::SEARCH;

  size_t base = nRecords / nThreads;
  size_t remains = nRecords % nThreads;
  size_t start = 0;

  for (size_t i = 0; i < nThreads; ++i) {
    size_t end = start + base - 1;
    if (remains > 0) {
      --remains;
      ++end;
    }

    threads.push_back(
        thread(&knnBFThreadSearch, i, start, end));

    start = end + 1;
  }
}

void threadDestroy() {
  {
    lock_guard<mutex> lk(mtx);
    req.type = Request::Type::SHUTDOWN;
    fill(threadReady.begin(), threadReady.end(), true);
  }
  cvThreadReady.notify_all();
  for (size_t i = 0, len = threads.size(); i < len; ++i) threads[i].join();
}

vector<Candidate> knnBFSearch(
    double query,
    const vector<double>& records,
    size_t topK) {
  {
    lock_guard<mutex> lk(mtx);

    req.query = query;
    req.records = &records;
    req.topK = topK;
    req.type = Request::Type::SEARCH;

    req.candidates.clear();
    fill(threadReady.begin(), threadReady.end(), true);
  }

  cvThreadReady.notify_all();

  {
    unique_lock<mutex> lk(mtx);
    cvDone.wait(lk, &isDone);
  }
  return vector<Candidate>(req.candidates.begin(), req.candidates.end());
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printUsageAndExit();
  }

  const string datasetFile(argv[1]);
  const string queriesFile(argv[2]);
  const size_t topK = atoi(argv[3]);

  const vector<double> records = readNumbers(datasetFile);
  const vector<double> queries = readNumbers(queriesFile);

  size_t count = 0;

  cout.precision(6);
  cout.setf(ios::fixed, ios::floatfield);

  size_t nThreads = 8;
  threadInit(nThreads, records.size());
  for (auto query : queries) {
    ++count;
    cerr << "processing " << count << ": " << query << endl;

    const vector<Candidate> results = knnBFSearch(query, records, topK);
    cout << query << ":";
    for (size_t i = 0, len = results.size(); i < len; ++i) {
      if (i > 0) cout << ",";
      cout << results[i].index;
    }
    cout << endl;
  }

  threadDestroy();

  return 0;
}
