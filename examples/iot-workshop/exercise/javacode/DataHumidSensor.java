package javacode;

import java.nio.ByteBuffer;

public class DataHumidSensor extends Sensor {

  public static final String ANSI_CYAN = "\u001B[36m";
	public static final String ANSI_RESET = "\u001B[0m";

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
		return "DataHumidSensor [getSensor_temp()=" + ANSI_CYAN + getSensor_temp() + ANSI_RESET
				+ ", getSensor_humid()=" + ANSI_CYAN + getSensor_humid() + ANSI_RESET + ", getAdv1()="
				+ ANSI_CYAN + getAdv1() + ANSI_RESET + ", getAdv2()=" + ANSI_CYAN + getAdv2() + ANSI_RESET + ", getAdv3()="
				+ ANSI_CYAN + getAdv3() + ANSI_RESET + ", getTemp()=" + ANSI_CYAN + getTemp() + ANSI_RESET + ", getBattery()="
				+ ANSI_CYAN + getBattery() + ANSI_RESET + ", getAddress()=" + ANSI_CYAN + getAddress() + ANSI_RESET + "]";
	}

}