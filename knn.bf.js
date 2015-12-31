#! /usr/bin/env babel-node --presets es2015
import fs from 'fs';

function readNumbers(filename) {
  const str = fs.readFileSync(filename, { encoding: 'utf8' });
  return str.trim()
    .split('\n')
    .map((line) => parseFloat(line));
}

function printUsageAndExit() {
  console.error('Usage: knn.bf.js <datasetFile> <queriesFile> <topK>');
  console.error('  Foreach query in <queriesFile> print <topK> nearest neighbor');
  console.error('  in <datasetFile> and print to stdout.');
  process.exit(1);
}

function knnBFSearch(query, records, topK) {
  const res = [];
  records.forEach((record, index) => {
    const distance = Math.abs(record - query);
    const item = { index, record, distance };
    res.push(item);
    res.sort((a, b) => a.distance - b.distance);
    if (res.length > topK) res.pop();
  });
  return res;
}

if (!module.parent) {
  const argc = process.argv.length;
  if (argc < 5) {
    printUsageAndExit();
  }

  const datasetFile = process.argv[2];
  const queriesFile = process.argv[3];
  const topK = parseInt(process.argv[4], 10);

  if (isNaN(topK)) {
    printUsageAndExit();
  }

  const records = readNumbers(datasetFile);
  const queries = readNumbers(queriesFile);

  let count = 0;
  queries.forEach((query) => {
    ++count;
    console.error('processing ' + (count + ': ') + query);

    let results = knnBFSearch(query, records, topK);
    results = results.map((result) => '' + result.index);
    const line = query.toFixed(6) + ':' + results.join(',');
    console.log(line);
  });
}
