#include "RasterTilesCustomLayerHost.hpp"
#include <tiffio.h>
#include <iostream>
#include <glad/glad.h>
#include <mbgl/util/geometry.hpp>
#include <mbgl/util/projection.hpp>


RasterTilesCustomLayerHost::RasterTilesCustomLayerHost(/* args */)
{
    
}

RasterTilesCustomLayerHost::~RasterTilesCustomLayerHost()
{
    m_hasIniOpengl = false;
}

void RasterTilesCustomLayerHost::initialize() 
{
    if (iniOpenglByGLAD()!=0)
    {
        m_hasIniOpengl = true;
        m_ourShader = new Shader("D:\\work\\maplibre-native\\bin\\texture.vs", "D:\\work\\maplibre-native\\bin\\texture.fs");
        m_dataManager.buildDataTree();
    }
}

int  RasterTilesCustomLayerHost::iniOpenglByGLAD()
{
	return gladLoadGL();
}

void RasterTilesCustomLayerHost::render(const feidu::CustomLayerRenderParameters& param) 
{
    //return;
    if (!m_hasIniOpengl) return;
    //projection mat
	glm::mat4 projection = glm::mat4(1.0f);
	int proIndex = 0;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			projection[i][j] = param.projectionMatrix[proIndex];
			proIndex++;
		}
	}

    mbgl::LatLng offset(40.3233243239999979, 96.4899729729999933);
    mbgl::ProjectedMeters mercatorOffset = mbgl::Projection::projectedMetersForLatLng(offset);

 //   //空间查询
 //   double minLonLat[2] = { param.minLon, param.minLat };
	//double maxLonLat[2] = { param.maxLon, param.maxLat };

	//std::vector<SatImageInfo> searchResult;
	//std::list<SatImageInfo> needRenderList;
	//if (m_dataManager.search(minLonLat, maxLonLat, searchResult)>0)
	//{
 //       for(int i= 0; i < searchResult.size();++i)
	//	{
	//		SatImageInfo satImageInfo = searchResult[i];
	//		needRenderList.push_back(satImageInfo);
	//	}

	//}

	//for(auto& var : needRenderList)
	//{
	//	std::string filePath = var.filePath;
 //       double minLon = var.box.min[0];
 //       double minLat = var.box.min[1];
 //       double maxLon = var.box.max[0];
 //       double maxLat = var.box.max[1];
 //       feidu::Bbox box = {minLon,minLat,maxLon,maxLat};
 //       drawTexture(filePath.c_str(),box,mercatorOffset.easting(),mercatorOffset.northing(), projection);
	//}


    //96.4899729729999933,40.3233243239999979 : 97.0100270270000067,40.6766756760000021
    {
         std::string filePath = "D:\\workDoc\\lidejun\\data\\testdata\\RD0100_DataCloud_K47D011002_2024H1_4FF_4326.tif";
        double minLon = 96.4899729729999933;
        double minLat = 40.3233243239999979;
        double maxLon = 97.0100270270000067;
        double maxLat = 40.6766756760000021;
        feidu::Bbox box = {minLon,minLat,maxLon,maxLat};
        drawTexture(filePath.c_str(),box,mercatorOffset.easting(),mercatorOffset.northing(), projection);
    }
   

    //107.4899729729729927,40.6566486486486554 : 108.0100000000000193,41.0100000000000051
    {
        std::string filePath = "D:\\workDoc\\lidejun\\data\\testdata\\RD0100_DataCloud_K48D010012_2024H1_4FF_4326.tif";
        double minLon = 107.4899729729729927;
        double minLat = 40.6566486486486554;
        double maxLon = 108.0100000000000193;
        double maxLat = 41.0100000000000051;
        feidu::Bbox box = {minLon,minLat,maxLon,maxLat};
        drawTexture(filePath.c_str(),box,mercatorOffset.easting(),mercatorOffset.northing(), projection);
    }
}

void RasterTilesCustomLayerHost::drawTexture(const char* filePath, feidu::Bbox& box, double offsetX, double offsetY, glm::mat4& projection)
{
    dataManager dataMgr = dataManager();
    double minLon = box.minLon;
    double minLat = box.minLat;
    double maxLon = box.maxLon;
    double maxLat = box.maxLat;

	mbgl::LatLng topRight(maxLat, maxLon);
    mbgl::LatLng bottomRight(minLat, maxLon);
    mbgl::LatLng bottomLeft(minLat, minLon);
    mbgl::LatLng topLeft (maxLat, minLon);
    mbgl::ProjectedMeters mercatorTopRight = mbgl::Projection::projectedMetersForLatLng(topRight);
    mbgl::ProjectedMeters mercatorBottomRight = mbgl::Projection::projectedMetersForLatLng(bottomRight);
    mbgl::ProjectedMeters mercatorBottomLeft = mbgl::Projection::projectedMetersForLatLng(bottomLeft);
    mbgl::ProjectedMeters mercatorTopLeft = mbgl::Projection::projectedMetersForLatLng(topLeft);
    /*mbgl::Point<double> mercatorTopRight = mbgl::Projection::mercatorXYFromLatLng(topRight);
    mbgl::Point<double> mercatorBottomRight = mbgl::Projection::mercatorXYFromLatLng(bottomRight);
    mbgl::Point<double> mercatorBottomLeft = mbgl::Projection::mercatorXYFromLatLng(bottomLeft);
    mbgl::Point<double> mercatorTopLeft = mbgl::Projection::mercatorXYFromLatLng(topLeft);*/

    // build and compile our shader zprogram
    // ------------------------------------
    //Shader ourShader("D:\\work\\maplibre-native\\bin\\texture.vs", "D:\\work\\maplibre-native\\bin\\texture.fs"); 

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    //float vertices[] = {
    //    // positions          // colors           // texture coords
    //     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    //     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    //    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    //    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    //};
    float vertices[] = {
        // positions          // colors           // texture coords
        mercatorTopRight.easting()-offsetX,  mercatorTopRight.northing()-offsetY, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        mercatorBottomRight.easting()-offsetX, mercatorBottomRight.northing()-offsetY, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        mercatorBottomLeft.easting()-offsetX, mercatorBottomLeft.northing()-offsetY, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        mercatorTopLeft.easting()-offsetX,  mercatorTopLeft.northing()-offsetY, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };

    //float vertices[] = {
    //    // positions          // colors           // texture coords
    //    mercatorTopRight.easting(),  mercatorTopRight.northing(), 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    //    mercatorBottomRight.easting(), mercatorBottomRight.northing(), 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    //    mercatorBottomLeft.easting(), mercatorBottomLeft.northing(), 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    //    mercatorTopLeft.easting(),  mercatorTopLeft.northing(), 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    //};

    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture = -1;
    std::map<std::string, unsigned int>::iterator itor = m_file2textureMap.find(filePath);
    if (itor!=m_file2textureMap.end()) {
        texture = itor->second;
    }
    else
    {
        texture = dataMgr.LoadGeoTIFFAsTexture(filePath);
        m_file2textureMap.insert(std::make_pair<>(filePath,texture ));
    }
     

    // render loop
    // -----------
    //while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        //processInput(window);

        // render
        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        m_ourShader->use();
        m_ourShader->setMat4("projection", projection);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    //glDeleteTextures(1, &texture);
}

void RasterTilesCustomLayerHost::deinitialize() 
{

}

unsigned int RasterTilesCustomLayerHost::LoadGeoTIFFAsTexture(const char* filePath)
{
    TIFF* tiff = TIFFOpen(filePath, "r");
    if (!tiff) {
        std::cerr << "无法打开文件: " << filePath << std::endl;
        return 0;
    }

    uint32 width, height;
    size_t npixels;
    uint32* raster;

    TIFFGetField(tiff, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(tiff, TIFFTAG_IMAGELENGTH, &height);
    npixels = width * height;
    raster = (uint32*) _TIFFmalloc(npixels * sizeof(uint32));
    GLuint textureID = -1;
    if (raster != nullptr) {
        if (TIFFReadRGBAImage(tiff, width, height, raster, 0)) {
            // 创建OpenGL纹理
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster);

            // 设置纹理参数
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            std::cout << "成功读取并创建纹理: " << filePath << std::endl;
        }
        _TIFFfree(raster);
    }

    TIFFClose(tiff);
    return textureID;
}