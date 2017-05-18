#include "ScreenManager.h"

void ScreenManager::run() {
    initscr();

    check_terminal_size();
    load_template();

    attron(A_BOLD);
    print_at_pos(WEBSITE_POS, state->get_start_website().c_str());
    print_at_pos(MAX_LINKS_POS, state->get_max_links());
    print_at_pos(NUMBER_OF_WORKERS_POS, state->get_number_of_worker_threads());

    while(state->is_new_data_available()) {
        print_at_pos(CURRENT_LINKS_POS, state->get_processed_links_count());
        print_at_pos(PERCENTAL_PROGRESS_POS, state->get_current_progress());

        for (int i = 0; i < state->get_number_of_worker_threads(); ++i) {
            print_worker_info(i);
        }

        refresh();

        if(state->is_done()) {
            break;
        }
    }

    getch();
    endwin();
}

void ScreenManager::print_worker_info(int i) {
    if(state->get_worker_current_links(i) == -1) {
        clear_worker_info(i);
        return;
    }

    print_at_pos(GENERIC_WORKER_CURRENT_LINKS_POS.move(i, 0), state->get_worker_current_links(i));
    print_at_pos(GENERIC_WORKER_BATCH_SIZE_POS.move(i, 0), state->get_worker_batch_size(i));
    print_progress_bar(GENERIC_WORKER_THREAD_PROGRESS_POS.move(i, 0), state->get_worker_thread_progress(i));
}

void ScreenManager::check_terminal_size() {
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    getmaxyx(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH);

    if(SCREEN_WIDTH != EXPECTED_SCREEN_WIDTH || SCREEN_HEIGHT != EXPECTED_SCREEN_HEIGHT) {
        printw("The terminal should have a size of %d x %d chars", EXPECTED_SCREEN_WIDTH, EXPECTED_SCREEN_HEIGHT);
        getch();
        endwin();

        exit(-1);
    }
}

void ScreenManager::load_template() {
    std::fstream template_window(TEMPLATE_FILE);
    if(!template_window.is_open()) {
        printw("Error while opening template file.");
        getch();
        endwin();

        exit(-1);
    }

    std::string line;
    for (int i = 0; i <= EXPECTED_SCREEN_HEIGHT; ++i) {
        std::getline(template_window, line);
        mvprintw(i, 0, line.c_str());
    }

    template_window.close();
    refresh();
}

void ScreenManager::print_at_pos(Point position, int value) {
    char BUF[100];
    sprintf(BUF, "%d", value);
    print_at_pos(position, BUF);
}

void ScreenManager::print_at_pos(Point position, const char* value) {
    mvprintw(position.get_x(), position.get_y(), value);
    move(0, 0);
}

void ScreenManager::print_progress_bar(Point position, double percent) {
    move(position.get_x(), position.get_y());

    int current_len = (int) (PROGRESS_BAR_LENGTH * percent);
    for(int i = 0; i < current_len - 1; ++i) {
        printw("=");
        move(position.get_x(), position.get_y() + i + 1);
    }

    printw(">");
    move(0, 0);
}

void ScreenManager::clear_progress_bar(Point position) {
    move(position.get_x(), position.get_y());
    for(int i = 0; i < PROGRESS_BAR_LENGTH; ++i) {
        printw(" ");
        move(position.get_x(), position.get_y() + i + 1);
    }

    move(0, 0);
}

void ScreenManager::clear_worker_info(int i) {
    print_at_pos(GENERIC_WORKER_CURRENT_LINKS_POS.move(i, 0), "     ");
    print_at_pos(GENERIC_WORKER_BATCH_SIZE_POS.move(i, 0), "     ");
    clear_progress_bar(GENERIC_WORKER_THREAD_PROGRESS_POS.move(i, 0));
}















