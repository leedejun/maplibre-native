#pragma once
#include <mbgl/util/geo.hpp>

namespace mbgl {
	class MercatorCoordinate
	{
	public:
		MercatorCoordinate(double _x,double _y, double _z=0.0);
		~MercatorCoordinate();

		double x;
		double y;
		double z;

		/*
		* The circumference of the world in meters at the equator.
		*/
		static const  double circumferenceAtEquator;

		/*
		* The circumference of the world in meters at the given latitude.
		*/
		static double circumferenceAtLatitude(double latitude);

		static double mercatorXfromLng(double lng);

		static double mercatorYfromLat(double lat);

		static double mercatorZfromAltitude(double altitude, double lat);

		static double lngFromMercatorX(double x);

		static double latFromMercatorY(double y);

		static double altitudeFromMercatorZ(double z, double y);

		/**
		* Determine the Mercator scale factor for a given latitude, see
		* https://en.wikipedia.org/wiki/Mercator_projection#Scale_factor
		*
		* At the equator the scale factor will be 1, which increases at higher latitudes.
		*
		* @param {number} lat Latitude
		* @returns {number} scale factor
		* @private
		*/
		static double mercatorScale(double lat);


		/**
		* Project a `LngLat` to a `MercatorCoordinate`.
		*
		* @param {LngLatLike} lngLatLike The location to project.
		* @param {number} altitude The altitude in meters of the position.
		* @returns {MercatorCoordinate} The projected mercator coordinate.
		* @example
		* var coord = mapboxgl.MercatorCoordinate.fromLngLat({ lng: 0, lat: 0}, 0);
		* coord; // MercatorCoordinate(0.5, 0.5, 0)
		*/
		static MercatorCoordinate fromLatLng(LatLng laln, double altitude = 0.0);

		/**
		* Returns the `LngLat` for the coordinate.
		*
		* @returns {LngLat} The `LngLat` object.
		* @example
		* var coord = new mapboxgl.MercatorCoordinate(0.5, 0.5, 0);
		* var latLng = coord.toLngLat(); // LngLat(0, 0)
		*/
		LatLng toLatLng();

		/**
		* Returns the altitude in meters of the coordinate.
		*
		* @returns {number} The altitude in meters.
		* @example
		* var coord = new mapboxgl.MercatorCoordinate(0, 0, 0.02);
		* coord.toAltitude(); // 6914.281956295339
		*/
		double toAltitude();

		/**
		* Returns the distance of 1 meter in `MercatorCoordinate` units at this latitude.
		*
		* For coordinates in real world units using meters, this naturally provides the scale
		* to transform into `MercatorCoordinate`s.
		*
		* @returns {number} Distance of 1 meter in `MercatorCoordinate` units.
		*/
		double meterInMercatorCoordinateUnits();
	};

}