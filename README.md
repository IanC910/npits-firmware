# Capstone Company 10 Embedded
### Setup to be done on the Pi:
I2C:
- $ sudo raspi-config -> enable i2c
- $ sudo apt install i2c-tools
- $ sudo apt install libi2c-dev

GPIO:
- $ sudo apt install gpiod
- $ sudo apt install libgpiod2
- $ sudo apt install libgpiod-dev

GoPro:
If not already installed,
- $ sudo apt install libcurl4-openssl-dev
- $ sudo apt install libjsoncpp-dev
- $ sudo apt install ffmpeg
- $ sudo apt install dhcpcd5
- $ sudo systemctl restart dhcpcd

sudo apt install libcurl4-openssl-dev
1. **Edit the `wpa_supplicant.conf` file to connect to the GoPro's Wi-Fi:**
   ```bash
   sudo nano /etc/wpa_supplicant/wpa_supplicant.conf
   ```
   - Add the following lines:
     ```conf
     network={
         ssid="GoPro-SSID"       # Replace with your GoPro's Wi-Fi SSID
         psk="GoProPassword"     # Replace with your GoPro's Wi-Fi password
         key_mgmt=WPA-PSK
         priority=1
     }
     ```
   - Replace `GoPro-SSID` and `GoProPassword` with your actual GoPro credentials.

2. **Restart networking services to apply the changes:**
   ```bash
   sudo systemctl restart networking
   sudo systemctl restart dhcpcd
   ```

3. **Verify the connection to the GoPro:**
   - $ ifconfig wlan0
   or
   - $ iwconfig