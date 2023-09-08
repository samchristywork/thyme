#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

class ArgParser {
public:
  vector<string> positionalArgs;

  ArgParser(int argc, char **argv) {
  }

  bool isSet(const string &option) const {
  }

  string get(const string &option, const string &defaultValue = "") const {
  }

private:
  map<string, string> options;
};

#endif // ARG_PARSER_H
