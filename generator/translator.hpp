#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <iostream>

#include "support/parts.hpp"

namespace fs = std::filesystem;

class Translator {

    /*
     * Логика обработки ссылок сложнее остальных тегов, поэтому вынесем обработку в этот метод.
     */
    void LinkConnector(std::string line, const fs::path &new_root = "") {
        std::string link;
        auto found_start = line.find(' ');

        if (found_start == std::string::npos) {
            link = line.substr(0);
        } else {
            link = line.substr(0, found_start);
        }

        auto link_size = link.size();
        auto line_size = line.size();
        std::string name;

        if (line.rfind("http", 0) == 0 || line.rfind("//", 0) == 0) { // online links
            if (line.rfind("//", 0) == 0) {
                link = "https://" + link.substr(2);
                line = line.substr(2);
                link_size -= 2;
            }

            if (link_size == line_size) {
                // => https://google.com case
                name = link;
            } else {
                // => https://google.com google case
                name = line.substr(link_size + 1);
            }
        } else {
            // inner links. Считаем, что все пути относительные (в корне, который передается первым аргументом)
            // Также, пусть в исходном проекте все ссылки на другие страницы в формате .gmi (т.е. нет ссылок на .html).
            if (isGMI(fs::path(link))) {
                link = ReplaceRoots("", new_root, link).replace_extension("html");
            } else {
                link = ReplaceRoots("", new_root, link);
            }

            if (link_size == line_size) {
                name = fs::path(link).filename().replace_extension("html");
            } else {
                name = line.substr(link_size + 1);
            }
        }

        result_.append(
                "<p><a href=\"" + link + "\">" + name + "</a></p>"
        );
    }

    void CheckList() {
        if (!in_list_ and tag_ == "*") {
            result_.append("<ul>");
            in_list_ = true;
        } else if (in_list_ and tag_ != "*") {
            result_.append("</ul>");
            in_list_ = false;
        }
    }

    void CheckNonFormat() {
        if (!in_nonformat_ and tag_ == "```") {
            result_.append("<pre>");
            in_nonformat_ = true;
        } else if (in_nonformat_ and tag_ == "```") {
            result_.append("</pre>");
            in_nonformat_ = false;
        }
    }

public:
    std::string GmI2Html(const fs::path &fpath, const fs::path &new_root) {
        std::ifstream infile(fpath.string());

        for (std::string line; getline(infile, line);) {

            if (line.empty()) {
                tag_ = "NULL";
                CheckList();
                CheckNonFormat();
                result_.append("\n");
                tag_.erase();
                continue;
            }

            size_t i = 0;
            while (line[i] != ' ' && i < line.size()) {
                tag_.push_back(line[i++]);
            }

            CheckList();
            CheckNonFormat();

            int token_st = i + 1;

            if (in_nonformat_) {
                if (line != "```") {
                    result_.append(line);
                }
                result_.push_back('\n');
                tag_.erase();
                continue;
            }

            if (tag_ == "#") {
                result_.append(
                        "<h1>" + line.substr(token_st) + "</h1>"
                );
            } else if (tag_ == "##") {
                result_.append(
                        "<h2>" + line.substr(token_st) + "</h2>"
                );
            } else if (tag_ == "###") {
                result_.append(
                        "<h3>" + line.substr(token_st) + "</h3>"
                );
            } else if (tag_ == "*") {
                result_.append(
                        "<li>" + line.substr(token_st) + "</li>"
                );
            } else if (tag_ == ">") {
                result_.append(
                        "<blockquote><p>" + line.substr(token_st) + "</p></blockquote>"
                );
            } else if (tag_ == "=>") {
                LinkConnector(line.substr(token_st), new_root);
            } else {

            }

            result_.append("\n");
            tag_.erase();
        }

        return std::move(result_);
    }

private:
    std::string result_, tag_;
    bool in_list_ = false;
    bool in_nonformat_ = false;
};