# My DIY Home Automation System

**Hello World!** In this repository I'll document and share my achievements in the developing my DIY Home Automation System. I'll try to be as much detailed as I can so that you can to the same thing in your own Home

## Used Technologies

I will primarily use open source hardware and software like:

* [Arduino](https://www.arduino.cc) - used to control lights and switches, collect data from different sensors spreaded in my house
* [Raspberry Pi](https://www.raspberrypi.org) - act as the system server
* [Mosquitto MQTT](http://mosquitto.org) - message broker that will handle all the communications between the devices and the server
* [Fritzing](http://fritzing.org/home/) - to create the schemes and custom PCBs used in the Arduino/ESP/Spark devices
* [OwnTracks](http://owntracks.org) - mobile app that will send your current coordinates to Home Assistant trough MQTT
* [Home Assistant](https://home-assistant.io) - will be the *brain* of my system
* [InfluxDB](https://influxdb.com) - time-series metrics and events database that will serve the data to Grafana
* [Grafana](http://grafana.org) - graph and dashboard builder for visualizing time series metrics, will be used to visualize all the sensors data

I'm also going to use other hardware widely used in the IoT world like:

* [SparkCore/Particle](https://www.particle.io) - I'm using them because I already have two but they will been soon replaced for ESP8266 nodes
* [ESP8622](http://www.esp8266.com) - wifi capable microchip that can be programable with the Arduino IDE

## First Steps

I started configuring my Raspberry Pi, I had one with me for a time but I never did a thing with it, so I had no clue where to start. I will list the steps I did in my setup:

1. **Download the Raspbian Jessie image [here](https://www.raspberrypi.org/downloads/raspbian/)**

	- I went with the full version of Jessie because I'll use the ```GUI``` later for my data visualisations. An 8G SD card is recommended!		
	 
2. **Setup the MicroSD Card to the RaspberryPi**

	- I'm using a Mac, so I used a awesome program: [ApplePi Baker](http://www.tweaking4all.com/hardware/raspberry-pi/macosx-apple-pi-baker/). This program facilitates the process of creating a bootable SD for your Raspberry Pi and it also can help you to make a clone of your configured SD card
	 
3. **Expand Filesystem**

	- After you boot from your SD, your system will not see all the space avaliable in your SD card, so, to fix that you need to open your Terminal screen and follow this simple steps:
		1. Type ```sudo raspi-config```
		2. Choose the first option: Expand Filesystem. That's all :) 

4. **Now we will install Mosquitto (MQTT Broker), that will enable all our devices to talk with the server**
	- With your Terminal window open, run these commands:

	 ```
	 sudo wget http://repo.mosquitto.org/debian/mosquitto-repo.gpg.key
	 sudo apt-key add mosquitto-repo.gpg.key
	 cd /etc/apt/sources.list.d/
	 sudo wget http://repo.mosquitto.org/debian/mosquitto-jessie.list
	 sudo apt-get update
	 sudo apt-get install mosquitto
	 
	 ```
	- I found these procedure [here](http://mosquitto.org/2013/01/mosquitto-debian-repository/)
	- The MQTT Broker will use the his default port: 1883

5. **Install MQTT Client**

	- For debugging purpose is nice to have a MQTT Client installed in your Raspberry Pi. Just follow this command:

		```
		sudo apt-get install mosquitto mosquitto-clients
		```

6. **Install pip3**

	- The easiest way to install the Home Assistant is using ```pip3``` so, let's install it:

		```
		sudo apt-get install python3-pip
		```
		
7. **Install Home Assistant**

	- Now we'll finally install Home Assistant using ```pip3``` 
		
		```
		sudo pip3 install homeassistant
		hass --open-ui
		```
	- For more infos and instructions to install in other machines please visit the Home Assistant [Getting Started](https://home-assistant.io/getting-started/) page.

8. **Just one more thing**

	- One important step is to setup your Raspberry Pi to run the Home Assistant after the boot, if you installed the Jessie version like me you can follow these steps:
	
		```
		cd /lib/systemd/system/
		sudo nano home-assistant.service
		```
		
		Now paste this code:
		
		```
		[Unit]
		Description=Home Assistant
		After=network.target
		
		[Service]
		Type=simple
		ExecStart=/usr/local/bin/hass
		
		User=pi
		
		[Install]
		WantedBy=multi-user.target

		```
		
		Press ```CTRL + O``` to write the file, press ```ENTER``` and now ```CTRL + X```  than ```ENTER``` again to close it.
		
	- If you went with other Linux distribution you can look this [page](https://home-assistant.io/getting-started/autostart/)
	
		
Now, if everything ran smoothly you have successfully set your server and you probably have a browser window with this address: [http://localhost:8123](http://localhost:8123)

There isn't much to see there, for now. I strongly recommend that access this page to start your configuration: [Home Assistant Configuration](https://home-assistant.io/getting-started/configuration/)

## What's Next?

Well, now our system is ready to go! If you follow the instructions page in the [Home Assistant](https://home-assistant.io/getting-started/configuration/) page you saw that it will automatically search for the supported devices in your home.

If don't have any, you will have to put a little more work to make Home Assistant useful to you. The first thing to do is add the MQTT Platform to your ```configuration.yaml``` file.

```
# Example configuration.yaml entry
mqtt:
  broker: IP_ADDRESS_BROKER
  port: 1883
  client_id: home-assistant-1
  keepalive: 60
  username: USERNAME
  password: PASSWORD
  certificate: /home/paulus/dev/addtrustexternalcaroot.crt
```

Here I'll share the code I'm using in my Arduino and Sparks to interact with the MQTT [Components](https://home-assistant.io/components/#all) available in the Home Assistant:

* [MQTT Sensor](https://github.com/carmolim/my-home-automation/tree/b2af1560acc529c6db4df9d586847ac89d883e12/Examples/MQTT%20Sensor)
* MQTT Binary Sensor - work in progress
* MQTT Switch - work in progress

I just want to remember you that this is a work in progress. I'm still learning about coding for Arduino, Home Automation, Internet of Things etc. Those codes will probably change over time. Feel free to help me, and help others!

## Future

I pretend to add the instructions for installing and running the [InfluxDB](https://influxdb.com) and [Grafana](http://grafana.org), but I didn't have the time to do this on the Raspberry Pi. Now I'm running in my Mac Mini, I'm pretty excited about this!