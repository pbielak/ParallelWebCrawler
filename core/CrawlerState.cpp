#include "CrawlerState.h"

CrawlerState::CrawlerState(std::string _start_website,
                           int _max_links,
                           int _number_of_worker_threads)  : processed_links_count(0),
                                                             total_scheduled_links_count(0),
                                                             max_links(_max_links),
                                                             start_website(_start_website),
                                                             number_of_worker_threads(_number_of_worker_threads) {
    worker_states.resize((unsigned long) number_of_worker_threads, std::make_pair(-1, -1));
}

void CrawlerState::update_processed_links_count_by(int delta) {
    std::unique_lock<std::mutex> lk(cv_mtx);
    processed_links_count += delta;
    cv.notify_one();
}

void CrawlerState::update_worker_thread_progress(int worker_id, int links_processed, int batch_size) {
    std::unique_lock<std::mutex> lk(cv_mtx);
    worker_states[worker_id] = std::make_pair(links_processed, batch_size);
    cv.notify_one();
}

void CrawlerState::update_total_scheduled_links_count_by(int delta) {
    std::unique_lock<std::mutex> lk(cv_mtx);
    total_scheduled_links_count += delta;
}

void CrawlerState::update_top_words(std::string word, int delta_count) {
    std::unique_lock<std::mutex> lk(cv_mtx);
    top_words[word] += delta_count;
    cv.notify_one();
}

int CrawlerState::get_processed_links_count() {
    return processed_links_count;
}

int CrawlerState::get_current_progress() {
    return (int) ((processed_links_count * 100.0) / max_links);
}

std::string CrawlerState::get_start_website() {
    return start_website;
}

int CrawlerState::get_max_links() {
    return max_links;
}

bool CrawlerState::is_new_data_available() {
    std::unique_lock<std::mutex> lk(cv_mtx);
    cv.wait(lk);
    return true;
}

bool CrawlerState::is_done() {
    return processed_links_count >= max_links;
}

int CrawlerState::get_number_of_worker_threads() {
    return number_of_worker_threads;
}

double CrawlerState::get_worker_thread_progress(int worker_id) {
    return worker_states[worker_id].first / (double) worker_states[worker_id].second;
}

int CrawlerState::get_worker_current_links(int worker_id) {
    return worker_states[worker_id].first;
}

int CrawlerState::get_worker_batch_size(int worker_id) {
    return worker_states[worker_id].second;
}

int CrawlerState::get_total_scheduled_links_count() {
    return total_scheduled_links_count;
}

void CrawlerState::wait_for_finish() {
    std::unique_lock<std::mutex> lk(cv_mtx);
    cv.wait(lk, [this] { return is_done(); });
}

std::pair<std::string,int> CrawlerState::get_top_word(int rank) {
    if(top_words.empty()) {
        return std::make_pair(nullptr, -1);
    }

    std::vector<std::pair<std::string, int>> ranked_words;

    for(auto it = top_words.begin(); it != top_words.end(); ++it) {
        ranked_words.push_back(*it);
    }

    if(ranked_words.size() < rank) {
        return std::make_pair(nullptr, -1);
    }

    std::sort(ranked_words.begin(), ranked_words.end(), [=](std::pair<std::string, int>& a, std::pair<std::string, int>& b) {
        return a.second > b.second;
    });

    return ranked_words[rank];
}

























