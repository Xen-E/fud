/****************************************************************************
*  Copyright (c) 2022 Xen <xen-dev@pm.me> xen-e.github.io                   *
*  This file is part of the File URLs Doctor project, AKA FUD               *
*  FUD is free software; you can redistribute it and/or modify it under     *
*  the terms of the GNU Lesser General Public License (LGPL) as published   *
*  by the Free Software Foundation; either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*  FUD is distributed in the hope that it will be useful, but WITHOUT       *
*  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    *
*  FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public      *
*  License for more details.                                                *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with this program. If not, see <https://www.gnu.org/licenses>.     *
*****************************************************************************/

#ifndef VERSIONS_H
#define VERSIONS_H

#include <iostream>
#include <string>
#include <vector>

#include <curl/curl.h>

using namespace std;

namespace Architecture {
    const bool i686 = sizeof(void*) != 8 ? true : false;
	const string humanReadable = i686 ? "32bit" : "64bit";
}

namespace Developer {
    const string domain = "xen-e.github.io";
	const string name   = "Xen";
	const string email  = "xen-dev@pm.me";
}

namespace Product {
    const string name       = "File URLs Doctor";
    const string shortName  = "FUD";
    const unsigned int major = 1;
	const unsigned int minor = 0;
    const string version     = to_string(major) + "." + to_string(minor);
}

namespace Libraries {
    const string libcurlVersion(curl_version());
}

namespace usedCompiler {
    #if defined (__llvm__) || defined (__clang__)
        const string name    = "clang";
        const string version =  to_string(__clang_major__) + "." +
                                to_string(__clang_minor__) + "." +
                                to_string(__clang_patchlevel__);

    #elif __GNUC__
        const string name    = "GCC";
        const string version =  to_string(__GNUC__) + "." +
                                to_string(__GNUC_MINOR__) + "." +
                                to_string(__GNUC_PATCHLEVEL__);

    #elif _MSC_VER
        const string name    = "MSVC";
        const string version =  to_string(_MSC_VER);

    #elif __MINGW32__
        const string name    = "MinGW32";
        const string version =  to_string(__MINGW32_MAJOR_VERSION) + "." +
                                to_string(__MINGW32_MINOR_VERSION);

    #elif __MINGW64__
        const string name    = "MinGW64";
        const string version =  to_string(__MINGW64_MAJOR_VERSION) + "." +
                                to_string(__MINGW64_MINOR_VERSION);

    #elif
        const string name    = "Unknown compiler";
        const string version = "N/A";
    #endif
}


#endif // VERSIONS_H
