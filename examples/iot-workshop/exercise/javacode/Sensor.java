package javacode;

import java.nio.ByteBuffer;

public abstract class Sensor {
	private int adv1;
	private int adv2;
	private int adv3;
	private int temp;
	private int battery;
	private String address;

	public Sensor(byte[] data, String address) {
		try {
			ByteBuffer buffer = ByteBuffer.wrap(data);
			this.adv1 = Sensor.converteData(buffer.getShort());
			this.adv2 = Sensor.converteData(buffer.getShort());
			this.adv3 = Sensor.converteData(buffer.getShort());
			this.temp = Sensor.converteData(buffer.getShort());
			this.battery = Sensor.converteData(buffer.getShort());
		} catch (Exception exp) {
			System.out.println("Exception: " + exp);
			this.adv1 = this.adv2 = this.adv3 = this.temp = this.battery = 0;
		}
		this.address = address;
	}

	public int getAdv1() {
		return adv1;
	}

	public void setAdv1(int adv1) {
		this.adv1 = adv1;
	}

	public int getAdv2() {
		return adv2;
	}

	public void setAdv2(int adv2) {
		this.adv2 = adv2;
	}

	public int getAdv3() {
		return adv3;
	}

	public void setAdv3(int adv3) {
		this.adv3 = adv3;
	}

	public int getTemp() {
		return temp;
	}

	public void setTemp(int temp) {
		this.temp = temp;
	}

	public int getBattery() {
		return battery;
	}

	public void setBattery(int battery) {
		this.battery = battery;
	}

	public String getAddress() {
		return address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	/**
	 * Kéo dài bit thành 16 bit. e.g: 1101 -> 0000 0000 0000 1101
	 */
	protected static int converteData(short value) {
		return value & 0xffff;
	}

}