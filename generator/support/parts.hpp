#pragma once

#include <filesystem>

namespace fs = std::filesystem;

bool isGMI(const fs::path& path) {
  return path.extension() == ".gmi";
}

/*
 * root/..../file.ext => new_root/..../file.ext
 */
fs::path ReplaceRoots(const fs::path& root_dir, const fs::path& new_root_dir,
                      const fs::path& path) {
  auto relative = fs::relative(path, root_dir);
  auto new_path = new_root_dir / relative;
  return new_path;
}