## Yahboom Raspberry Pi RGB Cooling HAT

This small C script will turn on the fan when temp goes above 50C and will 
show some text on the OLED display.

### install

```
apt install -y wiringpi cmake
cmake -B build .
make -Cbuild -j4
sudo make -Cbuild install
```

Will install binary into `/usr/local/bin/rgb_cooling_hat`

### systemd service autostart

Auto-start binary at system startup:

```
sudo cp rgb_cooling_hat.service /etc/systemd/system/rgb_cooling_hat.service
sudo systemctl enable rgb_cooling_hat
```

Start it

```
sudo systemctl start rgb_cooling_hat
```

## development

Best to `sshfs` this repo.

```
sshfs pi@192.168.178.70:rgb_cooling_hat rgb_cooling_hat
```