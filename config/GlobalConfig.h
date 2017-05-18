#pragma once

#include "../util/Point.h"

namespace GuiConstants {
    /* TERMINAL CONSTANTS */
    const int EXPECTED_SCREEN_WIDTH = 128;
    const int EXPECTED_SCREEN_HEIGHT = 24;
    const char* const TEMPLATE_FILE = "template.txt";

    /* USER CONFIG CONSTANTS */
    const Point WEBSITE_POS = Point(2, 17);
    const Point MAX_LINKS_POS = Point(3, 17);
    const Point NUMBER_OF_WORKERS_POS = Point(5, 37);

    /* PROGRESS CONSTANTS */
    const Point CURRENT_LINKS_POS = Point(4, 20);
    const Point PERCENTAL_PROGRESS_POS = Point(4, 33);

    const Point GENERIC_WORKER_CURRENT_LINKS_POS = Point(12, 38);
    const Point GENERIC_WORKER_BATCH_SIZE_POS = Point(12, 46);
    const Point GENERIC_WORKER_THREAD_PROGRESS_POS = Point(12, 52);
    const int PROGRESS_BAR_LENGTH = 73;

    /* RESULTS CONSTANTS */
    const int TOP_WORDS_COUNT = 10;
    const Point GENERIC_TOP_WORD_TXT_POS = Point(12, 7);
}