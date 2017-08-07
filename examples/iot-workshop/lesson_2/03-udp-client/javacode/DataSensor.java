package javacode;

public class DataSensor extends Sensor {

	public DataSensor(byte[] data, String address) {
		super(data, address);
	}

	@Override
	public String toString() {
		return "DataSensor [getAdv1()=" + getAdv1() + ", getAdv2()="
				+ getAdv2() + ", getAdv3()=" + getAdv3() + ", getTemp()="
				+ getTemp() + ", getBattery()=" + getBattery()
				+ ", getAddress()=" + getAddress() + "]";
	}

}
