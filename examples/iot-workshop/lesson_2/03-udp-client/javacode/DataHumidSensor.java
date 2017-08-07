package javacode;

import java.nio.ByteBuffer;

public class DataHumidSensor extends Sensor {
	private final static Integer OFFSET = 10;
	private final static Integer LENGTH = 4;
	private int sensor_temp;
	private int sensor_humid;

	public DataHumidSensor(byte[] data, String address) {
		super(data, address);
		ByteBuffer buffer = ByteBuffer.wrap(data, OFFSET, LENGTH);
		this.sensor_temp = converteData(buffer.getShort());
		this.sensor_humid = converteData(buffer.getShort());
	}

	public int getSensor_temp() {
		return sensor_temp;
	}

	public void setSensor_temp(int sensor_temp) {
		this.sensor_temp = sensor_temp;
	}

	public int getSensor_humid() {
		return sensor_humid;
	}

	public void setSensor_humid(int sensor_humid) {
		this.sensor_humid = sensor_humid;
	}

	@Override
	public String toString() {
		return "DataHumidSensor [getSensor_temp()=" + getSensor_temp()
				+ ", getSensor_humid()=" + getSensor_humid() + ", getAdv1()="
				+ getAdv1() + ", getAdv2()=" + getAdv2() + ", getAdv3()="
				+ getAdv3() + ", getTemp()=" + getTemp() + ", getBattery()="
				+ getBattery() + ", getAddress()=" + getAddress() + "]";
	}

}