::I made this batch file to generate a very small executable using the static version of CURL.
::If you want to use it make sure to replace the path in "-L" flag with your static CURL library
::Note: If you want to reach a very small size, then you should build the library only with basic/core functions
::Try this: ./configure --disable-shared --enable-static \
::--disable-dependency-tracking --enable-ipv6 --disable-ftp --disable-file \
::--disable-ldap --disable-ldaps --disable-rtsp --disable-proxy --disable-dict \
::--disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp \
::--disable-gopher --disable-sspi --disable-manual --disable-zlib --without-zlib
::This will produce a very small static CURL library for your future projects.
::
:: <3 Xen <xen-dev@pm.me> 2022 xen-e.github.io
::
@echo off
ECHO Make sure to read this batch file source code!
ECHO.
ECHO Checking if bin folder exists or creating one...
if not exist "bin" mkdir "bin" ::Making sure "bin" folder exists before building
ECHO OK, building...
g++ -s -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -DNDEBUG -DCURL_STATICLIB src/*.cpp -o bin/fud-small-static -Iinclude -LC:\\Libraries\\curl-7.75.0\\gcc-64-static\\lib -lcurl -lnghttp2 -lidn2 -lpsl -lssl -lcrypto -lcrypto -lgdi32 -lzstd -lbrotlidec -lws2_32
ECHO Done.