import fs from 'fs';

function readNumbers(filename) {
  const str = fs.readFileSync(filename, { encoding: 'utf8' });
  return str.trim()
    .split('\n')
    .map((line) => parseFloat(line));
}

function printUsageAndExit() {
  console.log('Usage node knn.bf.js <datasetFile> <queriesFile> <topK>');
  console.log('  Foreach query in <queriesFile> print <topK> nearest neighbor');
  console.log('  in <datasetFile> and print to stdout.');
  process.exit();
}

function knnBFSearch(query, records, topK) {
  let res = [];
  records.forEach((record) => {
    const distance = Math.abs(record - query);
    const item = { record, distance };
    res.push(item);
    res.sort((a, b) => a.distance < b.distance);
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
  const topK = parseInt(process.argv[4]);

  if (isNaN(topK)) {
    printUsageAndExit();
  }

  const records = readNumbers(datasetFile);
  const queries = readNumbers(queriesFile);

  let count = 0;
  queries.forEach((query) => {
    let results = knnBFSearch(query, records, topK);
    results = results.map((result) => result.record + ':' + result.distance);
    let line = query + '|' + results.join(',');

    ++count;
    console.error('processing ' + (count + ' :') + query);
    console.log(line);
  });
}
