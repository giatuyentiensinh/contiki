package javacode;

public class DataSensor extends Sensor {

	public static final String ANSI_RESET = "\u001B[0m";
	public static final String ANSI_BLACK = "\u001B[30m";
	public static final String ANSI_RED = "\u001B[31m";
	public static final String ANSI_GREEN = "\u001B[32m";
	public static final String ANSI_YELLOW = "\u001B[33m";
	public static final String ANSI_BLUE = "\u001B[34m";
	public static final String ANSI_PURPLE = "\u001B[35m";
	public static final String ANSI_CYAN = "\u001B[36m";
	public static final String ANSI_WHITE = "\u001B[37m";

	public DataSensor(byte[] data, String address) {
		super(data, address);
	}

	@Override
	public String toString() {
		return "DataSensor [getAdv1()=" + ANSI_CYAN + getAdv1() + ANSI_RESET+ ", getAdv2()="
				+ ANSI_CYAN + getAdv2() + ANSI_RESET + ", getAdv3()=" + ANSI_CYAN + getAdv3() + ANSI_RESET + ", getTemp()="
				+ ANSI_CYAN + getTemp() + ANSI_RESET + ", getBattery()=" + ANSI_CYAN + getBattery() + ANSI_RESET 
				+ ", getAddress()=" + ANSI_RED + getAddress() + ANSI_RESET + "]";
	}

}
