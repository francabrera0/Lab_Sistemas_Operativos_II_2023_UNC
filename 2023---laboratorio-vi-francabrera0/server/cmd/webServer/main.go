package main

import (
	"fmt"
	"webServer/packages/controller"
	"webServer/packages/db"
	"webServer/packages/middleware"

	"github.com/gin-gonic/gin"
)

func main() {
	err := db.Connect()
	if err != nil {
		fmt.Println("Error creating database")
	}
	r := gin.Default()

	//Users endpoints
	userPublic := r.Group("/api/users")
	userPublic.POST("/createuser", controller.RegisterUser)
	userPublic.POST("/login", controller.Login)

	userProtected := r.Group("/api/users/logged")
	userProtected.Use(middleware.JwtAuthMiddleware())
	userProtected.GET("/listall", controller.ListAll)

	//Processing endpoints
	sensorPublic := r.Group("processing/")
	sensorPublic.GET("/summary", controller.SensorSummary)

	sensorProtected := r.Group("/processing/logged")
	sensorProtected.Use(middleware.JwtAuthMiddleware())
	sensorProtected.POST("/submit", controller.SensorSubmit)

	r.Run(":8080")
	db.Close()
}
