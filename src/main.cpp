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

float timeout;
high_resolution_clock::time_point start;
string stderrFilename;
string stdoutFilename;

int countLines(string filename) {
  ifstream infile(filename);
  string line;

  int count = 0;
  while (getline(infile, line)) {
    count++;
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

  int stdoutLines = countLines(stdoutFilename);
  int stderrLines = countLines(stderrFilename);

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

void printStats(string stdoutFilename) {
  auto currentTime = high_resolution_clock::now();
  auto duration = duration_cast<milliseconds>(currentTime - start);

  long seconds = duration.count() / 1000;
  long millis = duration.count() % 1000;
  int lines = countLines(stdoutFilename);

  // The spaces before the \r are necessary in case the line length changes.
  printf("%ld.%03ld seconds: %d lines of output   \r", seconds, millis, lines);
}

void redirectOutput(string stdoutFilename, string stderrFilename) {
  const char *out = stdoutFilename.c_str();
  int stdoutFd = open(out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (stdoutFd == -1) {
    perror("Open stdout failed");
    exit(EXIT_FAILURE);
  }

  const char *err = stderrFilename.c_str();
  int stderrFd = open(err, O_WRONLY | O_CREAT | O_TRUNC, 0644);
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

void handleChild(string stdoutFilename, string stderrFilename,
                 const char *command, vector<char *> args) {
  redirectOutput(stdoutFilename, stderrFilename);

  execvp(command, &args[0]);

  // Unreachable unless exec fails
  perror("Exec failed");
  exit(EXIT_FAILURE);
}

void handleParent(string stdoutFilename, pid_t pid) {
  makeCursorInvisible();

  int status;
  float timeTaken = 0;
  while (true) {
    // Check if child has exited
    if (waitpid(pid, &status, WNOHANG) == pid) {
      break;
    } else {
      printStats(stdoutFilename);
      usleep(1000);
    }

    auto currentTime = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(currentTime - start);
    timeTaken = duration.count() / 1000.0;
    if (timeout > 0 && timeTaken > timeout) {
      cout << endl;
      cout << "Timeout (" << timeout << " seconds) reached." << endl;
      kill(pid, SIGKILL);
      break;
    }
  }
  printStats(stdoutFilename);
  cout << endl;

  cleanup();
}

int main(int argc, char *argv[]) {
  ArgParser args =
      ArgParser("thyme", "1.0.0", "Run a command and record its output");

  args.registerOption("o", "stdout", "file", "stdout.txt", true,
                      "The file to write stdout to");
  args.registerOption("e", "stderr", "file", "stderr.txt", true,
                      "The file to write stderr to");
  args.registerOption("t", "timeout", "seconds", "0", true,
                      "The maximum time to run the command for");

  args.registerExample("thyme -o stdout.txt -e stderr.txt -t 10 sleep 5");
  args.registerExample("thyme bash -c \"ls\"");

  args.process(argc, argv);

  stdoutFilename = args.get("o", "stdout");
  stderrFilename = args.get("e", "stderr");
  timeout = stof(args.get("t", "timeout"));

  // Register signal handler
  signal(SIGINT, signalHandler);

  // Record start time
  start = high_resolution_clock::now();

  if (args.positionalArgs.size() == 0) {
    args.usage();
    exit(EXIT_FAILURE);
  }

  // Fork
  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork failed");
    exit(EXIT_FAILURE);
  }

  if (pid == 0) {
    // Null-terminate args
    auto c_args = vector<char *>(args.positionalArgs.size());
    for (size_t i = 0; i < args.positionalArgs.size(); ++i) {
      c_args[i] = const_cast<char *>(args.positionalArgs[i].c_str());
    }
    c_args.push_back(nullptr);

    handleChild(stdoutFilename, stderrFilename, args.positionalArgs[0].c_str(),
                c_args);
  } else {
    handleParent(stdoutFilename, pid);
  }
}
