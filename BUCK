cxx_binary(
  name = "app", 
  srcs = [
    "main.cpp", 
  ], 
  deps = [
    ":server",
  ], 
)

cxx_library(
  name = "server", 
  srcs = [
    "src/TcpServer.cpp",
    "src/TcpConnection.cpp",
  ], 
  deps = [
  ], 
)

cxx_library(
  name = "optional", 
  headers = [
    "src/Optional.hpp",
  ], 
  deps = [
  ], 
)

cxx_library(
  name = "cache", 
  headers = [
    "src/Cache.hpp",
  ], 
  exported_headers = [
    "src/Cache.hpp",
  ],
  deps = [
  ], 
)

cxx_test(
  name = "test_cache", 
  linker_flags = [
    "-lgtest",
    "-lgtest_main",
    "-pthread",
  ],
  srcs = [
    "src/TestCache.cpp",
  ], 
  deps = [
    ":cache",
  ], 
)

cxx_library(
  name = "bla", 
  headers = [
    "bla.hpp",
  ], 
)

cxx_test(
  name = "tests", 
  linker_flags = [
    "-lgtest",
    "-lgtest_main",
    "-pthread",
  ],
  srcs = [
    "test.cpp",
  ], 
  deps = [
    ":bla",
  ],
)
