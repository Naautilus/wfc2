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

unordered_map<cellGrid, vector<cellGrid>> existingResults;

vector<cellGrid> findValidSolutions(cellGrid& input, int iteration) {
  cout << string(iteration, ' ');
  cout << "hashmap size: " + to_string(existingResults.size()) + "\n";
  //cout << string(iteration, ' ');
  //cout << to_string(hash<cellGrid>{}(input)) + "\n";
  cout << string(iteration, ' ');
  cout << "findValidSolutions called on input with " + to_string(input.numCollapsedCells()) + " collapsed cells and a total entropy of " + to_string(input.totalEntropy()) + "\n";
  cout << string(iteration, ' ');
  if (existingResults.find(input) != existingResults.end()) {
    cout << "existingResults contains output\n";
    //return existingResults[input];
    return {};
  }
  if (!input.isValid()) {
    cout << "connection rules not met\n";
    existingResults[input] = {};
    return {};
  }
  if (input.isCollapsed()) {
    cout << "input collapsed\n";
    existingResults[input] = vector(1, input);
    return vector(1, input);
  }

  vector<cellGrid> output;
  input.sortCellsByEntropy();
  for (cell& c : input.cells) {
    if (c.getEntropy() <= 1) continue;
    for (int i = 0; i < c.orientedBlockOptions.size(); i++) {
      orientedBlock option = c.orientedBlockOptions[i];
      c.orientedBlockOptions.erase(c.orientedBlockOptions.begin()+i);
      cout << "option " + to_string(option.block) + " removed from cell at " + to_string(c.position.x) + to_string(c.position.y) + to_string(c.position.z) + "\n";
      vector<cellGrid> temp = findValidSolutions(input, iteration + 1);
      output.insert(output.end(), temp.begin(), temp.end());
      c.orientedBlockOptions.insert(c.orientedBlockOptions.begin()+i, option);
    }
  }
  cout << "output of size " + to_string(output.size()) + ": ";
  for (cellGrid cg : output) {
    cout << static_cast<string>(cg);
  }
  cout << "\n";
  existingResults[input] = output;
  return output;
}