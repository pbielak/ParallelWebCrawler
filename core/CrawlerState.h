#pragma once

#include <condition_variable>
#include <string>
#include <vector>
#include <map>
#include "../config/GlobalConfig.h"

class CrawlerState {
private:
    std::condition_variable cv;
    std::mutex cv_mtx;

    std::string start_website;
    int processed_links_count;
    int max_links;

    int total_scheduled_links_count;

    int number_of_worker_threads;
    std::vector<std::pair<int, int>> worker_states;
    std::map<std::string, int> top_words;

public:
    CrawlerState(std::string _start_website, int _max_links, int _number_of_worker_threads);
    void update_processed_links_count_by(int delta);
    void update_worker_thread_progress(int worker_id, int links_processed, int batch_size);
    void update_total_scheduled_links_count_by(int delta);
    void update_top_words(std::string word, int delta_count);

    std::string get_start_website();
    int get_processed_links_count();
    int get_max_links();
    int get_number_of_worker_threads();
    int get_current_progress();
    int get_total_scheduled_links_count();

    int get_worker_current_links(int worker_id);
    int get_worker_batch_size(int worker_id);
    double get_worker_thread_progress(int worker_id);

    std::pair<std::string, int> get_top_word(int rank);

    bool is_new_data_available();
    bool is_done();

    void wait_for_finish();
};