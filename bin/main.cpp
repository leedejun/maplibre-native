#include <args.hxx>
//#include <unistd.h>
#include "unistd.h"
#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include "httplib.h"
#include "interface-basictiles.hpp"
#include "renderThread.hpp"
#include <args.hxx>

//////////////////////////////////
//test
#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>

#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/style/style.hpp>

#include "dataManager.h"
#include "RasterTilesCustomLayerHost.hpp"
//////////////////////////////////


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

int main(int argc, char* argv[])
{
  // 创建一个 ArgumentParser 对象
    args::ArgumentParser parser("This is a test program.", "This goes after the options.");
    // 创建一个 Group 对象
    args::Group group(parser, "This group is all exclusive:", args::Group::Validators::All);
    // 创建一些 ValueFlag 对象
    args::ValueFlag<std::string> host(group, "host", "The host ip address", {'h', "host"});
    args::ValueFlag<int> customPort(group, "port", "The port number", {'p', "port"});
    // 解析命令行参数
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    // // 获取参数的值
    // if (host) std::cout << "Host: " << args::get(host) << std::endl;
    // if (port) std::cout << "Port: " << args::get(port) << std::endl;
    
    const std::string strHost = host ? args::get(host):ipAddress;
    const int iPort = customPort ? args::get(customPort) : 8080;

  // args::ArgumentParser argumentParser("Mapbox GL render tool");
  // args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {"help"});
  // args::ValueFlag<std::string> hostValue(argumentParser, "number", "host ipAddress", {'host', "host"});
  // args::ValueFlag<uint32_t> portValue(argumentParser, "number", "host port", {'port', "port"});

  // args::ValueFlag<uint32_t> widthValue(argumentParser, "pixels", "Image width", {'w', "width"});
  // args::ValueFlag<uint32_t> heightValue(argumentParser, "pixels", "Image height", {'h', "height"});

  // try {
  //     argumentParser.ParseCLI(argc, argv);
  // } catch (const args::Help&) {
  //     std::cout << argumentParser;
  //     exit(0);
  // } catch (const args::ParseError& e) {
  //     std::cerr << e.what() << std::endl;
  //     std::cerr << argumentParser;
  //     exit(1);
  // } catch (const args::ValidationError& e) {
  //     std::cerr << e.what() << std::endl;
  //     std::cerr << argumentParser;
  //     exit(2);
  // }

  // const std::string strHost = hostValue ? args::get(hostValue):ipAddress;
  // const uint32_t iPort = portValue ? args::get(portValue) : 8080;


  renderThread::instance();
  std::cout << " server step0" << std::endl;
  try {
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

  // Set a route for satellite tiles
  svr.Get(R"(/tiles/satellite/([\d]+)/([\d]+)/([\d]+)\.png)", [](const Request &req, Response &res) {
    // 获取请求参数
    std::string fullPath = req.matches[0];
    bool isPngSuffix = findSuffix(fullPath, ".png");
    if (isPngSuffix)
    {
      res.set_header("Access-Control-Allow-Origin", "*");
      res.set_header("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
      res.set_header("Access-Control-Allow-Credentials", "true");
      res.set_header("Content-Type", "image/png");
    }
    else
    {
      // Send a 404 error if the file is not found
      res.status = 500;
      res.set_content("This file format is not surported", "text/plain");
      return;
    }
    
    std::string filename = std::string("D:\\work\\maplibre-native\\data") + fullPath;
    std::cout << " satellite filename:" << filename << std::endl;
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
  // svr.Get(R"(/styles/basic/([\d]+)/([\d]+)/([\d]+)\.png)", interface_basictiles());
  svr.Get(R"(/styles/([\w]+)/([\d]+)/([\d]+)/([\d]+)\.png)", interface_basictiles());
  svr.Get("/stop", [&](const Request& req, Response& res) { svr.stop(); });
  
   // Run servers
  // auto httpThread = std::thread([&]() { svr.listen(ipAddress.c_str(), atoi(port.c_str())); });
  // httpThread.join();
  // svr.listen(ipAddress.c_str(), atoi(port.c_str()));
  svr.listen(strHost.c_str(), iPort);
  std::cout << " server step10" << std::endl;
  } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
            exit(1);
        }
  return 1;
}

std::string readJsonFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    
    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filePath << std::endl;
        return "";
    }

    return buffer.str();
}

//int main(int argc, char* argv[]) {
//    args::ArgumentParser argumentParser("MapLibre Native render tool");
//    args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {"help"});
//
//    args::ValueFlag<std::string> backendValue(argumentParser, "Backend", "Rendering backend", {"backend"});
//    args::ValueFlag<std::string> apikeyValue(argumentParser, "key", "API key", {'t', "apikey"});
//    args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
//    args::ValueFlag<std::string> outputValue(argumentParser, "file", "Output file name", {'o', "output"});
//    args::ValueFlag<std::string> cacheValue(argumentParser, "file", "Cache database file name", {'c', "cache"});
//    args::ValueFlag<std::string> assetsValue(
//        argumentParser, "file", "Directory to which asset:// URLs will resolve", {'a', "assets"});
//
//    args::Flag debugFlag(argumentParser, "debug", "Debug mode", {"debug"});
//
//    args::ValueFlag<double> pixelRatioValue(argumentParser, "number", "Image scale factor", {'r', "ratio"});
//
//    args::ValueFlag<double> zoomValue(argumentParser, "number", "Zoom level", {'z', "zoom"});
//
//    args::ValueFlag<double> lonValue(argumentParser, "degrees", "Longitude", {'x', "lon"});
//    args::ValueFlag<double> latValue(argumentParser, "degrees", "Latitude", {'y', "lat"});
//    args::ValueFlag<double> bearingValue(argumentParser, "degrees", "Bearing", {'b', "bearing"});
//    args::ValueFlag<double> pitchValue(argumentParser, "degrees", "Pitch", {'p', "pitch"});
//    args::ValueFlag<uint32_t> widthValue(argumentParser, "pixels", "Image width", {'w', "width"});
//    args::ValueFlag<uint32_t> heightValue(argumentParser, "pixels", "Image height", {'h', "height"});
//
//    try {
//        argumentParser.ParseCLI(argc, argv);
//    } catch (const args::Help&) {
//        std::cout << argumentParser;
//        exit(0);
//    } catch (const args::ParseError& e) {
//        std::cerr << e.what() << std::endl;
//        std::cerr << argumentParser;
//        exit(1);
//    } catch (const args::ValidationError& e) {
//        std::cerr << e.what() << std::endl;
//        std::cerr << argumentParser;
//        exit(2);
//    }
//
//    /*const double lat = latValue ? args::get(latValue) : 0;
//    const double lon = lonValue ? args::get(lonValue) : 0;
//    const double zoom = zoomValue ? args::get(zoomValue) : 0;*/
//
//    //96.4899729729999933,40.3233243239999979 : 97.0100270270000067,40.6766756760000021
//    const double lat = (40.3233243239999979+40.6766756760000021)*0.5;
//    const double lon = (96.4899729729999933+97.0100270270000067)*0.5;
//    const double zoom = 3;
//    const double bearing = bearingValue ? args::get(bearingValue) : 0;
//    const double pitch = pitchValue ? args::get(pitchValue) : 0;
//    const double pixelRatio = pixelRatioValue ? args::get(pixelRatioValue) : 1;
//
//    const uint32_t width = widthValue ? args::get(widthValue) : 512;
//    const uint32_t height = heightValue ? args::get(heightValue) : 512;
//    const std::string output = outputValue ? args::get(outputValue) : "out.png";
//    const std::string cache_file = cacheValue ? args::get(cacheValue) : "cache.sqlite";
//    const std::string asset_root = assetsValue ? args::get(assetsValue) : ".";
//
//    // Try to load the apikey from the environment.
//    const char* apikeyEnv = getenv("MLN_API_KEY");
//    const std::string apikey = apikeyValue ? args::get(apikeyValue) : (apikeyEnv ? apikeyEnv : std::string());
//
//    const bool debug = debugFlag ? args::get(debugFlag) : false;
//
//    using namespace mbgl;
//
//    auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
//    std::string style = styleValue ? args::get(styleValue) : mapTilerConfiguration.defaultStyles().at(0).getUrl();
//
//    util::RunLoop loop;
//
//    HeadlessFrontend frontend({width, height}, static_cast<float>(pixelRatio));
//    Map map(frontend,
//            MapObserver::nullObserver(),
//            MapOptions()
//                .withMapMode(MapMode::Static)
//                .withSize(frontend.getSize())
//                .withPixelRatio(static_cast<float>(pixelRatio)),
//            ResourceOptions()
//                .withCachePath(cache_file)
//                .withAssetPath(asset_root)
//                .withApiKey(apikey)
//                .withTileServerOptions(mapTilerConfiguration));
//
//
//    //test style
//    style = std::string("D:\\work\\maplibre-native\\data\\style.json");
//
//    if (style.find("://") == std::string::npos) {
//        style = std::string("file://") + style;
//    }
//
//    //map.getStyle().loadURL(style);
//    std::string styleJson = readJsonFile("D:\\work\\maplibre-native\\data\\style.json");
//    map.getStyle().loadJSON(styleJson);
//
//    //test
//    std::unique_ptr<feidu::CustomLayerHostInterface> host = nullptr;
//    host.reset(new RasterTilesCustomLayerHost());
//    map.addCustomLayer("testtif", std::move(host));
//
//    map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom).withBearing(bearing).withPitch(pitch));
//
//    if (debug) {
//        map.setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus
//                           : mbgl::MapDebugOptions::NoDebug);
//    }
//
//    try {
//        std::ofstream out(output, std::ios::binary);
//        out << encodePNG(frontend.render(map).image);
//        out.close();
//
//        ////test
//        //dataManager* dataMgr = new dataManager();
//        //std::string filePath = "D:\\workDoc\\lidejun\\data\\testdata\\RD0100_DataCloud_K47D011002_2024H1_4FF_4326.tif";
//        //dataMgr->LoadGeoTIFFAsTexture(filePath.c_str());
//
//    } catch (std::exception& e) {
//        std::cout << "Error: " << e.what() << std::endl;
//        exit(1);
//    }
//
//    return 0;
//}
