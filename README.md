PWM Based H-Bridge Control Module
====================

This is a simple code to manage a H-Bridge based driver as the TB6612 or the clasical L298 for a 4 wheel vehicle.
Is based on ESP32 PWM library for motors: MCPWM

The configuration is 2 PWM drivers. Each driver manage one side of the vehicle with 2 weels.

Each driver is managed applying the PWM generated signal directly in the INA-INB imputs of the module, leaving the PWMx connected to source as a "enable".
