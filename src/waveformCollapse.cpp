#include "cellDefinitions.cpp"

long hash_sub(long hash, int input) {
  int HASH_MULTIPLIER = 26347859;
  hash += input;
  hash *= HASH_MULTIPLIER;
  hash %= INT_MAX;
  return hash;
}

// Custom hash function for cellGrid
namespace std {
  template<>
  struct hash<cellGrid> {
    size_t operator()(const cellGrid& cg) const {
      long h = 0;
      h = hash_sub(h, cg.size.x);
      h = hash_sub(h, cg.size.y);
      h = hash_sub(h, cg.size.z);
      for (cell c : cg.cells) {
        h = hash_sub(h, c.position.x);
        h = hash_sub(h, c.position.y);
        h = hash_sub(h, c.position.z);
        for (orientedBlock o : c.orientedBlockOptions) {
          h = hash_sub(h, o.block);
          for (int i = 0; i < 6; i++) {
            h = hash_sub(h, o.connectionTypes[i]);
          }
        }
      }
      return static_cast<size_t>(h);
    }
  };
}

int getSearchQuantityForLevel(int numCells, int iteration) {
  //for (int i = 0; i < iteration; i++) {
  //  numCells--;
  //}
  ////cout << "getSearchQuantityForLevel: " + to_string(numCells) + " cells\n";
  //return numCells;
  return 1;
}

vector<int> getLowestEntropies(cellGrid& input, int num) {
  vector<int> output;
  for (int i = 0; i < num; i++) {
    int minimumEntropy = INT_MAX;
    int minimumIndex = -1;
    for (int j = 0; j < input.cells.size(); j++) {
      int entropy = input.cells[j].getEntropy();
      if (entropy <= 1) continue;
      if (find(output.begin(), output.end(), j) != output.end()) continue;
      if (entropy < minimumEntropy) {
        minimumEntropy = entropy;
      }
      minimumIndex = j;
    }
    output.push_back(minimumIndex);
  }
  //cout << "getLowestEntropies: ";
  //for (int num : output) {
  //  cout << to_string(input.cells[num].getEntropy()) + "@" + to_string(num) + " ";
  //}
  //cout << "\n";
  return output;
}

const int MAX_DESIRED_OUTPUT_SIZE = 10;

unordered_map<cellGrid, vector<cellGrid>> existingResults;

vector<cellGrid> findValidSolutions(cellGrid& input, int iteration, int previousOutputSize) {
  //cout << string(iteration, ' ');
  //cout << "hashmap size: " + to_string(existingResults.size()) + "\n";
  //cout << string(iteration, ' ');
  //cout << "input: " + static_cast<string>(input) + "\n";
  //cout << string(iteration, ' ');
  //cout << to_string(hash<cellGrid>{}(input)) + "\n";
  //cout << string(iteration, ' ');
  //cout << "findValidSolutions called on input with " + to_string(input.numCollapsedCells()) + " collapsed cells and a total entropy of " + to_string(input.totalEntropy()) + " and a previous output size of " + to_string(previousOutputSize) + "\n";
  if (existingResults.find(input) != existingResults.end()) {
    //cout << string(iteration, ' ');
    //cout << "existingResults contains output\n";
    //return existingResults[input];
    return {};
  }
  if (!input.isValid()) {
    //cout << string(iteration, ' ');
    //cout << "connection rules not met\n";
    existingResults[input] = {};
    return {};
  }
  if (input.isCollapsed()) {
    //cout << string(iteration, ' ');
    //cout << "input collapsed\n";
    existingResults[input] = vector(1, input);
    return vector(1, input);
  }

  vector<cellGrid> output;
  vector<int> cellsToSearch = getLowestEntropies(input, getSearchQuantityForLevel(input.cells.size(), iteration));
  for (int cellIndex : cellsToSearch) {
    cell& c = input.cells[cellIndex];
    if (c.getEntropy() <= 1) continue;
    for (int i = 0; i < c.orientedBlockOptions.size(); i++) {
      orientedBlock option = c.orientedBlockOptions[i];
      c.orientedBlockOptions.erase(c.orientedBlockOptions.begin()+i);
      //cout << "option " + to_string(option.block) + " removed from cell at " + to_string(c.position.x) + to_string(c.position.y) + to_string(c.position.z) + "\n";
      vector<cellGrid> temp = findValidSolutions(input, iteration + 1, output.size() + previousOutputSize);
      output.insert(output.end(), temp.begin(), temp.end());
      c.orientedBlockOptions.insert(c.orientedBlockOptions.begin()+i, option);
      if (output.size() + previousOutputSize > MAX_DESIRED_OUTPUT_SIZE) break;
    }
    if (output.size() + previousOutputSize > MAX_DESIRED_OUTPUT_SIZE) break;
  }
  //cout << string(iteration, ' ');
  //cout << "output of size " + to_string(output.size());// + ": ";
  //for (cellGrid cg : output) {
  //  cout << static_cast<string>(cg) << to_string(hash<cellGrid>{}(cg));
  //}
  //cout << "\n";
  existingResults[input] = output;
  return output;
}