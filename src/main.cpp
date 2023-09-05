#include <chrono>
#include <iostream>
#include <signal.h>
#include <vector>

using namespace std;

chrono::high_resolution_clock::time_point start;

const char *stdoutFilename = "stdout.txt";
const char *stderrFilename = "stderr.txt";

// TODO: Inefficient
int countLines(const char *filename) {
  std::ifstream infile(filename);
  std::string line;
  int count = 0;

  while (std::getline(infile, line)) {
    ++count;
  }

  return count;
}

void makeCursorInvisible() {
  cout << "\033[?25l";
  cout.flush();
}

void makeCursorVisible() {
  cout << "\033[?25h";
  cout.flush();
}

void printTimeUsage() {
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);

  struct rusage usage;
  if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
    perror("getrusage failed");
    exit(EXIT_FAILURE);
  }

  auto real = duration.count() / 1e6;
  auto userCPU = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec / 1e6;
  auto systemCPU = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec / 1e6;

  cout << endl;
  cout << "Time usage:" << endl;
  cout << "\tReal: " << real << " seconds" << endl;
  cout << "\tUser CPU: " << userCPU << " seconds" << endl;
  cout << "\tSystem CPU: " << systemCPU << " seconds" << endl;
}

void cleanup() {
  printTimeUsage();

  auto stdoutLines = countLines(stdoutFilename);
  auto stderrLines = countLines(stderrFilename);

  cout << endl;
  cout << "Output saved to:" << endl;
  cout << "\t" << stdoutFilename << ": (" << stdoutLines << " lines)" << endl;
  cout << "\t" << stderrFilename << ": (" << stderrLines << " lines)" << endl;

  makeCursorVisible();
}

void signalHandler(int signum) {
  cout << endl;
  cout << "Interrupt signal (" << signum << ") received.\n";

  exit(signum);
}

void printStats(const char *stdoutFilename) {
  auto currentTime = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(currentTime - start);

  auto seconds = duration.count() / 1000;
  auto millis = duration.count() % 1000;
  auto lines = countLines(stdoutFilename);

  printf("%ld.%03ld seconds: %d lines of output\r", seconds, millis, lines);
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
