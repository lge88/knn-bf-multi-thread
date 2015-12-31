#! /usr/bin/env babel-node --presets es2015
function randomDouble(low, high) {
  const diff = high - low;
  return low + Math.random() * diff;
}

function generateData(N, low, high) {
  const out = [];
  for (let i = 0; i < N; ++i) {
    out.push(randomDouble(low, high));
  }
  return out;
}

function printUsageAndExit() {
  console.log('Usage: random.vectors.js <N> <low> <high>');
  console.log('  generate <N> double range from <low> to <high>.');
  console.log('  <N> is a positive integer.');
  console.log('  <low> and <high> are number.');
  process.exit(0);
}

if (!module.parent) {
  const argc = process.argv.length;
  if (argc < 5) {
    printUsageAndExit();
  }

  const N = parseInt(process.argv[2], 10);
  const low = parseFloat(process.argv[3]);
  const high = parseFloat(process.argv[4]);

  if (isNaN(N) || isNaN(low) || isNaN(high)) {
    printUsageAndExit();
  }

  const data = generateData(N, low, high);
  data.forEach(d => console.log(d));
}
