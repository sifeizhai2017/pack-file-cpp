#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>
#include <ctime>
#include <filesystem>
#include <algorithm>
#include <stdexcept>

namespace fs = std::filesystem;

// 文件信息结构体
struct FileInfo
{
    std::string fileName;
    std::size_t size;
    std::string filePath;
    std::size_t fileOffset;
    std::time_t timeCreate;

    FileInfo() = default;
    FileInfo(const std::string &name, std::size_t sz, const std::string &path,
             std::size_t offset, std::time_t time)
        : fileName(name), size(sz), filePath(path), fileOffset(offset), timeCreate(time) {}
};

// 现代C++文件管理器类
class FileManager
{
private:
    std::vector<char> fileBuf_;

public:
    // 读取文件内容到缓冲区
    void readFile(const std::string &fileName);

    // 将单个文件写入包中
    void writeSingleFile(const std::string &filePath, std::size_t fileSize, std::ofstream &packStream);

    // 获取文件缓冲区
    const std::vector<char> &getFileBuf() const { return fileBuf_; }
};

void FileManager::readFile(const std::string &fileName)
{
    std::cout << "当前正在读取的文件：" << fileName << std::endl;

    std::ifstream file(fileName, std::ios::binary | std::ios::ate);
    if (!file.is_open())
    {
        throw std::runtime_error("无法打开文件：" + fileName);
    }

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);

    fileBuf_.resize(size);
    if (!file.read(fileBuf_.data(), size))
    {
        throw std::runtime_error("读取文件失败：" + fileName);
    }

    std::cout.write(fileBuf_.data(), size);
    std::cout << std::endl;
}

void FileManager::writeSingleFile(const std::string &filePath, std::size_t fileSize, std::ofstream &packStream)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open())
    {
        throw std::runtime_error("无法打开文件：" + filePath);
    }

    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);
    packStream.write(buffer.data(), fileSize);
}

// 文件列表管理类
class FileList
{
private:
    std::vector<FileInfo> files_;
    std::string searchPath_;

public:
    explicit FileList(const std::string &searchPath) : searchPath_(searchPath) {}

    // 初始化文件列表
    void initFileList();

    // 获取文件数量
    std::size_t getFileNum() const { return files_.size(); }

    // 获取文件列表
    const std::vector<FileInfo> &getFiles() const { return files_; }

    // 重载[]操作符
    const std::string &operator[](std::size_t index) const
    {
        if (index >= files_.size())
        {
            throw std::out_of_range("文件索引超出范围");
        }
        return files_[index].fileName;
    }
};

void FileList::initFileList()
{
    try
    {
        fs::path searchDir = fs::path(searchPath_).parent_path();
        std::string pattern = fs::path(searchPath_).filename().string();

        // 如果是通配符模式，处理所有文件
        if (pattern == "*.*" || pattern == "*")
        {
            for (const auto &entry : fs::directory_iterator(searchDir))
            {
                if (entry.is_regular_file())
                {
                    auto filePath = entry.path().string();
                    auto fileName = entry.path().filename().string();
                    auto fileSize = entry.file_size();
                    auto fileTime = std::chrono::duration_cast<std::chrono::seconds>(
                                        entry.last_write_time().time_since_epoch())
                                        .count();

                    files_.emplace_back(fileName, fileSize, filePath, 0, fileTime);
                    std::cout << "找到文件：" << fileName << std::endl;
                }
            }
        }
        else
        {
            // 处理特定文件
            if (fs::exists(searchPath_) && fs::is_regular_file(searchPath_))
            {
                fs::path filePath(searchPath_);
                auto fileName = filePath.filename().string();
                auto fileSize = fs::file_size(filePath);
                auto fileTime = std::chrono::duration_cast<std::chrono::seconds>(
                                    fs::last_write_time(filePath).time_since_epoch())
                                    .count();

                files_.emplace_back(fileName, fileSize, searchPath_, 0, fileTime);
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        throw std::runtime_error("文件系统错误：" + std::string(e.what()));
    }
}

// 索引管理类
class IndexManager
{
private:
    std::unique_ptr<FileList> fileList_;
    std::string packPath_;

public:
    // 初始化文件列表
    std::unique_ptr<FileList> initFileListClass(const std::string &filePath);

    // 计算初始偏移量
    std::size_t calculateInitialOffset(std::size_t fileCount);

    // 写入索引块
    void writeIndexBlock(std::ofstream &packStream, FileInfo &fileInfo, std::size_t &currentOffset);
};

std::unique_ptr<FileList> IndexManager::initFileListClass(const std::string &filePath)
{
    fileList_ = std::make_unique<FileList>(filePath);
    return std::make_unique<FileList>(filePath);
}

std::size_t IndexManager::calculateInitialOffset(std::size_t fileCount)
{
    std::size_t offset = sizeof(std::size_t) + fileCount * sizeof(FileInfo);
    std::cout << "初始偏移量为：" << offset << std::endl;
    return offset;
}

void IndexManager::writeIndexBlock(std::ofstream &packStream, FileInfo &fileInfo, std::size_t &currentOffset)
{
    fileInfo.fileOffset = currentOffset;

    // 写入文件信息结构体
    packStream.write(reinterpret_cast<const char *>(&fileInfo.fileName), sizeof(fileInfo.fileName));
    packStream.write(reinterpret_cast<const char *>(&fileInfo.size), sizeof(fileInfo.size));
    packStream.write(reinterpret_cast<const char *>(&fileInfo.filePath), sizeof(fileInfo.filePath));
    packStream.write(reinterpret_cast<const char *>(&fileInfo.fileOffset), sizeof(fileInfo.fileOffset));
    packStream.write(reinterpret_cast<const char *>(&fileInfo.timeCreate), sizeof(fileInfo.timeCreate));

    currentOffset += fileInfo.size;
    std::cout << "已写入 " << fileInfo.fileName << " 的信息" << std::endl;
}

// 包管理类
class PackManager
{
private:
    std::unique_ptr<FileManager> fileManager_;
    std::unique_ptr<IndexManager> indexManager_;

public:
    PackManager() : fileManager_(std::make_unique<FileManager>()),
                    indexManager_(std::make_unique<IndexManager>()) {}

    // 打包文件
    void packFiles(const std::string &filePath, const std::string &packPath);

    // 显示包内所有文件
    void showFiles(const std::string &packPath);

    // 显示特定数量的文件
    void showFiles(const std::string &packPath, int count);

    // 解包所有文件
    void unpackFiles(const std::string &packPath, const std::string &unpackPath);

    // 解包特定文件
    void unpackFiles(const std::string &packPath, const std::string &unpackPath, int index);

private:
    // 创建目录
    void createDirectory(const std::string &dirPath);

    // 解包特定文件的辅助函数
    void unpackSpecificFile(const FileInfo &fileInfo, std::ifstream &packStream,
                            const std::string &unpackPath);
};

void PackManager::packFiles(const std::string &filePath, const std::string &packPath)
{
    std::cout << "开始打包文件..." << std::endl;

    // 初始化文件列表
    auto fileList = indexManager_->initFileListClass(filePath);
    fileList->initFileList();

    auto fileCount = fileList->getFileNum();
    if (fileCount == 0)
    {
        throw std::runtime_error("没有找到要打包的文件");
    }

    std::ofstream packStream(packPath, std::ios::binary);
    if (!packStream.is_open())
    {
        throw std::runtime_error("无法创建包文件：" + packPath);
    }

    // 写入文件数量
    packStream.write(reinterpret_cast<const char *>(&fileCount), sizeof(fileCount));

    // 计算初始偏移量
    std::size_t currentOffset = indexManager_->calculateInitialOffset(fileCount);

    // 写入文件信息结构体
    auto files = fileList->getFiles();
    for (auto &fileInfo : files)
    {
        auto mutableFileInfo = fileInfo; // 创建可修改的副本
        indexManager_->writeIndexBlock(packStream, mutableFileInfo, currentOffset);
    }

    // 写入文件内容
    for (const auto &fileInfo : files)
    {
        fileManager_->writeSingleFile(fileInfo.filePath, fileInfo.size, packStream);
    }

    std::cout << "打包完成！共打包 " << fileCount << " 个文件。" << std::endl;
}

void PackManager::showFiles(const std::string &packPath)
{
    std::ifstream packStream(packPath, std::ios::binary);
    if (!packStream.is_open())
    {
        throw std::runtime_error("无法打开包文件：" + packPath);
    }

    std::size_t fileCount;
    packStream.read(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));

    std::cout << "===============================================================================" << std::endl;
    std::cout << "文件名\t\t\t大小\t\t\t创建时间" << std::endl;
    std::cout << "===============================================================================" << std::endl;

    for (std::size_t i = 0; i < fileCount; ++i)
    {
        FileInfo fileInfo;
        packStream.read(reinterpret_cast<char *>(&fileInfo), sizeof(FileInfo));

        std::cout << fileInfo.fileName << "\t\t" << fileInfo.size << "\t\t"
                  << std::ctime(&fileInfo.timeCreate);
    }

    std::cout << "===============================================================================" << std::endl;
    std::cout << "文件总数：" << fileCount << std::endl;
}

void PackManager::showFiles(const std::string &packPath, int displayCount)
{
    std::ifstream packStream(packPath, std::ios::binary);
    if (!packStream.is_open())
    {
        throw std::runtime_error("无法打开包文件：" + packPath);
    }

    std::size_t fileCount;
    packStream.read(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));

    std::cout << "===============================================================================" << std::endl;

    int shown = 0;
    for (std::size_t i = 0; i < fileCount && shown < displayCount; ++i)
    {
        FileInfo fileInfo;
        packStream.read(reinterpret_cast<char *>(&fileInfo), sizeof(FileInfo));

        std::cout << fileInfo.fileName << "\t\t" << fileInfo.size << "\t\t"
                  << std::ctime(&fileInfo.timeCreate);
        ++shown;

        if (shown % displayCount == 0)
        {
            std::cout << "按回车键继续..." << std::endl;
            std::cin.get();
        }
    }

    std::cout << "===============================================================================" << std::endl;
}

void PackManager::unpackFiles(const std::string &packPath, const std::string &unpackPath)
{
    createDirectory(unpackPath);

    std::ifstream packStream(packPath, std::ios::binary);
    if (!packStream.is_open())
    {
        throw std::runtime_error("无法打开包文件：" + packPath);
    }

    std::size_t fileCount;
    packStream.read(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));

    std::vector<FileInfo> files(fileCount);
    for (std::size_t i = 0; i < fileCount; ++i)
    {
        packStream.read(reinterpret_cast<char *>(&files[i]), sizeof(FileInfo));
    }

    for (const auto &fileInfo : files)
    {
        unpackSpecificFile(fileInfo, packStream, unpackPath);
    }

    std::cout << "解包完成！共解包 " << fileCount << " 个文件。" << std::endl;
}

void PackManager::unpackFiles(const std::string &packPath, const std::string &unpackPath, int index)
{
    createDirectory(unpackPath);

    std::ifstream packStream(packPath, std::ios::binary);
    if (!packStream.is_open())
    {
        throw std::runtime_error("无法打开包文件：" + packPath);
    }

    std::size_t fileCount;
    packStream.read(reinterpret_cast<char *>(&fileCount), sizeof(fileCount));

    if (index < 0 || static_cast<std::size_t>(index) >= fileCount)
    {
        throw std::out_of_range("文件索引超出范围");
    }

    // 跳转到指定文件的信息位置
    packStream.seekg(sizeof(std::size_t) + index * sizeof(FileInfo));

    FileInfo fileInfo;
    packStream.read(reinterpret_cast<char *>(&fileInfo), sizeof(FileInfo));

    unpackSpecificFile(fileInfo, packStream, unpackPath);

    std::cout << "解包完成！" << std::endl;
}

void PackManager::createDirectory(const std::string &dirPath)
{
    try
    {
        if (!fs::exists(dirPath))
        {
            fs::create_directories(dirPath);
        }
    }
    catch (const fs::filesystem_error &e)
    {
        throw std::runtime_error("创建目录失败：" + std::string(e.what()));
    }
}

void PackManager::unpackSpecificFile(const FileInfo &fileInfo, std::ifstream &packStream,
                                     const std::string &unpackPath)
{
    fs::path outputPath = fs::path(unpackPath) / fileInfo.fileName;

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile.is_open())
    {
        throw std::runtime_error("无法创建输出文件：" + outputPath.string());
    }

    packStream.seekg(fileInfo.fileOffset);

    std::vector<char> buffer(fileInfo.size);
    packStream.read(buffer.data(), fileInfo.size);
    outputFile.write(buffer.data(), fileInfo.size);

    std::cout << "已解包：" << fileInfo.fileName << std::endl;
}

// 主函数
int main(int argc, char *argv[])
{
    try
    {
        if (argc < 2)
        {
            std::cout << "用法：" << std::endl;
            std::cout << "  打包：" << argv[0] << " <源路径> <包路径>" << std::endl;
            std::cout << "  显示：" << argv[0] << " -l <包路径>" << std::endl;
            std::cout << "  显示指定数量：" << argv[0] << " -l<数字> <包路径>" << std::endl;
            std::cout << "  解包：" << argv[0] << " -u <包路径> <解包路径>" << std::endl;
            std::cout << "  解包指定文件：" << argv[0] << " -u<数字> <包路径> <解包路径>" << std::endl;
            return 1;
        }

        auto packManager = std::make_unique<PackManager>();

        std::string firstArg(argv[1]);

        if (firstArg[0] != '-')
        {
            // 打包模式
            if (argc < 3)
            {
                std::cerr << "打包模式需要源路径和包路径" << std::endl;
                return 1;
            }
            std::cout << "源文件路径：" << argv[1] << std::endl;
            std::cout << "包文件路径：" << argv[2] << std::endl;
            packManager->packFiles(argv[1], argv[2]);
        }
        else if (firstArg[1] == 'l')
        {
            // 显示模式
            if (argc < 3)
            {
                std::cerr << "显示模式需要包路径" << std::endl;
                return 1;
            }

            if (firstArg.length() == 2)
            {
                packManager->showFiles(argv[2]);
            }
            else
            {
                std::string numStr = firstArg.substr(2);
                int count = std::stoi(numStr);
                packManager->showFiles(argv[2], count);
            }
        }
        else if (firstArg[1] == 'u')
        {
            // 解包模式
            if (argc < 4)
            {
                std::cerr << "解包模式需要包路径和解包路径" << std::endl;
                return 1;
            }

            if (firstArg.length() == 2)
            {
                packManager->unpackFiles(argv[2], argv[3]);
            }
            else
            {
                std::string numStr = firstArg.substr(2);
                int index = std::stoi(numStr);
                packManager->unpackFiles(argv[2], argv[3], index);
            }
        }
        else
        {
            std::cerr << "未知参数：" << firstArg << std::endl;
            return 1;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误：" << e.what() << std::endl;
        return 1;
    }

    return 0;
}