# 📦 FilePack - File Packing & Unpacking Tool 🚀

Welcome to **FilePack**, a powerful and easy-to-use tool for packing and unpacking multiple files into a single archive
file! 🎉

Whether you're managing large numbers of files or just want a clean way to store and extract data, FilePack is
here to help. 🧰

This project is built using modern C++ practices and supports operations such as:

- 📁 Packing multiple files into a single archive file
- 🔓 Unpacking files from an archive
- 📋 Listing all files contained in an archive
- 🔍 Extracting a specific file from an archive

---

## 🧩 Features

- **📦 Archive Management**: Pack and unpack files using a custom binary format that stores metadata like file names,
  sizes, and timestamps.
- **📄 File Indexing**: Each archive contains an index block that allows fast access to file metadata.
- **📂 Directory Support**: Automatically creates output directories when unpacking.
- **🖥️ Cross-Platform**: Built with modern C++ and `std::filesystem`, ensuring compatibility across operating systems.
- **🔍 Interactive CLI**: Command-line interface with support for listing files with pagination and extracting specific
  files by index.

---

## 🛠️ How It Works

### File Packing

When you pack files:

1. The program scans the specified directory or file paths.
2. It collects metadata (name, size, path, creation time) for each file.
3. All files are written into a single binary archive file.
4. An index block is written at the beginning of the archive for fast lookup.

### File Unpacking

When you unpack:

1. The program reads the index block to retrieve file metadata.
2. Each file is extracted individually to the specified output directory.
3. You can extract all files or only a specific one using its index.

---

## 🧰 Usage Guide

### 📝 Command Line Interface

Run the program from the command line with the following syntax:

```bash
FilePack [OPTIONS]
```

#### 📦 Packing Files

```bash
FilePack <source-path> <archive-path>
```

Example:

```bash
FilePack ./files_to_pack ./archive.pack
```

#### 📋 Listing Files in an Archive

```bash
FilePack -l<page-size> <archive-path>
```

Example:

```bash
FilePack -l20 ./archive.pack
```

> This will list 20 files per page.

#### 🔓 Unpacking Files

Unpack all files:

```bash
FilePack -u <archive-path> <output-directory>
```

Unpack a specific file by index:

```bash
FilePack -u<index> <archive-path> <output-directory>
```

Example:

```bash
FilePack -u3 ./archive.pack ./extracted/
```

---

## 🧪 Example Commands

```bash
# Pack files from a folder
FilePack ./data ./archive.pack

# List all files in archive (20 per page)
FilePack -l20 ./archive.pack

# Unpack all files
FilePack -u ./archive.pack ./extracted/

# Extract only the 5th file in the archive
FilePack -u5 ./archive.pack ./extracted/
```

---

## 🏗️ Development

This project uses modern C++ features and leverages the `std::filesystem` library for cross-platform compatibility.

### 📁 Project Structure

- [main.cpp](file://f:\Codes\pack-file-cpp\main.cpp) – Entry point and command-line interface.
- [modern.cpp](file://f:\Codes\pack-file-cpp\modern.cpp) – Core logic for file packing, unpacking, and indexing.
- [last.cpp](file://f:\Codes\pack-file-cpp\last.cpp) – Legacy implementation using C-style file handling and manual
  memory management.
- [README.md](file://f:\Codes\pack-file-cpp\README.md) – This documentation 📄

---

## 🧪 Testing

You can test the application by:

1. Packing a directory of your choice.
2. Listing the contents to verify.
3. Unpacking and checking if all files are correctly restored.

---

## 🙌 Contributing

Contributions are welcome! If you'd like to enhance FilePack with features like compression, encryption, or GUI support,
feel free to open a pull request. 🤝

---

## 📜 License

MIT License © 2025 FilePack. Free to use, modify, and distribute. 📚

---

## 📬 Feedback & Issues

Have a bug to report or a feature to suggest? Please open an issue on GitHub or reach out to the maintainers. 📧

---

Enjoy packing and unpacking your files with speed and simplicity! 🎯  
**FilePack – Your File Management Companion 🧑‍💻**