#include "renderThread.hpp"
#include <mbgl/style/style.hpp>
#include <mbgl/style/light.hpp>
#include <iostream>


renderThread* renderThread::m_instance = nullptr;

renderThread::renderThread(/* args */)
{
    m_threadPool = new UTThreadPool(1);
    m_basicMapReady = false;
}

renderThread::~renderThread()
{
    if (m_threadPool)
    {
        delete m_threadPool;
        m_threadPool = nullptr;
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

void renderThread::prepareBasicMap()
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
    //             if (!m_basicFrontend)
    //             {
    //                 m_basicFrontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio), gfx::HeadlessBackend::SwapBehaviour::NoFlush,
    //                                 gfx::ContextMode::Unique);
    //             }

    //             if (!m_basicMap)
    //             {
    //                 m_basicMap = new Map(*m_basicFrontend,
    //                     MapObserver::nullObserver(),
    //                     MapOptions()
    //                         .withMapMode(MapMode::Static)
    //                         .withSize(m_basicFrontend->getSize())
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
    //             m_basicMap->getStyle().loadURL(basicStyle);
    //             std::thread::id threadID = std::this_thread::get_id ();
    //             std::cout << "prepareBasicMap Thread ID: " << threadID << std::endl;
    //             m_basicMapReady.store(true);
    //             std::cout << "prepareBasicMap m_basicMapReady: " << m_basicMapReady << std::endl;
    //             return true;
    //         }
    //     );

    //     while (!m_basicMapReady.load())
    //     {
    //         std::cout << "while prepareBasicMap m_basicMapReady: " << m_basicMapReady << std::endl;
    //         resFuture.wait();
    //     }

    //     std::cout << "after while prepareBasicMap m_basicMapReady: " << m_basicMapReady << std::endl;
    // }

    if(m_basicMapReady.load())
    {
        return;
    }

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
    if (!m_basicFrontend)
    {
        m_basicFrontend = new HeadlessFrontend({width, height}, static_cast<float>(pixelRatio), gfx::HeadlessBackend::SwapBehaviour::NoFlush,
                        gfx::ContextMode::Unique);
    }

    if (!m_basicMap)
    {
        m_basicMap = new Map(*m_basicFrontend,
            MapObserver::nullObserver(),
            MapOptions()
                .withMapMode(MapMode::Tile)
                .withSize(m_basicFrontend->getSize())
                .withPixelRatio(static_cast<float>(pixelRatio))
                .withCrossSourceCollisions(true),
                ResourceOptions()
                .withCachePath(cache_file)
                .withAssetPath(asset_root)
                .withApiKey(apikey)
                .withTileServerOptions(mapTilerConfiguration));
    }
    
    //set style
    std::string basicStyle = "./data/styles/basic.json";
    if (basicStyle.find("://") == std::string::npos) {
        basicStyle = std::string("file://") + basicStyle;
    }
    m_basicMap->getStyle().loadURL(basicStyle);
    m_basicMapReady.store(true);
}

bool renderThread::isBasicMapReady()
{
    return m_basicMapReady.load();
}

std::string renderThread::renderBasicMap(double zoom, double lon, double lat)
{
    std::string str="";
    std::future<std::string> resFuture = renderThread::instance()->Enqueue(
    [&,this,&str]() {
        static util::RunLoop loop(util::RunLoop::Type::New);
        std::thread::id threadID = std::this_thread::get_id ();
        prepareBasicMap();
        if (m_basicMap && m_basicFrontend)
        {
            m_basicMap->jumpTo(CameraOptions().withCenter(LatLng{lat, lon}).withZoom(zoom));
            str = encodePNG(m_basicFrontend->renderInLoop(*m_basicMap, loop).image);
        }
        return str;
    }
    );

    while(str=="")
    {
        resFuture.wait();
    }

    return str;
}