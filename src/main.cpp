#include <chrono>
#include <iostream>
#include <signal.h>
#include <vector>

using namespace std;

chrono::high_resolution_clock::time_point start;

const char *stdoutFilename = "stdout.txt";
const char *stderrFilename = "stderr.txt";

void signalHandler(int signum) {
  cout << endl;
  cout << "Interrupt signal (" << signum << ") received.\n";

  exit(signum);
}

void handleChild(const char *stdoutFilename, const char *stderrFilename,
                 char *command, vector<char *> args) {
  exit(EXIT_FAILURE);
}

void handleParent(const char *stdoutFilename, pid_t pid) {
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

  // Record start time
  start = chrono::high_resolution_clock::now();

  // Fork
  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    return 1;
  }

  if (pid == 0) {
    handleChild(stdoutFilename, stderrFilename, argv[1], childArgs);
  } else {
    handleParent(stdoutFilename, pid);
  }
}
