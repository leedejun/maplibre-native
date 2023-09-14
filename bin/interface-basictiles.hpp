#pragma once

#include "httplib.h"
#include <mbgl/map/map.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
#include "ut_thread_pool.h"
#include <mbgl/util/image.hpp>
#include <uv.h>
#include <atomic>
#include <mbgl/util/run_loop.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace mbgl;

namespace feidu{
class interface_basictiles
{
public:

  struct RenderOptions;

  // interface_basictiles();
  // ~interface_basictiles();

  void operator()(const httplib::Request & req, httplib::Response & res);
  void initMap(); 

  void startRender(const RenderOptions& options);
  void renderFinished();

  void handle_eptr(std::exception_ptr eptr) // passing by value is ok
  {
      try {
          if (eptr) {
              std::rethrow_exception(eptr);
          }
      } catch(const std::exception& e) {
          std::cout << "Caught exception \"" << e.what() << "\"\n";
      }
  }

  protected:

  static Map* g_map;
  static HeadlessFrontend* g_frontend;
  // UTThreadPool* g_UTThreadPool = nullptr;

  ///
  // float pixelRatio;
  // mbgl::MapMode mode;
  // bool crossSourceCollisions;
  // std::unique_ptr<mbgl::HeadlessFrontend> frontend;
  // std::unique_ptr<mbgl::Map> map;

  // std::exception_ptr error;
  // mbgl::PremultipliedImage image;
  // std::unique_ptr<RenderRequest> req;

  // Async for delivering the notifications of render completion.
    // uv_async_t* async;
    // std::atomic_bool loadedStyle;

    // std::mutex mtx; // 互斥锁
    // std::condition_variable cv; // 条件变量
    // std::atomic_bool m_isrendered;
};
}