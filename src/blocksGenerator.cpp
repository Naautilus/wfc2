#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <string.h>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <filesystem>
#include <chrono>
#include <cassert>
#include <unordered_map>
#include <climits>

using namespace std;

enum axis {
  xpos,
  ypos,
  zpos,
  xneg,
  yneg,
  zneg,
  AXIS_COUNT
};

enum blockType {
  noBlock,
  connector,
  packer,
  pellet,
  compactor,
  BLOCK_TYPE_COUNT
};

enum connectionType {
  noConnection,
  connectorToConnectorAndPacker,
  packerToPayload,
  CONNECTION_TYPE_COUNT
};

bool isValidConnection(connectionType a, connectionType b) {
  if ((int)a > (int)b) swap(a,b);
  auto p = pair(a,b);
  if(p == pair(noConnection, noConnection)) return true;
  if(p == pair(noConnection, connectorToConnectorAndPacker)) return true;
  if(p == pair(noConnection, packerToPayload)) return true;
  if(p == pair(connectorToConnectorAndPacker, connectorToConnectorAndPacker)) return true;
  if(p == pair(connectorToConnectorAndPacker, packerToPayload)) return false;
  if(p == pair(packerToPayload, packerToPayload)) return true;
  cout << "isValidConnection reached unintended point\n"; return false;
}

struct orientedBlock {
  blockType block;
  vector<connectionType> connectionTypes;
  orientedBlock() {
    cout << "a\n";
    block = noBlock;
    connectionTypes = vector(6, noConnection);
    cout << "b\n";
  }
  orientedBlock(blockType b, vector<connectionType> cT) {
    assert(cT.size() == 6);
    block = b;
    connectionTypes = cT;
  }
  orientedBlock(blockType b, connectionType cT0, connectionType cT1, connectionType cT2, connectionType cT3, connectionType cT4, connectionType cT5) {
    block = b;
    connectionTypes.resize(6);
    connectionTypes[0] = cT0;
    connectionTypes[1] = cT1;
    connectionTypes[2] = cT2;
    connectionTypes[3] = cT3;
    connectionTypes[4] = cT4;
    connectionTypes[5] = cT5;
  }
  bool operator==(const orientedBlock& other) const {
    return
      (block == other.block) &&
      (connectionTypes == other.connectionTypes);
  }
  operator string() const {
    string output = "[blockType " + to_string(block) + ", connections {";
    for (int i = 0; i < 6; i++) {
      output += to_string(connectionTypes[i]);
      if (i < 5) output += ", ";
    }
    output += "}]";
    return output;
  }
  connectionType getConnectionType(int index) {
    if (connectionTypes.size() != 6) {
      cout << "connectionTypes.size() != 6\n";
    }
    return connectionTypes[index];
  }
};

bool isPositive(axis ax) {
  switch(ax) {
    case xpos: return true;
    case ypos: return true;
    case zpos: return true;
  }
  return false;
}

axis abs(axis ax) {
  return (axis)(((int)ax)%3);
}

axis opposite(axis ax) {
  return (axis)((((int)ax)+3)%6);
}

void rotate(orientedBlock &input, axis ax) { // rotate 90deg using right-hand rule
  axis newAxes[6];
  int rotationCount = 1;
  if (!isPositive(ax)) rotationCount = 3;
  ax = abs(ax);
  switch(ax) {
    case xpos:
      newAxes[0] = xpos;
      newAxes[1] = zpos;
      newAxes[2] = yneg;
      newAxes[3] = xneg;
      newAxes[4] = zneg;
      newAxes[5] = ypos;
      break;
    case ypos:
      newAxes[0] = zneg;
      newAxes[1] = ypos;
      newAxes[2] = xpos;
      newAxes[3] = zpos;
      newAxes[4] = yneg;
      newAxes[5] = xneg;
      break;
    case zpos:
      newAxes[0] = ypos;
      newAxes[1] = xneg;
      newAxes[2] = zpos;
      newAxes[3] = yneg;
      newAxes[4] = xpos;
      newAxes[5] = zneg;
      break;
    default:
      cout << "void rotate(orientedBlock input, axis ax) reached default in switch statement\n";
  }
  for (int i = 0; i < rotationCount; i++) {
    vector<connectionType> connectionTypesOld = input.connectionTypes;
    for (int j = 0; j < 6; j++) {
      input.connectionTypes[j] = connectionTypesOld[newAxes[j]];
    }
  }
}

vector<orientedBlock> removeDuplicates(vector<orientedBlock> input) {
  vector<orientedBlock> output;
  for (const orientedBlock& b : input) {
    auto it = find(output.begin(), output.end(), b);
    if (it == output.end()) output.push_back(b);
  }
  return output;
}

vector<orientedBlock> getAllOrientations(orientedBlock input) {
  orientedBlock originalInput = input;
  vector<orientedBlock> output;
  //cout << "original xpos at xpos\n";
  for (int j = 0; j < 4; j++) { // original xpos at xpos
    output.push_back(input);
    rotate(input, xpos);
  }
  input = originalInput;
  rotate(input, zneg);
  //cout << "original xpos at ypos\n";
  for (int j = 0; j < 4; j++) { // original xpos at ypos
    output.push_back(input);
    rotate(input, ypos);
  }
  input = originalInput;
  rotate(input, ypos);
  //cout << "original xpos at zpos\n";
  for (int j = 0; j < 4; j++) { // original xpos at zpos
    output.push_back(input);
    rotate(input, zpos);
  }
  input = originalInput;
  rotate(input, ypos);
  rotate(input, ypos);
  //cout << "original xpos at xneg\n";
  for (int j = 0; j < 4; j++) { // original xpos at xneg
    output.push_back(input);
    rotate(input, xpos);
  }
  input = originalInput;
  rotate(input, zpos);
  //cout << "original xpos at yneg\n";
  for (int j = 0; j < 4; j++) { // original xpos at yneg
    output.push_back(input);
    rotate(input, ypos);
  }
  input = originalInput;
  rotate(input, yneg);
  //cout << "original xpos at zneg\n";
  for (int j = 0; j < 4; j++) { // original xpos at zneg
    output.push_back(input);
    rotate(input, zpos);
  }
  return removeDuplicates(output);
}

vector<orientedBlock> getBlocksUnrotated() {
  vector<orientedBlock> blocksUnrotated;
  blocksUnrotated.push_back(orientedBlock(noBlock, vector(6, noConnection)));
  blocksUnrotated.push_back(orientedBlock(connector, vector(6, connectorToConnectorAndPacker)));
  blocksUnrotated.push_back(orientedBlock(packer, connectorToConnectorAndPacker, connectorToConnectorAndPacker, packerToPayload, packerToPayload, connectorToConnectorAndPacker, packerToPayload));
  blocksUnrotated.push_back(orientedBlock(pellet, vector(6, packerToPayload)));
  blocksUnrotated.push_back(orientedBlock(compactor, vector(6, packerToPayload)));
  return blocksUnrotated;
}

const vector<orientedBlock> BLOCKS_UNROTATED = getBlocksUnrotated();

vector<orientedBlock> getBlocksRotated() {
  vector<orientedBlock> blocksRotated;
  for (orientedBlock b : BLOCKS_UNROTATED) {
    vector<orientedBlock> temp = getAllOrientations(b);
    blocksRotated.insert(blocksRotated.end(), temp.begin(), temp.end());
  }
  vector<orientedBlock> blocksRotated_SUBSET;
  blocksRotated_SUBSET.push_back(blocksRotated[0]);
  blocksRotated_SUBSET.push_back(blocksRotated[1]);
  return blocksRotated_SUBSET;
}

const vector<orientedBlock> BLOCKS_ROTATED = getBlocksRotated();