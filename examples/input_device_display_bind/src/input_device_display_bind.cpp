#include <iostream>
#include <cstdio>
#include <sstream>
#include <getopt.h>

#include "input_manager.h"

void Help(const std::string title = {})
{
    std::cout << title << std::endl;
    printf(
"Usage\n"
"-q     --query                             Query input device and display infomation\n"
"-s     --set 'inputDeivceId displayId'     Query input device and display infomation\n"
    );
}

int main(int argc, char *argv[])
{
    struct option headOptions[] = {
        {"query", no_argument, NULL, 'q'},
        {"set", required_argument, NULL, 's'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    if (argc < 2) {
        Help();
        return 0;
    }

    int32_t optionIndex = 0;
    optind = 0;
    int32_t c = 0;
    if ((c = getopt_long(argc, argv, "qs:h?", headOptions, &optionIndex)) != -1) {
        switch (c) {
            case 'q': {
                printf("query\n");
                break;
            }
            case 's': {
                std::istringstream iss(optarg);
                int32_t deviceId = -1;
                int32_t displayId = -1;
                iss >> deviceId >> displayId;
                if (iss.fail()) {
                    Help("Arg is not right");
                    return -1;
                }
                printf("deviceId:%d, displayId:%d\n", deviceId, displayId);
                break;
            }
            case 'h':
            default: {
                Help();
                break;
            }
        }
    };
    return 0;
}