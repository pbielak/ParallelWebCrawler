#pragma once

#include <ncurses.h>
#include <fstream>
#include "../config/GlobalConfig.h"
#include "../core/CrawlerState.h"

using namespace GuiConstants;

class ScreenManager {
private:
    CrawlerState* state;

    void check_terminal_size();
    void load_template();

    void print_at_pos(Point position, int value);
    void print_at_pos(Point position, const char* value);

    void print_worker_info(int i);
    void print_progress_bar(Point position, double percent);
    void clear_progress_bar(Point position);
    void clear_worker_info(int i);

    void print_top_word_info(int i);

public:
    ScreenManager(CrawlerState* _state) : state(_state) {}
    void run();
};