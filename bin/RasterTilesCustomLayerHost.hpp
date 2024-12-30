#pragma once
#include "ftmap.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include <map>
#include "dataManager.h"

class RasterTilesCustomLayerHost: public feidu::CustomLayerHostInterface
{
private:
    int  iniOpenglByGLAD();
    unsigned int LoadGeoTIFFAsTexture(const char* filePath);
    void drawTexture(const char* filePath, feidu::Bbox& box, double offsetX, double offsetY,glm::mat4& projection);

    bool m_hasIniOpengl;
    Shader* m_ourShader;

    std::map<std::string, unsigned int> m_file2textureMap;
    dataManager m_dataManager;

public:
    RasterTilesCustomLayerHost(/* args */);
    ~RasterTilesCustomLayerHost();
    
    void initialize() override;
	void render(const feidu::CustomLayerRenderParameters& param) override;
	void deinitialize() override;

};


