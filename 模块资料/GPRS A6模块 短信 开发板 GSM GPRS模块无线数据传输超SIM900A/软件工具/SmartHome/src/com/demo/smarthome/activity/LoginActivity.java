package com.demo.smarthome.activity;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.nio.ByteBuffer;
import java.util.List;

import com.demo.smarthome.dao.ConfigDao;
import com.demo.smarthome.service.ConfigService;
import com.demo.smarthome.service.HttpConnectService;
import com.demo.smarthome.service.Cfg;
import com.demo.smarthome.tools.MD5Tools;
import com.demo.smarthome.tools.StrTools;
import com.demo.smarthome.R;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.app.Activity;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.Window;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.demo.smarthome.device.Dev;
import com.hq.bean.LedNative;

/**
 * 登录类
 * 
 * @author Administrator
 * 
 */
public class LoginActivity extends Activity {
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();

	}

	@Override
	protected void onStart() {
		// TODO Auto-generated method stub
		super.onStart();

		// if(Cfg.register){
		// try {
		// Thread.sleep(1000);
		// } catch (InterruptedException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }
		// txtName.setText(Cfg.regUserName);
		// txtPassword.setText(Cfg.regUserPass);
		// new LoginThread().start();
		// try {
		// Thread.sleep(1000);
		// } catch (InterruptedException e) {
		// // TODO Auto-generated catch block
		// e.printStackTrace();
		// }
		// new LoginThread().start();
		// }
	}

	TextView title = null;

	EditText txtName = null;
	EditText txtPassword = null;

	String name = "";
	String password = "";
	boolean isLogin = false;
	private static final String TAG = "LoginActivity";
	ConfigService dbService;
	static final int LOGIN_SUCCEED = 0;
	static final int LOGIN_ERROR = 1;
	static final int GET_DEV_SUCCEED = 2;
	static final int GET_DEV_ERROR = 3;

	Handler handler = new Handler() {

		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			// dataToui();
			switch (msg.what) {
			case LOGIN_SUCCEED:
				isLogin = true;
				dbService.SaveSysCfgByKey(Cfg.KEY_USER_NAME, txtName.getText()
						.toString());
				dbService.SaveSysCfgByKey(Cfg.KEY_PASS_WORD, txtPassword
						.getText().toString());
				Toast.makeText(LoginActivity.this, "登录成功", Toast.LENGTH_SHORT)
						.show();
				// 跳转到设置界面
				Intent intent = new Intent();
				intent.setClass(LoginActivity.this, MainActivity.class);
				startActivity(intent);// 打开新界面
				finish();
				// 获取设备列表
				// new GetDevListThread().start();
				break;
			case LOGIN_ERROR:
				if (Cfg.register) {
					Cfg.register = false;
					new LoginThread().start();
					return;
				}
				Toast.makeText(LoginActivity.this, "登录失败！", Toast.LENGTH_SHORT)
						.show();
				// finish();
				break;

			case GET_DEV_SUCCEED:
				Toast.makeText(LoginActivity.this, "获取设备列表成功！",
						Toast.LENGTH_SHORT).show();
				// 跳转到设置界面
				// Intent intent = new Intent();
				// intent.setClass(LoginActivity.this, MainActivity.class);
				// startActivity(intent);// 打开新界面
				// finish();
				break;
			case GET_DEV_ERROR:
				Toast.makeText(LoginActivity.this, "获取设备列表失败！",
						Toast.LENGTH_SHORT).show();
				//
				break;
			default:
				break;

			}
		}

	};
	private UDPThread udphelper = null;
	private Thread tReceived = null;;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		// requestWindowFeature(Window.FEATURE_CUSTOM_TITLE); // 注意顺序
		requestWindowFeature(Window.FEATURE_NO_TITLE); // 注意顺序
		setContentView(R.layout.activity_login);
		// getWindow().setFeatureInt(Window.FEATURE_CUSTOM_TITLE,R.layout.title);
		title = (TextView) findViewById(R.id.titlelogin);
		title.setClickable(true);
		title.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				finish();
			}

		});

		txtName = (EditText) findViewById(R.id.loginTxtName);
		txtPassword = (EditText) findViewById(R.id.loginTxtPassword);

		// Intent intent = this.getIntent();
		// String strDevId = intent.getStringExtra("devId");
		// String strDevPass = intent.getStringExtra("devPass");
		//
		// if((strDevId!=null)&&(!strDevId.isEmpty())){
		// if((strDevPass!=null)&&(!strDevPass.isEmpty())){
		// txtName.setText(strDevId );
		// txtPassword.setText(strDevPass );
		// }
		// }

		Button btnOk = (Button) findViewById(R.id.loginBtnOk);
		btnOk.setOnClickListener(new BtnOkOnClickListener());
		Button btnReg = (Button) findViewById(R.id.loginBtnReg);
		btnReg.setOnClickListener(new BtnRegOnClickListener());
		Button btnSetup = (Button) findViewById(R.id.loginBtnSetup);
		btnSetup.setOnClickListener(new BtnSetupOnClickListener());
		Button btnSmartLink = (Button) findViewById(R.id.loginBtnSmartLink);
		btnSmartLink.setOnClickListener(new BtnSmartLinkOnClickListener());

		dbService = new ConfigDao(LoginActivity.this.getBaseContext());
		// Cfg.userName = dbService.getCfgByKey(Cfg.KEY_USER_NAME);
		// Cfg.userName = dbService.getCfgByKey(Cfg.KEY_USER_NAME);

		txtName.setText(Cfg.userName);

		

		// txtName.setText("asdf");
		// txtPassword.setText("asdf");
		// WifiManager manager = (WifiManager) this
		// .getSystemService(Context.WIFI_SERVICE);
		// WifiManager.MulticastLock lock =
		// manager.createMulticastLock("UDPwifi");
		// lock.acquire();
		// udphelper = new UDPThread(manager);
		// // udphelper.start();
		//
		// //传递WifiManager对象，以便在UDPHelper类里面使用MulticastLock
		// // udphelper.addObserver(LoginActivity.this);
		// tReceived = new Thread(udphelper);
		// tReceived.start();

		// new UDPThread().start();
		// new UDPReceiveThread().start();
		// Log.v("LoginThread", "UDPReceiveThread start..");
		// new UDPThread().start();
		// test
		// int id = 10159981;
		// long pass = 2973179566l;
		// Dev dev = new Dev();
		// dev.setId("1234567890");
		// dev.setNickName("123456789");
		// DevDao dao = new DevDao(LoginActivity.this.getBaseContext());
		// dao.saveDev(dev);
		// Log.v("LoginThread", "dev:"+dev.getId()+" "+dev.getNickName());
		//
		// dev.setId("12345678901");
		// dev.setNickName("1234567891");
		// dao.saveDev(dev);
		// Log.v("LoginThread", "dev:"+dev.getId()+" "+dev.getNickName());
		// dev = dao.getDevById("1234567890");
		// Log.v("LoginThread",
		// "select dev:"+dev.getId()+" "+dev.getNickName());
		// dev = dao.getDevById("12345678901");
		// Log.v("LoginThread",
		// "select dev:"+dev.getId()+" "+dev.getNickName());
		//
		// List<Dev> listDev = dao.getDevList();
		// for(Dev d : listDev){
		// Log.v("LoginThread", "out dev:"+dev.getId()+" "+dev.getNickName());
		// }
		Cfg.userName = dbService.getCfgByKey(Cfg.KEY_USER_NAME);

		Intent intent = this.getIntent();
		String strDevId = intent.getStringExtra("devId");
		String strDevPass = intent.getStringExtra("devPass");

		if ((strDevId != null) && (!strDevId.isEmpty())) {
			if ((strDevPass != null) && (!strDevPass.isEmpty())) {
				txtName.setText(strDevId);
				txtPassword.setText(strDevPass);
				new LoginThread().start();
			}
		} else {
			txtName.setText(dbService.getCfgByKey(Cfg.KEY_USER_NAME));
			txtPassword.setText(dbService.getCfgByKey(Cfg.KEY_PASS_WORD));
			// Cfg.userName = dbService.getCfgByKey(Cfg.KEY_USER_NAME);
		}
		name = txtName.getText().toString();
		password = txtPassword.getText().toString();
		if (name.trim().isEmpty()) {
			// Toast.makeText(getApplicationContext(), "请输入用户名", 0).show();
			txtName.setFocusable(true);
			return;
		}
		if (password.trim().isEmpty()) {
			// Toast.makeText(getApplicationContext(), "请输入密码", 0).show();
			txtPassword.setFocusable(true);
			return;
		}

		// new LoginThread().start();

		// txtName.setText("asdf");
		// txtPassword.setText("asdf");
		// if(!txtName.getText().toString().isEmpty()){
		// if(!txtPassword.getText().toString().isEmpty()){
		// // new LoginThread().start();
		// // try {
		// // Thread.sleep(1000);
		// // } catch (InterruptedException e) {
		// // // TODO Auto-generated catch block
		// // e.printStackTrace();
		// // }
		// //// if(isLogin){
		// //// return;
		// //// }
		// new LoginThread().start();
		// }
		//
		// }
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.login, menu);
		return true;
	}

	/**
	 * 登录按钮监听类
	 * 
	 * @author Administrator
	 * 
	 */
	class BtnOkOnClickListener implements OnClickListener {

		@Override
		public void onClick(View v) {
			// TODO Auto-generated method stub
			name = txtName.getText().toString();
			password = txtPassword.getText().toString();
			if (name.trim().isEmpty()) {
				Toast.makeText(getApplicationContext(), "请输入用户名", 0).show();
				txtName.setFocusable(true);
				return;
			}
			if (password.trim().isEmpty()) {
				Toast.makeText(getApplicationContext(), "请输入密码", 0).show();
				txtPassword.setFocusable(true);
				return;
			}

			new LoginThread().start();

		}

	}

	/**
	 * 注册按钮监听类
	 * 
	 * @author Administrator
	 * 
	 */
	class BtnRegOnClickListener implements OnClickListener {

		@Override
		public void onClick(View v) {

			Intent intent = new Intent();
			intent.setClass(LoginActivity.this, RegisterActivity.class);// CaptureActivity
			Bundle bundle = new Bundle();
			bundle.putInt("type", 2);
			intent.putExtras(bundle);
			startActivity(intent);
			finish();
		}

	}

	/**
	 * 设置按钮监听类
	 * 
	 * @author Administrator
	 * 
	 */
	class BtnSetupOnClickListener implements OnClickListener {

		@Override
		public void onClick(View v) {
			Intent intent = new Intent();
			intent.setClass(LoginActivity.this, SetupDevActivity.class);
			startActivity(intent);

		}

	}

	class BtnSmartLinkOnClickListener implements OnClickListener {

		@Override
		public void onClick(View v) {
			Intent intent = new Intent();
			intent.setClass(
					LoginActivity.this,
					com.espressif.iot.esptouch.demo_activity.EsptouchDemoActivity.class);
			startActivity(intent);

		}

	}

	/**
	 * 登录
	 * 
	 * @author Administrator
	 * 
	 */
	class LoginThread extends Thread {

		@Override
		public void run() {
			Message message = new Message();
			message.what = LOGIN_ERROR;

			Log.v("LoginThread", "LoginThread start..");
			String md5Pass;
			if (password.length() >= 20) {
				md5Pass = password;
			} else {
				md5Pass = MD5Tools.string2MD5(password).toUpperCase();
			}
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			String info = HttpConnectService.userLogin(name, (md5Pass));

			if (info.length() > 10) {
				String[] text = info.split(":");
				int index = 0;
				if (text.length == 4) {
					index = 1;
					Cfg.torken = text[index++];
					Cfg.userId = StrTools.hexStringToBytes(StrTools
							.strNumToBig(text[index++]));// id 要倒序
					Cfg.passWd = StrTools.hexStringToBytes(StrTools
							.strNumToHex(text[index++]));

					Cfg.userName = name;
					message.what = LOGIN_SUCCEED;
					// Log.v("LoginThread", "Cfg.torken:" + Cfg.torken);
					// Log.v("LoginThread", "Cfg.userId:" + Cfg.userId);
					// Log.v("LoginThread", "Cfg.userName:" + Cfg.userName);
					// Log.v("LoginThread", "Cfg.userId:" + Cfg.userId);
					// Log.v("LoginThread", "Cfg.passWd:" + Cfg.passWd);
				}
			}
			handler.sendMessage(message);
		}
	}

	/**
	 * 获得设备列表线程
	 * 
	 * @author Administrator
	 * 
	 */
	class GetDevListThread extends Thread {

		@Override
		public void run() {
			Message message = new Message();
			message.what = GET_DEV_ERROR;

			Log.v("GetDevListThread", "GetDevListThread start..");

			List<Dev> listDev = HttpConnectService.getDeviceList(Cfg.userName,
					new String(Cfg.torken));

			Cfg.listDev = listDev;
			for (Dev dev : listDev) {
				Log.v("GetDevListThread", "dev:" + dev);

			}
			if (listDev.size() > 0) {
				message.what = GET_DEV_SUCCEED;
			}
			handler.sendMessage(message);
		}
	}

	class UDPThread extends Thread {

		public String echo(String msg) {
			return " adn echo:" + msg;
		}

		public void run() {
			int port = 2468;
			// DatagramChannel channel = null;
			final int MAX_SIZE = 1024;

			byte[] buf = new byte[1024];
			DatagramPacket dp = new DatagramPacket(buf, 1024);
			// try {
			// channel = DatagramChannel.open();
			// } catch (IOException e1) {
			// // TODO Auto-generated catch block
			// e1.printStackTrace();
			// }
			InetAddress local = null;
			DatagramSocket socket = null;
			try {
				local = InetAddress.getByName("192.168.1.88"); // 本机测试
																// Cfg.DEV_UDP_IPADDR
				// local = InetAddress.getLocalHost(); // 本机测试
				System.out.println("local:" + local);
				socket = new DatagramSocket(null);
			} catch (SocketException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (UnknownHostException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			SocketAddress localAddr = new InetSocketAddress(port);
			try {
				socket.setReuseAddress(true);
				socket.bind(new InetSocketAddress(port));
				// socket.bind(localAddr);
			} catch (SocketException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			System.out.println("UDP服务器启动");

			ByteBuffer receiveBuffer = ByteBuffer.allocate(MAX_SIZE);

			String msg = "android. ";
			DatagramPacket dPacket = new DatagramPacket(msg.getBytes(),
					msg.length(), local, Cfg.DEV_UDP_PORT);
			try {
				socket.send(dPacket);
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			}
			while (true) {
				try {
					// 发送设置为广播
					socket.setBroadcast(true);
					// socket.send(dPacket);

					socket.receive(dp);
					String strInfo = new String(dp.getData(), 0, dp.getLength());
					System.out.println(strInfo);

					// receiveBuffer.clear();
					// InetSocketAddress client = (InetSocketAddress) channel
					// .receive(receiveBuffer);
					// // 接收来自任意一个EchoClient的数据报
					// receiveBuffer.flip();
					// String msg =
					// Charset.forName("utf8").decode(receiveBuffer)
					// .toString();
					// System.out.println(client.getAddress() + ":" +
					// client.getPort()
					// + ">" + msg);
					// channel.send(ByteBuffer.wrap(echo(msg).getBytes()),
					// client);
					// // 给EchoClient回复一个数据报
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}

	// public class UdpHelper implements Runnable {
	// public Boolean IsThreadDisable = false;//指示监听线程是否终止
	// private WifiManager.MulticastLock lock;
	// InetAddress mInetAddress;
	// public UdpHelper() {
	//
	// }
	// public UdpHelper(WifiManager manager) {
	// this.lock= manager.createMulticastLock("UDPwifi");
	// }
	// public void StartListen() {
	// // UDP服务器监听的端口
	// Integer port = 2468;
	// // 接收的字节大小，客户端发送的数据不能超过这个大小
	// byte[] message = new byte[100];
	// try {
	// // 建立Socket连接
	// DatagramSocket datagramSocket = new DatagramSocket(port);
	// datagramSocket.setBroadcast(true);
	// DatagramPacket datagramPacket = new DatagramPacket(message,
	// message.length);
	// try {
	// while (!IsThreadDisable) {
	// // 准备接收数据
	// Log.d("UDP Demo", "准备接受");
	// this.lock.acquire();
	//
	// datagramSocket.receive(datagramPacket);
	// String strMsg=new String(datagramPacket.getData()).trim();
	// Log.d("UDP Demo", datagramPacket.getAddress()
	// .getHostAddress().toString()
	// + ":" +strMsg );this.lock.release();
	// }
	// } catch (IOException e) {//IOException
	// e.printStackTrace();
	// }
	// } catch (SocketException e) {
	// e.printStackTrace();
	// }
	//
	// }
	// public void send(String message) {
	// message = (message == null ? "Hello IdeasAndroid!" : message);
	// int server_port = 8904;
	// Log.d("UDP Demo", "UDP发送数据:"+message);
	// DatagramSocket s = null;
	// try {
	// s = new DatagramSocket();
	// } catch (SocketException e) {
	// e.printStackTrace();
	// }
	// InetAddress local = null;
	// try {
	// local = InetAddress.getByName("255.255.255.255");
	// } catch (UnknownHostException e) {
	// e.printStackTrace();
	// }
	// int msg_length = message.length();
	// byte[] messageByte = message.getBytes();
	// DatagramPacket p = new DatagramPacket(messageByte, msg_length, local,
	// server_port);
	// try {
	//
	// s.send(p);
	// s.close();
	//
	// } catch (IOException e) {
	// e.printStackTrace();
	// }
	// }
	//
	// @Override
	// public void run() {
	// StartListen();
	// }
	// }
	//
	// }
	class UDPReceiveThread extends Thread {
		public String echo(String msg) {
			return "and echo:" + msg;
		}

		public void run() {
			Log.v("UDPReceiveThread", "run start..");
			// 接收的字节大小，客户端发送的数据不能超过MAX_UDP_DATAGRAM_LEN
			byte[] lMsg = new byte[1024];
			// 实例化一个DatagramPacket类
			DatagramPacket dp = new DatagramPacket(lMsg, lMsg.length);
			// 新建一个DatagramSocket类
			DatagramSocket ds = null;
			try {
				// UDP服务器监听的端口
				// 发送设置为广播
				// ds.setBroadcast(true);
				InetAddress local = null;
				try {
					local = InetAddress.getByName(Cfg.DEV_UDP_IPADDR); // 本机测试
					ds = new DatagramSocket(); // 注意此处要先在配置文件里设置权限,否则会抛权限不足的异常
					// local = InetAddress.getLocalHost(); // 本机测试
					System.out.println("local:" + local);
				} catch (UnknownHostException e) {
					e.printStackTrace();
				}
				String msg = "android hello.";
				int msg_len = msg == null ? 0 : msg.getBytes().length;
				DatagramPacket dPacket = new DatagramPacket(msg.getBytes(),
						msg_len, local, Cfg.DEV_UDP_PORT);
				ds.setBroadcast(true);
				ds.send(dPacket);

				Log.v("UDPReceiveThread", "run 1..");
				ds = new DatagramSocket(2468);
				sleep(2000);
				// 准备接收数据
				Log.v("UDPReceiveThread", "run 2..");
				ds.receive(dp);
				Log.v("UDPReceiveThread", "run 3..");
				String strInfo = new String(dp.getData(), 0, dp.getLength());
				System.out.println(strInfo);
			} catch (SocketException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} finally {
				// 如果ds对象不为空，则关闭ds对象
				if (ds != null) {
					ds.close();
				}
			}

			Log.v("UDPReceiveThread", "run end..");
		}
	}

}
