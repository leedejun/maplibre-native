#pragma once

#include "httplib.h"
#include <mbgl/map/map.hpp>
#include <mbgl/gfx/backend.hpp>
#include <mbgl/gfx/headless_frontend.hpp>
using namespace mbgl;

namespace feidu{
class interface_basictiles
{
public:

  void operator()(const httplib::Request & req, httplib::Response & res);
  static void initMap(); 

  protected:

  static Map* g_map;
  static HeadlessFrontend* g_frontend;
};
}