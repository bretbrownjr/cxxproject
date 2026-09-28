#include <cxxp/process.hxx>

#include <array>
#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <spawn.h>
#include <stdexcept>
#include <sys/wait.h>
#include <system_error>
#include <unistd.h>

extern char **environ;

namespace cxxp {

namespace {

void check(int error, std::string const &context) {
  if (error != 0) {
    throw std::system_error(error, std::generic_category(), context);
  }
}

void validate(std::string const &value) {
  if (value.find('\0') != std::string::npos) {
    throw std::invalid_argument("process strings cannot contain NUL bytes");
  }
}

struct Descriptor {
  int value = -1;
  ~Descriptor() {
    if (value >= 0)
      ::close(value);
  }
};

struct Actions {
  posix_spawn_file_actions_t value;
  Actions() {
    check(posix_spawn_file_actions_init(&value), "initialize process actions");
  }
  ~Actions() { posix_spawn_file_actions_destroy(&value); }
};

struct Child {
  pid_t pid;
  ~Child() {
    if (pid > 0) {
      ::kill(pid, SIGKILL);
      while (::waitpid(pid, nullptr, 0) < 0 && errno == EINTR) {
      }
    }
  }
};

} // namespace

std::filesystem::path resolve_executable(std::filesystem::path const &name) {
  auto const text = name.string();
  validate(text);
  if (text.empty())
    throw std::invalid_argument("empty executable name");
  auto usable = [](std::filesystem::path const &path) {
    std::error_code error;
    return std::filesystem::is_regular_file(path, error) &&
           ::access(path.c_str(), X_OK) == 0;
  };
  if (name.has_parent_path()) {
    auto path = std::filesystem::absolute(name);
    if (usable(path))
      return path;
  } else {
    auto const *environment_path = std::getenv("PATH");
    // An unset PATH uses the Linux default; empty entries mean caller cwd.
    std::string const path =
        environment_path ? environment_path : "/bin:/usr/bin";
    std::size_t begin = 0;
    do {
      auto end = path.find(':', begin);
      auto candidate = std::filesystem::absolute(
          std::filesystem::path(path.substr(begin, end - begin)) / name);
      if (usable(candidate))
        return candidate;
      if (end == std::string::npos)
        break;
      begin = end + 1;
    } while (true);
  }
  throw std::runtime_error("cannot find executable: " + text);
}

ProcessResult run_process(std::filesystem::path const &executable,
                          std::vector<std::string> const &arguments,
                          std::filesystem::path const &working_directory,
                          bool capture_stdout) {
  if (!executable.is_absolute() || !working_directory.is_absolute()) {
    throw std::invalid_argument(
        "process requires absolute executable and working directory paths");
  }
  validate(executable.string());
  validate(working_directory.string());
  std::vector<std::string> storage{executable.string()};
  for (auto const &argument : arguments) {
    validate(argument);
    storage.push_back(argument);
  }
  std::vector<char *> argv;
  for (auto &argument : storage)
    argv.push_back(argument.data());
  argv.push_back(nullptr);

  Descriptor read_end, write_end;
  if (capture_stdout) {
    int pipe_fds[2];
    check(::pipe2(pipe_fds, O_CLOEXEC) < 0 ? errno : 0, "create stdout pipe");
    read_end.value = pipe_fds[0];
    write_end.value = pipe_fds[1];
    // Keep pipe endpoints away from stdio even if the caller closed it.
    for (auto *fd : {&read_end, &write_end}) {
      if (fd->value <= STDERR_FILENO) {
        int moved = ::fcntl(fd->value, F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
        check(moved < 0 ? errno : 0, "move stdout pipe");
        ::close(fd->value);
        fd->value = moved;
      }
    }
  }
  Actions actions;
  check(posix_spawn_file_actions_addchdir_np(&actions.value,
                                             working_directory.c_str()),
        "set child directory");
  check(posix_spawn_file_actions_addopen(&actions.value, STDIN_FILENO,
                                         "/dev/null", O_RDONLY, 0),
        "close child input");
  if (capture_stdout) {
    check(posix_spawn_file_actions_adddup2(&actions.value, write_end.value,
                                           STDOUT_FILENO),
          "capture stdout");
    check(posix_spawn_file_actions_addclose(&actions.value, read_end.value),
          "close child pipe reader");
    check(posix_spawn_file_actions_addclose(&actions.value, write_end.value),
          "close child pipe writer");
  } else {
    check(posix_spawn_file_actions_addopen(&actions.value, STDOUT_FILENO,
                                           "/dev/null", O_WRONLY, 0),
          "discard stdout");
  }

  pid_t pid;
  check(posix_spawn(&pid, executable.c_str(), &actions.value, nullptr,
                    argv.data(), environ),
        "launch " + executable.string());
  Child child{pid};
  ProcessResult result;
  if (capture_stdout) {
    ::close(write_end.value);
    write_end.value = -1;
    std::array<char, 8192> buffer;
    while (true) {
      auto count = ::read(read_end.value, buffer.data(), buffer.size());
      if (count < 0 && errno == EINTR)
        continue;
      check(count < 0 ? errno : 0, "read child stdout");
      if (count == 0)
        break;
      result.output.append(buffer.data(), static_cast<std::size_t>(count));
    }
  }
  int status;
  while (::waitpid(pid, &status, 0) < 0) {
    if (errno == EINTR)
      continue;
    check(errno, "wait for " + executable.string());
  }
  child.pid = -1;
  if (WIFEXITED(status)) {
    result.exit_code = WEXITSTATUS(status);
  }
  else if (WIFSIGNALED(status)) {
    result.signal = WTERMSIG(status);
  }
  return result;
}

} // namespace cxxp
