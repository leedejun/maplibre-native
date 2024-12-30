#pragma once
#include <array>

namespace feidu {

    // This struct is a 1:1 copy of mbgl::CustomLayerRenderParameters.
    struct CustomLayerRenderParameters {
        double width;
        double height;
        double latitude;
        double longitude;
        double zoom;
        double bearing;
        double pitch;
        double fieldOfView;
        std::array<double, 16> projectionMatrix;
        std::array<double, 16> viewMatrix;
        std::array<double, 16> modelMatrix;
        double worldSize;
        double minLon;
        double minLat;
        double maxLon;
        double maxLat;
    };

    class CustomLayerHostInterface {
        public:
            virtual ~CustomLayerHostInterface() = default;
            virtual void initialize() = 0;
            virtual void render(const CustomLayerRenderParameters&) = 0;
            virtual void deinitialize() = 0;
        };

    struct Bbox {
         double minLon;
        double minLat;
        double maxLon;
        double maxLat;
    };
}