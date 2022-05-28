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

#ifndef CHECKER_H
#define CHECKER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <regex>
#include <chrono>

#include <curl/curl.h>
#include <colors.h>

using namespace std;
using namespace chrono;

extern int    timeout;
extern bool   ipv6;
extern bool   followRedirects;
extern long   maxRedirects;
extern bool   useProxy;
extern string proxy;
extern bool   verbose;


class timer
{
using clock           = steady_clock;
using time_point_type = time_point<clock, milliseconds>;

public:
    timer () { start = time_point_cast<milliseconds>(clock::now()); }

    long getTimeElapsed() {
        auto end = clock::now();
        return duration_cast<milliseconds>(end - start).count();
    }
    string getTimeElapsedStr() {
        const long t(getTimeElapsed());
        if (t < 1000) return to_string(t) + " milliseconds";
        else if (t >= 1000 && t < 60000) return to_string(t/1000) + " second(s)";
        else return to_string(t/60000) + " minute(s)";
    }

private:
    time_point_type start;
};

struct DiagnosedFile
{
    string path, name;
    vector<long> lineNums;
    vector<int> positions;
    vector<string> allLinks;
};
class Checker
{
private:
    inline static vector<string> files;

    static size_t writeCallback(const char *in, size_t size, size_t num, string *out)
    {
        const size_t totalBytes(size * num);
        out->append(in, totalBytes);
        return totalBytes;
    }
    static const string baseName(const string &filePath)
    {
        return filePath.substr(filePath.find_last_of("/\\") + 1);
    }

public:
    Checker(const vector<string> &files) { this->files = files; }
    const vector<DiagnosedFile> extractURLS();
    static void checkURLs(const vector<DiagnosedFile> &diagnosedFiles);
};

#endif // CHECKER_H
