#include <args.hxx>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include "httplib.h"
#include "interface-basictiles.hpp"
#include "renderThread.hpp"

using namespace std;
using namespace httplib;
using namespace feidu;

Server svr;
std::string ipAddress = "192.168.3.19";
std::string port = "8080";

// 一个简单的函数，用于读取文件内容并返回一个字符串
std::string read_file(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary | std::ios::ate);
  if (file) {
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
      return std::string(buffer.begin(), buffer.end());
    }
  }
  return "";
}

bool isFileExists_access(string& name) {
  return (access(name.c_str(), F_OK) != -1);
}


bool findSuffix(const std::string& src, const std::string& suffix)
{
  size_t pos = src.rfind(suffix); // 反向查找子串的位置
  if (pos != string::npos && pos + suffix.size() == src.size()) // 判断子串是否在字符串的末尾
  {
      cout << src<< " ends with " << suffix << endl;
      return true;
  }
  else
  {
      cout << src<< " does not end with " << suffix << endl;
      return false;
  }
}

int main(int argc, char * argv[])
{
  renderThread::instance();
  std::cout << " server step0" << std::endl;

  // Set a route for /fonts
  ///e.g. /fonts/Roboto Regular/0-255.pbf
  svr.Get(R"(/fonts/([\w\s\-_]+)/([\d]+-[\d]+)\.pbf)", [&](const httplib::Request& req, httplib::Response& res) {
    std::cout << " matches start !" << std::endl;
    // 获取请求参数
    std::string fontstack = req.matches[1];
    std::string range = req.matches[2];

    // std::cout << " fontstack:" << fontstack << std::endl;

    // std::cout << " range:" << range << std::endl;

    // 拼接字体缓存的路径，假设缓存都放在 cache 目录下
    std::string cache_path = std::string("./data/fonts/") + fontstack + std::string("/") + range + std::string(".pbf");

    // std::cout << " cache_path:" << cache_path << std::endl;

    // 检查缓存是否存在
    if (isFileExists_access(cache_path)) {
      // 如果存在，直接读取缓存并返回
      std::string cache_content = read_file(cache_path);
      res.status = 200;
      res.set_content(cache_content, "application/x-protobuf");
      return;
    }
    else
    {
      // 如果不存在，返回404错误
      res.status = 404;
      res.set_content("Font not found", "text/plain");
      return;
    }

  });

  // Set a route for sprites/sprite
  ///sprites/spriteblue.json
  svr.Get(R"(/sprites/[\w]+(@2x)?\.(json|png))", [](const Request &req, Response &res) {

    // 获取请求参数
    std::string fullPath = req.matches[0];
    bool isJsonSuffix = findSuffix(fullPath, ".json");
    bool isPngSuffix = findSuffix(fullPath, ".png");

    if (isJsonSuffix)
    {
      res.set_header("Content-Type", "application/json");
    }
    else if (isPngSuffix)
    {
      res.set_header("Content-Type", "image/png");
    }
    else
    {
      // Send a 404 error if the file is not found
      res.status = 500;
      res.set_content("This file format is not surported", "text/plain");
      return;
    }
    
    std::string filename = std::string("./data") + fullPath;
    std::cout << " sprite filename:" << filename << std::endl;
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
      // Send the file content as the response body
      res.body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
      file.close();
      res.status = 200;
    } else {
      // Send a 404 error if the file is not found
      res.status = 404;
      res.set_content("File not found", "text/plain");
    }
  });

  // interface_basictiles basictiles;
  // svr.Get("/styles/basic", [&](const Request& req, Response& res) { basictiles(req, res); });
  // svr.Get(R"(/styles/basic/([\d]+)/([\d]+)/([\d]+)\.png)", [&](const Request& req, Response& res) { basictiles(req, res); });
  svr.Get(R"(/styles/basic/([\d]+)/([\d]+)/([\d]+)\.png)", interface_basictiles());
  svr.Get("/stop", [&](const Request& req, Response& res) { svr.stop(); });
  
   // Run servers
  // auto httpThread = std::thread([&]() { svr.listen(ipAddress.c_str(), atoi(port.c_str())); });
  // httpThread.join();
  svr.listen(ipAddress.c_str(), atoi(port.c_str()));
  std::cout << " server step10" << std::endl;
  return 1;
}
