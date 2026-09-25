# MantisTrax

MantisTrax is an off-grid SenseCAP T1000-A tracker locator for the M5Stack Cardputer-Adv + Cap LoRa-1262.

## Hardware

- M5Stack Cardputer-Adv
- Cap LoRa-1262 with RP-SMA LoRa antenna installed
- SenseCAP T1000-A configured for US915 / ABP

## Firmware

The firmware receives LoRaWAN uplinks directly, verifies the LoRaWAN MIC, decrypts the T1000 application payload locally, extracts GNSS coordinates, and renders a relative minimap using the Cardputer's own GNSS and BMI270 IMU.

No gateway, Wi-Fi, cellular network, map tiles, or cloud service is required during tracking.

## Configuration

Edit `data/config.json` before building to preload the T1000 DevAddr and session keys. The Cardputer also exposes the credentials and radio data-rate settings through its runtime setup screen and persists edited credentials in NVS.

The initial project values are placeholders and must be replaced with the credentials generated for the actual T1000-A.

## Important

Install the LoRa antenna before powering the Cap. M5Stack explicitly warns that powering the module without its antenna can permanently damage the hardware.
