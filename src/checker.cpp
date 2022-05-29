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

#include <checker.h>

const vector<DiagnosedFile> Checker::extractURLS()
{
    vector<DiagnosedFile> dFiles;
    vector<string> existingLinks;

    for(const string &file: files) {
        if (verbose) cout << "Reading \"" << file << "\"...\n";

        ifstream reader(file);
        if (reader.is_open() && !reader.fail()) {
            DiagnosedFile currentFile;
            currentFile.path = file;
            currentFile.name = baseName(file);

            string line; long lineNum = 1;
            while (getline(reader, line)) {
                regex self_regex("(http:\\/\\/|ftp:\\/\\/|https:\\/\\/|www\\.)([\\w_-]+(?:(?:\\.[\\w_-]+)+))([\\w.,@?^=%&:\\/~+#-]*[\\w@?^=%&\\/~+#-])?",
                regex_constants::ECMAScript | regex_constants::icase);
                smatch match;
                if (regex_search(line, match, self_regex)) {
                    if (verbose) cout << "\tURL detected: \"" << match[0] << "\", Line:" << lineNum << ", at:" << match.position()+1 << endl;

                    if (std::find(existingLinks.begin(), existingLinks.end(), match[0]) != existingLinks.end()) {
                        if (verbose) dye("\tDuplicate URL detected!\n", warn);
                        if (!duplicateCheck) continue;
                    }

                    currentFile.lineNums.push_back(lineNum);
                    currentFile.positions.push_back(match.position(0)+1);
                    currentFile.allLinks.push_back(match[0]);

                    existingLinks.push_back(match[0]);
                }
                lineNum++;
            }
            dFiles.push_back(currentFile);
        }
        else {
            dye("Failed to open/read \"" + file + "\".\n", error);
        }
    }
    return dFiles;
}


void Checker::checkURLs(const vector<DiagnosedFile> &diagnosedFiles)
{
    if (files.size() < 1) {
        dye("Hmm, weird... files list is empty but for some reason this function was called. ", error);
        throw (001);
    }
    if (diagnosedFiles.size() < 1) {
        dye("Lookes like there's no URLs to check, probably files reading/opening problem. "
            "If you didn't face any files errors then please make sure the files have URLs in them.", error);
        throw (002);
    }

    CURL *curl = curl_easy_init();
    string data;

    if (curl) {
        if (verbose) cout << "Verbose mode is enabled.\n";

        //Time out in seconds
        if (verbose) cout << "Timeout: " << timeout << endl;
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);

        //Follow HTTP redirects if necessary, by default it's disabled
        const string followRedirectsStr = followRedirects ? "YES" : "NO";
        if (verbose) cout << "Follow Redirects?: " << followRedirectsStr << endl;
        if (followRedirects)
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        if (verbose) cout << "Maximum Redirects: " << maxRedirects << endl;
        curl_easy_setopt(curl, CURLOPT_MAXREDIRS, maxRedirects);

        if (CURL_REDIRECT_PROTOCOL_ALL)
            curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, CURLPROTO_ALL);
        else {
            curl_easy_setopt(curl, CURLOPT_REDIR_PROTOCOLS, (CURL_REDIRECT_PROTOCOL_HTTP   ? CURLPROTO_HTTP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_HTTPS  ? CURLPROTO_HTTPS  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_FTP    ? CURLPROTO_FTP    : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_FTPS   ? CURLPROTO_FTPS   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_FILE   ? CURLPROTO_FILE   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_GOPHER ? CURLPROTO_GOPHER : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_IMAP   ? CURLPROTO_IMAP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_IMAPS  ? CURLPROTO_IMAPS  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_LDAP   ? CURLPROTO_LDAP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_LDAPS  ? CURLPROTO_LDAPS  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_POP3   ? CURLPROTO_POP3   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_POP3S  ? CURLPROTO_POP3S  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMP   ? CURLPROTO_RTMP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMPE  ? CURLPROTO_RTMPE  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMPS  ? CURLPROTO_RTMPS  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMPT  ? CURLPROTO_RTMPT  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMPTE ? CURLPROTO_RTMPTE : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTMPTS ? CURLPROTO_RTMPTS : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_RTSP   ? CURLPROTO_RTSP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SCP    ? CURLPROTO_SCP    : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SFTP   ? CURLPROTO_SFTP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SMB    ? CURLPROTO_SMB    : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SMBS   ? CURLPROTO_SMBS   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SMTP   ? CURLPROTO_SMTP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_SMTPS  ? CURLPROTO_SMTPS  : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_TELNET ? CURLPROTO_TELNET : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_TFTP   ? CURLPROTO_TFTP   : 0) |
                                                            (CURL_REDIRECT_PROTOCOL_DICT   ? CURLPROTO_DICT   : 0)
                                                            );
    }


        //IPv4 is much faster than IPv6 when it comes to DNS resolution time
        const string ipv6EnabledStr = ipv6 ? "YES" : "NO";
        if (verbose) cout << "IPv6 Enabled?: " << ipv6EnabledStr << endl;
        if (ipv6)
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V6);
        else
            curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

        //Default scheme is http://, default port is 1080.
        //A numerical IPv6 address must be written within [brackets]
        const string useProxyStr = useProxy ? "YES" : "NO";
        if (verbose) cout << "Use Proxy?: " << useProxyStr << endl;
        if (verbose && useProxy) cout << "Proxy: " << proxy << endl;
        if (useProxy && !proxy.empty())
            curl_easy_setopt(curl, CURLOPT_PROXY, proxy.c_str());

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);


        for (const auto &dFile: diagnosedFiles) {
            const size_t dFileIndex = &dFile-&diagnosedFiles[0];
            cout << "-----------------------------------------------------------------------------------------------------\n";
            cout << (dFileIndex+1) << "/" << diagnosedFiles.size() << " -> Working on file: \"" << dFile.name << "\"" << endl;

            for (const string &URL: dFile.allLinks) {

                const int urlIndex = &URL-&dFile.allLinks[0];

                cout << "\t" << (urlIndex+1) << "/" << dFile.allLinks.size() << " -> Working on URL: \"" << URL << "\"" << endl;
                if (verbose) cout << "\t\tLine:" << dFile.lineNums.at(urlIndex) << ", at:" << dFile.positions.at(urlIndex) << endl;

                curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());

                timer timer;
                CURLcode res_code = curl_easy_perform(curl);
                cout << "\t\tTook: " << timer.getTimeElapsedStr() << endl;

                if (res_code == CURLE_OK) {
                    long http_code;
                    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

                    if (verbose) cout << "\t\tHTTP response code: " << http_code << endl;

                    if (http_code == 200) {
                        if (verbose) dye("\t\tGood link: \"" + URL + "\".\n", done);
                    }
                    else {
                        dye("\nIN FILE -> [ " + dFile.name + " ]\tFIXME!\n" +
                            "\tDEAD LINK: \"" + URL + "\"\n" +
                            "\tLine:" + to_string(dFile.lineNums.at(urlIndex)) + ", at:" +
                            to_string(dFile.positions.at(urlIndex)) +
                            ". Path:\"" + dFile.path + "\"\n\n", warn);
                    }
                }
                else {
                    if (res_code == 28) dye("\t\tRequest was timed out (" + to_string(timeout) + " sec).\n", error);
                    else {
                        const string err_msg = curl_easy_strerror(res_code);
                        dye("\t\t" + err_msg + "\n", error);
                    }
                    if (verbose) cout << "\t\tCURL response code: " << res_code << endl;
                }
            }
        }

        curl_easy_cleanup(curl);
    }
}
