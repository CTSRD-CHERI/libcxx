#ifndef FILESYSTEM_TEST_HELPER_H
#define FILESYSTEM_TEST_HELPER_H

#include "filesystem_include.h"

#include <unistd.h> // for ftruncate, symlink, unlink

#include <cassert>
#include <cstdio> // for printf
#include <string>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <regex>

#include "test_macros.h"
#include "rapid-cxx-test.h"
#include "format_string.h"

// For creating socket files
#if !defined(__FreeBSD__) && !defined(__APPLE__)
# include <sys/socket.h>
# include <sys/un.h>
#endif

// static test helpers

namespace StaticEnv {

// Tests that use these utilities should add '<...>/Inputs/static_test_env'
// to their FILE_DEPENDENCIES, to make sure the directory is made available
// to the test. Assuming that, the 'static_test_env' will be available in the
// directory where the test is run.
static const fs::path Root = fs::current_path() / "static_test_env";

inline fs::path makePath(fs::path const& p) {
    // env_path is expected not to contain symlinks.
    fs::path const& env_path = Root;
    return env_path / p;
}

static const fs::path TestFileList[] = {
        makePath("empty_file"),
        makePath("non_empty_file"),
        makePath("dir1/file1"),
        makePath("dir1/file2")
};
const std::size_t TestFileListSize = sizeof(TestFileList) / sizeof(fs::path);

static const fs::path TestDirList[] = {
        makePath("dir1"),
        makePath("dir1/dir2"),
        makePath("dir1/dir2/dir3")
};
const std::size_t TestDirListSize = sizeof(TestDirList) / sizeof(fs::path);

static const fs::path File          = TestFileList[0];
static const fs::path Dir           = TestDirList[0];
static const fs::path Dir2          = TestDirList[1];
static const fs::path Dir3          = TestDirList[2];
static const fs::path SymlinkToFile = makePath("symlink_to_empty_file");
static const fs::path SymlinkToDir  = makePath("symlink_to_dir");
static const fs::path BadSymlink    = makePath("bad_symlink");
static const fs::path DNE           = makePath("DNE");
static const fs::path EmptyFile     = TestFileList[0];
static const fs::path NonEmptyFile  = TestFileList[1];
static const fs::path CharFile      = "/dev/null"; // Hopefully this exists

static const fs::path DirIterationList[] = {
    makePath("dir1/dir2"),
    makePath("dir1/file1"),
    makePath("dir1/file2")
};
const std::size_t DirIterationListSize = sizeof(DirIterationList)
                                        / sizeof(fs::path);

static const fs::path DirIterationListDepth1[] = {
    makePath("dir1/dir2/afile3"),
    makePath("dir1/dir2/dir3"),
    makePath("dir1/dir2/symlink_to_dir3"),
    makePath("dir1/dir2/file4"),
};

static const fs::path RecDirIterationList[] = {
    makePath("dir1/dir2"),
    makePath("dir1/file1"),
    makePath("dir1/file2"),
    makePath("dir1/dir2/afile3"),
    makePath("dir1/dir2/dir3"),
    makePath("dir1/dir2/symlink_to_dir3"),
    makePath("dir1/dir2/file4"),
    makePath("dir1/dir2/dir3/file5")
};

static const fs::path RecDirFollowSymlinksIterationList[] = {
    makePath("dir1/dir2"),
    makePath("dir1/file1"),
    makePath("dir1/file2"),
    makePath("dir1/dir2/afile3"),
    makePath("dir1/dir2/dir3"),
    makePath("dir1/dir2/file4"),
    makePath("dir1/dir2/dir3/file5"),
    makePath("dir1/dir2/symlink_to_dir3"),
    makePath("dir1/dir2/symlink_to_dir3/file5"),
};

static const std::pair<fs::path, fs::path> SymlinkList[] = {
    {"dne",        makePath("bad_symlink")},
    {"dir1",       makePath("symlink_to_dir")},
    {"empty_file", makePath("symlink_to_empty_file")},
    {"dir3",       makePath("dir1/dir2/symlink_to_dir3")}
};

} // namespace StaticEnv

namespace random_utils {
inline char to_hex(int ch) {
  return ch < 10 ? static_cast<char>('0' + ch)
                 : static_cast<char>('a' + (ch - 10));
}

inline char random_hex_char() {
  static std::mt19937 rd{std::random_device{}()};
  static std::uniform_int_distribution<int> mrand{0, 15};
  return to_hex(mrand(rd));
}

} // namespace random_utils

/// A RAII object that prepares the 'static_test_env' directory for usage in
/// tests.
///
/// Namely, it creates some symlinks that tests expect to be present.
/// We do it here instead of storing them in the repo to be
/// cross-toolchain-friendly. The primary use case for this are Windows-hosted
/// cross-toolchains.
/// Windows doesn't really have a concept of symlinks. So, when the monorepo
/// is cloned, those symlinks turn to ordinary text files.
/// If we cross-compiled libc++ for some symlink-friendly system
/// (e. g. Linux) and ran tests on the target system, some tests would fail.
/// Instead, we create symlinks here, when a test is already being executed.
class static_test_env {
public:
  static_test_env() {
    for (const auto *link = std::begin(StaticEnv::SymlinkList);
         link != std::end(StaticEnv::SymlinkList);
         ++link) {
      int ret = ::symlink(link->first.c_str(), link->second.c_str());
      assert(ret == 0);
    }
  }

  ~static_test_env() {
    for (const auto *link = std::begin(StaticEnv::SymlinkList);
         link != std::end(StaticEnv::SymlinkList);
         ++link) {
      int ret = ::unlink(link->second.c_str());
      assert(ret == 0);
    }
  }
};

struct scoped_test_env
{
    scoped_test_env() : test_root(random_path()) {
        std::string cmd = "mkdir -p " + test_root.native();
        int ret = std::system(cmd.c_str());
        assert(ret == 0);

        // Ensure that the root_path is fully resolved, i.e. it contains no
        // symlinks. The filesystem tests depend on that. We do this after
        // creating the root_path, because `fs::canonical` requires the
        // path to exist.
        test_root = fs::canonical(test_root);
    }

    ~scoped_test_env() {
        std::string cmd = "chmod -R 777 " + test_root.native();
        int ret = std::system(cmd.c_str());
        assert(ret == 0);

        cmd = "rm -r " + test_root.native();
        ret = std::system(cmd.c_str());
        assert(ret == 0);
    }

    scoped_test_env(scoped_test_env const &) = delete;
    scoped_test_env & operator=(scoped_test_env const &) = delete;

    fs::path make_env_path(std::string p) { return sanitize_path(p); }

    std::string sanitize_path(std::string raw) {
        assert(raw.find("..") == std::string::npos);
        std::string const& root = test_root.native();
        if (root.compare(0, root.size(), raw, 0, root.size()) != 0) {
            assert(raw.front() != '\\');
            fs::path tmp(test_root);
            tmp /= raw;
            return std::move(const_cast<std::string&>(tmp.native()));
        }
        return raw;
    }

    // Purposefully using a size potentially larger than off_t here so we can
    // test the behavior of libc++fs when it is built with _FILE_OFFSET_BITS=64
    // but the caller is not (std::filesystem also uses uintmax_t rather than
    // off_t). On a 32-bit system this allows us to create a file larger than
    // 2GB.
    std::string create_file(std::string filename, uintmax_t size = 0) {
#if defined(__LP64__) || defined(__CHERI_PURE_CAPABILITY__)
        auto large_file_fopen = fopen;
        auto large_file_ftruncate = ftruncate;
        using large_file_offset_t = off_t;
#else
        auto large_file_fopen = fopen64;
        auto large_file_ftruncate = ftruncate64;
        using large_file_offset_t = off64_t;
#endif

        filename = sanitize_path(std::move(filename));

        if (size > std::numeric_limits<large_file_offset_t>::max()) {
            fprintf(stderr, "create_file(%s, %ju) too large\n",
                    filename.c_str(), size);
            abort();
        }

        FILE* file = large_file_fopen(filename.c_str(), "we");
        if (file == nullptr) {
            fprintf(stderr, "fopen %s failed: %s\n", filename.c_str(),
                    strerror(errno));
            abort();
        }

        if (large_file_ftruncate(
                fileno(file), static_cast<large_file_offset_t>(size)) == -1) {
            fprintf(stderr, "ftruncate %s %ju failed: %s\n", filename.c_str(),
                    size, strerror(errno));
            fclose(file);
            abort();
        }

        fclose(file);
        return filename;
    }

    std::string create_dir(std::string filename) {
        filename = sanitize_path(std::move(filename));
        std::string cmd = "mkdir " + filename;
        int ret = std::system(cmd.c_str());
        assert(ret == 0);
        return filename;
    }

    std::string create_symlink(std::string source, std::string to) {
        source = sanitize_path(std::move(source));
        to = sanitize_path(std::move(to));
        std::string cmd = "ln -s " + source + ' ' + to;
        int ret = std::system(cmd.c_str());
        assert(ret == 0);
        return to;
    }

    std::string create_hardlink(std::string source, std::string to) {
        source = sanitize_path(std::move(source));
        to = sanitize_path(std::move(to));
        std::string cmd = "ln " + source + ' ' + to;
        int ret = std::system(cmd.c_str());
        assert(ret == 0);
        return to;
    }

    std::string create_fifo(std::string file) {
        file = sanitize_path(std::move(file));
        std::string cmd = "mkfifo " + file;
        int ret = std::system(cmd.c_str());
        assert(ret == 0);
        return file;
    }

  // OS X and FreeBSD doesn't support socket files so we shouldn't even
  // allow tests to call this unguarded.
#if !defined(__FreeBSD__) && !defined(__APPLE__)
    std::string create_socket(std::string file) {
        file = sanitize_path(std::move(file));

        ::sockaddr_un address;
        address.sun_family = AF_UNIX;
        assert(file.size() <= sizeof(address.sun_path));
        ::strncpy(address.sun_path, file.c_str(), sizeof(address.sun_path));
        int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
        ::bind(fd, reinterpret_cast<::sockaddr*>(&address), sizeof(address));
        return file;
    }
#endif

    fs::path test_root;

private:
    static std::string unique_path_suffix() {
        std::string model = "test.%%%%%%";
        for (auto & ch :  model) {
          if (ch == '%')
            ch = random_utils::random_hex_char();
        }
        return model;
    }

    // This could potentially introduce a filesystem race with other tests
    // running at the same time, but oh well, it's just test code.
    static inline fs::path random_path() {
        fs::path tmp = fs::temp_directory_path();
        fs::path p = fs::path(tmp) / unique_path_suffix();
        return p;
    }
};

// Misc test types

#define MKSTR(Str) {Str, TEST_CONCAT(L, Str), TEST_CONCAT(u, Str), TEST_CONCAT(U, Str)}

struct MultiStringType {
  const char* s;
  const wchar_t* w;
  const char16_t* u16;
  const char32_t* u32;

  operator const char* () const { return s; }
  operator const wchar_t* () const { return w; }
  operator const char16_t* () const { return u16; }
  operator const char32_t* () const { return u32; }
};

const MultiStringType PathList[] = {
        MKSTR(""),
        MKSTR(" "),
        MKSTR("//"),
        MKSTR("."),
        MKSTR(".."),
        MKSTR("foo"),
        MKSTR("/"),
        MKSTR("/foo"),
        MKSTR("foo/"),
        MKSTR("/foo/"),
        MKSTR("foo/bar"),
        MKSTR("/foo/bar"),
        MKSTR("//net"),
        MKSTR("//net/foo"),
        MKSTR("///foo///"),
        MKSTR("///foo///bar"),
        MKSTR("/."),
        MKSTR("./"),
        MKSTR("/.."),
        MKSTR("../"),
        MKSTR("foo/."),
        MKSTR("foo/.."),
        MKSTR("foo/./"),
        MKSTR("foo/./bar"),
        MKSTR("foo/../"),
        MKSTR("foo/../bar"),
        MKSTR("c:"),
        MKSTR("c:/"),
        MKSTR("c:foo"),
        MKSTR("c:/foo"),
        MKSTR("c:foo/"),
        MKSTR("c:/foo/"),
        MKSTR("c:/foo/bar"),
        MKSTR("prn:"),
        MKSTR("c:\\"),
        MKSTR("c:\\foo"),
        MKSTR("c:foo\\"),
        MKSTR("c:\\foo\\"),
        MKSTR("c:\\foo/"),
        MKSTR("c:/foo\\bar"),
        MKSTR("//"),
        MKSTR("/finally/we/need/one/really/really/really/really/really/really/really/long/string")
};
const unsigned PathListSize = sizeof(PathList) / sizeof(MultiStringType);

template <class Iter>
Iter IterEnd(Iter B) {
  using VT = typename std::iterator_traits<Iter>::value_type;
  for (; *B != VT{}; ++B)
    ;
  return B;
}

template <class CharT>
const CharT* StrEnd(CharT const* P) {
    return IterEnd(P);
}

template <class CharT>
std::size_t StrLen(CharT const* P) {
    return StrEnd(P) - P;
}

// Testing the allocation behavior of the code_cvt functions requires
// *knowing* that the allocation was not done by "path::__str_".
// This hack forces path to allocate enough memory.
inline void PathReserve(fs::path& p, std::size_t N) {
  auto const& native_ref = p.native();
  const_cast<std::string&>(native_ref).reserve(N);
}

template <class Iter1, class Iter2>
bool checkCollectionsEqual(
    Iter1 start1, Iter1 const end1
  , Iter2 start2, Iter2 const end2
  )
{
    while (start1 != end1 && start2 != end2) {
        if (*start1 != *start2) {
            return false;
        }
        ++start1; ++start2;
    }
    return (start1 == end1 && start2 == end2);
}


template <class Iter1, class Iter2>
bool checkCollectionsEqualBackwards(
    Iter1 const start1, Iter1 end1
  , Iter2 const start2, Iter2 end2
  )
{
    while (start1 != end1 && start2 != end2) {
        --end1; --end2;
        if (*end1 != *end2) {
            return false;
        }
    }
    return (start1 == end1 && start2 == end2);
}

// We often need to test that the error_code was cleared if no error occurs
// this function returns an error_code which is set to an error that will
// never be returned by the filesystem functions.
inline std::error_code GetTestEC(unsigned Idx = 0) {
  using std::errc;
  auto GetErrc = [&]() {
    switch (Idx) {
    case 0:
      return errc::address_family_not_supported;
    case 1:
      return errc::address_not_available;
    case 2:
      return errc::address_in_use;
    case 3:
      return errc::argument_list_too_long;
    default:
      assert(false && "Idx out of range");
      std::abort();
    }
  };
  return std::make_error_code(GetErrc());
}

inline bool ErrorIsImp(const std::error_code& ec,
                       std::vector<std::errc> const& errors) {
  for (auto errc : errors) {
    if (ec == std::make_error_code(errc))
      return true;
  }
  return false;
}

template <class... ErrcT>
inline bool ErrorIs(const std::error_code& ec, std::errc First, ErrcT... Rest) {
  std::vector<std::errc> errors = {First, Rest...};
  return ErrorIsImp(ec, errors);
}

// Provide our own Sleep routine since std::this_thread::sleep_for is not
// available in single-threaded mode.
void SleepFor(std::chrono::seconds dur) {
    using namespace std::chrono;
#if defined(_LIBCPP_HAS_NO_MONOTONIC_CLOCK)
    using Clock = system_clock;
#else
    using Clock = steady_clock;
#endif
    const auto wake_time = Clock::now() + dur;
    while (Clock::now() < wake_time)
        ;
}

inline bool PathEq(fs::path const& LHS, fs::path const& RHS) {
  return LHS.native() == RHS.native();
}

struct ExceptionChecker {
  std::errc expected_err;
  fs::path expected_path1;
  fs::path expected_path2;
  unsigned num_paths;
  const char* func_name;
  std::string opt_message;

  explicit ExceptionChecker(std::errc first_err, const char* func_name,
                            std::string opt_msg = {})
      : expected_err{first_err}, num_paths(0), func_name(func_name),
        opt_message(opt_msg) {}
  explicit ExceptionChecker(fs::path p, std::errc first_err,
                            const char* func_name, std::string opt_msg = {})
      : expected_err(first_err), expected_path1(p), num_paths(1),
        func_name(func_name), opt_message(opt_msg) {}

  explicit ExceptionChecker(fs::path p1, fs::path p2, std::errc first_err,
                            const char* func_name, std::string opt_msg = {})
      : expected_err(first_err), expected_path1(p1), expected_path2(p2),
        num_paths(2), func_name(func_name), opt_message(opt_msg) {}

  void operator()(fs::filesystem_error const& Err) {
    TEST_CHECK(ErrorIsImp(Err.code(), {expected_err}));
    TEST_CHECK(Err.path1() == expected_path1);
    TEST_CHECK(Err.path2() == expected_path2);
    LIBCPP_ONLY(check_libcxx_string(Err));
  }

  void check_libcxx_string(fs::filesystem_error const& Err) {
    std::string message = std::make_error_code(expected_err).message();

    std::string additional_msg = "";
    if (!opt_message.empty()) {
      additional_msg = opt_message + ": ";
    }
    auto transform_path = [](const fs::path& p) {
      if (p.native().empty())
        return "\"\"";
      return p.c_str();
    };
    std::string format = [&]() -> std::string {
      switch (num_paths) {
      case 0:
        return format_string("filesystem error: in %s: %s%s", func_name,
                             additional_msg, message);
      case 1:
        return format_string("filesystem error: in %s: %s%s [%s]", func_name,
                             additional_msg, message,
                             transform_path(expected_path1));
      case 2:
        return format_string("filesystem error: in %s: %s%s [%s] [%s]",
                             func_name, additional_msg, message,
                             transform_path(expected_path1),
                             transform_path(expected_path2));
      default:
        TEST_CHECK(false && "unexpected case");
        return "";
      }
    }();
    TEST_CHECK(format == Err.what());
    if (format != Err.what()) {
      fprintf(stderr,
              "filesystem_error::what() does not match expected output:\n");
      fprintf(stderr, "  expected: \"%s\"\n", format.c_str());
      fprintf(stderr, "  actual:   \"%s\"\n\n", Err.what());
    }
  }

  ExceptionChecker(ExceptionChecker const&) = delete;
  ExceptionChecker& operator=(ExceptionChecker const&) = delete;

};

#endif /* FILESYSTEM_TEST_HELPER_HPP */
