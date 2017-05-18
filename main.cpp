#include "gui/ScreenManager.h"
#include "core/CrawlerState.h"
#include "core/ThreadPool.h"
#include "core/ConcurrentQueue.h"
#include <thread>
#include <iostream>
#include <cassert>
#include "net/Parser.h"
#include "net/WebsiteDownloader.h"

//void increment_progress_bar(CrawlerState* state, int id, int batch_size) {
//    for(int i = 0; i <= batch_size; i++) {
//        std::this_thread::sleep_for(std::chrono::milliseconds(10));
//        state->update_worker_thread_progress(id, i, batch_size);
//    }
//
//    state->update_processed_links_count_by(batch_size);
//    state->update_worker_thread_progress(id, -1, batch_size);
//}

void worker_process(int id, CrawlerState* state, ConcurrentQueue<std::set<std::string>>* batch_queue) {
    std::set<std::string> batch = batch_queue->pop();
    int counter = 0;

    for(auto it = batch.begin(); it != batch.end(); ++it) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::string website_data = WebsiteDownloader::get_website(*it);

        std::set<std::string> new_batch = Parser::parse_links(website_data);

        if(state->get_total_scheduled_links_count() + new_batch.size() > state->get_max_links()) {
            int max_elements = state->get_max_links() - state->get_total_scheduled_links_count();
            assert(max_elements >= 0);
            std::set<std::string> trimmed_new_batch(new_batch.begin(), std::next(new_batch.begin(), max_elements));
            batch_queue->push(trimmed_new_batch);
            state->update_total_scheduled_links_count_by((int) trimmed_new_batch.size());
        }
        else {
            batch_queue->push(new_batch);
            state->update_total_scheduled_links_count_by((int) new_batch.size());
        }

        counter++;
        state->update_worker_thread_progress(id, counter, (int) batch.size());
        state->update_processed_links_count_by(1);
    }

    state->update_worker_thread_progress(id, -1, (int) batch.size());
}

void batch_manager(ThreadPool* pool, CrawlerState* state, ConcurrentQueue<std::set<std::string>>* queue) {
    while(!state->is_done()) {
        for (int i = 0; i < state->get_number_of_worker_threads(); i++) {
            pool->enqueue([i, &state, &queue]() { worker_process(i, state, queue); });
        }
    }
}



int main(int argc, char* argv[]) {
    if(argc != 4) {
        printf("Usage ./%s start_website max_links number_of_worker_threads", argv[0]);
        return -1;
    }

    std::string start_website = argv[1];
    int max_links = atoi(argv[2]);
    int number_of_worker_threads = atoi(argv[3]);

    curl_global_init(CURL_GLOBAL_ALL);

    CrawlerState* crawlerState = new CrawlerState(start_website, max_links, number_of_worker_threads);

    ScreenManager* screenManager = new ScreenManager(crawlerState);
    ThreadPool* threadPool = new ThreadPool((size_t) number_of_worker_threads);
    ConcurrentQueue<std::set<std::string>>* batch_queue = new ConcurrentQueue<std::set<std::string>>();

    /* Start process by putting initial batch */
    std::set<std::string> first_batch;
    first_batch.insert(start_website);
    batch_queue->push(first_batch);
    crawlerState->update_total_scheduled_links_count_by(1);
    ///////////////////////////////////////////

    std::thread display_thread(&ScreenManager::run, screenManager),
                batch_manager_thread(batch_manager, threadPool, crawlerState, batch_queue);

    batch_manager_thread.join();
    display_thread.join();

    delete threadPool;
    delete screenManager;
    delete crawlerState;
    delete batch_queue;
    curl_global_cleanup();
    return 0;
}