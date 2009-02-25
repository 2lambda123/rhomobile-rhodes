package com.rhomobile.rhodes.geolocation;

import com.rhomobile.rhodes.RhodesInstance;

import android.content.Context;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Looper;
import android.util.Log;

public class GeoLocationImpl implements LocationListener {

	private LocationManager locationManager;
	private double longitude = 0;
	private double latitude = 0;
	private boolean determined = false;

	public GeoLocationImpl() {

		Looper.prepare();

		setCurrentGpsLocation(null);
	}

	private void setCurrentGpsLocation(Location location) {
		try {
			if (location == null) {
				locationManager = (LocationManager) RhodesInstance
						.getInstance().getSystemService(
								Context.LOCATION_SERVICE);
				locationManager.requestLocationUpdates(
						LocationManager.GPS_PROVIDER, 0, 0, this);
				location = locationManager
						.getLastKnownLocation(LocationManager.GPS_PROVIDER);
			}
			if (location != null) {
				longitude = location.getLongitude();
				latitude = location.getLatitude();
				determined = true;
			} else {
				determined = false;
			}

		} catch (Exception e) {
			determined = false;
			Log.e(GeoLocationImpl.class.getSimpleName(), e.getMessage());
		}
	}

	public void onLocationChanged(Location location) {
		setCurrentGpsLocation(location);
	}

	public void onProviderDisabled(String provider) {
		setCurrentGpsLocation(null);

	}

	public void onProviderEnabled(String provider) {
		setCurrentGpsLocation(null);
	}

	public void onStatusChanged(String provider, int status, Bundle extras) {
		setCurrentGpsLocation(null);
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
