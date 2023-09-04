#include <iostream>
#include <vector>

using namespace std;

void signalHandler(int signum) {
  cout << endl;
  cout << "Interrupt signal (" << signum << ") received.\n";

  exit(signum);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " command [args...]" << endl;
    return EXIT_FAILURE;
  }

  // Collect arguments to be passed to child
  vector<char *> childArgs;
  for (int i = 1; i < argc; ++i) {
    childArgs.push_back(argv[i]);
  }
  childArgs.push_back(NULL);

  // Register signal handler
  signal(SIGINT, signalHandler);
}
