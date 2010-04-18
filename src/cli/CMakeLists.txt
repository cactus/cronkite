set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../lib/)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cli.h.in
    cli.h @ONLY)

# build cronkite cli binary
add_executable(cli cli.c)
target_link_libraries(cli cronkite)
set_target_properties(cli PROPERTIES OUTPUT_NAME cronkite)
# install target for cronkite cli binary
install(TARGETS cli DESTINATION bin)

#### add tests
## test version output
set(TESTSDIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
add_test(NAME t-cli-version
    COMMAND ${TESTSDIR}/cli-version.sh $<TARGET_FILE:cli>)

## test to ensure help output works
add_test(NAME t-cli-help
    COMMAND ${TESTSDIR}/cli-help.sh $<TARGET_FILE:cli>)

## test to ensure a badarg results in returnval=1
add_test(NAME t-cli-badarg
    COMMAND ${TESTSDIR}/cli-badarg.sh $<TARGET_FILE:cli>)

## test to ensure a too short query does results in returnval=2
add_test(NAME t-cli-badquery
    COMMAND ${TESTSDIR}/cli-badquery.sh $<TARGET_FILE:cli>)
# this test hits the network. make sure it doesn't go on forever
set_tests_properties(t-cli-badquery
    PROPERTIES TIMEOUT 5)

## test a good query with a local file
add_test(NAME t-cli-localfile
    COMMAND ${TESTSDIR}/cli-localfile.sh $<TARGET_FILE:cli>)
set_tests_properties(t-cli-localfile PROPERTIES 
    ENVIRONMENT "CRONKITE_AURURL=file://${TESTSDIR}/example.json")

## test a query with a bad url
add_test(NAME t-cli-badurl
    COMMAND ${TESTSDIR}/cli-badurl.sh $<TARGET_FILE:cli>)

## test a query with no url
add_test(NAME t-cli-nourl
    COMMAND ${TESTSDIR}/cli-nourl.sh $<TARGET_FILE:cli>)

## test a result with no data
add_test(NAME t-cli-empty
    COMMAND ${TESTSDIR}/cli-empty.sh $<TARGET_FILE:cli>)
set_tests_properties(t-cli-empty PROPERTIES 
    ENVIRONMENT "CRONKITE_AURURL=file://${TESTSDIR}/empty.json")

## test a result containing invalid json (notjson)
add_test(NAME t-cli-notjson
    COMMAND ${TESTSDIR}/cli-notjson.sh $<TARGET_FILE:cli>)
set_tests_properties(t-cli-notjson PROPERTIES 
    ENVIRONMENT "CRONKITE_AURURL=file://${TESTSDIR}/notjson.txt")

## do a valgrind run, if vangrind is found
if(VALGRIND_FOUND)
    add_test(NAME t-cli-memcheck
        COMMAND ${VALGRIND_EXECUTABLE} ${VALGRIND_OPTIONS} $<TARGET_FILE:cli> -search example)
    set_tests_properties(t-cli-memcheck PROPERTIES 
        ENVIRONMENT "CRONKITE_AURURL=file://${TESTSDIR}/example.json")
endif(VALGRIND_FOUND)

