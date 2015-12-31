#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <cmath>

using namespace std;

vector<float> readNumbers(const string& filename) {
  ifstream ifs(filename);
  vector<float> nums;

  float num;
  while (ifs >> num) nums.push_back(num);
  return nums;
}

void printUsageAndExit() {
  cout << "Usage node knn.bf.js <datasetFile> <queriesFile> <topK>" << endl
       << "  Foreach query in <queriesFile> print <topK> nearest neighbor" << endl
       << "  in <datasetFile> and print to stdout." << endl;
  exit(0);
}

struct Candidate {
  float record;
  float distance;
  size_t index;
};

struct CandidateComp {
  bool operator() (const Candidate& lhs, const Candidate& rhs) const {
    return lhs.distance < rhs.distance;
  }
};

vector<Candidate> knnBFSearch(
    float query,
    const vector<float>& records,
    size_t topK) {
  set<Candidate, CandidateComp> res;

  for (size_t index = 0, len = records.size(); index < len; ++index) {
    const float record = records[index];
    const float distance = abs(record - query);
    Candidate item;
    item.record = record;
    item.distance = distance;
    item.index = index;

    res.insert(item);
    if (res.size() > topK) {
      set<Candidate, CandidateComp>::iterator it = res.end();
      --it;
      res.erase(it);
    }
  }

  return vector<Candidate>(res.begin(), res.end());
}

void writeNumbers(const vector<float>& nums, ostream& out) {
  for (auto num : nums) out << num << endl;
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    printUsageAndExit();
  }

  const string datasetFile(argv[1]);
  const string queriesFile(argv[2]);
  const size_t topK = atoi(argv[3]);

  const vector<float> records = readNumbers(datasetFile);
  const vector<float> queries = readNumbers(queriesFile);

  size_t count = 0;

  cout.precision(6);
  cout.setf(ios::fixed, ios::floatfield);
  for (auto query : queries) {
    ++count;
    cerr << "processing " << count << " :" << query << endl;

    const vector<Candidate> results = knnBFSearch(query, records, topK);
    // cout << query << "|";
    for (size_t i = 0, len = results.size(); i < len; ++i) {
      if (i > 0) cout << ",";
      // cout << results[i].record << ":" << results[i].distance;
      cout << results[i].index;
    }
    cout << endl;
  }

  return 0;
}
