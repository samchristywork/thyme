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
  bool calledProcess = false;

  ArgParser(string programName, string version, string description) {
    this->programName = programName;
    this->version = version;
    this->description = description;

    this->registerOption("h", "help", "", "", false, "Show this help message");
    this->registerOption("v", "version", "", "", false,
                         "Show version and license information");
  }

  void process(int argc, char **argv) {
    this->calledProcess = true;

    bool positionalArgReached = false;

    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];

      if (positionalArgReached) {
        positionalArgs.push_back(arg);
        continue;
      }

      if (arg.substr(0, 2) == "--" || arg.substr(0, 1) == "-") {
        string option = arg.substr(arg.substr(0, 2) == "--" ? 2 : 1);

        if (option == "") {
          positionalArgReached = true;
          continue;
        }

        bool found = false;
        for (auto &registeredOption : registeredOptions) {
          if (registeredOption.shortName == option ||
              registeredOption.longName == option) {
            found = true;
            if (registeredOption.hasValue) {
              if (i + 1 < argc) {
                options[registeredOption.shortName] = argv[i + 1];
                options[registeredOption.longName] = argv[i + 1];
                i++;
              } else {
                cout << "Missing value for option " << option << endl;
                exit(1);
              }
            } else {
              options[registeredOption.shortName] = "true";
              options[registeredOption.longName] = "true";
            }
            break;
          }
        }
      } else {
        positionalArgReached = true;
        i--;
      }
    }
  }

  void registerOption(string shortOption, string longOption, string units,
                      string defaultValue, bool hasValue, string description) {

    registeredOptions.push_back(
        {shortOption, longOption, units, description, defaultValue, hasValue});
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
