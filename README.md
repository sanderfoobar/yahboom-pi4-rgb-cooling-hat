## Yahboom Raspberry Pi RGB Cooling HAT

![https://i.imgur.com/dq8DsVf.png](https://i.imgur.com/dq8DsVf.png)

This small C script will turn on the fan when temp goes above 50C and will 
show some text on the OLED display. I disabled the RGB.

### install

```
apt install -y wiringpi cmake
cmake -B build .
make -Cbuild -j4
sudo make -Cbuild install
```

Will install an executable into `/usr/local/bin/rgb_cooling_hat`

### systemd service autostart

Auto-start executable at raspberry pi startup:

```
sudo cp rgb_cooling_hat.service /etc/systemd/system/rgb_cooling_hat.service
sudo systemctl enable rgb_cooling_hat
```

Start the program:

```
sudo systemctl start rgb_cooling_hat
```

