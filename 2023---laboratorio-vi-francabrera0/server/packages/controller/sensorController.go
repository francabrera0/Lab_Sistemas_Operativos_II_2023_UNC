package controller

import (
	"net/http"
	"webServer/packages/model"
	"webServer/packages/model/service"

	"github.com/gin-gonic/gin"
)

// POST
// Submit information about sensor
//
//	Require authorization
//	ERROR: Require add username in the values
func SensorSubmit(c *gin.Context) {
	var info model.SensorInfo

	if err := c.ShouldBindJSON(&info); err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"error": err.Error()})
		return
	}

	err := service.SaveSensorInfo(info)

	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"message": "Could not save data"})
	} else {
		c.JSON(http.StatusOK, gin.H{"message": "Data saved"})
	}

}

// GET
// Return a list with all sensor information
func SensorSummary(c *gin.Context) {
	sensorInfo, err := service.GetSensorInfo()

	if err != nil {
		c.JSON(http.StatusBadRequest, err)
	}

	data := map[string]interface{}{
		"sensors": sensorInfo,
	}
	c.JSON(http.StatusOK, data)

}
