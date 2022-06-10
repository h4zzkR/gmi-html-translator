include(FetchContent)

# --------------------------------------------------------------------

# Offline mode (uncomment next line to enable)
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

# set(FETCHCONTENT_QUIET OFF)

# --------------------------------------------------------------------

# Libraries

# --------------------------------------------------------------------

message(STATUS "FetchContent: tiny-exe")

FetchContent_Declare(
        exe
        GIT_REPOSITORY https://github.com/h4zzkR/tiny-exe.git
        GIT_TAG main
)
FetchContent_MakeAvailable(exe)
