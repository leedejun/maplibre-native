#include "interface-basictiles.hpp"
#include <mbgl/map/map_options.hpp>
#include <mbgl/util/image.hpp>
#include <mbgl/tile/tile_id.hpp>
#include <mbgl/util/geometry.hpp>
#include<mbgl/util/constants.hpp>
#include <mbgl/style/style.hpp>
#include <mbgl/style/light.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include "renderThread.hpp"


using namespace mbgl;

namespace feidu
{
  Map* interface_basictiles::g_map = nullptr;
  HeadlessFrontend* interface_basictiles::g_frontend = nullptr;


  struct interface_basictiles::RenderOptions {
    double zoom = 0;
    double bearing = 0;
    mbgl::style::Light light;
    double pitch = 0;
    double latitude = 0;
    double longitude = 0;
    mbgl::Size size = {512, 512};
    bool axonometric = false;
    double xSkew = 0;
    double ySkew = 1;
    std::vector<std::string> classes;
    mbgl::MapDebugOptions debugOptions = mbgl::MapDebugOptions::NoDebug;
};

  // interface_basictiles::interface_basictiles()
  // {
  //   // g_UTThreadPool = new UTThreadPool(1);
  //   // g_UTThreadPool->Enqueue(
  //   //   [](){
  //   //     interface_basictiles::initMap();
  //   //   }
  //   // );


  // // loadedStyle = false;
  // // m_isrendered = false;

  // // std::cout << " interface_basictiles Constructor 0!" << std::endl;
  // // const uint32_t width =  512;
  // // const uint32_t height =  512;

  // // pixelRatio = 1.0f;
  // // mode = MapMode::Tile;
  // // crossSourceCollisions = true;
  // // std::cout << " interface_basictiles Constructor 1.1!" << std::endl;

  // // {
  // //   util::RunLoop loop;
  // //   frontend = std::make_unique<mbgl::HeadlessFrontend>(mbgl::Size{512, 512}, static_cast<float>(pixelRatio), 
  // //   gfx::HeadlessBackend::SwapBehaviour::NoFlush, gfx::ContextMode::Shared);

  // //   std::cout << " interface_basictiles Constructor 1.2!" << std::endl;
  // //   map = std::make_unique<mbgl::Map>(*frontend,
  // //                                       MapObserver::nullObserver(),
  // //                                       mbgl::MapOptions()
  // //                                           .withSize(frontend->getSize())
  // //                                           .withPixelRatio(pixelRatio)
  // //                                           .withMapMode(mode)
  // //                                           .withCrossSourceCollisions(crossSourceCollisions),
  // //                                       mbgl::ResourceOptions().withPlatformContext(reinterpret_cast<void*>(this)),
  // //                                       mbgl::ClientOptions());
  // //   // loop.stop();
  // // }
  
  // //   std::cout << " interface_basictiles Constructor 2!" << std::endl;
  // //   async = new uv_async_t;
  // //   std::cout << " interface_basictiles Constructor 3!" << std::endl;
  // //   async->data = this;
  // //   uv_async_init(
  // //       uv_default_loop(), async, [](uv_async_t* h) { 
  // //         reinterpret_cast<interface_basictiles*>(h->data)->renderFinished(); 
  // //         });

  // //   // Make sure the async handle doesn't keep the loop alive.
  // //   uv_unref(reinterpret_cast<uv_handle_t*>(async));
  // //   std::cout << " interface_basictiles Constructor 4!" << std::endl;
  //   // loop.stop();
  // }

  // interface_basictiles::~interface_basictiles()
  // {
  //   // if (g_UTThreadPool)
  //   // {
  //   //   delete g_UTThreadPool;
  //   //   g_UTThreadPool = nullptr;
  //   // }
  // }

  void interface_basictiles::initMap()
  {
  // std::cout << " initMap server step1" << std::endl;

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

  // std::cout << " initMap server step2" << std::endl;

  // auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();

  // util::RunLoop loop;
  // g_frontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio));

  // std::cout << " initMap server step3" << std::endl;

  // g_map = new Map(*g_frontend,
  //         MapObserver::nullObserver(),
  //         MapOptions()
  //             .withMapMode(MapMode::Tile)
  //             .withSize(g_frontend->getSize())
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
  // g_map->getStyle().loadURL(basicStyle);

  // std::cout << " initMap server step4" << std::endl;
  }

void interface_basictiles::operator()(const httplib::Request & req, httplib::Response & res)
{

  // 获取请求参数
  if (req.matches.size()<4)
  {
    return;
  }

  int x =0;
  int y =0;
  int z =0;
  
  std::string fullPath = req.matches[0];
  std::string strZ = req.matches[1];
  z = std::stoi(strZ);
  std::string strX = req.matches[2];
  x = std::stoi(strX);
  
  std::string strY = req.matches[3];
  y = std::stoi(strY);

  std::cout << " z=" << z << std::endl;
  std::cout << " x=" << x << std::endl;
  std::cout << " y=" << y << std::endl;

  // util::RunLoop loop;
  std::cout << " interface_basictiles start!" << std::endl;
  // if (g_map==nullptr || g_frontend==nullptr)
  // {
  //   initMap();
  // }

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

  std::cout << " server step1" << std::endl;

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

  std::cout << " server step2" << std::endl;

  //set style once
  // if (!loadedStyle.load())
  // {
  //   // util::RunLoop loop;
  //   std::string basicStyle = "./data/styles/basic.json";
  //   if (basicStyle.find("://") == std::string::npos) {
  //   basicStyle = std::string("file://") + basicStyle;
  //   }
  //   map->getStyle().loadURL(basicStyle);
  //   loadedStyle.store(true);
  // }
  
  std::cout << " interface_basictiles step 1" << std::endl;
    // int x =0;
    // int y =0;
    // int z =0;
    
    // if (req.has_param("x")) {
    //   std::string strX = req.get_param_value("x");
    //   x = std::stoi(strX);

    //   std::cout << " x=" << x << std::endl;
    // }

    // if (req.has_param("y")) {
    //   std::string strY = req.get_param_value("y");
    //   y = std::stoi(strY);

    //   std::cout << " y=" << y << std::endl;
    // }

    // if (req.has_param("z")) {
    //   std::string strZ = req.get_param_value("z");
    //   z = std::stoi(strZ);

    //   std::cout << " z=" << z << std::endl;
    // }

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
        // static bool testOnce = false;
        // if (!testOnce)
        // std::mutex mtx;
        // {
        //   std::lock_guard<std::mutex> lk(mtx);
        //   g_map->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
        // }
        // testOnce = true;
    }

    // if (debug) {
    //     g_map->setDebug(debug ? mbgl::MapDebugOptions::TileBorders | mbgl::MapDebugOptions::ParseStatus
    //                     : mbgl::MapDebugOptions::NoDebug);
    // }

    std::cout << " interface_basictiles step 4" << std::endl;

    try {
      // {
      //   // util::RunLoop loop;
      //   interface_basictiles::RenderOptions options;
      //   options.zoom = zoom;
      //   options.longitude = lon;
      //   options.latitude = lat;
      //   startRender(options);
      //   while (!image.valid() && !m_isrendered.load()) {
      //     // util::RunLoop::Get()->runOnce();
      //     loop.runOnce();
      //   }
      // }

      // if (error)
      // {
      //   uv_unref(reinterpret_cast<uv_handle_t*>(async));
      //   handle_eptr(error);
      //   error = nullptr;
      //   auto img = std::move(image);
      //   assert(!image.data);
      // }
      // else
      // {
      //   // std::unique_lock<std::mutex> lock(mtx); // 上锁
      //     // cv.wait(lock, [this] { return image.valid(); }); // 等待条件满足
      //     uv_unref(reinterpret_cast<uv_handle_t*>(async));
      //     auto img = std::move(image);
      //     assert(!image.data);
      //     std::string str = encodePNG(img);
      //     res.status = 200;
      //     res.set_content(str, "image/png");
      // }
      std::string str="";
      std::future<std::string> resFuture = renderThread::instance()->Enqueue(
        [=,&str]() {
          auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
          util::RunLoop loop(util::RunLoop::Type::New);
          HeadlessFrontend frontend({width, height}, static_cast<float>(pixelRatio), gfx::HeadlessBackend::SwapBehaviour::NoFlush,
                            gfx::ContextMode::Unique);

          Map map(frontend,
                  MapObserver::nullObserver(),
                  MapOptions()
                      .withMapMode(MapMode::Static)
                      .withSize(frontend.getSize())
                      .withPixelRatio(static_cast<float>(pixelRatio))
                      .withCrossSourceCollisions(true),
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
          map.getStyle().loadURL(basicStyle);
          std::thread::id threadID = std::this_thread::get_id ();
          std::cout << "http get Thread ID: " << threadID << std::endl;
          map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
          // map.jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom).withBearing(bearing).withPitch(pitch));
          str = encodePNG(frontend.renderInLoop(map, loop).image);
          return str;
        }
      );

      while(str=="")
      {
        resFuture.wait();
      }
      // std::cout << "str: " << str << std::endl;
      res.status = 200;
      res.set_content(str, "image/png");
      // util::RunLoop loop;
      // std::thread::id threadID = std::this_thread::get_id ();
      // std::cout << "Thread ID: " << threadID << std::endl;
      // g_map->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
      // std::string str = encodePNG(g_frontend->render(*g_map).image);
      // res.status = 200;
      // res.set_content(str, "image/png");

      // g_UTThreadPool->Enqueue([=,&g_map,&g_frontend]()
      // {
      //   // util::RunLoop loop;
      //   std::thread::id threadID = std::this_thread::get_id ();
      //   std::cout << "Thread ID: " << threadID << std::endl;
      //   g_map->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
      //   std::string str = encodePNG(g_frontend->render(*g_map).image);
      //   // res.status = 200;
      //   // res.set_content(str, "image/png");
      // }
      // );
      
    } catch (std::exception& e) {
        std::string str = std::string("Error: ") + e.what();
        res.status = 400;
        res.set_content(str, "text/plain");

        std::cout << " interface_basictiles step 4.1:" <<str<< std::endl;
    }
    std::cout << " interface_basictiles step 5" << std::endl;
    // loop.stop();
}

void interface_basictiles::startRender(const interface_basictiles::RenderOptions& options) 
{
    // m_isrendered.store(false);
    // frontend->setSize(options.size);
    // map->setSize(options.size);

    // mbgl::CameraOptions camera;
    // camera.center = mbgl::LatLng{options.latitude, options.longitude};
    // camera.zoom = options.zoom;
    // camera.bearing = options.bearing;
    // camera.pitch = options.pitch;

    // auto projectionOptions =
    //     mbgl::ProjectionMode().withAxonometric(options.axonometric).withXSkew(options.xSkew).withYSkew(options.ySkew);

    // map->setProjectionMode(projectionOptions);

    // map->renderStill(camera, options.debugOptions, [this](const std::exception_ptr& eptr) {
    //     if (eptr) {
    //         // std::unique_lock<std::mutex> lock(mtx); // 上锁
    //         error = eptr;
    //         m_isrendered.store(true);
    //         // cv.notify_one(); // 通知一个等待的线程
    //         uv_async_send(async);
    //     } else {
    //         // std::unique_lock<std::mutex> lock(mtx); // 上锁
    //         assert(!image.data);
    //         image = frontend->readStillImage();
    //         m_isrendered.store(true);
    //         // cv.notify_one(); // 通知一个等待的线程
    //         uv_async_send(async);
    //     }
    // });

    // // Retain this object, otherwise it might get destructed before we are
    // // finished rendering the still image.
    // // Ref();

    // // Similarly, we're now waiting for the async to be called, so we need to
    // // make sure that it keeps the loop alive.
    // uv_ref(reinterpret_cast<uv_handle_t*>(async));
}

void interface_basictiles::renderFinished() {
    // assert(req);

    // Nan::HandleScope scope;

    // We're done with this render call, so we're unrefing so that the loop could close.
    // uv_unref(reinterpret_cast<uv_handle_t*>(async));
    // m_isrendered.store(true);

    // Move the callback and image out of the way so that the callback can start a new render call.
    // auto request = std::move(req);
    // auto img = std::move(image);
    // assert(request);

    // These have to be empty to be prepared for the next render call.
    // assert(!req);
    // assert(!image.data);

    // v8::Local<v8::Function> callback = Nan::New(request->callback);
    // v8::Local<v8::Object> target = Nan::New<v8::Object>();

    // if (error) {
        
    // } else if (img.data) {
        
    // } else {
    //   std::cout << " Didn't get an image" << std::endl;
    // }

    // There is no render pending anymore, we the GC could now delete this
    // object if it went out of scope.
    // Unref();
}

}  // namespace feidu