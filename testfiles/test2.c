/*
* Can be used to test duplicated URLs
*/
#include <stdio.h>

#define URL "https://www.example.org"

int main() {
	//prints URLs "https://www.example.org" and "www.example.com"
	printf("URL1: %s\nURL2: %s\n", URL, "www.example.com");

	return 0;
}