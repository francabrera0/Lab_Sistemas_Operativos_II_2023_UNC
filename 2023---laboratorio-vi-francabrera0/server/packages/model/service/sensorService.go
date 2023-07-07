package service

import (
	"webServer/packages/db"
	"webServer/packages/model"
)

// Save sensor information
func SaveSensorInfo(info model.SensorInfo) error {
	return db.SaveInfo(info)
}

// Get sensors information
func GetSensorInfo() ([]model.SensorInfo, error) {
	return db.GetInfo()
}
