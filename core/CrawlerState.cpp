#include "CrawlerState.h"

CrawlerState::CrawlerState(std::string _start_website,
                           int _max_links,
                           int _number_of_worker_threads)  : processed_links_count(0),
                                                             total_scheduled_links_count(0),
                                                             max_links(_max_links),
                                                             start_website(_start_website),
                                                             number_of_worker_threads(_number_of_worker_threads) {
    worker_states.resize((unsigned long) number_of_worker_threads, std::make_pair(0, std::numeric_limits<int>::max()));
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





















