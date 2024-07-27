#include "waveformCollapse.cpp"

int main() {
  srand(time(NULL));
  for (orientedBlock b : BLOCKS_ROTATED) {
    cout << static_cast<string>(b) + "\n";
  }
  cout << "start test\n";
  cellGrid g = cellGrid(point(5, 5, 5));
  vector<cellGrid> output = findValidSolutions(g, 0, 0);
  for (cellGrid cg : output) {
    cout << static_cast<string>(cg);
    cout << to_string(hash<cellGrid>{}(cg));
    cout << endl;
  }

}