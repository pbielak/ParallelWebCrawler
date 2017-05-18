#pragma once

#include "../gui/ScreenManager.h"
#include "../core/ThreadPool.h"
#include "../core/ConcurrentQueue.h"
#include <iostream>
#include <cassert>
#include "../net/Parser.h"
#include "../net/WebsiteDownloader.h"
#include "../core/WordCounter.h"

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

            WordCounter::update_crawler_state(state, website_data);

            counter++;
            state->update_worker_thread_progress(id, counter, (int) batch.size());
            state->update_processed_links_count_by(1);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
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

