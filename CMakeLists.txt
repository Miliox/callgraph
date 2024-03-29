cmake_minimum_required (VERSION 3.20)
project(callgraph)

set(CMAKE_CXX_STANDARD   20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    ${PROJECT_NAME} src/string_cache.cpp src/graph.cpp src/main.cpp src/parser.cpp)

target_include_directories(
    ${PROJECT_NAME} PRIVATE include)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    STRING(JOIN " "
        CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS}"
        "-Wall"                # warn almost everything
        "-Wextra"              # warn even more
        "-Wshadow"             # warn when name shadowing
        "-Wnon-virtual-dtor"   # warn on class with virtual methods has no virtual destructor
        "-pedantic"            # warn on language extensions
        "-Wold-style-cast"     # warn on c style casting
        "-Wcast-align"         # warn on potential performance problem casts
        "-Wunused"             # warn on anything not used
        "-Woverloaded-virtual" # warn if virtual function was overloaded instead of overriden
        "-Wpedantic"           # warn if non-standard c++ is used
        "-Wconversion"         # warn on type conversion that loses data
        "-Wsign-conversion"    # warn on sign conversion
        "-Wmisleading-indentation" # warn if indentation implies blocks where blocks do not exist
        "-Wnull-dereference"   # warn if a null dereference is detected
        "-Wdouble-promotion"   # warn if float is implicit promoted to double
        "-Wformat=2"           # warn on security issues in print formatted
        # "-Weverything"         # enable every possible warning
        "-Weffc++"             # enforce effective c++ programming rules
        "-Wno-c++98-compat"    # ignore c++98 compatibility issues
        "-Werror"              # treat warn as errors
    )

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        STRING(JOIN " "
            CMAKE_CXX_FLAGS
            "${CMAKE_CXX_FLAGS}"
            "-Wuseless-cast"    # warn on perform cast to same type
            "-Wduplicated-cond" # warn on if-else that has duplicated conditions
        )
    endif()

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        STRING(JOIN " "
            "-fsanitize=address"
            # "-fsanitize=memory"
            # "-fsanitize=thread"
            "-fsanitize=undefined"
             CMAKE_CXX_FLAGS_DEBUG)

        if (CMAKE_SYSTEM_NAME MATCHES "Linux")
            if (IPO_SUPPORTED)
                set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=cfi")
            endif()
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=dataflow")
            set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=safe-stack")
        endif()
    endif()
endif()