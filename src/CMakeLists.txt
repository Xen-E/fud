#Source files should be listed here under "srcFiles"
set(srcFiles main.cpp checker.cpp)

#this is for static linking only, if you're building a
#shared version then remove.
add_definitions ( -DCURL_STATICLIB )

#Processing source files
add_executable(FUD ${srcFiles})

#"curl-config" application can be used to detect required libraries
target_link_libraries ( FUD ${CURL_LIBRARIES} )
