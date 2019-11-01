macro( copy_bin file )
   add_custom_command( TARGET ${file} POST_BUILD COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin )
   add_custom_command( TARGET ${file} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${file} ${CMAKE_BINARY_DIR}/bin/ )
endmacro( copy_bin )

#
# Set variable ${out_semver} to a semantic version taken from a git tag.
# Set variable ${out_dirty} to a current (probably dirty) git tag.
#
# Semantic version is a 3-component string "X.Y.Z".
#
# Example:
#
#   get_version_from_git(semver dirty)
#   message(STATUS "semver = ${semver}, dirty git version = ${dirty}")
#
function(get_version_from_git out_semver out_dirty)
  execute_process(
    COMMAND         git describe --tags --dirty
    OUTPUT_VARIABLE git_tag_raw
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  string(REGEX REPLACE "^v([0-9]+\\.[0-9]+\\.[0-9]+).*" "\\1" semver "${git_tag_raw}")

  # return
  set(${out_semver} "${semver}" PARENT_SCOPE)
  set(${out_dirty} "${git_tag_raw}" PARENT_SCOPE)
endfunction()

#
# get_version_component("11.22.33" 0 x) # => x = 11
# get_version_component("11.22.33" 1 x) # => x = 22
# get_version_component("11.22.33" 2 x) # => x = 33
#
function(get_version_component semantic_version component_index out)
  string(REPLACE "." ";" version_list "${semantic_version}")
  list(GET version_list "${component_index}" component)

  # return
  set(${out} "${component}" PARENT_SCOPE)
endfunction()
