package com.demo.smarthome.service;

import java.util.ArrayList;
import java.util.List;

import com.demo.smarthome.device.Dev;

import android.app.Application;


/**
 * 应用程序全局变量 类
 * @author Administrator
 *
 */
public class Cfg extends Application
{
	public final static String VERSION ="10001";

	public static String savePath="//sdcard//myImage/";

	public final static String SendBoardCastName ="com.demo.smarthome.service.socketconnect";
	public final static String ReceiveMessage = "com.demo.smarthome.service.receivemessage";

	public final static String KEY_PASS_WORD ="password";
	public final static String KEY_USER_NAME ="username";

	//  发布地址  cloud.ai-thinker.com
	//	cloud.ai-thinker.com
	//	admin
	//	admin_!@#*()
	//  调试地址 tangdengan.xicp.net
//	public final static String WEBSERVICE_SERVER_URL="http://182.139.160.79:8020/service/s.asmx";
	public final static String WEBSERVICE_SERVER_URL="http://cloud.ai-thinker.com/service/s.asmx";
	public final static String TCP_SERVER_URL="cloud.ai-thinker.com";
//	public final static String WEBSERVICE_SERVER_URL="http://tangdengan.xicp.net:8020/service/s.asmx";
//	public final static String TCP_SERVER_URL="tangdengan.xicp.net";
//	public final static String TCP_SERVER_URL="182.139.160.79";

	public final static int  TCP_SERVER_PORT=6009;

//	public final static String DEV_UDP_IPADDR="192.168.5.88"; //192.168.1.255
	public final static int  DEV_UDP_SEND_PORT=2468;
	public final static int  DEV_UDP_SEND_DELAY=100;
	public final static int  DEV_UDP_READ_DELAY=15; //udp 扫描等待15秒

	public final static String DEV_UDP_IPADDR="192.168.4.1";
	public final static int  DEV_UDP_PORT=8001;

	public static byte[] userId= new byte[0];
	public static String userName="";
	public static long id=0;
	public static byte[] passWd= new byte[0];
	public static String torken = "";
	public static byte[] tcpTorken = new byte[0];


	public static boolean isLogin=false;
	public static boolean isSubmitDev=false;
	public static boolean isDeleteDev=false;

	public static boolean register=false;
	public  static String regUserName ="";
	public  static String regUserPass ="";

	public static List<Dev> listDev=new  ArrayList<Dev>();
	private static List<Dev> listDevScan=new  ArrayList<Dev>();//扫描的设备

	public  static String deviceId ="";
	public  static String devicePwd ="";

//	public static final int timeDelayDef = 50; // 命令延时时间 ms
//	public static final int timeOutDef = 30 * (1000 / timeDelayDef); // 命令超时时间
//																		// 20秒
//	public static final int timeReSendTimeDef = 5 * (1000 / timeDelayDef); // 命令重发超时时间

	public static Dev getDevById(String id)
	{
		Dev dev = null;
		for (Dev d : listDev)
		{
			if (d.getId().equals(id))
			{
				dev = d;
				break;
			}
		}
		return dev;
	}


	public static Dev getDevScan()
	{
		Dev dev = null;
		for (Dev d : listDevScan)
		{
			dev = d;
			listDevScan.remove(d);
			break;

		}
		return dev;
	}
	public static void putDevScan(Dev dev)
	{
		if(dev == null)
		{
			return;
		}
		for (Dev d : listDev)
		{
			if (d.getId().equals(dev.getId()))
			{
				return;
			}
		}
		for (Dev d : listDevScan)
		{
			if (d.getId().equals(dev.getId()))
			{
				return;
			}
		}
		listDevScan.add(dev);
		return ;
	}

	public static void devScanClean()
	{
		listDevScan.clear();
		return ;
	}
}
