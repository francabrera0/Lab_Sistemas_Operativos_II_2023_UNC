package middleware

import (
	"net/http"

	"webServer/packages/token"

	"github.com/gin-gonic/gin"
)

// Authentication middleware
func JwtAuthMiddleware() gin.HandlerFunc {
	return func(c *gin.Context) {
		err := token.TokenValid(c)
		if err != nil {
			c.JSON(http.StatusUnauthorized, gin.H{"error": "Unauthorized"})
			c.Abort()
			return
		}
		c.Next()
	}
}
