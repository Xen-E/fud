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

#include <iostream>
#include <filesystem>
#include <string>
#include <algorithm>

#include <checker.h>
#include <colors.h>
#include <versions.h>

using namespace std;
namespace fs = filesystem;

int    timeout          = 30;    //sec
bool   ipv6             = false; //Forces IPv6
bool   followRedirects  = true;  //Follow HTTP redirects?
long   maxRedirects     = -1;    //-1 = infinite | 0 = no redirects.
bool   useProxy         = false; //Use proxy?
string proxy;                    //http:// https:// socks4:// socks4a:// socks5:// socks5h://
bool   verbose          = false;


bool recursiveSearch = false;
bool ANSI = true;

void displayHelp()
{
    dye("\n\t" + Product::name + "\tVersion: " + Product::version + "\t" + Architecture::humanReadable +
        "\n\tCompiled using " + usedCompiler::name + " Version: " + usedCompiler::version +
        "\n\tWritten by " + Developer::name + "<" + Developer::email + ">\t" + Developer::domain + "\n\n", dim);

    cout << "\t ======================================================================================\n"
            "\t|      Argument     |         Value       |Default|              Description           |\n"
            "\t ======================================================================================\n"
            "\t| --timeout         | Number              |  30   | Time in seconds before timeout     |\n"
            "\t| --recursive       | true, false         | false | Scan directories recursively       |\n"
            "\t| --ipv6            | true, false         | false | Enables IPv6 instead of IPv4       |\n"
            "\t| --followredirects | true, false         | true  | Follow URL redirections?           |\n"
            "\t| --maxredirects    | Number              |  -1   | Redirections limit (-1 = infinite) |\n"
            "\t| --ansi            | true, false         | auto  | Enables ANSI escape sequences      |\n"
            "\t| --verbose         | true, false         | false | Enables verbose mode               |\n"
            "\t| --proxy           | SCHEME://PROXY:PORT | NULL  | Use proxy to make requests, if no  |\n"
            "\t|                   | Schemes:            |       | port is provided then 1080 will be |\n"
            "\t|                   |   http:// (default) |       | used. and if no scheme is provided |\n"
            "\t|                   |   https://          |       | http:// will be used. A numerical  |\n"
            "\t|                   |   socks4://         |       | IPv6 must be written within:  [ ]  |\n"
            "\t|                   |   socks4a://        |       | https://0.0.0.0:1234               |\n"
            "\t|                   |   socks5://         |       | socks4://proxy.com:80              |\n"
            "\t|                   |   socks5h://        |       | socks5h://[0:0:0:0:0:0:0:0]:8080   |\n"
            "\t ======================================================================================\n\n";

    dye("\t" + Libraries::libcurlVersion + "\n", dim);
}

size_t number_of_files_in_directory(fs::path path, bool recursive)
{
    using fp = bool (*)(const fs::path&);
    if (recursive) {
        using fs::recursive_directory_iterator;
        return count_if(recursive_directory_iterator(path), {}, (fp)fs::is_regular_file);
    }
    else {
        using fs::directory_iterator;
        return count_if(directory_iterator(path), {}, (fp)fs::is_regular_file);
    }
}

int main(int argc, char *argv[])
{
    //Enable ANSI escape sequences if not running on Windows
    #ifndef WIN32
        ANSI = true;
    #else
        ANSI = false;
    #endif

    if (argc == 1 ||
        (argc == 2 && strcmp(argv[1], "--help") == 0) ||
        (argc == 2 && strcmp(argv[1], "--version") == 0)) {
        displayHelp();
        return 0;
    }
    else {
        vector<string> nonArgs;
        for (int a = 1; a < argc; a++) {
            const string arg_str(argv[a]);

            if (arg_str.find("--recursive=") != string::npos) {
                string arg_r(arg_str.substr(12));
                for (auto &c: arg_r) { c = tolower(c); }
                try {
                    if ((arg_r.length() == 4 && arg_r.find("true") != string::npos) ||
                        (arg_r.length() == 5 && arg_r.find("false") != string::npos))
                        recursiveSearch = arg_r == "true" ? true : false;
                    else {
                        dye("Unknown Recursive argument value." + arg_r + "\n", error);
                        return -1;
                    }
                }
                catch (invalid_argument const &ex) {
                    dye("Recursive invalid argument: " + arg_str + "\n", error);
                }
            }
            else if (arg_str.find("--timeout=") != string::npos) {
                try {
                    size_t pos;
                    const long t = stol(arg_str.substr(10), &pos);
                    if (pos < arg_str.substr(10).size()) {
                        dye("Trailing characters after Timeout number: " + to_string(t) + "\n", error);
                        return -1;
                    }
                    if (t < 0) {
                        dye("Timeout number cannot be negative: " + to_string(t) + "\n", error);
                        return -1;
                    }
                    timeout = t;
                }
                catch (invalid_argument const &ex) {
                    dye("Timeout invalid number: " + arg_str + "\n", error);
                    return -1;
                }
                catch (out_of_range const &ex) {
                    dye("Timeout number out of range: " + arg_str + "\n", error);
                    return -1;
                }
            }
            else if (arg_str.find("--ipv6=") != string::npos) {
                string arg_ipv6(arg_str.substr(7));
                for (auto &c: arg_ipv6) { c = tolower(c); }
                try {
                    if ((arg_ipv6.length() == 4 && arg_ipv6.find("true") != string::npos) ||
                        (arg_ipv6.length() == 5 && arg_ipv6.find("false") != string::npos))
                        ipv6 = arg_ipv6 == "true" ? true : false;
                    else {
                        dye("Unknown IPv6 argument value." + arg_ipv6 + "\n", error);
                        return -1;
                    }
                }
                catch (invalid_argument const &ex) {
                    dye("IPv6 invalid argument: " + arg_str + "\n", error);
                }
            }
            else if (arg_str.find("--followredirects=") != string::npos) {
                string arg_fr(arg_str.substr(18));
                for (auto &c: arg_fr) { c = tolower(c); }
                try {
                    if ((arg_fr.length() == 4 && arg_fr.find("true") != string::npos) ||
                        (arg_fr.length() == 5 && arg_fr.find("false") != string::npos))
                        followRedirects = arg_fr == "true" ? true : false;
                    else {
                        dye("Unknown Follow Redirects argument value." + arg_fr + "\n", error);
                        return -1;
                    }
                }
                catch (invalid_argument const &ex) {
                    dye("Follow Redirects invalid argument: " + arg_str + "\n", error);
                }
            }
            else if (arg_str.find("--maxredirects=") != string::npos) {
                try {
                    size_t pos;
                    const long m = stol(arg_str.substr(15), &pos);
                    if (pos < arg_str.substr(15).size()) {
                        dye("Trailing characters after Maximum Redirects number: " + to_string(m) + "\n", error);
                        return -1;
                    }
                    if (m < -1) {
                        dye("Maximum Redirects number cannot be lower than \"-1\": " + to_string(m) + "\n", error);
                        return -1;
                    }
                    maxRedirects = m;
                }
                catch (invalid_argument const &ex) {
                    dye("Maximum Redirects invalid number: " + arg_str + "\n", error);
                    return -1;
                }
                catch (out_of_range const &ex) {
                    dye("Maximum Redirects number out of range: " + arg_str + "\n", error);
                    return -1;
                }
            }
            else if (arg_str.find("--ansi=") != string::npos) {
                string arg_ansi(arg_str.substr(7));
                for (auto &c: arg_ansi) { c = tolower(c); }
                try {
                    if ((arg_ansi.length() == 4 && arg_ansi.find("true") != string::npos) ||
                        (arg_ansi.length() == 5 && arg_ansi.find("false") != string::npos))
                        ANSI = arg_ansi == "true" ? true : false;
                    else {
                        dye("Unknown ANSI argument value." + arg_ansi + "\n", error);
                        return -1;
                    }
                }
                catch (invalid_argument const &ex) {
                    dye("Verbose invalid argument: " + arg_str + "\n", error);
                }
            }
            else if (arg_str.find("--verbose=") != string::npos) {
                string arg_vb(arg_str.substr(10));
                for (auto &c: arg_vb) { c = tolower(c); }
                try {
                    if ((arg_vb.length() == 4 && arg_vb.find("true") != string::npos) ||
                        (arg_vb.length() == 5 && arg_vb.find("false") != string::npos))
                        verbose = arg_vb == "true" ? true : false;
                    else {
                        dye("Unknown Verbose argument value." + arg_vb + "\n", error);
                        return -1;
                    }
                }
                catch (invalid_argument const &ex) {
                    dye("Verbose invalid argument: " + arg_str + "\n", error);
                }
            }
            else if (arg_str.find("--proxy=") != string::npos) {
                const string arg_proxy(arg_str.substr(8));
                try {
                    useProxy = true;
                    proxy = arg_proxy;
                }
                catch (invalid_argument const &ex) {
                    dye("Proxy invalid argument: " + arg_str + "\n", error);
                }
            }
            else { nonArgs.push_back(arg_str); }
        }

        if (nonArgs.size() > 0) {
            vector<string> paths;
            for (const auto &path: nonArgs) { //Loop through files/dirs

                if (fs::is_directory(path)) { //Checks if path is directory

                    //Warn if directory is big
                    const size_t dir_size = number_of_files_in_directory(path, recursiveSearch);
                    if (dir_size > 1000) {
                        string dir_size_answer;
                        ask_about_dir:
                        dye("\"" + path + "\" has " + to_string(dir_size) + " files, "
                            "This can take a while especially if each file has URLs, "
                            "Do you want to continue?(y, n): ", warn);
                        cin >> dir_size_answer;
                        if (dir_size_answer.find("y") != string::npos) {
                            cout << "OK. Continuing...\n";
                        }
                        else if (dir_size_answer.find("n") != string::npos) {
                            cout << "Cancelling...\n";
                            return 0;
                        }
                        else {
                            cout << "Use 'y' for Yes or 'n' for No.\n";
                            goto ask_about_dir;
                        }
                    }

                    if (recursiveSearch) {
                        for (const auto &p: fs::recursive_directory_iterator(path)) {
                            if (fs::is_regular_file(p)) paths.push_back(p.path().string());
                        }
                    }
                    else {
                        for (const auto &p: fs::directory_iterator(path)) {
                            if (fs::is_regular_file(p)) paths.push_back(p.path().string());
                        }
                    }
                }
                else { //Path is not a directory
                    if (fs::is_regular_file(path)) { //Path is a regular file
                        paths.push_back(path);
                    }
                    else { //Path is neither
                        dye("File/Directory is invalid: \"" + path + "\"\n", warn);
                    }
                }
            }
            if (paths.size() > 0) {
                try {
                    cout << "initializing the checker...\n";
                    Checker checker(paths);
                    cout << "Starting...\n";
                    checker.checkURLs(checker.extractURLS());
                }
                catch (int err_code) {
                    dye(Product::shortName + " error code: " + to_string(err_code) + "\n", error);
                    return -1;
                }
            }
            else {
                dye("Looks like there's no files or directories to check. "
                    "Please make sure the paths you entered are valid.\n", error);
                return -1;
            }
        }
        else {
            dye("No files or directories detected, there's only arguments.\n", error);
            return -1;
        }
    }

    return 0;
}
