#include "util/common.h"


int main(int argc, char* argv[]) {
    if(argc != 4) {
        printf("Usage ./%s start_website max_links number_of_worker_threads", argv[0]);
        return -1;
    }

    std::string start_website = argv[1];
    const int max_links = atoi(argv[2]);
    const int number_of_worker_threads = atoi(argv[3]);

    assert(max_links < 5000);
    assert(number_of_worker_threads > 0 && number_of_worker_threads <= 10);

    curl_global_init(CURL_GLOBAL_ALL);

    ConcurrentQueue<std::set<std::string>>* batch_queue = new ConcurrentQueue<std::set<std::string>>();
    CrawlerState* state = new CrawlerState(start_website, max_links, number_of_worker_threads);
    ThreadPool* pool = new ThreadPool(number_of_worker_threads);
    ScreenManager* screenManager = new ScreenManager(state);


    std::set<std::string> initial_batch; initial_batch.insert(start_website);
    batch_queue->push(initial_batch);
    state->update_total_scheduled_links_count_by(1);

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