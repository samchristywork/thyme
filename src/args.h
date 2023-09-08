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
    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];

      if (arg.substr(0, 2) == "--") {
        string longOpt = arg.substr(2);
        string::size_type pos = longOpt.find('=');

        if (pos != string::npos) {
          string key = longOpt.substr(0, pos);
          string value = longOpt.substr(pos + 1);
          options[key] = value;
        } else {
          options[longOpt] = "";
        }
      } else if (arg[0] == '-') {
        string shortOpt = arg.substr(1);
        if (shortOpt.length() == 1 && i + 1 < argc && argv[i + 1][0] != '-') {
          options[shortOpt] = argv[++i];
        } else {
          options[shortOpt] = "";
        }
      }
    }
  }

  bool isSet(const string &option) const {
  }

  string get(const string &option, const string &defaultValue = "") const {
  }

private:
  map<string, string> options;
};

#endif // ARG_PARSER_H
