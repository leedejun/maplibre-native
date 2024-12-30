#include <mbgl/gfx/backend_scope.hpp>
#include <mbgl/gfx/renderer_backend.hpp>
#include <mbgl/style/layers/custom_layer_impl.hpp>
#include <mbgl/renderer/layers/render_custom_layer.hpp>
#include <mbgl/map/transform_state.hpp>
#include <mbgl/math/angles.hpp>
#include <mbgl/renderer/bucket.hpp>
#include <mbgl/renderer/paint_parameters.hpp>
#include <mbgl/util/mat4.hpp>

#if MLN_LEGACY_RENDERER
#include <mbgl/platform/gl_functions.hpp>
#include <mbgl/gl/context.hpp>
#include <mbgl/gl/renderable_resource.hpp>
#endif

#if MLN_DRAWABLE_RENDERER
#include <mbgl/gfx/context.hpp>
#include <mbgl/renderer/layer_group.hpp>
#include <mbgl/gfx/drawable_custom_layer_host_tweaker.hpp>
#include <mbgl/gfx/drawable_builder.hpp>

#if !MLN_LEGACY_RENDERER
// TODO: platform agnostic error checks
#define MBGL_CHECK_ERROR(cmd) (cmd)
#endif
#endif

namespace mbgl {

using namespace style;

namespace {

inline const CustomLayer::Impl& impl(const Immutable<style::Layer::Impl>& impl) {
    assert(impl->getTypeInfo() == CustomLayer::Impl::staticTypeInfo());
    return static_cast<const CustomLayer::Impl&>(*impl);
}

} // namespace

RenderCustomLayer::RenderCustomLayer(Immutable<style::CustomLayer::Impl> _impl)
    : RenderLayer(makeMutable<CustomLayerProperties>(std::move(_impl))),
      host(impl(baseImpl).host) {
    assert(gfx::BackendScope::exists());
    MBGL_CHECK_ERROR(host->initialize());
}

RenderCustomLayer::~RenderCustomLayer() {
    assert(gfx::BackendScope::exists());
    if (contextDestroyed) {
        host->contextLost();
    } else {
        MBGL_CHECK_ERROR(host->deinitialize());
    }
}

void RenderCustomLayer::evaluate(const PropertyEvaluationParameters&) {
    passes = RenderPass::Translucent;
    // It is fine to not update `evaluatedProperties`, as `baseImpl` should never be updated for this layer.
}

bool RenderCustomLayer::hasTransition() const {
    return false;
}
bool RenderCustomLayer::hasCrossfade() const {
    return false;
}

void RenderCustomLayer::markContextDestroyed() {
    contextDestroyed = true;
}

void RenderCustomLayer::prepare(const LayerPrepareParameters&) {}

#if MLN_LEGACY_RENDERER
void RenderCustomLayer::render(PaintParameters& paintParameters) {
    if (host != impl(baseImpl).host) {
        // If the context changed, deinitialize the previous one before initializing the new one.
        if (host && !contextDestroyed) {
            MBGL_CHECK_ERROR(host->deinitialize());
        }
        host = impl(baseImpl).host;
        MBGL_CHECK_ERROR(host->initialize());
    }

    // TODO: remove cast
    auto& glContext = static_cast<gl::Context&>(paintParameters.context);
    const TransformState& state = paintParameters.state;

    // Reset GL state to a known state so the CustomLayer always has a clean slate.
    glContext.bindVertexArray = 0;
  /*  glContext.setDepthMode(paintParameters.depthModeForSublayer(0, gfx::DepthMaskType::ReadOnly));
    glContext.setStencilMode(gfx::StencilMode::disabled());
    glContext.setColorMode(paintParameters.colorModeForRenderPass());
    glContext.setCullFaceMode(gfx::CullFaceMode::disabled());*/

    glContext.setDepthMode(paintParameters.depthModeFor3D());
	glContext.setStencilMode(gfx::StencilMode::disabled());
	glContext.setColorMode(paintParameters.colorModeForRenderPass());
	glContext.setCullFaceMode(gfx::CullFaceMode::disabled());

    CustomLayerRenderParameters parameters;

    parameters.width = state.getSize().width;
    parameters.height = state.getSize().height;
    parameters.latitude = state.getLatLng().latitude();
    parameters.longitude = state.getLatLng().longitude();
    parameters.zoom = state.getZoom();
    parameters.bearing = util::rad2deg(-state.getBearing());
    parameters.pitch = state.getPitch();
    parameters.fieldOfView = state.getFieldOfView();
    parameters.worldSize = Projection::worldSize(state.getScale());
    /*mat4 projMatrix;
    state.getProjMatrix(projMatrix);*/

    //计算当前地图范围bounds
    auto topLeft = state.screenCoordinateToLatLng({ 0, static_cast<float>(state.getSize().height) });
	auto bottomRight = state.screenCoordinateToLatLng({ static_cast<float>(state.getSize().width), 0 });

	LatLngBounds  bounds = LatLngBounds::hull(topLeft, bottomRight);

	parameters.minLon = bounds.west();
	parameters.minLat = bounds.south();
	parameters.maxLon = bounds.east();
	parameters.maxLat = bounds.north();

    mat4 projMatrix;
	mbgl::matrix::identity(projMatrix);
	//state.getProjMatrix(projMatrix);
	state.getMercatorMatrix(projMatrix);

    const double worldSize = Projection::worldSize(state.getScale());
	matrix::scale(projMatrix, projMatrix, worldSize, worldSize, worldSize);
	//shanghai
	mbgl::LatLng dataCenter(40.3233243239999979, 96.4899729729999933);
	//beijing
	//mbgl::LatLng dataCenter(39.953266083056526, 116.44502881591701);
	mbgl::Point<double> transformModel = mbgl::Projection::mercatorXYFromLatLng(dataCenter);
	double transformModelZ = mbgl::Projection::mercatorZfromAltitude(10, transformModel.y);
	MercatorCoordinate merc(transformModel.x, transformModel.y, transformModelZ);
	double scaleModel2Mector = merc.meterInMercatorCoordinateUnits();

	mat4 modelMatrix;
	mbgl::matrix::identity(modelMatrix);
	mbgl::matrix::translate(modelMatrix, modelMatrix, transformModel.x, transformModel.y, transformModelZ);
	mbgl::matrix::scale(modelMatrix, modelMatrix, scaleModel2Mector, -scaleModel2Mector, scaleModel2Mector);
	mbgl::matrix::multiply(projMatrix, projMatrix, modelMatrix);

    parameters.projectionMatrix = projMatrix;
    MBGL_CHECK_ERROR(host->render(parameters));

    // Reset the view back to our original one, just in case the CustomLayer
    // changed the viewport or Framebuffer.
    paintParameters.backend.getDefaultRenderable().getResource<gl::RenderableResource>().bind();
    glContext.setDirtyState();
}
#endif

#if MLN_DRAWABLE_RENDERER
void RenderCustomLayer::update([[maybe_unused]] gfx::ShaderRegistry& shaders,
                               gfx::Context& context,
                               [[maybe_unused]] const TransformState& state,
                               [[maybe_unused]] const RenderTree& renderTree,
                               [[maybe_unused]] UniqueChangeRequestVec& changes) {
    std::unique_lock<std::mutex> guard(mutex);

    // create layer group
    if (!layerGroup) {
        if (auto layerGroup_ = context.createLayerGroup(layerIndex, /*initialCapacity=*/1, getID())) {
            setLayerGroup(std::move(layerGroup_), changes);
        }
    }

    auto* localLayerGroup = static_cast<LayerGroup*>(layerGroup.get());

    // check if host changed and update
    bool hostChanged = (host != impl(baseImpl).host);
    if (hostChanged) {
        // If the context changed, deinitialize the previous one before initializing the new one.
        if (host && !contextDestroyed) {
            MBGL_CHECK_ERROR(host->deinitialize());
        }
        host = impl(baseImpl).host;
        MBGL_CHECK_ERROR(host->initialize());
    }

    // create drawable
    if (localLayerGroup->getDrawableCount() == 0 || hostChanged) {
        localLayerGroup->clearDrawables();

        // create tweaker
        auto tweaker = std::make_shared<gfx::DrawableCustomLayerHostTweaker>(host);

        // create empty drawable using a builder
        std::unique_ptr<gfx::DrawableBuilder> builder = context.createDrawableBuilder(getID());
        auto& drawable = builder->getCurrentDrawable(true);
        drawable->setIsCustom(true);
        drawable->setRenderPass(RenderPass::Translucent);

        // assign tweaker to drawable
        drawable->addTweaker(tweaker);

        // add drawable to layer group
        localLayerGroup->addDrawable(std::move(drawable));
        ++stats.drawablesAdded;
    }
}
#endif

} // namespace mbgl
