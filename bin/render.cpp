#include <mbgl/map/map.hpp>
#include <mbgl/map/map_options.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/util/run_loop.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include<mbgl/util/constants.hpp>
#include <vector>


#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include <mbgl/style/style.hpp>

#include <args.hxx>

#include <cstdlib>
#include <iostream>
#include <fstream>

// int main(int argc, char* argv[]) {
//     args::ArgumentParser argumentParser("Mapbox GL render tool");
//     args::HelpFlag helpFlag(argumentParser, "help", "Display this help menu", {"help"});

//     args::ValueFlag<std::string> backendValue(argumentParser, "Backend", "Rendering backend", {"backend"});
//     args::ValueFlag<std::string> apikeyValue(argumentParser, "key", "API key", {'t', "apikey"});
//     args::ValueFlag<std::string> styleValue(argumentParser, "URL", "Map stylesheet", {'s', "style"});
//     args::ValueFlag<std::string> outputValue(argumentParser, "file", "Output file name", {'o', "output"});
//     args::ValueFlag<std::string> cacheValue(argumentParser, "file", "Cache database file name", {'c', "cache"});
//     args::ValueFlag<std::string> assetsValue(
//         argumentParser, "file", "Directory to which asset:// URLs will resolve", {'a', "assets"});

//     args::Flag debugFlag(argumentParser, "debug", "Debug mode", {"debug"});

//     args::ValueFlag<double> pixelRatioValue(argumentParser, "number", "Image scale factor", {'r', "ratio"});

//     args::ValueFlag<double> zoomValue(argumentParser, "number", "Zoom level", {'z', "zoom"});

//     args::ValueFlag<double> lonValue(argumentParser, "degrees", "Longitude", {'x', "lon"});
//     args::ValueFlag<double> latValue(argumentParser, "degrees", "Latitude", {'y', "lat"});
//     args::ValueFlag<double> bearingValue(argumentParser, "degrees", "Bearing", {'b', "bearing"});
//     args::ValueFlag<double> pitchValue(argumentParser, "degrees", "Pitch", {'p', "pitch"});
//     args::ValueFlag<uint32_t> widthValue(argumentParser, "pixels", "Image width", {'w', "width"});
//     args::ValueFlag<uint32_t> heightValue(argumentParser, "pixels", "Image height", {'h', "height"});

//     try {
//         argumentParser.ParseCLI(argc, argv);
//     } catch (const args::Help&) {
//         std::cout << argumentParser;
//         exit(0);
//     } catch (const args::ParseError& e) {
//         std::cerr << e.what() << std::endl;
//         std::cerr << argumentParser;
//         exit(1);
//     } catch (const args::ValidationError& e) {
//         std::cerr << e.what() << std::endl;
//         std::cerr << argumentParser;
//         exit(2);
//     }

//     auto XYZtoTileCenterLatLng = [](const mbgl::CanonicalTileID& tileID)->mbgl::Point<double>{
//         const double size = mbgl::util::EXTENT * std::pow(2, tileID.z);
//         const double x0 = mbgl::util::EXTENT * static_cast<double>(tileID.x);
//         const double y0 = mbgl::util::EXTENT * static_cast<double>(tileID.y);

//         auto tileCoordinatesToLatLng = [&](const mbgl::Point<int16_t>& p) {
//             double y2 = 180 - (p.y + y0) * 360 / size;
//             return mbgl::Point<double>((p.x + x0) * 360 / size - 180, std::atan(std::exp(y2 * M_PI / 180)) * 360.0 / M_PI - 90.0);
//         };

//         mbgl::Point<int16_t> tileCoord = mbgl::Point<int16_t>(mbgl::util::EXTENT*0.5, mbgl::util::EXTENT*0.5);
//         mbgl::Point<double> result = tileCoordinatesToLatLng(tileCoord);
//         return result;
//     };


//     // const double lat = latValue ? args::get(latValue) : 0;
//     // const double lon = lonValue ? args::get(lonValue) : 0;
//     // const double zoom = zoomValue ? args::get(zoomValue) : 0;
//     const double bearing = bearingValue ? args::get(bearingValue) : 0;
//     const double pitch = pitchValue ? args::get(pitchValue) : 0;
//     const double pixelRatio = pixelRatioValue ? args::get(pixelRatioValue) : 1;

//     const uint32_t width = widthValue ? args::get(widthValue) : 512;
//     const uint32_t height = heightValue ? args::get(heightValue) : 512;
//     const std::string output = outputValue ? args::get(outputValue) : "out.png";
//     const std::string cache_file = cacheValue ? args::get(cacheValue) : "cache.sqlite";
//     const std::string asset_root = assetsValue ? args::get(assetsValue) : ".";

//     // Try to load the apikey from the environment.
//     const char* apikeyEnv = getenv("MLN_API_KEY");
//     const std::string apikey = apikeyValue ? args::get(apikeyValue) : (apikeyEnv ? apikeyEnv : std::string());

//     const bool debug = debugFlag ? args::get(debugFlag) : false;

//     using namespace mbgl;

//     auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
//     std::string style = styleValue ? args::get(styleValue) : mapTilerConfiguration.defaultStyles().at(0).getUrl();

//     util::RunLoop loop;

//     HeadlessFrontend frontend({width, height}, static_cast<float>(pixelRatio));
//     Map map(frontend,
//             MapObserver::nullObserver(),
//             MapOptions()
//                 .withMapMode(MapMode::Static)
//                 .withSize(frontend.getSize())
//                 .withPixelRatio(static_cast<float>(pixelRatio)),
//             ResourceOptions()
//                 .withCachePath(cache_file)
//                 .withAssetPath(asset_root)
//                 .withApiKey(apikey)
//                 .withTileServerOptions(mapTilerConfiguration));

//     if (style.find("://") == std::string::npos) {
//         style = std::string("file://") + style;
//     }

//     map.getStyle().loadURL(style);

//     //test xyz to latlon
    

//     std::vector<mbgl::CanonicalTileID> listTiles;
//     listTiles.push_back(mbgl::CanonicalTileID(5,25,13));
//     listTiles.push_back(mbgl::CanonicalTileID(5,25,12));
//     listTiles.push_back(mbgl::CanonicalTileID(5,25,11));
    
//     listTiles.push_back(mbgl::CanonicalTileID(5,26,10));
//     listTiles.push_back(mbgl::CanonicalTileID(5,26,11));
//     listTiles.push_back(mbgl::CanonicalTileID(5,26,12));
//     listTiles.push_back(mbgl::CanonicalTileID(5,26,13));


//     for (size_t i = 0; i < listTiles.size(); i++)
//     {
        
//         mbgl::CanonicalTileID& canonicalTileID = listTiles[i];
//         std::cout << "canonicalTileID.z: " << int(canonicalTileID.z) << std::endl;
//         std::stringstream ss;
//         ss << "tile_"<<int(canonicalTileID.z)<< "_" << canonicalTileID.x <<"_"<<canonicalTileID.y<<".png";
//         std::string pngName = ss.str();
//         mbgl::Point<double> latlon= XYZtoTileCenterLatLng(canonicalTileID);

//         double lat = latlon.y;
//         double lon = latlon.x;
//         double zoom = double(canonicalTileID.z);

//         std::cout << "lat: " << lat << std::endl;
//         std::cout << "lon: " << lon << std::endl;
//         std::cout << "zoom: " << zoom << std::endl;
//         {
//             map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom).withBearing(bearing).withPitch(pitch));
//         }

//         if (debug) {
//             map.setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus
//                             : mbgl::MapDebugOptions::NoDebug);
//         }

//         try {
//             std::ofstream out(pngName, std::ios::binary);
//             out << encodePNG(frontend.render(map).image);
//             out.close();
//         } catch (std::exception& e) {
//             std::cout << "Error: " << e.what() << std::endl;
//             exit(1);
//         }
//     }

//     return 0;
// }
