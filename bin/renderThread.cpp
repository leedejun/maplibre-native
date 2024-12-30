#include "renderThread.hpp"
#include <mbgl/style/style.hpp>
#include <mbgl/style/light.hpp>
#include <iostream>
#include "RasterTilesCustomLayerHost.hpp"
#include <iostream>
#include <fstream>


renderThread* renderThread::m_instance = nullptr;

renderThread::renderThread(/* args */)
{
    m_threadPool = new UTThreadPool(1);
    m_map = nullptr;
    m_frontend = nullptr;
    m_mapReady = false;
    m_styleName = "basic";
}

renderThread::~renderThread()
{
    if (m_threadPool)
    {
        delete m_threadPool;
        m_threadPool = nullptr;
    }

    if (m_map)
    {
        delete m_map;
        m_map = nullptr;
    }

    if (m_frontend)
    {
        delete m_frontend;
        m_frontend = nullptr;
    }
    
}

renderThread* renderThread::instance()
{
    if (!m_instance)
    {
        static renderThread tempInstance = renderThread();
        m_instance = &tempInstance;
    }
    return m_instance;
}

void renderThread::prepareMap(std::string styleName)
{
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

    // if (m_threadPool)
    // {
    //     std::future<bool> resFuture = m_threadPool->Enqueue(
    //         [&,this]()
    //         {
    //             util::RunLoop loop;
    //             auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
    //             if (!m_frontend)
    //             {
    //                 m_frontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio), gfx::HeadlessBackend::SwapBehaviour::NoFlush,
    //                                 gfx::ContextMode::Unique);
    //             }

    //             if (!m_map)
    //             {
    //                 m_map = new Map(*m_frontend,
    //                     MapObserver::nullObserver(),
    //                     MapOptions()
    //                         .withMapMode(MapMode::Static)
    //                         .withSize(m_frontend->getSize())
    //                         .withPixelRatio(static_cast<float>(pixelRatio))
    //                         .withCrossSourceCollisions(true),
    //                         ResourceOptions()
    //                         .withCachePath(cache_file)
    //                         .withAssetPath(asset_root)
    //                         .withApiKey(apikey)
    //                         .withTileServerOptions(mapTilerConfiguration));
    //             }
                
    //             //set style
    //             std::string basicStyle = "./data/styles/basic.json";
    //             if (basicStyle.find("://") == std::string::npos) {
    //                 basicStyle = std::string("file://") + basicStyle;
    //             }
    //             m_map->getStyle().loadURL(basicStyle);
    //             std::thread::id threadID = std::this_thread::get_id ();
    //             std::cout << "prepareBasicMap Thread ID: " << threadID << std::endl;
    //             m_mapReady.store(true);
    //             std::cout << "prepareBasicMap m_mapReady: " << m_mapReady << std::endl;
    //             return true;
    //         }
    //     );

    //     while (!m_mapReady.load())
    //     {
    //         std::cout << "while prepareBasicMap m_mapReady: " << m_mapReady << std::endl;
    //         resFuture.wait();
    //     }

    //     std::cout << "after while prepareBasicMap m_mapReady: " << m_mapReady << std::endl;
    // }

    if(m_mapReady.load() && m_styleName ==styleName)
    {
        return;
    }

    m_mapReady.store(false);
    m_styleName = styleName;
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

    // util::RunLoop loop;
    auto mapTilerConfiguration = mbgl::TileServerOptions::MapTilerConfiguration();
    if (!m_frontend)
    {
        m_frontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio), gfx::HeadlessBackend::SwapBehaviour::NoFlush,
                        gfx::ContextMode::Unique);
    }

    if (!m_map)
    {
        m_map = new Map(*m_frontend,
            MapObserver::nullObserver(),
            MapOptions()
                .withMapMode(MapMode::Tile)
                .withSize(m_frontend->getSize())
                .withPixelRatio(static_cast<float>(pixelRatio))
                .withCrossSourceCollisions(true),
                ResourceOptions()
                .withCachePath(cache_file)
                .withAssetPath(asset_root)
                .withApiKey(apikey)
                .withTileServerOptions(mapTilerConfiguration));
    }
    
    ////set style
    //std::string stylePath = std::string("./data/styles/")+ m_styleName + ".json";
    //if (stylePath.find("://") == std::string::npos) {
    //    stylePath = std::string("file://") + stylePath;
    //}
    //m_map->getStyle().loadURL(stylePath);

    //test style
    std::string styleJson = readJsonFile("D:\\work\\maplibre-native\\data\\style.json");
    m_map->getStyle().loadJSON(styleJson);

    //test
    std::unique_ptr<feidu::CustomLayerHostInterface> host = nullptr;
    host.reset(new RasterTilesCustomLayerHost());
    m_map->addCustomLayer("testtif", std::move(host));

    m_mapReady.store(true);
}

bool renderThread::isMapReady()
{
    return m_mapReady.load();
}

std::string renderThread::readJsonFile(const std::string& filePath) {
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

std::string renderThread::renderMap(const std::string& styleName, double zoom, double lon, double lat)
{
    std::string style = styleName;
    std::future<std::string> resFuture = renderThread::instance()->Enqueue(
    [&,this]() {
        std::string strPng = "";
        static util::RunLoop loop(util::RunLoop::Type::New);
        prepareMap(style);
        if (m_map && m_frontend)
        {
            m_map->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
            strPng = encodePNG(m_frontend->renderInLoop(*m_map, loop).image);
        }
        return strPng;
    }
    );
    return resFuture.get();
}