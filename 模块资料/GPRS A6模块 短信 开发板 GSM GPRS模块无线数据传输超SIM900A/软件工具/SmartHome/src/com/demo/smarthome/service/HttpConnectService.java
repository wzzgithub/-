package com.demo.smarthome.service;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

import com.demo.smarthome.device.Dev;
import com.demo.smarthome.tools.DateTools;

/**
 * Http连接 服务类
 * 
 * @author Administrator
 * 
 */
public class HttpConnectService {

	private static final String TAG = "HttpConnectService";

	// private static final String strUrl =
	// "http://tangdengan.xicp.net:8020/service/s.asmx";
	// private static final String strUrl =
	// "http://192.168.1.104:8019/service/s.asmx";

	/**
	 * 注册用户
	 * 
	 * @param name
	 *            用户名
	 * @param passwd
	 *            密码
	 * @param mobile
	 * @param email
	 * @param deviceId
	 * @param devicePwd
	 * @return 返回""无效 返回“[2]91da0702-8234-473b-9f41-6895fc0ab936"有效
	 */
	public static String registUser(String name, String passwd, String mobile,
			String email, String deviceId, String devicePwd) {
		String guId = "";
		boolean ok = false;
		;
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
			buffer.append("                                                                 ");
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
	 * 用户登录
	 * 
	 * @param name
	 * @param passwd
	 * @return
	 */
	public static String userLogin(String name, String passwd) {
		String guId = "";
		boolean ok = false;
		;
		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);
			outMsg(TAG, "=======userLogin======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Self.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(10000);// 连接超时 单位毫秒
			connection.setReadTimeout(2000);// 读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"M2MHelper/userLogin\""); //

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append(" <userLogin xmlns=\"M2MHelper\">");
			buffer.append("<userName>");
			buffer.append(name);
			buffer.append("</userName>");

			buffer.append("<passWord>");
			buffer.append(passwd);
			buffer.append("</passWord>");

			buffer.append("</userLogin>");
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);

			out.write(buffer.toString().getBytes());
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			// outMsg(TAG, "=============result:"+str);
			String result = getFindResultByString(str, "<userLoginResult>",
					"</userLoginResult>");

			if ((result != null) && (result.length() >= 1)) {

				ok = true;
				guId = result;

				outMsg(TAG, "=============登录成功");

			}

			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============登录失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============登录失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============登录失败！!");
			e.printStackTrace();
		}
		outMsg(TAG, "=============ok:" + ok);
		if (!ok) {
			outMsg(TAG, "=============登录失败.");
		}
		return guId;
	}

	public static List<Dev> getDeviceList(String name, String torken) {
		List<Dev> listDev = new ArrayList<Dev>();
		boolean ok = false;
		;
		boolean isOnLine = false;
		try {

			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);
			outMsg(TAG, "=======chkUser======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Self.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			// connection.setConnectTimeout(10000);//连接超时 单位毫秒
			// connection.setReadTimeout(2000);//读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"M2MHelper/getDeviceList\""); //

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append(" <getDeviceList xmlns=\"M2MHelper\">");
			buffer.append("<userName>");
			buffer.append(name);
			buffer.append("</userName>");

			buffer.append("<torken>");
			buffer.append(torken);
			buffer.append("</torken>");

			buffer.append("</getDeviceList>");
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);

			out.write(buffer.toString().getBytes());
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			// outMsg(TAG, "=============result:"+str);
			String result = getFindResultByString(str, "<getDeviceListResult>",
					"</getDeviceListResult>");

			if ((result != null) && (result.length() >= 1)) {
				String[] dev = result.split(";");
				int index = 0;
				for (int i = 0; i < dev.length; i++) {
					String[] info = dev[i].split(",");
					Dev d = new Dev();
					index = 0;
					if (info.length == 6) {
						d.setId(info[index++]);
						d.setNickName(info[index++]);
						d.setLastUpdate(info[index++]);
						d.setTorken(info[index++]);
						d.setIpPort(info[index++]);
						// d.setOnLine(info[index++].equals("1"));
						// 比较时间 大于2分钟 认为不在线
						isOnLine = info[index++].equals("1");
						if (isOnLine) {
							if (DateTools.getNowTimeByLastTimeDifference(d
									.getLastUpdate()) > 120) {
								isOnLine = false;
							}
						}
						d.setOnLine(isOnLine);
						listDev.add(d);
						ok = true;
					}
				}

				Log.v("GetDevList", "result:" + result);

				// outMsg(TAG, "=============登录成功");

			}

			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============查询设备列表失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============查询设备列表失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============查询设备列表失败！!");
			e.printStackTrace();
		}
		outMsg(TAG, "=============ok:" + ok);
		if (!ok) {
			outMsg(TAG, "=============查询设备列表失败.");
		} else {
			outMsg(TAG, "=============查询设备列表成功.");
		}
		return listDev;
	}

	public static String chkUser(String name, String passwd) {
		String guId = "";
		boolean ok = false;
		;
		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);
			outMsg(TAG, "=======chkUser======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Cfg.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(10000);// 连接超时 单位毫秒
			connection.setReadTimeout(2000);// 读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"VehicleHelper/chkUser\"");

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append(" <chkUser xmlns=\"VehicleHelper\">");
			buffer.append("<userName>");
			// buffer.append("admin");
			buffer.append(name);
			buffer.append("</userName>");

			buffer.append("<passWord>");
			// buffer.append("admin");
			buffer.append(passwd);
			buffer.append("</passWord>");

			buffer.append("</chkUser>");
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			out.write(buffer.toString().getBytes());
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			String result = getFindResultByString(str, "<chkUserResult>",
					"</chkUserResult>");
			if ((result != null) && (result.length() >= 38)) {

				ok = true;
				guId = result;

				outMsg(TAG, "=============登录成功");
				// Self.guId =result;
				// outMsg(TAG, "=============Self.isLogin:"
				// + Self.isLogin);
				// outMsg(TAG, "=============Self.userName:"
				// + Self.userName);
				// outMsg(TAG, "=============Self.userId:"
				// + Self.userId);
				// outMsg(TAG, "=============Self.guId:" + Self.guId);
				// Intent intent = new Intent();
				// intent.setClass(LoginActivity.this, MainActivity.class);
				// startActivity(intent);
				// finish();
			}

			//
			// int len = 0;
			// byte b[] = new byte[1024];
			// String s = null;
			// StringBuilder sb = new StringBuilder();
			// outMsg(TAG, "=============StringBuffer");
			// while ((len = in.read(b)) != -1) {
			// outMsg(TAG, "=============str=br.readLine()");
			// s = new String(b, 0, len, "utf-8");
			// sb.append(s);
			// }
			out.close();
			in.close();

			connection.disconnect();
			// outMsg(TAG, "=============:" + sb);
			//
			// String str = sb.toString();
			// int indexStart = 0;
			// int indexEnd = 0;
			// indexStart = str.indexOf("<chkUserResult>");
			// indexEnd = str.indexOf("</chkUserResult>");
			// outMsg(TAG, "=============str:" + str);
			// outMsg(TAG, "=============indexStart:" + indexStart
			// + "  indexEnd:" + indexEnd);
			//
			// if ((indexStart != 0) && (indexStart < str.length())&& (indexEnd
			// != 0)) {
			// indexStart += 15;
			// if (indexEnd > (indexStart + 38)) {
			// s = str.substring(indexStart, indexEnd);
			// outMsg(TAG, "=============str=" + s);
			//
			// if (s.length() >= 38) {
			// Self.isLogin = true;
			// Self.userName = name;
			// ok = true;
			//
			// outMsg(TAG, "=============登录成功");
			// Self.guId =s;
			// guId = s ;
			// outMsg(TAG, "=============Self.isLogin:"
			// + Self.isLogin);
			// outMsg(TAG, "=============Self.userName:"
			// + Self.userName);
			// outMsg(TAG, "=============Self.userId:"
			// + Self.userId);
			// outMsg(TAG, "=============Self.guId:" + Self.guId);
			// // Intent intent = new Intent();
			// // intent.setClass(LoginActivity.this, MainActivity.class);
			// // startActivity(intent);
			// // finish();
			// }
			// }
			// }

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============登录失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============登录失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============登录失败！!");
			e.printStackTrace();
		}
		outMsg(TAG, "=============ok:" + ok);
		if (!ok) {
			outMsg(TAG, "=============登录失败.");
		}
		return guId;
	}

	public static boolean heartThrob(String userName, String torken) {
		boolean ok = false;
		;
		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);

			outMsg(TAG, "=======heartThrob======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Cfg.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(10000);// 连接超时 单位毫秒
			connection.setReadTimeout(2000);// 读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"M2MHelper/heartThrob\"");

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			// buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			// buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			// buffer.append("<soap:Body>");
			// buffer.append(" <heartThrob xmlns=\"VehicleHelper\">");
			// buffer.append("<userName>");
			// buffer.append(userName);
			// buffer.append("</userName>");
			// buffer.append("<torken>");
			// buffer.append(torken);
			// buffer.append("</torken>");
			//
			// buffer.append("</heartThrob>");
			// buffer.append("</soap:Body>");
			// buffer.append("</soap:Envelope>");
			// buffer.append("                                                                 ");

			// POST /service/s.asmx HTTP/1.1
			// Host: cloud.ai-thinker.com
			// Content-Type: text/xml; charset=utf-8
			// Content-Length: length
			// SOAPAction: "M2MHelper/heartThrob"
			//
			// <?xml version="1.0" encoding="utf-8"?>
			// <soap:Envelope
			// xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			// xmlns:xsd="http://www.w3.org/2001/XMLSchema"
			// xmlns:soap="http://schemas.xmlsoap.org/soap/envelope/">
			// <soap:Body>
			// <heartThrob xmlns="M2MHelper">
			// <userName>string</userName>
			// <torken>string</torken>
			// </heartThrob>
			// </soap:Body>
			// </soap:Envelope>
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append(" <heartThrob xmlns=\"M2MHelper\">");
			buffer.append("<userName>");
			buffer.append(userName);
			buffer.append("</userName>");
			buffer.append("<torken>");
			buffer.append(torken);
			buffer.append("</torken>");

			buffer.append("</heartThrob>");
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			out.write(buffer.toString().getBytes());
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");
			String str = getResultByStream(in);
			String result = getFindResultByString(str, "<heartThrobResult>",
					"</heartThrobResult>");
			if ((result != null)
					&& (result.equals("OK") || result.equals("ok"))) {
				ok = true;
				outMsg(TAG, "=============心跳成功");
			}
			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============心跳失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============心跳失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============注册失败！!");
			e.printStackTrace();
		}
		outMsg(TAG, "=============ok:" + ok);
		if (!ok) {
			outMsg(TAG, "=============心跳失败.");
		}
		return ok;

	}

	/**
	 * 
	 * @param userName
	 *            用户名
	 * @param torken
	 *            KEY
	 * @param logDate
	 *            时间yyyy-MM-dd HH:mm:ss
	 * @param vehicleNO
	 *            设备编号
	 * @param comment
	 *            客户返回信息
	 * @param base64Binary
	 *            图片Base64加密过的string
	 * @return 返回""无效 返回 字符串有效
	 */
	public static String addLogs(String userName, String torken,
			String logDate, String vehicleNO, String comment) {
		String guId = "";
		boolean ok = false;
		;
		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);

			outMsg(TAG, "=======addLogs======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Cfg.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(10000);// 连接超时 单位毫秒
			connection.setReadTimeout(2000);// 读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"VehicleHelper/addLogs\"");

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append("<addLogs xmlns=\"VehicleHelper\">");
			// userName
			buffer.append("<userName>");
			// buffer.append("admin");
			buffer.append(userName);
			buffer.append("</userName>");
			// torken
			buffer.append("<torken>");
			buffer.append(torken);
			buffer.append("</torken>");
			// logDate
			buffer.append("<logDate>");
			buffer.append(logDate);
			buffer.append("</logDate>");

			// vehicleNO
			buffer.append("<vehicleNO>");
			// buffer.append("admin");
			buffer.append(vehicleNO);
			buffer.append("</vehicleNO>");
			// comment
			buffer.append("<comment>");
			// buffer.append("admin");
			buffer.append(comment);
			buffer.append("</comment>");

			buffer.append("</addLogs>");
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			out.write(buffer.toString().getBytes());
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);
			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			String result = getFindResultByString(str, "<addLogsResult>",
					"</addLogsResult>");
			if ((result != null) && (result.length() >= 1)) {
				guId = result;
				// //传图片
				// for(int i=0;i<3;i++){
				// if(addPhoto(userName,torken,
				// result,vehicleNO,base64Binary)){//存图片OK
				// break;
				// }
				// }
			}

			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============添加日志失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============添加日志失败！!");
			e.printStackTrace();
		} catch (Exception e) {
			outMsg(TAG, "=============注册失败！!");
			e.printStackTrace();
		}
		return guId;
	}

	/**
	 * 检查是否有新版本的软件资源可以下载
	 * 
	 * @param userName
	 * @param torken
	 * @param versionId
	 * @return 返回新APP的url
	 */
	public static String getNewAppUrl(String userName, String torken,
			String versionId) {
		String newAppUrl = "";
		boolean ok = false;
		;
		try {
			java.net.URL url = new java.net.URL(Cfg.WEBSERVICE_SERVER_URL);

			outMsg(TAG, "=======addLogs======strUrl:"
					+ Cfg.WEBSERVICE_SERVER_URL);
			HttpURLConnection connection = (HttpURLConnection) url
					.openConnection();
			// Cfg.connection = connection;
			connection.setDoInput(true);
			connection.setDoOutput(true);
			connection.setConnectTimeout(10000);// 连接超时 单位毫秒
			connection.setReadTimeout(2000);// 读取超时 单位毫秒
			connection.setRequestMethod("POST");
			connection.setRequestProperty("Content-Type",
					"text/xml; charset=utf-8");

			connection.setRequestProperty("SOAPAction",
					"\"VehicleHelper/getNewAPP\""); // 修改

			OutputStream out = connection.getOutputStream();
			StringBuffer buffer = new StringBuffer(500);
			buffer.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
			buffer.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">");
			buffer.append("<soap:Body>");
			buffer.append("<getNewAPP xmlns=\"VehicleHelper\">");// 修改
			// userName
			buffer.append("<userName>");
			// buffer.append("admin");
			buffer.append(userName);
			buffer.append("</userName>");
			// torken
			buffer.append("<torken>");
			buffer.append(torken);
			buffer.append("</torken>");
			// logDate
			buffer.append("<version>");
			buffer.append(versionId);
			buffer.append("</version>");

			buffer.append("</getNewAPP>");// 修改
			buffer.append("</soap:Body>");
			buffer.append("</soap:Envelope>");
			buffer.append("                                                                 ");
			out.write(buffer.toString().getBytes());
			outMsg(TAG, "=============StringBuffer.len:" + buffer.length());
			outMsg(TAG, "=============StringBuffer:" + buffer);

			InputStream in = connection.getInputStream();
			outMsg(TAG, "=============StringBuffer");

			String str = getResultByStream(in);
			String result = getFindResultByString(str, "<getNewAPPResult>",
					"</getNewAPPResult>");// 修改
			if ((result != null) && (result.length() >= 10)) {
				// guId = result;
				newAppUrl = result;

			}

			out.close();
			in.close();

			connection.disconnect();

		} catch (MalformedURLException e) {
			outMsg(TAG, "=============检查新版本失败！");
			e.printStackTrace();
		} catch (IOException e) {
			outMsg(TAG, "=============检查新版本失败！!");
			e.printStackTrace();
		}
		return newAppUrl;
	}

	/**
	 * 检查是否连上网络
	 * 
	 * @return
	 */
	public static boolean isConnectToNet(Context icontext) {
		// Context context = icontext.getApplicationContext();
		// ConnectivityManager connectivity = (ConnectivityManager) context
		// .getSystemService(Context.CONNECTIVITY_SERVICE);
		// NetworkInfo[] info;
		// if (connectivity != null) {
		// info = connectivity.getAllNetworkInfo();
		// if (info != null) {
		// for (int i = 0; i < info.length; i++) {
		// if (info[i].getTypeName().equals("WIFI") && info[i].isConnected()) {
		// return true;
		// }
		// }
		// }
		// }
		// return false;
		ConnectivityManager mConnectivityManager = (ConnectivityManager) icontext
				.getSystemService(Context.CONNECTIVITY_SERVICE);
		NetworkInfo mNetworkInfo = mConnectivityManager.getActiveNetworkInfo();
		if ((mNetworkInfo != null) && mNetworkInfo.isAvailable()) {
			return true;
		}
		return false;

	}

	// private===
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
