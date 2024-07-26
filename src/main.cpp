#include "waveformCollapse.cpp"

int main() {
  for (orientedBlock b : BLOCKS_ROTATED) {
    cout << static_cast<string>(b) + "\n";
  }
  cout << "start test\n";
  cellGrid g = cellGrid(point(2, 1, 1));
  vector<cellGrid> output = findValidSolutions(g, 0);
  for (cellGrid cg : output) {
    cout << static_cast<string>(cg);
    cout << endl;
  }

}