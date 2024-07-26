#include "blocksGenerator.cpp"

struct point {
  int x;
  int y;
  int z;
  point() {
    x = 0;
    y = 0;
    z = 0;
  }
  point(int x_, int y_, int z_) {
    x = x_;
    y = y_;
    z = z_;
  }
  point axisToPoint(axis ax) {
    bool sign = isPositive(ax);
    switch(abs(ax)) {
      case 0: return point(1, 0, 0)*sign;
      case 1: return point(0, 1, 0)*sign;
      case 2: return point(0, 0, 1)*sign;
      default: cout << "point axisToPoint(axis ax) reached default in switch statement\n"; return point(0, 0, 0);
    }
  }
  point adjacent(axis ax) {
    return *this+axisToPoint(ax);
  }
  point operator+(const point& p) const {
    return point(x+p.x, y+p.y, z+p.z);
  }
  point operator-(const point& p) const {
    return point(x-p.x, y-p.y, z-p.z);
  }
  point operator*(int m) {
    return point(x*m, y*m, z*m);
  }
  bool operator==(const point& p) const {
    return x==p.x && y==p.y && z==p.z;
  }
  bool operator!=(const point& p) const {
    return !(x==p.x && y==p.y && z==p.z);
  }
  bool inBounds(point& size) {
    bool xBound = (x >= 0 && x < size.x);
    bool yBound = (y >= 0 && y < size.y);
    bool zBound = (z >= 0 && z < size.z);
    return xBound && yBound && zBound;
  }
  vector<point> allPointsBounded() {
    vector<point> output;
    for (int x_ = 0; x_ < x; x_++) {
      for (int y_ = 0; y_ < y; y_++) {
        for (int z_ = 0; z_ < z; z_++) {
          output.push_back(point(x_, y_, z_));
        }
      }
    }
    return output;
  }
};

struct cell {
  vector<orientedBlock> orientedBlockOptions;
  point position;
  cell(point position_) {
    orientedBlockOptions = BLOCKS_ROTATED;
    position = position_;
  }
  void removeOption(orientedBlock o) {
    auto it = find(orientedBlockOptions.begin(), orientedBlockOptions.end(), o);
    assert(it != orientedBlockOptions.end());
    orientedBlockOptions.erase(it);
  }
  void addOption(orientedBlock o) {
    orientedBlockOptions.push_back(o);
  }
  int getEntropy() const {
    return orientedBlockOptions.size();
  }
  bool isCollapsed() const {
    return (orientedBlockOptions.size() == 1);
  }
};

bool compareCellEntropy(const cell& c1, const cell& c2) {
  return c1.getEntropy() < c2.getEntropy();
}

struct cellGrid {
  point size;
  vector<cell> cells;
  cellGrid(point size_) {
    size = size_;
    for (point p : size.allPointsBounded()) {
      cells.push_back(cell(p));
    }
  }
  bool isCollapsed() {
    for (cell& c : cells) {
      if (!c.isCollapsed()) return false;
    }
    return true;
  }
  void sortCellsByEntropy() {
    sort(cells.begin(), cells.end(), compareCellEntropy);
  }
  cell* cellAt(point p) {
    for (cell& c : cells) {
      if (c.position == p) return &c;
    }
    cout << "cell* cellAt(point p) failed to find a cell";
    return nullptr;
  }
  bool isValid() {
    for (cell& c0 : cells) {
      if (!c0.isCollapsed()) continue;
      for (int i = 0; i < AXIS_COUNT; i++) {
        axis ax = (axis)i;
        if (!c0.position.adjacent(ax).inBounds(size)) continue;
        cell* c1 = cellAt(c0.position.adjacent(ax));
        if (!c1->isCollapsed()) continue;
        connectionType connection0 = c0.orientedBlockOptions.front().getConnectionType(ax);
        axis TEST0 = ax;
        axis TEST1 = opposite(ax);
        connectionType connection1 = c1->orientedBlockOptions.front().getConnectionType(opposite(ax));
        if (!isValidConnection(connection0, connection1)) return false;
      }
    }
    return true;
  }
  int numCollapsedCells() {
    int count = 0;
    for (cell& c : cells) {
      if (c.isCollapsed()) {
        count++;
      }
    }
    return count;
  }
  int totalEntropy() {
    int count = 0;
    for (cell& c : cells) {
      count += c.getEntropy();
    }
    return count;
  }
  bool checkBlockConnections() {
    bool ok = true;
    for (int i = 0; i < cells.size(); i++) {
      for (int j = 0; j < cells[i].orientedBlockOptions.size(); j++) {
        if (cells[i].orientedBlockOptions[j].connectionTypes.size() != 6) {
          cout << "checkBlockConnections() detected a bad connection type on cell " + to_string(i) + ", block " + to_string(j) + "\n";
          ok = false;
        }
      }
    }
    return ok;
  }
  int getSizeInMemory() const {
    int sizeInMemory = 0;
    sizeInMemory += sizeof(*this);
    for (cell c : cells) {
      sizeInMemory += sizeof(c);
      for (orientedBlock o : c.orientedBlockOptions) {
        sizeInMemory += sizeof(o);
        sizeInMemory += 6*sizeof(int);
      }
    }
    return sizeInMemory;
  }
  bool operator==(const cellGrid& other) const {
    if (size != other.size) return false;
    if (cells.size() != other.cells.size()) return false;
    for (int i = 0; i < cells.size(); i++) {
      if (cells[i].position != other.cells[i].position) return false;
      if (cells[i].orientedBlockOptions.size() != other.cells[i].orientedBlockOptions.size()) return false;
      for (int j = 0; j < cells[i].orientedBlockOptions.size(); j++) {
        if (cells[i].orientedBlockOptions[j].block != other.cells[i].orientedBlockOptions[j].block) return false;
        for (int k = 0; k < 6; k++) {
          if (cells[i].orientedBlockOptions[j].connectionTypes[k] != other.cells[i].orientedBlockOptions[j].connectionTypes[k]) return false;
        }
      }
    }
    return true;
  }
  operator string() const {
    string output = "[";
    for (int i = 0; i < cells.size(); i++) {
      if (cells[i].isCollapsed()) {
        output += to_string(cells[i].orientedBlockOptions[0].block);
      } else {
        output += "?";
      }
    }
    output += "]";
    return output;
  }
};