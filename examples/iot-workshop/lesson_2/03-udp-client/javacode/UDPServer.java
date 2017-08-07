package javacode;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;

public class UDPServer {

	private static final Integer HOST = 5678;
	private static final String MESSAGE = "Message from UDPServer.java";

	public static void main(String[] args) throws IOException {
		System.out.println("Server is running.");
		DatagramSocket server = new DatagramSocket(HOST);
		while (true) {
			byte[] receiveData = new byte[1024];
			DatagramPacket receivePacket = new DatagramPacket(receiveData,
					receiveData.length);
			server.receive(receivePacket);
			byte[] data = receivePacket.getData();

			Sensor sensor = new DataSensor(data, receivePacket
					.getAddress().toString());
			System.out.println(sensor);

			InetAddress IPAddress = receivePacket.getAddress();
			int port = receivePacket.getPort();
			DatagramPacket sendPacket = new DatagramPacket(MESSAGE.getBytes(),
					MESSAGE.getBytes().length, IPAddress, port);
			server.send(sendPacket);
		}
	}
}
