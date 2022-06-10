#pragma once

#include <queue>
#include <iostream>

#include <exe/executors/thread_pool.hpp>

#include "fs_writer.hpp"
#include "translator.hpp"
#include <filesystem>

namespace fs = std::filesystem;
using namespace exe::executors;

/*
 * Класс-интерфейс, получает аргументы на вход,
 * манипулирует другими сущностями
 */
class SiteGenerator {

public:
    SiteGenerator(const std::string &in_dir, const std::string &out_dir) :
            in_dir_(in_dir), out_dir_(out_dir), cp_(in_dir_, out_dir_) {

        // запуск обхода директории
        Execute(pool_, [this]() {
            Traverse();
        });

        pool_.WaitIdle();
        pool_.Stop();
    }

private:

    void SubmitCopy(CopyBatch batch) {
        cp_.Copy(std::move(batch));
    }

    void SubmitWrite(const fs::path &path, std::string content) {
        cp_.Create(path, std::move(content));
    }

    void SubmitTranslate(const fs::path &fpath, const fs::path &new_root, const fs::path &dir_path) {
        Execute(pool_, [this, dir_path, fpath, new_root]() {
            auto html_content = Translator().GmI2Html(dir_path, out_dir_);
            auto path = ReplaceRoots(in_dir_, out_dir_, dir_path).replace_extension("html");
            SubmitWrite(path, html_content);
        });
    }

    /*
     * Обход дерева каталога: файлы, которые нужно просто скопировать, распределяются по батчам
     * и отправляются на исполнение.
     */
    void Traverse() {
        CopyBatch batch;
        for (const fs::directory_entry &dir_entry:
                fs::recursive_directory_iterator(in_dir_)) {

            auto ss = dir_entry.path().string();

            if (!dir_entry.is_directory() and isGMI(dir_entry.path())) {
                // запуск транслятора gmi2html
                SubmitTranslate(dir_entry.path(), out_dir_, dir_entry.path());
            } else {
                if (!batch.Add(dir_entry)) {
                    SubmitCopy(std::move(batch)); // push batch to FSWriter-queue
                    batch.Add(dir_entry);
                }
            }
        }

        if (batch.ptr != 0) {
            SubmitCopy(std::move(batch));
        }
    }


private:
    fs::path in_dir_, out_dir_;
    ThreadPool pool_{4};
    FSWriter cp_;
};
