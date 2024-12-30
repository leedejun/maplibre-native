#include "mercator_coordinate.h"
#include <cmath>

namespace mbgl {

	const  double MercatorCoordinate::circumferenceAtEquator = 2 * M_PI * 6378137.0;

	MercatorCoordinate::MercatorCoordinate(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}


	MercatorCoordinate::~MercatorCoordinate()
	{
	}

	double MercatorCoordinate::circumferenceAtLatitude(double latitude)
	{
		return circumferenceAtEquator * std::cos(latitude * M_PI / 180.0);
	}

	double MercatorCoordinate::mercatorXfromLng(double lng)
	{
		return (180.0 + lng) / 360.0;
	}

	double MercatorCoordinate::mercatorYfromLat(double lat)
	{
		return (180.0 - (180.0 / M_PI * std::log(std::tan(M_PI / 4.0 + lat * M_PI / 360.0)))) / 360.0;
	}

	double MercatorCoordinate::mercatorZfromAltitude(double altitude, double lat)
	{
		return altitude / circumferenceAtLatitude(lat);
	}

	double MercatorCoordinate::lngFromMercatorX(double x)
	{
		return x * 360.0 - 180.0;
	}

	double MercatorCoordinate::latFromMercatorY(double y)
	{
		const double y2 = 180.0 - y * 360.0;
		return 360.0 / M_PI * std::atan(std::exp(y2 * M_PI / 180.0)) - 90.0;
	}

	double MercatorCoordinate::altitudeFromMercatorZ(double z, double y)
	{
		return z * circumferenceAtLatitude(latFromMercatorY(y));
	}

	double MercatorCoordinate::mercatorScale(double lat)
	{
		return 1 / std::cos(lat * M_PI / 180.0);
	}

	MercatorCoordinate MercatorCoordinate::fromLatLng(LatLng laln, double altitude /*= 0.0*/)
	{
		return MercatorCoordinate(
			mercatorXfromLng(laln.longitude()),
			mercatorYfromLat(laln.latitude()),
			mercatorZfromAltitude(altitude, laln.latitude()));
	}

	mbgl::LatLng MercatorCoordinate::toLatLng()
	{
		return LatLng(
			latFromMercatorY(y),
			lngFromMercatorX(x));
	}

	double MercatorCoordinate::toAltitude()
	{
		return altitudeFromMercatorZ(z, y);
	}

	double MercatorCoordinate::meterInMercatorCoordinateUnits()
	{
		// 1 meter / circumference at equator in meters * Mercator projection scale factor at this latitude
		return 1 / circumferenceAtEquator * mercatorScale(latFromMercatorY(y));
	}

}