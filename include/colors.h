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

#ifndef COLORS_H
#define COLORS_H

#include <iostream>
#include <string>

#ifdef WIN32
    #include <windows.h>
#endif

using namespace std;

enum Color {
    bold        = 0x0001,
    dim         = 0x0002,
    underline   = 0x0003,
    warn        = 0x0004,
    done        = 0x0005,
    error       = 0x0006
};

extern bool ANSI; //Global

static void dye(const string &msg, Color color)
{
    /*
    for(int k=1; k<=255;k++) {
        SetConsoleTextAttribute(commandLine, k);
        cout << k << " -> hahahahahahahahahaaha" << endl;
    }*/

    if (ANSI) {
        switch (color) {
        case bold: cout << "\e[1m" << msg << "\e[0m"; break;
        case dim: cout << "\e[2m" << msg << "\e[0m"; break;
        case underline: cout << "\e[4m" << msg << "\e[0m"; break;
        case warn: cout << "\e[33m" << msg << "\e[0m"; break;
        case done: cout << "\e[32m" << msg << "\e[0m"; break;
        case error: cout << "\e[31m" << msg << "\e[0m"; break;
        default: cout << "\e[0m" << msg << "\e[0m";
        }
    }
    else {
        #ifdef WIN32
        HANDLE commandLine = GetStdHandle(STD_OUTPUT_HANDLE);
        switch (color) {
        case bold:
            SetConsoleTextAttribute(commandLine, 15);
            cout << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        case dim:
            SetConsoleTextAttribute(commandLine, 8);
            cout << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        case underline:
            SetConsoleTextAttribute(commandLine, 15);
            cout << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        case warn:
            SetConsoleTextAttribute(commandLine, 6);
            cout << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        case done:
            SetConsoleTextAttribute(commandLine, 10);
            cout << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        case error:
            SetConsoleTextAttribute(commandLine, 12);
            cerr << msg;
            SetConsoleTextAttribute(commandLine, 7);
            break;
        default:
            SetConsoleTextAttribute(commandLine, 7);
            cout << msg;
            break;
        }
        #else
            cout << msg;
        #endif
    }
}

#endif // COLORS_H
