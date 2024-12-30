#include "dataManager.h"
//#include <QFile>
//#include <QDir>
//#include <QString>
#include <iostream>
#include <tiffio.h>
#include <glad/glad.h>


std::vector<SatImageInfo> dataManager::m_searchResult;

dataManager::dataManager()
{
	m_satCache = new LCache(128);
}


dataManager::~dataManager()
{
	if (m_satCache)
	{
		delete m_satCache;
	}
}

bool dataManager::buildDataTree()
{
	{
			
			//96.4899729729999933,40.3233243239999979 : 97.0100270270000067,40.6766756760000021
			{
				SatImageInfo satImageInfo;
				satImageInfo.filePath = "D:\\workDoc\\lidejun\\data\\testdata\\RD0100_DataCloud_K47D011002_2024H1_4FF_4326.tif";
				double minLon = 96.4899729729999933;
				double minLat = 40.3233243239999979;
				double maxLon = 97.0100270270000067;
				double maxLat = 40.6766756760000021;
				satImageInfo.box = {minLon,minLat,maxLon,maxLat};
				satImageInfo.texture = LoadGeoTIFFAsTexture(satImageInfo.filePath.c_str());
				m_indexTree.Insert(satImageInfo.box.min, satImageInfo.box.max, satImageInfo);
			}
   

			//107.4899729729729927,40.6566486486486554 : 108.0100000000000193,41.0100000000000051
			{
				SatImageInfo satImageInfo;
				satImageInfo.filePath = "D:\\workDoc\\lidejun\\data\\testdata\\RD0100_DataCloud_K48D010012_2024H1_4FF_4326.tif";
				double minLon = 107.4899729729729927;
				double minLat = 40.6566486486486554;
				double maxLon = 108.0100000000000193;
				double maxLat = 41.0100000000000051;
				satImageInfo.box = {minLon,minLat,maxLon,maxLat};
				satImageInfo.texture = LoadGeoTIFFAsTexture(satImageInfo.filePath.c_str());
				m_indexTree.Insert(satImageInfo.box.min, satImageInfo.box.max, satImageInfo);
			}
		}

	return true;
}

//bool dataManager::buildECMTree(std::string& rootDir)
//{
//	QDir dir(QString(rootDir.c_str()));
//	if (!dir.exists())
//	{
//		qWarning("Cannot find the example directory");
//		return false;
//	}
//
//	dir.setFilter(QDir::Files);
//	QFileInfoList list = dir.entryInfoList();
//	for (int i = 0; i < list.size(); ++i)
//	{
//		QFileInfo fileInfo = list.at(i);
//		QString filename = fileInfo.fileName();
//
//		if (filename.endsWith("_H.dae")|| filename.endsWith("_B1.dae"))
//		{
//			ECMfileInfo ecmInfo;
//			ecmInfo.filePath = fileInfo.filePath().toStdString();
//			ecmInfo.ID = filename.toStdString();
//
//			long long ndsTileID = -1;
//			if (ecmFileName2NDSTileID(ecmInfo.ID, ndsTileID))
//			{
//				Rect box;
//				if (NDSTileID2Rect(ndsTileID, box))
//				{
//					//gcj2wgs84
//					gcj_To_Gps84(box.min[1], box.min[0], &box.min[1], &box.min[0]);
//					gcj_To_Gps84(box.max[1], box.max[0], &box.max[1], &box.max[0]);
//
//					ecmInfo.box = box;
//					m_indexTree.Insert(ecmInfo.box.min, ecmInfo.box.max, ecmInfo);
//				}   
//			}
//		}
//	}
//
//	return true;
//}

bool dataManager::SearchCallback(SatImageInfo satImageInfo)
{
	std::cout << "Hit data rect " << satImageInfo.ID << "\n";
	m_searchResult.push_back(satImageInfo);
	return true; // keep going
}

int dataManager::search(const double a_min[2], const double a_max[2], std::vector<SatImageInfo>& result)
{
	if (m_searchResult.size())
	{
		m_searchResult.clear();
	}

	int count = m_indexTree.Search(a_min, a_max, SearchCallback);

	if (count>0)
	{
		result = m_searchResult;
	}

	return count;
}

bool dataManager::tryGetFromCache(std::string& key, SatelliteImage& satelliteImage)
{
	bool ret = false;

	if (m_satCache!=NULL)
	{
		ret = m_satCache->tryGet(key, satelliteImage);
	}
	
	return ret;
}

void dataManager::insertToCache(const std::string& key, const SatelliteImage& satelliteImage)
{
	if (m_satCache != NULL)
	{
		SatelliteImage& alisSatelliteImage = const_cast<SatelliteImage&>(satelliteImage);
		alisSatelliteImage.id = key;
		m_satCache->insert(key, satelliteImage);
	}
}

bool dataManager::ecmFileName2NDSTileID(const std::string& filename, long long& ndsTileID)
{
	bool bSuccess = true;
	const char* data = filename.data();
	int beginPosFirstNum = -1;
	int endPosFirstNum = -1;
	for (int i=0; i<filename.size(); ++i)
	{
		char c = data[i];
		if (c>='0' && c<= '9')
		{
			if (beginPosFirstNum ==-1)
			{
				beginPosFirstNum = i;
			}
			endPosFirstNum = i;
		}
		else
		{
			if (beginPosFirstNum !=-1)
			{
				endPosFirstNum = i;
				break;
			}
		}
	}

	if (beginPosFirstNum ==-1)
	{
		return false;
	}

	std::string ss = filename.substr(beginPosFirstNum, endPosFirstNum - beginPosFirstNum);

	try
	{
		ndsTileID = std::stoll(ss);
	}
	catch (std::invalid_argument &e)
	{
		std::cout << e.what() << std::endl;

		return false;
	}
	

	return bSuccess;
}

bool dataManager::NDSTileID2Rect(int ndsTileID, Rect& box)
{
	bool ret = false;
	PackedTileId a_uiPackedTileId = ndsTileID;

	short a_sLevel;

	ret = NdsDal_Util_GetLevelNumByPackedTileId(a_uiPackedTileId, &a_sLevel);

	if (!ret)
	{
		return ret;
	}

	tagAbsoluteBoundingBox a_stArea;

	ret = NdsDal_Util_GetTileBoundingBox(ndsTileID, a_sLevel, &a_stArea);

	box.min[0] = NdsDal_Util_ConvertXPosToLng(a_stArea.stSouthWestPosition.iX);
	box.min[1] = NdsDal_Util_ConvertYPosToLat(a_stArea.stSouthWestPosition.iY);
	box.max[0] = NdsDal_Util_ConvertXPosToLng(a_stArea.stNorthEastPosition.iX);
	box.max[1] = NdsDal_Util_ConvertYPosToLat(a_stArea.stNorthEastPosition.iY);

	return ret;
}

//输入: 正规化坐标
//输出:	经纬度
//功能:	正规化坐标到经纬度的转换
//返回: 经纬度	
#define POSCOEFFICIENT  4294967296.0

double dataManager::NdsDal_Util_ConvertXPosToLng(int a_iPosX)
{
	return ((double)a_iPosX / POSCOEFFICIENT) * 360;
}
double dataManager::NdsDal_Util_ConvertYPosToLat(int a_iPosY)
{
	return ((double)a_iPosY / POSCOEFFICIENT) * 360;
}

int dataManager::NdsDal_Util_GetLevelNumByPackedTileId(PackedTileId a_uiPackedTileId, short* a_psLevel)
{
	int counter = 32;
	unsigned int mask = 0x80000000;

	//Packed representation of a tile id using 32 bits instead of 8+32 bits for level number and tile number.
	//Observing that level n has 2^(2n+1) tiles and that the level numbers range from 0 to 15, we conclude 
	//that tileNum < 2^(16+levelNum) and (2^(16+levelNum)) + tileNum < 2^32.Thus, we define 
	//packedTileId = (2^(16+levelNum)) + tileNum. From the packedTileId, we obtain the level and tile numbers 
	//by the following formula: levelNum = log(tileId)-16 tileNum = tileId - (2^(16+levelNum))
	//log is the base 2 logarithm. 
	//It corresponds to the position index of the most significant 1-bit.
	do
	{
		counter--;
		if (a_uiPackedTileId&mask)
		{
			break;
		}

		mask >>= 1;
	} while (counter > 0);

	//log is the base 2 logarithm
	*a_psLevel = counter - 16;

	return 1;
}

int dataManager::NdsDal_Util_GetTileBoundingBox(int a_iTileId, short a_sLevel, PST_ABSOLUTEBOUNDINGBOX a_pstArea)
{
	INT tileSize = (1 << (31 - a_sLevel));
	ST_LPOINT stCoord;

	NdsDal_Util_ConvertMortonToCoord(a_iTileId - (1 << (16 + a_sLevel)), &stCoord);
	stCoord.ulLongitude <<= (31 - a_sLevel);
	stCoord.ulLatitude <<= (31 - a_sLevel);

	a_pstArea->stSouthWestPosition.iX = stCoord.ulLongitude;
	a_pstArea->stSouthWestPosition.iY = stCoord.ulLatitude;
	a_pstArea->stNorthEastPosition.iX = stCoord.ulLongitude + tileSize;
	a_pstArea->stNorthEastPosition.iY = stCoord.ulLatitude + tileSize;

	return 1;
}

int dataManager::NdsDal_Util_ConvertMortonToCoord(MortonCode a_mortonCode, PST_LPOINT a_pstCoord)
{
	int		i;
	unsigned int bit = 1;

	a_pstCoord->ulLongitude = 0;
	a_pstCoord->ulLatitude = 0;

	for (i = 0; i < 32; i++)
	{
		a_pstCoord->ulLongitude |= (unsigned int)(a_mortonCode & (INT64)bit);
		a_mortonCode >>= 1;

		a_pstCoord->ulLatitude |= (unsigned int)(a_mortonCode & (INT64)bit);
		bit <<= 1;
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//输入: TileID	
//输出:	a_bLevel	
//功能:	根据TileID算层号
//返回: 1表示成功 0表示失败
int dataManager::NdsDal_Util_CalLevNumByTileID(int a_TileID, unsigned char* a_bLevel)
{
	int i = 0;
	while (1)
	{
		if (a_TileID / 2 == 0)
		{
			break;
		}
		a_TileID = a_TileID / 2;
		i++;
	}
	*a_bLevel = i - 16;
	return 1;
}
//////////////////////////////////////////////////////////////////////////
//输入: TileID	
//输出:	Rect经纬度	
//功能:	根据TileID算Tile的边框（经纬度）
//返回: 1表示成功 0表示失败
int dataManager::NdsDal_Util_CalTileRectLngLatByTileID(int a_iTileID, PST_TILERECTLONLAT a_pstTmpTile)
{
	ST_TILERECT stTileRect;
	if (NdsDal_Util_CalTileRectPosByTileID(a_iTileID, &stTileRect) == 0)
	{
		return 0;
	}
	a_pstTmpTile->dLeftLon = NdsDal_Util_ConvertXPosToLng(stTileRect.iLeftX);
	a_pstTmpTile->dRightLon = NdsDal_Util_ConvertXPosToLng(stTileRect.iRightX);

	a_pstTmpTile->dDownLat = NdsDal_Util_ConvertYPosToLat(stTileRect.iDownY);
	a_pstTmpTile->dUpLat = NdsDal_Util_ConvertYPosToLat(stTileRect.iUpY);
	return 1;
}
//////////////////////////////////////////////////////////////////////////
//输入: TileID	
//输出:	Rect（正规化坐标）	
//功能:	根据TileID算Tile的边框（正规化）
//返回: 1表示成功 0表示失败
int dataManager::NdsDal_Util_CalTileRectPosByTileID(int a_iTileId, PST_TILERECT a_pstTilePos)
{
	unsigned char bLevel;
	int  iPosition;
	int  iTileNum;
	int  iMinX = 0;
	int  iMinY = 0;

	NdsDal_Util_CalLevNumByTileID(a_iTileId, &bLevel);
	iTileNum = a_iTileId - (1 << (16 + bLevel));
	for (iPosition = 0; iPosition < bLevel; iPosition++)
	{
		if (iTileNum & 1 << (2 * iPosition))
		{
			iMinX |= (1 << iPosition);
		}

		if (iTileNum & 1 << (2 * iPosition + 1))
		{
			iMinY |= (1 << iPosition);
		}

	}
	if (iTileNum & 1 << (2 * bLevel))
	{
		iMinX |= 1 << bLevel;
	}


	///////////////////////
	iMinX <<= (32 - (bLevel + 1));
	iMinY <<= (32 - (bLevel + 1));
	if (iMinY & (1 << 30))
	{
		iMinY |= 1 << 31;//把31bit设置成和30bit一样。
	}
	a_pstTilePos->iLeftX = iMinX;
	a_pstTilePos->iDownY = iMinY;
	a_pstTilePos->iRightX = iMinX + (1 << (32 - (bLevel + 1)));
	a_pstTilePos->iUpY = iMinY + (1 << (32 - (bLevel + 1)));
	return 1;
}
//////////////////////////////////////////////////////////////////////////

unsigned int dataManager::LoadGeoTIFFAsTexture(const char* filePath)
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
    GLuint textureID = 0;
    if (raster != nullptr) {
        if (TIFFReadRGBAImage(tiff, width, height, raster, 0)) {
            // 创建OpenGL纹理
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
			// set the texture wrapping parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			// set texture filtering parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster);
			glGenerateMipmap(GL_TEXTURE_2D);

            std::cout << "成功读取并创建纹理: " << filePath << std::endl;
        }
        _TIFFfree(raster);
    }

    TIFFClose(tiff);
    return textureID;
}
