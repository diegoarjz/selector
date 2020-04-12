# Default Selector version, build number and date
if (NOT DEFINED SELECTOR_VERSION)
    set(SELECTOR_VERSION "developer-version")
endif()

if (NOT DEFINED SELECTOR_BUILD_NUMBER)
    set(SELECTOR_BUILD_NUMBER "")
endif()

string(TIMESTAMP SELECTOR_BUILD_DATE "%d/%m/%Y %H:%M:%S")

if (DEFINED SELECTOR_ASSERTIONS)
    set(SELECTOR_ENABLE_ASSERTIONS 1)
else()
    set(SELECTOR_ENABLE_ASSERTIONS 0)
endif()

execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git log -1 --format="%aN <%aE>"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_AUTHOR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git log -1 --format="%aN <%aE>"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_AUTHOR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
  COMMAND git log -1 --format="%s"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_SUBJECT
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(SELECTOR_INCLUDE_GIT_INFO 1)
if (${GIT_BRANCH} EQUAL "master")
    set(SELECTOR_INCLUDE_GIT_INFO 0)
endif()

message(STATUS "Selector version:      ${SELECTOR_VERSION}")
message(STATUS "Selector build number: ${SELECTOR_BUILD_NUMBER}")
message(STATUS "Selector build date:   ${SELECTOR_BUILD_DATE}")
if (${SELECTOR_INCLUDE_GIT_INFO} EQUAL 1)
    message(STATUS "Git Branch:            ${GIT_BRANCH}")
    message(STATUS "Git Commit Hash:       ${GIT_COMMIT_HASH}")
    message(STATUS "Git Commit Author:     ${GIT_COMMIT_AUTHOR}")
    message(STATUS "Git Commit Subject:    ${GIT_COMMIT_SUBJECT}")
endif()
