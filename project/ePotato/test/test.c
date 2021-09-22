#include "wlan_test.h"
#include "record_test.h"
#include "play_test.h"
#include "http_test.h"

#include "test.h"

void allTest()
{
    // wlan test
    wlanTest();

    // record test
    recordTest();

    // play test
    playTest();

    // http test
    httpTest();
}