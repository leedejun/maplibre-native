#pragma once
#include <string>
#include "RTree.h"
#include <vector>
#include <iostream>
#include "default.h"
#include "LRUCache11.hpp"
#include <thread>
#include <deque>
#include <queue>
#include "SatelliteImage.hpp"

struct Rect
{
	Rect() {}

	Rect(double a_minX, double a_minY, double a_maxX, double a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}


	double min[2];
	double max[2];
};

struct SatImageInfo
{
	std::string ID;
	std::string filePath;
	Rect box;
	unsigned int texture;
};

/* ***************************************************************************
*   Struct      : tagTileRectDbl
*   Description : Tile的经纬度区域定义
*   Create      :
*   Update      :
*************************************************************************** */
typedef struct tagTileRectDbl {
	double dLeftLon;
	double dRightLon;
	double dDownLat;
	double dUpLat;
}ST_TILERECTLONLAT, *PST_TILERECTLONLAT;

/* ***************************************************************************
*   Struct      : tagTileRect
*   Description : Tile的正规化坐标区域定义
*   Create      :
*   Update      :
*************************************************************************** */
typedef struct tagTileRect {
	int iLeftX;
	int iRightX;
	int iDownY;
	int iUpY;
}ST_TILERECT, *PST_TILERECT;

/* ***************************************************************************
*   Struct      : tagIPOINT
*   Description : Point(INT Type)
*   Create      :
*   Update      :
*************************************************************************** */
typedef struct tagIPOINT {
	INT   	iX;						/*  X Point  */
	INT   	iY;						/*  Y Point  */
}ST_IPOINT, *PST_IPOINT;

typedef struct tagAbsoluteBoundingBox
{
	ST_IPOINT	stNorthEastPosition;
	ST_IPOINT	stSouthWestPosition;
}ST_ABSOLUTEBOUNDINGBOX, *PST_ABSOLUTEBOUNDINGBOX;



class dataManager
{
public:
	dataManager();
	~dataManager();

	//bool buildECMTree(std::string& rootDir);

	bool buildDataTree();

	static bool SearchCallback(SatImageInfo satImageInfo);

	int search(const double a_min[2], const double a_max[2], std::vector<SatImageInfo>& result);

	bool tryGetFromCache(std::string& key, SatelliteImage& satImage);

	void insertToCache(const std::string& key, const SatelliteImage& satImage);

	unsigned int LoadGeoTIFFAsTexture(const char* filePath);

private:

	const double pi = 3.1415926535897932384626;
	const double a = 6378245.0;
	const double ee = 0.00669342162296594323;

	bool ecmFileName2NDSTileID(const std::string& filename, long long& ndsTileID);

	bool NDSTileID2Rect(int ndsTileID, Rect& box);

	//////////////////////////////////////////////////////////////////////////
	//输入: TileID	
	//输出:	a_bLevel	
	//功能:	根据TileID算层号
	//返回: 1表示成功 0表示失败
	int NdsDal_Util_CalLevNumByTileID(int a_TileID, unsigned char* a_bLevel);
	//////////////////////////////////////////////////////////////////////////
	//输入: TileID	
	//输出:	Rect经纬度	
	//功能:	根据TileID算Tile的边框（经纬度）
	//返回: 1表示成功 0表示失败
	int NdsDal_Util_CalTileRectLngLatByTileID(int a_TileID, PST_TILERECTLONLAT a_pstTileLonLat);
	//////////////////////////////////////////////////////////////////////////
	//输入: TileID	
	//输出:	Rect（正规化坐标）	
	//功能:	根据TileID算Tile的边框（正规化）
	//返回: 1表示成功 0表示失败
	int NdsDal_Util_CalTileRectPosByTileID(int a_iTileID, PST_TILERECT a_pstTileRect);
	//////////////////////////////////////////////////////////////////////////
	//输入: 正规化坐标
	//输出:	经纬度
	//功能:	正规化坐标到经纬度的转换
	//返回: 经纬度	
	double NdsDal_Util_ConvertXPosToLng(int a_PosX);
	double NdsDal_Util_ConvertYPosToLat(int a_PosY);
	//////////////////////////////////////////////////////////////////////////

	int NdsDal_Util_GetLevelNumByPackedTileId(PackedTileId a_uiPackedTileId, short* a_psLevel);

	// 该接口有问题，例如，计算a_iTileId = 1048789，a_sLevel = 4的坐标范围异常
	int NdsDal_Util_GetTileBoundingBox(int a_iTileId, short a_sLevel, PST_ABSOLUTEBOUNDINGBOX a_pstArea);

	int NdsDal_Util_ConvertMortonToCoord(MortonCode a_mortonCode, PST_LPOINT a_pstCoord);

	/**
	* * 火星坐标系 (GCJ-02) to 84 * * @param lon * @param lat * @return
	* */
	void gcj_To_Gps84(double lat, double lon, double* outLat, double* outLon) {
		double wgLat, wgLon;
		transform(lat, lon, &wgLat, &wgLon);
		*outLon = lon * 2 - wgLon;
		*outLat = lat * 2 - wgLat;
	}

	void transform(double lat, double lon, double* outLat, double* outLon) {
		if (outOfChina(lat, lon)) {
			*outLat = lat;
			*outLon = lon;
			return;
		}
		double dLat = transformLat(lon - 105.0, lat - 35.0);
		double dLon = transformLon(lon - 105.0, lat - 35.0);
		double radLat = lat / 180.0 * pi;
		double magic = std::sin(radLat);
		magic = 1 - ee * magic * magic;
		double sqrtMagic = std::sqrt(magic);
		dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * pi);
		dLon = (dLon * 180.0) / (a / sqrtMagic * std::cos(radLat) * pi);
		*outLat = lat + dLat;
		*outLon = lon + dLon;
	}



	double transformLat(double x, double y) {
		double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y
			+ 0.2 * std::sqrt(std::abs(x));
		ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
		ret += (20.0 * std::sin(y * pi) + 40.0 * std::sin(y / 3.0 * pi)) * 2.0 / 3.0;
		ret += (160.0 * std::sin(y / 12.0 * pi) + 320 * std::sin(y * pi / 30.0)) * 2.0 / 3.0;
		return ret;
	}

	double transformLon(double x, double y) {
		double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1
			* std::sqrt(std::abs(x));
		ret += (20.0 * std::sin(6.0 * x * pi) + 20.0 * std::sin(2.0 * x * pi)) * 2.0 / 3.0;
		ret += (20.0 * std::sin(x * pi) + 40.0 * std::sin(x / 3.0 * pi)) * 2.0 / 3.0;
		ret += (150.0 * std::sin(x / 12.0 * pi) + 300.0 * std::sin(x / 30.0
			* pi)) * 2.0 / 3.0;
		return ret;
	}

	bool outOfChina(double lat, double lon) {
		if (lon < 72.004 || lon > 137.8347)
			return true;
		if (lat < 0.8293 || lat > 55.8271)
			return true;
		return false;
	}



	typedef RTree<SatImageInfo, double, 2, double> SpatialIndexTree;
	SpatialIndexTree m_indexTree;

	static std::vector<SatImageInfo> m_searchResult;

	using LCache = lru11::Cache<std::string, SatelliteImage, std::mutex>;

	LCache* m_satCache;

};

