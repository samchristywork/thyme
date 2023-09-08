#include <args.h>
#include <chrono>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/resource.h>
#include <sys/wait.h>
#include <vector>

using namespace std;
using namespace std::chrono;

high_resolution_clock::time_point start;

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

    makeCursorVisible();
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

  cleanup();

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

void redirectOutput(const char *stdoutFilename, const char *stderrFilename) {
  int stdoutFd = open(stdoutFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (stdoutFd == -1) {
    perror("Open stdout failed");
    exit(EXIT_FAILURE);
  }

  int stderrFd = open(stderrFilename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (stderrFd == -1) {
    perror("Open stderr failed");
    exit(EXIT_FAILURE);
  }

  if (dup2(stdoutFd, STDOUT_FILENO) == -1) {
    perror("Redirect stdout failed");
    exit(EXIT_FAILURE);
  }

  if (dup2(stderrFd, STDERR_FILENO) == -1) {
    perror("Redirect stderr failed");
    exit(EXIT_FAILURE);
  }

  close(stdoutFd);
  close(stderrFd);
}

void handleChild(const char *stdoutFilename, const char *stderrFilename,
                 char *command, vector<char *> args) {
  redirectOutput(stdoutFilename, stderrFilename);

  execvp(command, &args[0]);
  perror("Exec failed");
  exit(EXIT_FAILURE);
}

void handleParent(const char *stdoutFilename, pid_t pid) {
  makeCursorInvisible();
  int status;
  while (true) {
    if (waitpid(pid, &status, WNOHANG) == pid) {
      break;
    } else {
      printStats(stdoutFilename);
      usleep(1000);
    }
  }
  printStats(stdoutFilename);
  cout << endl;

  cleanup();
}

int main(int argc, char *argv[]) {
  ArgParser args(argc, argv);

  stdoutFilename = args.get("stdout", "stdout.txt");
  stderrFilename = args.get("stderr", "stderr.txt");

  // Register signal handler
  signal(SIGINT, signalHandler);

  // Record start time
  start = high_resolution_clock::now();

  // Fork
  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    return 1;
  }

  if (pid == 0) {
    // Null-terminate args
    args.positionalArgs.push_back(NULL);

    handleChild(stdoutFilename, stderrFilename, args.positionalArgs[0].c_str(),
                args.positionalArgs);
  } else {
    handleParent(stdoutFilename, pid);
  }
}
