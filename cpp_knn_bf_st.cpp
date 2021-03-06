#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <cstdlib>
#include <cmath>

using namespace std;

vector<double> readNumbers(const string& filename) {
  ifstream ifs(filename);
  vector<double> nums;

  double num;
  while (ifs >> num) nums.push_back(num);
  return nums;
}

void printUsageAndExit() {
  cerr << "Usage node knn.bf.js <datasetFile> <queriesFile> <topK>" << endl
       << "  Foreach query in <queriesFile> print <topK> nearest neighbor" << endl
       << "  in <datasetFile> and print to stdout." << endl;
  exit(1);
}

struct Candidate {
  double record;
  double distance;
  size_t index;
};

struct CandidateComp {
  bool operator() (const Candidate& lhs, const Candidate& rhs) const {
    if (lhs.distance == rhs.distance) {
      return lhs.index < rhs.index;
    }
    return lhs.distance < rhs.distance;
  }
};

vector<Candidate> knnBFSearch(
    double query,
    const vector<double>& records,
    size_t topK) {
  set<Candidate, CandidateComp> res;

  for (size_t index = 0, len = records.size(); index < len; ++index) {
    const double record = records[index];
    const double distance = abs(record - query);
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

void writeNumbers(const vector<double>& nums, ostream& out) {
  for (auto num : nums) out << num << endl;
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

  return 0;
}
