package model

import "time"

type User struct {
	Id        int       `json:"id"`
	Username  string    `json:"username" binding:"required"`
	Password  string    `json:"password" binding:"required"`
	CreatedAt time.Time `json:"createdAt"`
}

type SensorInfo struct {
	Username    string    `json:"username" binding:"required"`
	TotalMemory float32   `json:"totalMemory" binding:"required"`
	FreeMemory  float32   `json:"freeMemory" binding:"required"`
	UsedMemory  float32   `json:"usedMemory" binding:"required"`
	LastUpdate  time.Time `json:"lastUpdate"`
}
