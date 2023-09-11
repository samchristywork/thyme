#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#define LICENSE_STRING                                                         \
  "Copyright (C) 2023+ Sam Christy.\n"                                         \
  "License GPLv3+: GNU GPL version 3 or later "                                \
  "<http://gnu.org/licenses/gpl.html>\n"                                       \
  "\n"                                                                         \
  "This is free software; you are free to change and redistribute it.\n"       \
  "There is NO WARRANTY, to the extent permitted by law."

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

struct option {
  string shortName;
  string longName;
  string units;
  string description;
  string defaultValue;
  bool hasValue;
};

class ArgParser {
public:
  map<string, string> options;
  vector<string> positionalArgs;
  vector<option> registeredOptions;
  string programName;
  string version;
  string description;

  ArgParser(string programName, string version, string description) {
    this->programName = programName;
    this->version = version;
    this->description = description;
  }

    bool positionalArgReached = false;

    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];

      if (positionalArgReached) {
        positionalArgs.push_back(arg);
        continue;
      }

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
      } else {
        positionalArgReached = true;
        i--;
      }
    }
  }

  bool isSet(string option) { return options.find(option) != options.end(); }

  bool isSet(string shortOption, string longOption) {
    return isSet(shortOption) || isSet(longOption);
  }

  string get(string shortOption, string longOption, string defaultValue = "") {
    if (isSet(longOption)) {
      return options.at(longOption);
    }

    if (isSet(shortOption)) {
      return options.at(shortOption);
    }

    return defaultValue;
  }

private:
  map<string, string> options;
};

#endif
