#include "interface-basictiles.hpp"
#include <mbgl/map/map_options.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include<mbgl/util/constants.hpp>
#include <mbgl/style/style.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace mbgl;

namespace feidu
{

  Map* interface_basictiles::g_map = nullptr;
  HeadlessFrontend* interface_basictiles::g_frontend = nullptr;

  void interface_basictiles::initMap()
  {
  std::cout << " initMap server step1" << std::endl;

  const double bearing =  0;
  const double pitch =  0;
  const double pixelRatio =  1.0;

  const uint32_t width =  512;
  const uint32_t height =  512;
  const std::string output = "out.png";
  const std::string cache_file = "cache.sqlite";
  const std::string asset_root = ".";

  // Try to load the apikey from the environment.
  const char* apikeyEnv = getenv("MLN_API_KEY");
  const std::string apikey = apikeyEnv ? apikeyEnv : std::string();
  const bool debug = false;

  std::cout << " initMap server step2" << std::endl;

  auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();

  // util::RunLoop loop;
  g_frontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio));

  std::cout << " initMap server step3" << std::endl;

  g_map = new Map(*g_frontend,
          MapObserver::nullObserver(),
          MapOptions()
              .withMapMode(MapMode::Tile)
              .withSize(g_frontend->getSize())
              .withPixelRatio(static_cast<float>(pixelRatio)),
          ResourceOptions()
              .withCachePath(cache_file)
              .withAssetPath(asset_root)
              .withApiKey(apikey)
              .withTileServerOptions(mapTilerConfiguration));
   //set style
  std::string basicStyle = "./data/styles/basic.json";
  if (basicStyle.find("://") == std::string::npos) {
  basicStyle = std::string("file://") + basicStyle;
  }
  g_map->getStyle().loadURL(basicStyle);

  std::cout << " initMap server step4" << std::endl;

  }

void interface_basictiles::operator()(const httplib::Request & req, httplib::Response & res)
{
  util::RunLoop loop;
  std::cout << " interface_basictiles start!" << std::endl;
  if (g_map==nullptr || g_frontend==nullptr)
  {
    initMap();
  }

  std::cout << " interface_basictiles step 0" << std::endl;
  

  auto XYZtoTileCenterLatLng = [](const mbgl::CanonicalTileID& tileID)->mbgl::Point<double>{
        const double size = mbgl::util::EXTENT * std::pow(2, tileID.z);
        const double x0 = mbgl::util::EXTENT * static_cast<double>(tileID.x);
        const double y0 = mbgl::util::EXTENT * static_cast<double>(tileID.y);

        auto tileCoordinatesToLatLng = [&](const mbgl::Point<int16_t>& p) {
            double y2 = 180 - (p.y + y0) * 360 / size;
            return mbgl::Point<double>((p.x + x0) * 360 / size - 180, std::atan(std::exp(y2 * M_PI / 180)) * 360.0 / M_PI - 90.0);
        };

        mbgl::Point<int16_t> tileCoord = mbgl::Point<int16_t>(mbgl::util::EXTENT*0.5, mbgl::util::EXTENT*0.5);
        mbgl::Point<double> result = tileCoordinatesToLatLng(tileCoord);
        return result;
    };

  // std::cout << " server step1" << std::endl;

  // const double bearing =  0;
  // const double pitch =  0;
  // const double pixelRatio =  1.0;

  // const uint32_t width =  512;
  // const uint32_t height =  512;
  // const std::string output = "out.png";
  // const std::string cache_file = "cache.sqlite";
  // const std::string asset_root = ".";

  // // Try to load the apikey from the environment.
  // const char* apikeyEnv = getenv("MLN_API_KEY");
  // const std::string apikey = apikeyEnv ? apikeyEnv : std::string();
  // const bool debug = false;

  // std::cout << " server step2" << std::endl;

  // auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();

  // util::RunLoop loop;
  // HeadlessFrontend frontend({width, height}, static_cast<float>(pixelRatio));

  // std::cout << " server step3" << std::endl;

  // Map map(frontend,
  //         MapObserver::nullObserver(),
  //         MapOptions()
  //             .withMapMode(MapMode::Static)
  //             .withSize(frontend.getSize())
  //             .withPixelRatio(static_cast<float>(pixelRatio)),
  //         ResourceOptions()
  //             .withCachePath(cache_file)
  //             .withAssetPath(asset_root)
  //             .withApiKey(apikey)
  //             .withTileServerOptions(mapTilerConfiguration));
  //  //set style
  // std::string basicStyle = "./data/styles/basic.json";
  // if (basicStyle.find("://") == std::string::npos) {
  // basicStyle = std::string("file://") + basicStyle;
  // }
  // map.getStyle().loadURL(basicStyle);
  std::cout << " interface_basictiles step 1" << std::endl;
    int x =0;
    int y =0;
    int z =0;
    
    if (req.has_param("x")) {
      std::string strX = req.get_param_value("x");
      x = std::stoi(strX);

      std::cout << " x=" << x << std::endl;
    }

    if (req.has_param("y")) {
      std::string strY = req.get_param_value("y");
      y = std::stoi(strY);

      std::cout << " y=" << y << std::endl;
    }

    if (req.has_param("z")) {
      std::string strZ = req.get_param_value("z");
      z = std::stoi(strZ);

      std::cout << " z=" << z << std::endl;
    }

   std::cout << " interface_basictiles step 2" << std::endl;

    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "PUT,POST,GET,DELETE,OPTIONS");
    res.set_header("Access-Control-Allow-Credentials", "true");

    std::cout << " interface_basictiles step 3" << std::endl;
    
    mbgl::CanonicalTileID canonicalTileID(z, x, y);
    std::stringstream ss;
    ss << "tile_"<<int(canonicalTileID.z)<< "_" << canonicalTileID.x <<"_"<<canonicalTileID.y<<".png";
    std::string pngName = ss.str();
    mbgl::Point<double> latlon= XYZtoTileCenterLatLng(canonicalTileID);
    double lat = latlon.y;
    double lon = latlon.x;
    double zoom = double(canonicalTileID.z);

    std::cout << "lat: " << lat << std::endl;
    std::cout << "lon: " << lon << std::endl;
    std::cout << "zoom: " << zoom << std::endl;
    {
        // map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom).withBearing(bearing).withPitch(pitch));
        g_map->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
    }

    // if (debug) {
    //     g_map->setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus
    //                     : mbgl::MapDebugOptions::NoDebug);
    // }

    std::cout << " interface_basictiles step 4" << std::endl;

    try {
      std::string str = encodePNG(g_frontend->render(*g_map).image);
      res.status = 200;
      res.set_content(str, "image/png");
    } catch (std::exception& e) {
        std::string str = std::string("Error: ") + e.what();
        res.status = 400;
        res.set_content(str, "text/plain");

        std::cout << " interface_basictiles step 4.1:" <<str<< std::endl;
    }
    std::cout << " interface_basictiles step 5" << std::endl;
}

}  // namespace feidu