package com.demo.smarthome.service;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.List;

import com.demo.smarthome.device.Dev;
import com.demo.smarthome.iprotocol.IProtocol;
import com.demo.smarthome.protocol.Buff;
import com.demo.smarthome.protocol.MSGCMD;
import com.demo.smarthome.protocol.MSGCMDTYPE;
import com.demo.smarthome.protocol.Msg;
import com.demo.smarthome.protocol.PlProtocol;
import com.demo.smarthome.tools.DateTools;
import com.demo.smarthome.tools.StrTools;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;
import android.os.Binder;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.Toast;

/**
 * Socket服务类
 * 
 * @author Administrator
 * 
 */
public class SocketService extends Service
{

	// Dev dev = null;
	String ipAddr = "";
	int port = 0;
	int count;
	// boolean socketIsConnected = false;
	boolean isLogin = false;
	private Msg msg = new Msg();
	private byte[] sendStr;

	private static final String TAG = "SocketService";
	private SocketBinder socketBinder = new SocketBinder();

	
//	public final static String ACTION_RECV_MESSAGE = "com.demo.smarthome.service.ACTION_RECV_MESSAGE";

	public class SocketBinder extends Binder
	{
		public SocketService getService()
		{
			Log.i(TAG, "getService");
			return SocketService.this;
		}

	}

	Socket socket = null;
	boolean socketThreadIsRun = true;
	byte[] buffer = new byte[1024];
	byte[] data = new byte[2048];
	int dataLength = 0;
	OutputStream socketOut = null;
	InputStream socketIn = null;

	IProtocol protocol = new PlProtocol();

	void putSocketData()
	{
		synchronized (this)
		{/* 区块 */

		}
	}

	void gutSocketData()
	{
		synchronized (this)
		{/* 区块 */

		}
	}

	int heartbeatCount = 0;
	int HeartbeatMax = 30;// 30秒发送心跳
	int noDataCount = 0;
	Thread socketHeartbeatThread = new Thread()
	{

		@Override
		public void run()
		{
			int heartbeatCount = 0;
			int HeartbeatMax = 30;// 0秒发送心跳
			while (socketThreadIsRun)
			{
				try
				{
					Thread.sleep(1000);
				} catch (InterruptedException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (socket != null)
				{
					if (isLogin)
					{
						heartbeatCount++;
						// Log.i(TAG, DateTools.getNowTimeString()
						// + "==>heartbeatCount:" + heartbeatCount
						// + "    HeartbeatMax:" + HeartbeatMax);
						if (heartbeatCount >= HeartbeatMax)
						{
							Log.i(TAG, DateTools.getNowTimeString()
									+ "==>heartbeatCoun  time out! socketOut:"
									+ socketOut);
							heartbeatCount = 0; // 心跳

							msg.setCmdType(MSGCMDTYPE.valueOf(0xA0));
							msg.setCmd(MSGCMD.valueOf(0x01));
							msg.setId(Cfg.userId);
							msg.setTorken(Cfg.torken);
							msg.setDataLen(0);
							protocol.MessageEnCode(msg);
							if (!(socketOut == null))
							{
								// Log.i(TAG, "==发送 心跳指令！");
								sendStr = msg.getSendData();
								try
								{
									Log.i(TAG,
											DateTools.getNowTimeString()
													+ "==>发送 心跳指令！"
													+ StrTools
															.bytesToHexString(sendStr));
									socketOut.write(sendStr);
								} catch (IOException e)
								{
									// TODO Auto-generated catch block
									e.printStackTrace();
								}
							}
							HttpConnectService.heartThrob(Cfg.userName,
									Cfg.torken);

						}
						noDataCount++; //
						// Log.i(TAG, DateTools.getNowTimeString()
						// + "==>noDataCount:" + noDataCount
						// + "    HeartbeatMax * 3:" + (HeartbeatMax * 3));
						if (noDataCount >= HeartbeatMax * 3)
						{ // socket无有效数据
							noDataCount = 0; //
							socketClose();
						}

					}

				}

			}
		}
	};

	Thread socketThread = new Thread()
	{

		@Override
		public void run()
		{
			// TODO Auto-generated method stub
			int len;
			// int heartbeatCount = 0;
			// int HeartbeatMax = 300;// 0秒发送心跳
			// int noDataCount = 0;
			while (socketThreadIsRun)
			{
				try
				{
					Thread.sleep(100);
				} catch (InterruptedException e)
				{
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (socket == null)
				{
					try
					{
						Thread.sleep(500);
					} catch (InterruptedException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					if ("".equals(ipAddr))
					{
						continue;
					}
					if (port < 1000)
					{
						continue;
					}
					try
					{
						socket = new Socket(ipAddr, port);
						// socket.set
						dataLength = 0;
						socketOut = socket.getOutputStream();
						socketIn = socket.getInputStream();

					} catch (UnknownHostException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
						socketClose();
					} catch (IOException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
						socketClose();
						continue;
					} catch (Exception e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
						socketClose();
						continue;
					}
					SendBoardCast(true);
					Log.i(TAG, "==socketThread==ip:" + ipAddr + "  port:"
							+ port + " "
							+ ((socket == null) ? "conn error" : "conn ok"));

				}
				else
				{// socket connect...

					if (!isLogin)
					{// 发送登录指令
						msg.setCmdType(MSGCMDTYPE.valueOf(0xA0));
						msg.setCmd(MSGCMD.valueOf(0x00));
						msg.setId(Cfg.userId);
						msg.setTorken(Cfg.torken);
						msg.setData(Cfg.passWd);
						msg.setDataLen(Cfg.passWd.length);
						protocol.MessageEnCode(msg);
						if (!(socketOut == null))
						{
							sendStr = msg.getSendData();

							try
							{
								Log.i(TAG, DateTools.getNowTimeString() + "==>发送 登录指令！"
												+ StrTools.bytesToHexString(sendStr));
								socketOut.write(sendStr);
							} catch (IOException e)
							{
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
						}
						try
						{
							Thread.sleep(5000);
						} catch (InterruptedException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					try
					{
						Log.i(TAG, DateTools.getNowTimeString()
								+ "==read Data start！");
						len = socketIn.read(buffer);
						Log.i(TAG, DateTools.getNowTimeString()
								+ "==read Data end！");
					} catch (IOException e)
					{
						// TODO Auto-generated catch block
						len = 0;

						socketClose();
						e.printStackTrace();
					} catch (Exception e)
					{
						// TODO Auto-generated catch block
						len = 0;

						socketClose();
						e.printStackTrace();
					}
					if (len > 0)
					{
						Log.i(TAG, DateTools.getNowTimeString()
								+ "==code Data start！");
						noDataCount = 0;
						Log.i(TAG, "接收数据 长度:" + len);
						Log.i(TAG, "buffer:" + buffer);
						
						System.arraycopy(buffer, 0, data, dataLength, len);
						dataLength += len;

						// byte[] da = new byte[dataLength];
						Buff buff = new Buff();
						buff.data = new byte[dataLength];
						buff.length = dataLength;
						System.arraycopy(data, 0, buff.data, 0, dataLength);
						StrTools.printHexString(buff.data);
						
						// 广播接收到的数据
						sendBroadCast(Cfg.ReceiveMessage, buff.data);
						
						if (dataLength >= 21)
						{
							List<Msg> listMsg = protocol.checkMessage(buff);
							System.arraycopy(buff.data, 0, data, 0, buff.length);

							dataLength = buff.length;
							Log.i(TAG, "result listMsg.size():" + listMsg.size());
							StrTools.printHexString(buff.data);
							// dev.putMsg(listMsg);
							Log.i(TAG, "result :");
							
							for (Msg msg : listMsg)
							{
								// StrTools.printHexString(msg.getSendData());
								Log.i(TAG, "msg:" + msg.toString());
								if ((msg.getCmdType() == MSGCMDTYPE.CMDTYPE_A0)
										&& (msg.getCmd() == MSGCMD.CMD00))
								{
									isLogin = true;

									Log.i(TAG, DateTools.getNowTimeString()
											+ "==socket登录成功！");
								}
								if (msg.isSponse())
								{
									socketSendMessage(msg);
								}

							}

						}
						Log.i(TAG, DateTools.getNowTimeString()
								+ "==code Data end！");
					}
				}
			}
		}
	};

	@Override
	public IBinder onBind(Intent intent)
	{
		// TODO Auto-generated method stub
		Log.i(TAG, "=======onBind");
		return socketBinder;
	}

	@Override
	public void onCreate()
	{
		// TODO Auto-generated method stub
		// WifiManager manager = (WifiManager) this
		// .getSystemService(Context.WIFI_SERVICE);
		// UdpHelper udphelper = new UdpHelper(manager);
		//
		// //传递WifiManager对象，以便在UDPHelper类里面使用MulticastLock
		// udphelper.addObserver(SocketService.this);
		// tReceived = new Thread(udphelper);
		// tReceived.start();

		super.onCreate();

		ipAddr = Cfg.TCP_SERVER_URL;
		port = Cfg.TCP_SERVER_PORT;
		socketThread.start();
		socketHeartbeatThread.start();
		// socketThread.run();

		// new UDPThread().start();
		Log.i(TAG, "====onCreate");

	}

	@Override
	public void onDestroy()
	{
		// TODO Auto-generated method stub
		Log.i(TAG, "xxxxxxxxxxxonDestroy");
		socketThreadIsRun = false;
		try
		{
			Thread.sleep(1000);
		} catch (InterruptedException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		socketClose();
		super.onDestroy();
	}

	public void myMethod()
	{// String strIpAddr, int port
		Log.i(TAG, " myMethod");
		Log.i(TAG, "ip：" + ipAddr + "   port:" + port + "    strIpAddr:"
				+ ipAddr + "    port:" + port + "      socketIsConnected:");

	}

	public void myMethod(int args)
	{// String strIpAddr, int port
		Log.i(TAG, " myMethod args:" + args);
		Toast.makeText(SocketService.this, "正在执行", Toast.LENGTH_SHORT).show();
		// Log.i(TAG,
		// "ip："+ipAddr+"   port:"+port+"    strIpAddr:"+ipAddr+"    port:"+port+
		// "      socketIsConnected:"+socketIsConnected);

	}

	public void socketReConnect()
	{
		// this.dev = dev;
		// ipAddr = dev.getIpAddr().trim();
		// port = dev.getPort();
		socketClose();
	}

	public boolean socketIsConnected()
	{
		return (socket != null);
	}

	void socketClose()
	{
		try
		{
			if (socketOut != null)
			{
				socketOut.close();
			}
			if (socketIn != null)
			{
				socketIn.close();
			}
			if (socket != null)
			{
				socket.close();
			}
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// socketIsConnected = false;
		isLogin = false;
		socket = null;
		socketOut = null;
		socketIn = null;
		SendBoardCast(false);

	}

	public synchronized void socketSendMessage(Msg msg)
	{

		if (socket == null)
		{
			Log.i(TAG, "==socketSendMessage==xxx socket == null");
			return;
		}

		if (socketOut == null)
		{
			Log.i(TAG, "==socketSendMessage==xxx socketOut == null");
			return;
		}
		if (msg == null)
		{
			Log.i(TAG, "==socketSendMessage==xxx msg == null");
			return;
		}
		byte[] sendData = msg.getSendData();
		if (sendData == null)
		{
			return;
		}
		try
		{
			socketOut.write(sendData);
			Log.i(TAG,
					"=发送数据=socketSendMessage== send ok hex:"
							+ StrTools.bytesToHexString(sendData));
			// StrTools.printHexString(sendData);
		} catch (IOException e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace();
			socketClose();
		}

	}

	public synchronized void socketSendMessage(List<Msg> listMsg)
	{

		Log.i(TAG, "==socketSendMessage==listMsg:" + listMsg.size());

		for (Msg msg : listMsg)
		{
			if (socket == null)
			{
				Log.i(TAG, "==socketSendMessage==xxx socket == null");
				break;
			}

			if (socketOut == null)
			{
				Log.i(TAG, "==socketSendMessage==xxx socketOut == null");
				break;
			}
			if (msg == null)
			{
				Log.i(TAG, "==socketSendMessage==xxx msg == null");
				continue;
			}
			byte[] sendData = msg.getSendData();
			if (sendData == null)
			{
				continue;
			}
			try
			{
				socketOut.write(sendData);
				// Log.i(TAG,
				// "==socketSendMessage== send ok hex:"+StrTools.bytesToHexString(sendData));
				// StrTools.printHexString(sendData);
			} catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
				socketClose();
			}
		}

	}
	
	public void sendBroadCast(String action, byte[] data)
	{
		Intent intent = new Intent(action);
		intent.putExtra("receiveData", data);
		//LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
		SocketService.this.sendBroadcast(intent);
	}
	
	private void SendBoardCast(boolean isConnect)
	{
		String str = " ip:" + ipAddr + ":" + port;
		Intent intent = new Intent();
		boolean connOk = false;
		intent.setAction(Cfg.SendBoardCastName);
		if (isConnect)
		{
			str += "   连接成功。";
			connOk = true;
		}
		else
		{
			str += "   连接断开。";
		}
		intent.putExtra("result", str);
		intent.putExtra("conn", connOk);
		SocketService.this.sendBroadcast(intent);
	}

	public class UDPThread extends Thread
	{
		String Hostip = "127.0.0.1";
		// String ip="127.0.0.1";
		int port = Cfg.DEV_UDP_SEND_PORT;

		public UDPThread()
		{

		}

		public String echo(String msg)
		{
			return " adn echo:" + msg;
		}

		public void run()
		{

			boolean isSetup = false;
			DatagramSocket dSocket = null;
			byte[] buf = new byte[1024];
			DatagramPacket dp = new DatagramPacket(buf, 1024);

			InetAddress local = null;
			DatagramPacket dPacket;
			while (socketThreadIsRun)
			{
				try
				{
					sleep(1000);
				} catch (InterruptedException e1)
				{
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				try
				{
					local = InetAddress.getByName(Hostip); // 本机测试
					// local = InetAddress.getLocalHost(); // 本机测试
					Log.i(TAG, "local:" + local);
				} catch (UnknownHostException e)
				{
					e.printStackTrace();
					Log.i(TAG, "init localAddr error Hostip:" + Hostip);
					continue;
				}
				try
				{
					dSocket = new DatagramSocket(port); // 注意此处要先在配置文件里设置权限,否则会抛权限不足的异常
				} catch (SocketException e)
				{
					e.printStackTrace();
					Log.i(TAG, "init DatagramSocket error port:" + port);
					continue;
				}

				String localPort = dSocket.getLocalPort() + "";

				Log.i(TAG, "Hostip:" + Hostip + "   localPort:"
						+ localPort);

				String msg = "RPL:\"" + Hostip + "\",\"" + localPort + "\"";

				int msg_len = msg == null ? 0 : msg.getBytes().length;
				// DatagramPacket dPacket = new DatagramPacket(msg.getBytes(),
				// msg_len, local, port);

				// try {
				dPacket = new DatagramPacket(msg.getBytes(), msg_len, local,
						port);
				// } catch (OException e) {
				// Log.i(TAG, "====UDP init ok");
				// }

				Log.i(TAG, "====UDP init ok");
				break;
			}

			while (socketThreadIsRun)
			{
				try
				{

					dSocket.receive(dp);
					String strInfo = "";// dp.getSocketAddress().toString()+
										// " recvData:";
					strInfo = new String(dp.getData(), 0, dp.getLength());
					Log.i(TAG, strInfo);
					String str = strInfo;
					String[] tmp = str.split(":");
					for (String s : tmp)
					{
						Log.i(TAG, "item1:" + s);
					}
					if (tmp.length >= 2)
					{
						str = tmp[0];
						if (!str.equals("RPT"))
						{
							continue;
						}
						str = tmp[1];
						tmp = str.split(",");
						for (String s : tmp)
						{
							Log.i(TAG, "item2:" + s);
						}
						if (tmp.length >= 5)
						{
							Log.i(TAG, "tmp[0]:" + tmp[0]);
							Log.i(TAG, "tmp[1]:" + tmp[1]);
							String idStr = tmp[0].replace('"', ' ');
							String passStr = tmp[1].replace('"', ' ');
							Log.i(TAG, "idStr:" + idStr);
							Log.i(TAG, "pasStrs:" + passStr);
							StrTools.StrHexLowToLong(idStr);
							StrTools.StrHexLowToLong(passStr);
							StrTools.StrHexHighToLong(idStr);
							StrTools.StrHexHighToLong(passStr);
							Dev d = new Dev();
							d.setId(StrTools.StrHexLowToLong(idStr) + "");
							d.setPass(StrTools.StrHexHighToLong(passStr) + "");
							Cfg.putDevScan(d);
							// Cfg.listDevScan.add(d);
							// deviceId = StrTools.StrHexLowToLong(idStr)+"";;
							// devicePwd =
							// StrTools.StrHexHighToLong(passStr)+"";;
						}
					}

				} catch (IOException e)
				{
					e.printStackTrace();
				}
			}
		}
	}
}
