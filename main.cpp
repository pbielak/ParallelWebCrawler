#include "gui/ScreenManager.h"
#include "core/CrawlerState.h"
#include "core/ThreadPool.h"
#include "core/ConcurrentQueue.h"
#include <thread>
#include <iostream>
#include <cassert>
#include "net/Parser.h"
#include "net/WebsiteDownloader.h"

void worker_process(int id, CrawlerState* state, ConcurrentQueue<std::set<std::string>>* batch_queue) {
    while(!state->is_done()) {
        std::set<std::string> batch;
        try {
            batch = batch_queue->pop();
        } catch(std::runtime_error e) {
            return;
        }

        int counter = 0;

        for (auto it = batch.begin(); it != batch.end(); ++it) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            std::string website_data = WebsiteDownloader::get_website(*it);


            if (state->get_total_scheduled_links_count() < state->get_max_links()) {
                std::set<std::string> new_batch = Parser::parse_links(website_data);

                if (state->get_total_scheduled_links_count() + new_batch.size() > state->get_max_links()) {
                    int max_elements = state->get_max_links() - state->get_total_scheduled_links_count();
                    assert(max_elements >= 0);
                    std::set<std::string> trimmed_new_batch(new_batch.begin(),
                                                            std::next(new_batch.begin(), max_elements));
                    batch_queue->push(trimmed_new_batch);
                    state->update_total_scheduled_links_count_by((int) trimmed_new_batch.size());
                }
                else {
                    batch_queue->push(new_batch);
                    state->update_total_scheduled_links_count_by((int) new_batch.size());
                }
            }

            // TODO word count

            counter++;
            state->update_worker_thread_progress(id, counter, (int) batch.size());
            state->update_processed_links_count_by(1);
        }

        state->update_worker_thread_progress(id, -1, (int) batch.size());
    }
}

void batch_manager(ThreadPool* pool, CrawlerState* state, ConcurrentQueue<std::set<std::string>>* queue) {
    for (int i = 0; i < state->get_number_of_worker_threads(); i++) {
        pool->enqueue(worker_process, i, state, queue);
    }

    state->wait_for_finish();
    queue->terminate_queue();
}



int main(int argc, char* argv[]) {
    if(argc != 4) {
        printf("Usage ./%s start_website max_links number_of_worker_threads", argv[0]);
        return -1;
    }

    std::string start_website = argv[1];
    const int max_links = atoi(argv[2]);
    const int number_of_worker_threads = atoi(argv[3]);

    curl_global_init(CURL_GLOBAL_ALL);

    ConcurrentQueue<std::set<std::string>>* batch_queue = new ConcurrentQueue<std::set<std::string>>();
    CrawlerState* state = new CrawlerState(start_website, max_links, number_of_worker_threads);
    ThreadPool* pool = new ThreadPool(number_of_worker_threads);
    ScreenManager* screenManager = new ScreenManager(state);


    std::set<std::string> initial_batch; initial_batch.insert(start_website);
    batch_queue->push(initial_batch);
    state->update_total_scheduled_links_count_by(1);
    ///////////////////////////////////////////

    std::thread display_thread(&ScreenManager::run, screenManager);

    batch_manager(pool, state, batch_queue);

    display_thread.join();

    delete pool;
    delete batch_queue;
    delete screenManager;
    delete state;
    curl_global_cleanup();

    return 0;
}