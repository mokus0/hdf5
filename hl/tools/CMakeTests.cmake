
##############################################################################
##############################################################################
###           T E S T I N G                                                ###
##############################################################################
##############################################################################

# Make testfiles dir under build dir
FILE (MAKE_DIRECTORY "${PROJECT_BINARY_DIR}/testfiles")

#
# copy test files from source to build dir
#

ADD_CUSTOM_COMMAND (
    TARGET     gif2h5
    POST_BUILD
    COMMAND    ${CMAKE_COMMAND}
    ARGS       -E copy_if_different ${HDF5_HL_TOOLS_SOURCE_DIR}/gif2h5/testfiles/image1.gif ${PROJECT_BINARY_DIR}/testfiles/image1.gif
)

ADD_CUSTOM_COMMAND (
    TARGET     h52gif
    POST_BUILD
    COMMAND    ${CMAKE_COMMAND}
    ARGS       -E copy_if_different ${HDF5_HL_TOOLS_SOURCE_DIR}/gif2h5/testfiles/h52giftst.h5 ${PROJECT_BINARY_DIR}/testfiles/h52giftst.h5
)

# Remove any output file left over from previous test run
ADD_TEST (
    NAME HL_TOOLS-clear-objects
    COMMAND    ${CMAKE_COMMAND}
        -E remove 
        image1.gif
        image1.h5
)

ADD_TEST (NAME HL_TOOLS_h52gif COMMAND $<TARGET_FILE:h52gif> testfiles/h52giftst.h5 image1.gif -i image)

ADD_TEST (NAME HL_TOOLS_gif2h5 COMMAND $<TARGET_FILE:gif2h5> testfiles/image1.gif image1.h5)
