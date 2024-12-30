#pragma once
#include <functional>

#ifndef HAS_ECM
#define HAS_ECM 1
#endif

using namespace std;

typedef struct tagOffset {
	double offsetX;
	double offsetY;

	tagOffset(double x, double y)
	{
		offsetX = x;
		offsetY = y;
	}
}__tagOffset;

typedef struct tagPoint3D
{
	double x;
	double y;
	double z;
} _tagPoint;

typedef struct tagInputParam
{
	int zLevel;
	tagPoint3D originGeoPos;
	tagPoint3D outGeoPos;
	tagPoint3D inPos;
	tagPoint3D outPos;

} _tagInputParam;

typedef struct tagPOS_UV
{
	float x;
	float y;
	float z;
	float u;
	float v;
} _tagPOS_UV;

#if defined(WIN32) || defined(_WIN32_WCE)   
typedef __int64				INT64, *PINT64;	/**< 8Bytes */
typedef unsigned __int64	UINT64, *PUINT64;/**< 8Bytes */
#endif 
#ifdef LINUX
typedef long long			INT64, *PINT64;	/**< 8Bytes */
typedef unsigned long long  UINT64, *PUINT64;/**< 8Bytes */
#endif 


typedef int INT;

typedef INT64		MortonCode;

typedef INT64			PackedTileId;

typedef struct tagLPOINT {
	unsigned long	ulLongitude;					/*  X Point  */
	unsigned long	ulLatitude;						/*  Y Point  */
}ST_LPOINT, *PST_LPOINT;





#ifdef SUCCESS
#undef SUCCESS
#endif
#define SUCCESS						1

#ifdef FAIL
#undef FAIL
#endif
#define FAIL						0

#include <map>

typedef std::function<void(tagInputParam&)> Model2TileFunc;

typedef std::function<std::string(double, double, double)> Latlon2CanonicalTileIDFunc;

typedef std::function<void(std::map<std::string, std::array<double, 16> > &)> RenderModelFunc;