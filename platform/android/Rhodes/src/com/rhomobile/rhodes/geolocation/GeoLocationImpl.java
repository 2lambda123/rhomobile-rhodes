/*
 ============================================================================
 Author	    : Dmitry Moskalchuk
 Version	: 1.5
 Copyright  : Copyright (C) 2008 Rhomobile. All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ============================================================================
 */
package com.rhomobile.rhodes.geolocation;

import com.rhomobile.rhodes.Logger;
import com.rhomobile.rhodes.RhodesInstance;

import android.app.Activity;
import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.location.LocationProvider;
import android.os.Bundle;
import android.os.Looper;

public class GeoLocationImpl implements LocationListener {

	private static final String TAG = "GeoLocationImpl";
	private static final String PROVIDER = LocationManager.GPS_PROVIDER;
	private LocationManager locationManager = null;
	private boolean available = false;
	private double longitude = 0;
	private double latitude = 0;
	private boolean determined = false;
	
	private native void geoCallback();
	
	public GeoLocationImpl() {
		setCurrentGpsLocation(null);
	}

	private void setCurrentGpsLocation(Location location) {
		Logger.T(TAG, "GeoLocationImpl.setCurrentGpsLocation");
		try {
			if (locationManager == null) {
				Activity r = RhodesInstance.getInstance();
				locationManager = (LocationManager)r.getSystemService(Context.LOCATION_SERVICE);
				LocationProvider gpsProvider = locationManager.getProvider(PROVIDER);
				available = gpsProvider != null && locationManager.isProviderEnabled(PROVIDER);
				locationManager.requestLocationUpdates(PROVIDER, 0, 0, this, Looper.getMainLooper());
			}
			
			if (location == null)
				location = locationManager.getLastKnownLocation(PROVIDER);
			
			boolean prevDetermined = determined;
			double prevLat = latitude;
			double prevLon = longitude;
			
			if (location != null) {
				longitude = location.getLongitude();
				latitude = location.getLatitude();
				determined = true;
			} else {
				determined = false;
			}

			Logger.T(TAG, "gps enabled: " + new Boolean(locationManager.isProviderEnabled(PROVIDER)).toString());
			Logger.T(TAG, "determined: " + new Boolean(determined).toString());
			if (determined) {
				Logger.T(TAG, "longitude: " + new Double(longitude).toString());
				Logger.T(TAG, "latitude: " + new Double(latitude).toString());
			}
			
			if (determined != prevDetermined || latitude != prevLat || longitude != prevLon)
				geoCallback();
			
		} catch (Exception e) {
			determined = false;
			Logger.E(TAG, e.getMessage());
		}
	}

	public void onLocationChanged(Location location) {
		Logger.T(TAG, "onLocationChanged");
		setCurrentGpsLocation(location);
	}

	public void onProviderDisabled(String provider) {
		Logger.T(TAG, "onProviderDisabled");
		setCurrentGpsLocation(null);

	}

	public void onProviderEnabled(String provider) {
		Logger.T(TAG, "onProviderEnabled");
		setCurrentGpsLocation(null);
	}

	public void onStatusChanged(String provider, int status, Bundle extras) {
		Logger.T(TAG, "onStatusChanged");
		setCurrentGpsLocation(null);
	}

	public synchronized boolean isAvailable() {
		return available;
	}
	
	public synchronized double GetLatitude() {
		return latitude;
	}

	public synchronized double GetLongitude() {
		return longitude;
	}

	public synchronized boolean isKnownPosition() {
		return determined;
	}
	
}
