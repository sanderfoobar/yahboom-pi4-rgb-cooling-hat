## Yahboom Raspberry Pi RGB Cooling HAT

![https://i.imgur.com/taz0tA1.jpg](https://i.imgur.com/taz0tA1.jpg)

This small C script will turn on the fan when temp goes above 50C and will 
show some text on the OLED display. I disabled the RGB.

### install

make sure you have this line in `/boot/config.txt`:

```text
dtparam=i2c_arm=on
```

download & compile this program:

```text
sudo apt install -y wiringpi cmake build-essential gcc

git clone https://github.com/sanderfoobar/yahboom-pi4-rgb-cooling-hat.git
cd yahboom-pi4-rgb-cooling-hat

cmake -B build .
make -Cbuild -j4
sudo make -Cbuild install
```

Will install an executable into `/usr/local/bin/rgb_cooling_hat`

### systemd service autostart

Auto-start executable at raspberry pi startup:

```
sudo cp rgb_cooling_hat.service /etc/systemd/system/rgb_cooling_hat.service
sudo systemctl daemon-reload
sudo systemctl enable rgb_cooling_hat
```

Start the program:

```
sudo systemctl start rgb_cooling_hat
```

