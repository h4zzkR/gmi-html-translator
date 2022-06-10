#pragma once

#include <exe/executors/thread_pool.hpp>
#include <filesystem>

#include "support/copy_batch.hpp"
#include "support/parts.hpp"

#include <fstream>
#include <condition_variable>
#include <memory>

namespace fs = std::filesystem;
using namespace exe::executors;

/*
 * Отдельный тред, который последовательно исполняет filesystem write задачи.
 */
class FSWriter {
public:

    /*
     * batch мувается в лямбду, а сама лямбда уже внутри Execute аллоцируется на куче,
     * управление памятью переходит к тредпулу, поэтому объект живет до тех пор, пока не исполнится.
     */
    void Copy(CopyBatch batch) {
        Execute(thread_,
                [this, batch2run = std::move(batch)]() {
                    for (const fs::directory_entry &dir_entry: batch2run.buckets) {

                        auto new_path = ReplaceRoots(root_dir_, new_root_dir_, dir_entry.path());

                        if (dir_entry.is_directory()) {
                            fs::create_directory(new_path);
                        } else {
                            fs::copy_file(dir_entry, new_path, fs::copy_options::overwrite_existing);
                        }

                    }
                }
        );
    }

    void Create(const fs::path &fpath, std::string content) {
        Execute(thread_, [this, content2write = std::move(content), fpath]() {

            if (!exists(fpath.parent_path())) {
                // Copy еще не успел отработать, создадим иерархию директорий
                fs::create_directories(fpath.parent_path());
            }

            std::ofstream outfile(fpath.string());
            outfile << content2write << std::endl;
            outfile.close();
        });
    }

    FSWriter() = default;

    FSWriter(const fs::path &root_dir, const fs::path &new_root_dir) : root_dir_(root_dir),
                                                                       new_root_dir_(new_root_dir) {
    }

    ~FSWriter() {
        thread_.WaitIdle();
        thread_.Stop();
    }

private:
    fs::path root_dir_, new_root_dir_;
    ThreadPool thread_{1}; // 1 worker for copying
};