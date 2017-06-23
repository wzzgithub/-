package com.demo.smarthome.tools;

import android.content.Context;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;

/**
 * Ip 工具类
 * 
 * @author Administrator
 * 
 */
public class IpTools {
	public static String getIpV4StringByByte(byte[] ipStr, int index) {
		String strIp = "";
		if (ipStr != null) {
			if (ipStr.length >= 4) {
				for (int i = 0; i < 4; i++) {
					strIp += (int) (ipStr[i + index] & 0xFF) + "";
					if (i < 3) {
						strIp += ".";
					}
				}
			}
		}
		return strIp;
	}

	public static byte[] getIpV4Byte(String ipStr) {
		byte[] ipByte = new byte[4];
		if (ipStr == null) {
			return ipByte;
		}
		long[] ip = new long[4];
		// 先找到IP地址字符串中.的位置
		int position1 = ipStr.indexOf(".");
		int position2 = ipStr.indexOf(".", position1 + 1);
		int position3 = ipStr.indexOf(".", position2 + 1);

		// 将每个.之间的字符串转换成整型
		ip[0] = Long.parseLong(ipStr.substring(0, position1));
		ip[1] = Long.parseLong(ipStr.substring(position1 + 1, position2));
		ip[2] = Long.parseLong(ipStr.substring(position2 + 1, position3));
		ip[3] = Long.parseLong(ipStr.substring(position3 + 1));
		for (int i = 0; i < 4; i++) {
			ipByte[i] = (byte) (ip[i] % 256);
		}
		return ipByte;
	}

	public static String getIp(WifiManager wifiMan) {
		String ip = "";
		// WifiManager wifiMan = (WifiManager)
		// getSystemService(Context.WIFI_SERVICE);
		WifiInfo info = wifiMan.getConnectionInfo();
		String mac = info.getMacAddress();// 获得本机的MAC地址
		String ssid = info.getSSID();// 获得本机所链接的WIFI名称

		int ipAddress = info.getIpAddress();
		// String ipString = "";// 本机在WIFI状态下路由分配给的IP地址

		// 获得IP地址的方法一：
		if (ipAddress != 0) {
			ip = ((ipAddress & 0xff) + "." + (ipAddress >> 8 & 0xff) + "."
					+ (ipAddress >> 16 & 0xff) + "." + (ipAddress >> 24 & 0xff));
		}
		return ip;
	}

}
