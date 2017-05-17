#include "gui/ScreenManager.h"
#include "core/CrawlerState.h"
#include "core/ThreadPool.h"
#include <thread>
#include <iostream>

void increment_progress_bar(CrawlerState* state, int id, int batch_size) {
    for(int i = 0; i <= batch_size; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        state->update_worker_thread_progress(id, i, batch_size);
    }

    state->update_current_links_count_by(batch_size);
    state->update_worker_thread_progress(id, -1, batch_size);
}

int main(int argc, char* argv[]) {
    if(argc != 4) {
        printf("Usage ./%s start_website max_links number_of_worker_threads", argv[0]);
        return -1;
    }

    std::string start_website = argv[1];
    int max_links = atoi(argv[2]);
    int number_of_worker_threads = atoi(argv[3]);

    CrawlerState crawlerState(start_website, max_links, number_of_worker_threads);
    ScreenManager manager(&crawlerState);
    ThreadPool tp((size_t) number_of_worker_threads);

    std::thread display_thread(&ScreenManager::run, &manager);

    std::vector<int> batches;
    batches.push_back(30);
    batches.push_back(120);
    batches.push_back(300);
    batches.push_back(50);


    for(int i = 0; i < number_of_worker_threads; i++) {
        tp.enqueue([&crawlerState, i, &batches]() { increment_progress_bar(&crawlerState, i, batches[i]); });
    }

    display_thread.join();

    return 0;
}