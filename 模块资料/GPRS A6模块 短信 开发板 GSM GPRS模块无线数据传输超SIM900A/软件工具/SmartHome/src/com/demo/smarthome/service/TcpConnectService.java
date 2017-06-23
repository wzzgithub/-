package com.demo.smarthome.service;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.HttpURLConnection;
import java.net.InetAddress;
import java.net.MalformedURLException;
import java.net.SocketException;

import android.util.Log;

/**
 * Tcp连接 服务类
 * 
 * @author Administrator
 * 
 */
public class TcpConnectService {
	private static final String TAG = "TcpConnectService";

	public static String registUser(String name, String passwd, String mobile,
			String email, String deviceId, String devicePwd) {
		String guId = "";
		boolean ok = false;
		;

		DatagramSocket socket = null;
		String msg = null;
		try {
			socket = new DatagramSocket();
			// 从标准输入（键盘）读取数据
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					System.in));
			while (!(msg = reader.readLine()).equalsIgnoreCase("exit")) {
				// 产生一个InetAddress，内容是服务器端的IP
				InetAddress serverAddress = InetAddress.getByName("127.0.0.1");
				// 构造要发送消息的数据报， 并制定服务器监听的端口
				DatagramPacket packet = new DatagramPacket(msg.getBytes(),
						msg.getBytes().length, serverAddress, 1234);
				// 发送数据报
				socket.send(packet);
			}
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			socket.close();
		}

		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);
			outMsg(TAG, "=======registUser======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Cfg.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			// connection.setConnectTimeout(10000);//连接超时 单位毫秒
			// connection.setReadTimeout(2000);//读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");
			// POST /service/s.asmx HTTP/1.1
			// Content-Type: text/xml; charset=utf-8
			// SOAPAction: "M2MHelper/registUser"
			// Host: tangdengan.xicp.net:8020
			// Content-Length: 330
			// Expect: 100-continue
			// Accept-Encoding: gzip, deflate

			connection.setRequestProperty("SOAPAction",
					"\"M2MHelper/registUser\"");
			connection.setRequestProperty("Accept-Encoding", "gzip, deflate");

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(800);

			// <s:Envelope
			// xmlns:s="http://schemas.xmlsoap.org/soap/envelope/"><s:Body>
			// <registUser xmlns="M2MHelper"
			// xmlns:i="http://www.w3.org/2001/XMLSchema-instance">
			// <userName>mmm</userName><passWord>123321</passWord><mobile>mmm</mobile><email>mmm</email><deviceID>7900</deviceID><devicePWD>7900</devicePWD>
			// </registUser></s:Body></s:Envelope>
			buffer.append("\r\n\r\n<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\"><s:Body>");

			buffer.append(" <registUser xmlns=\"M2MHelper\" xmlns:i=\"http://www.w3.org/2001/XMLSchema-instance\">");
			buffer.append("<userName>");
			buffer.append(name);
			buffer.append("</userName>");

			buffer.append("<passWord>");
			buffer.append(passwd);
			buffer.append("</passWord>");
			// mobile
			buffer.append("<mobile>");
			buffer.append(mobile);
			buffer.append("</mobile>");
			// email
			buffer.append("<email>");
			buffer.append(email);
			buffer.append("</email>");
			// deviceID
			buffer.append("<deviceID>");
			buffer.append(deviceId);
			buffer.append("</deviceID>");
			// devicePWD
			buffer.append("<devicePWD>");
			buffer.append(devicePwd);
			buffer.append("</devicePWD>");

			buffer.append("</registUser></s:Body></s:Envelope>");
			// <?xml version="1.0" encoding="utf-8"?><soap:Envelope
			// xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/"
			// xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			// xmlns:xsd="http://www.w3.org/2001/XMLSchema"><soap:Body><registUserResponse
			// xmlns="M2MHelper"><registUserResult>FAIL:鐢ㄦ埛鍚嶅凡缁忓瓨鍦�!</registUserResult></registUserResponse></soap:Body></soap:Envelope>
			// buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			// buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			// buffer.append("<soap:Body>");
			// buffer.append(" <registUser xmlns=\"M2MHelper\">");
			// buffer.append("<userName>");
			// buffer.append(name);
			// buffer.append("</userName>");
			//
			// buffer.append("<passWord>");
			// buffer.append(passwd);
			// buffer.append("</passWord>");
			// //mobile
			// buffer.append("<mobile>");
			// buffer.append(mobile);
			// buffer.append("</mobile>");
			// //email
			// buffer.append("<email>");
			// buffer.append(email);
			// buffer.append("</email>");
			// //deviceID
			// buffer.append("<deviceID>");
			// buffer.append(deviceId);
			// buffer.append("</deviceID>");
			// //devicePWD
			// buffer.append("<devicePWD>");
			// buffer.append(devicePwd);
			// buffer.append("</devicePWD>");
			//
			// buffer.append("</registUser>");
			// buffer.append("</soap:Body>");
			// buffer.append("</soap:Envelope>");
			out.write(buffer.toString().getBytes());
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			String result = getFindResultByString(str, "<registUserResult>",
					"</registUserResult>");
			if ((result != null) && (result.length() >= 2)) {
				// if(result.equals("ok")){
				// outMsg(TAG, "=============注册成功");
				// ok = true;
				// guId = result
				// }else
				// if(result.equals("OK")){
				// outMsg(TAG, "=============注册成功");
				ok = true;
				// }
				guId = result;
			}
			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============注册失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============注册失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============注册失败！! !");
			e.printStackTrace();
		}
		outMsg(TAG, "=============ok:" + ok);
		if (!ok) {
			outMsg(TAG, "=============注册失败.");
		}
		return guId;
	}

	/**
	 * 
	 * @param str
	 * @param start
	 * @param end
	 * @return "" 无效 字符串 有效
	 */
	private static String getFindResultByString(String str, String start,
			String end) {
		int indexStart = 0;
		int indexEnd = 0;
		String result = "";
		String s = "";
		indexStart = str.indexOf(start);
		indexEnd = str.indexOf(end);
		outMsg(TAG, "=============str:" + str);
		outMsg(TAG, "=============indexStart:" + indexStart + "  indexEnd:"
				+ indexEnd);

		if ((indexStart != 0) && (indexStart < str.length()) && (indexEnd != 0)) {
			indexStart += start.length();
			if (indexEnd > (indexStart)) {
				s = str.substring(indexStart, indexEnd);
				outMsg(TAG, "=============str=" + s);

				if (s.length() > 0) {
					result = s;
				}
			}
		}
		return result;
	}

	private static String getResultByStream(InputStream in) throws IOException {
		int len = 0;
		byte b[] = new byte[1024];
		String s = null;
		StringBuilder sb = new StringBuilder();
		outMsg(TAG, "=============StringBuffer");
		while ((len = in.read(b)) != -1) {
			outMsg(TAG, "=============str=br.readLine()");
			s = new String(b, 0, len, "utf-8");
			sb.append(s);
		}
		return sb.toString();
	}

	private static void outMsg(String tag, String string) {
		// TODO Auto-generated method stub
		Log.i(tag, string);
	}

}
