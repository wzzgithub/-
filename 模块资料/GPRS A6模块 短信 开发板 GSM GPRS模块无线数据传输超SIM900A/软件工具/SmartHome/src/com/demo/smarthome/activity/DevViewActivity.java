package com.demo.smarthome.activity;

import java.text.DateFormat;
import java.util.Date;

import com.demo.smarthome.R;
import com.demo.smarthome.activity.ScanActivity.StartUDPThread;
import com.demo.smarthome.activity.ScanActivity.UDPThread;
import com.demo.smarthome.device.Dev;
import com.demo.smarthome.iprotocol.IProtocol;
import com.demo.smarthome.protocol.MSGCMD;
import com.demo.smarthome.protocol.MSGCMDTYPE;
import com.demo.smarthome.protocol.Msg;
import com.demo.smarthome.protocol.PlProtocol;
import com.demo.smarthome.service.Cfg;
import com.demo.smarthome.service.SocketService;
import com.demo.smarthome.service.SocketService.SocketBinder;
import com.demo.smarthome.tools.IpTools;
import com.demo.smarthome.tools.StrTools;
import com.demo.smarthome.view.SlipButton;

import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.Window;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

public class DevViewActivity extends Activity implements OnClickListener {
	/** button open led */
	private Button btnOpenLed;
	
	/** button close led */
	private Button btnCloseLed;
	
	/** button flase led */
	private Button btnFlashLed;
	
	/** button open led */
	private Button btnOpenBeeper;
	
	/** button close led */
	private Button btnCloseBeeper;
	
	/** button flase led */
	private Button btnFlashBeeper;
	
	/** String send data */
	String  SendData;
	Dev dev = null;
	TextView title = null;

	Button btnSendString;
	EditText editSendString;

	String recvDataBody;

	private ListView messageListView;
	private ArrayAdapter<String> listAdapter;

	int cmdSendCount = 0;
	boolean btnIsOpen = false;
	IProtocol protocol = new PlProtocol();

	private String TAG = "DevViewActivity";
	static final int SEND_SUCCEED = 0;
	static final int SEND_ERROR = 1;
	static final int REFRESH_START = 4;
	static final int TIME_OUT = 5;
	static final int CMD_SUCCEEDT = 6;
	static final int CMD_TIMEOUT = 7;
	static final int RECV_MESSAGE_LOG = 8;
	static final int SEND_MESSAGE_LOG = 9;

	Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);

			// devViewLampChagne();
			switch (msg.what) {
			case SEND_SUCCEED:
				Toast.makeText(DevViewActivity.this, "开关灯成功....",
						Toast.LENGTH_SHORT).show();
				break;
			case SEND_ERROR:
				Toast.makeText(DevViewActivity.this, "开关灯失败！",
						Toast.LENGTH_SHORT).show();
				break;
			case REFRESH_START:

				break;
			case CMD_SUCCEEDT:
				Toast.makeText(DevViewActivity.this, "命令执行成功！",
						Toast.LENGTH_SHORT).show();
				break;
			case CMD_TIMEOUT:
				Toast.makeText(DevViewActivity.this, "命令执行失败！",
						Toast.LENGTH_SHORT).show();
				break;
			case TIME_OUT:
				break;
			case RECV_MESSAGE_LOG: {
				// 接收到字符串时，增加一条listView
				String message = recvDataBody;
				String currentDateTimeString = DateFormat.getTimeInstance()
						.format(new Date());
				listAdapter.add("[" + currentDateTimeString + "] RX: "
						+ message); // 增加一个item
				messageListView
						.smoothScrollToPosition(listAdapter.getCount() - 1); // 平滑的滚动到最底下
			}
				break;
			case SEND_MESSAGE_LOG: {
				// Update the log with time stamp
//				String message = editSendString.getText().toString();
				String message = SendData.toString();
				String currentDateTimeString = DateFormat.getTimeInstance()
						.format(new Date());
				listAdapter.add("[" + currentDateTimeString + "] TX: "
						+ message); // 增加一个item
				messageListView
						.smoothScrollToPosition(listAdapter.getCount() - 1); // 平滑的滚动到最底下
			}
				break;
			default:
				break;
			}
		}
	};

	SocketBinder socketBinder;
	SocketService socketService;
	boolean isBinderConnected = false;

	IntentFilter intentFilter = null;
	SocketIsConnectReceiver socketConnectReceiver = new SocketIsConnectReceiver();

	private class SocketIsConnectReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			String action = intent.getAction();

			if (action.equals(Cfg.SendBoardCastName)) {
				if (intent.getBooleanExtra("conn", false)) {
					Log.i(TAG, "socket连接成功。");
				} else {
					Log.i(TAG, "socket连接失败。");
				}
			}

			// 接收到8266的透传数据
			if (action.equals(Cfg.ReceiveMessage)) {
				byte[] data = intent.getByteArrayExtra("receiveData");
				// String str = StrTools.bytesToHexString(data);
				// Log.i(TAG, "receiveData:" + str);

				// 设备向平台透传数据, 碰到了有符号与无符号数比较的问题，0xAA != -86, 转成同类型后ok
				if (((byte) (0xAA) == data[3]) && (data[4] == (byte) (0xEE))) {
					int end = data.length - 3; // 去掉尾部的2字节校验和包尾
					String body = new String(data, 27, end - 27);// 18加上1字节token长度和8字节token

					Log.i(TAG, "receiveData:" + body);

					recvDataBody = body;

					Message message = new Message();
					message.what = RECV_MESSAGE_LOG;
					handler.sendMessage(message);
				}
			}
		}
	}

	static void recvMessage() {
		;
	}

	private ServiceConnection conn = new ServiceConnection() {
		@Override
		public void onServiceConnected(ComponentName name, IBinder service) {
			// TODO Auto-generated method stub
			Log.i(TAG, "onServiceConnected");
			socketBinder = (SocketBinder) service;
			socketService = socketBinder.getService();
			socketService.myMethod();// ipAddr, port

			isBinderConnected = true;
		}

		@Override
		public void onServiceDisconnected(ComponentName name) {
			// TODO Auto-generated method stub
			Log.i(TAG, "onServiceDisconnected");
			isBinderConnected = false;
			socketBinder = null;
			socketService = null;
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		setContentView(R.layout.activity_dev_view);
		TextView title = (TextView) findViewById(R.id.titleDevView);
		initUI();
		initEvent();
		title.setClickable(true);
		title.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				finish();
			}
		});

		Intent intent = this.getIntent();
		String devId = intent.getStringExtra("devId");
		dev = Cfg.getDevById(devId);
		Log.i(TAG, "devId:" + devId);
		Log.i(TAG, "dev:" + dev);
		if (dev == null) {
			return;
		}

		title = (TextView) findViewById(R.id.titleDevView);
		title.setClickable(true);
		title.setOnClickListener(new OnClickListener() {
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				finish();
			}
		});

		btnSendString = (Button) findViewById(R.id.btnSendString);
		btnSendString.setOnClickListener(new BtnSendStringOnChangeListener());

		editSendString = (EditText) findViewById(R.id.editSendString);

		// 中间显示消息的listview
		messageListView = (ListView) findViewById(R.id.listMessage);
		listAdapter = new ArrayAdapter<String>(this, R.layout.message_detail);
		messageListView.setAdapter(listAdapter);
		messageListView.setDivider(null);

		try {
			if (!isBinderConnected) {
				bindService();
			}
		} catch (Exception e) {

		}

		// 注册广播接收者, 设置意图过滤器
		intentFilter = new IntentFilter();
		intentFilter.addAction(Cfg.SendBoardCastName);
		intentFilter.addAction(Cfg.ReceiveMessage);
		this.registerReceiver(socketConnectReceiver, intentFilter);

		cmdSendCount = 0;
		dev.isDataChange(false);
	}

	/**
	 * 定义事件
	 */
	private void initEvent() {
		// TODO Auto-generated method stub
		btnOpenLed.setOnClickListener(this);
		btnCloseLed.setOnClickListener(this);
		btnFlashLed.setOnClickListener(this);
		
		btnOpenBeeper.setOnClickListener(this);
		btnCloseBeeper.setOnClickListener(this);
		btnFlashBeeper.setOnClickListener(this);
	}

	/**
	 * 定义组件
	 */
	private void initUI() {
		// TODO Auto-generated method stub
		btnOpenLed = (Button) findViewById(R.id.btnOpenLed);
		btnCloseLed = (Button) findViewById(R.id.btnCloseLed);
		btnFlashLed = (Button) findViewById(R.id.btnFlashLed);
		
		btnOpenBeeper  = (Button) findViewById(R.id.btnOpenBeeper);
		btnCloseBeeper  = (Button) findViewById(R.id.btnCloseBeeper);
		btnFlashBeeper  = (Button) findViewById(R.id.btnFlashBeeper);
	}

	/**
	 * 点击事件
	 */
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.btnOpenLed:
			SendData = new String("111110");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
		case R.id.btnCloseLed:
			SendData = new String("100000");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
		case R.id.btnFlashLed:
			SendData = new String("125113");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
			
		case R.id.btnOpenBeeper:
			SendData = new String("211110");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
		case R.id.btnCloseBeeper:
			SendData = new String("200000");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
		case R.id.btnFlashBeeper:
			SendData = new String("225113");
			new StartBtnSendThread().start();
			Toast.makeText(DevViewActivity.this, SendData, 1000).show();
			break;
			
			
			
		default:
			break;
		}
	}

	private void bindService() {
		// TODO Auto-generated method stub
		Intent intent = new Intent(DevViewActivity.this, SocketService.class);
		bindService(intent, conn, Context.BIND_AUTO_CREATE);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.dev_view, menu);
		return true;
	}

	class BtnSendStringOnChangeListener implements OnClickListener {
		@Override
		public void onClick(View v) {
			if (dev == null) {
				return;
			}
			SendData = editSendString.getText().toString();
			new StartBtnSendThread().start();
		}
	}
	class StartBtnSendThread extends Thread {
		public void run() {
			byte[] data;
//			data = editSendString.getText().toString().getBytes();
			data = SendData.toString().getBytes();
			Log.d(TAG, new String(data));
			Msg msg = new Msg();
			msg.setId(StrTools.byteToSwapByte(StrTools
					.hexStringToBytes(StrTools.strNumToHex(dev.getId()))));
			msg.setCmdType(MSGCMDTYPE.valueOf((byte) 0xAA));
			msg.setCmd(MSGCMD.valueOf((byte) 0xFF));
			msg.setTorken(dev.getTorken());
			msg.setData(data);
			msg.setDataLen(data.length);
			protocol.MessageEnCode(msg);
			socketService.socketSendMessage(msg);

			// 发送的消息也显示在listView中
			Message message = new Message();
			message.what = SEND_MESSAGE_LOG;
			handler.sendMessage(message);
		}
	}
//	class StartBtnSendThread extends Thread {
//		public void run() {
//			byte[] data;
//			data = editSendString.getText().toString().getBytes();
//
//			Log.d(TAG, new String(data));
//			Msg msg = new Msg();
//			msg.setId(StrTools.byteToSwapByte(StrTools
//					.hexStringToBytes(StrTools.strNumToHex(dev.getId()))));
//			msg.setCmdType(MSGCMDTYPE.valueOf((byte) 0xAA));
//			msg.setCmd(MSGCMD.valueOf((byte) 0xFF));
//			msg.setTorken(dev.getTorken());
//			msg.setData(data);
//			msg.setDataLen(data.length);
//			protocol.MessageEnCode(msg);
//			socketService.socketSendMessage(msg);
//
//			// 发送的消息也显示在listView中
//			Message message = new Message();
//			message.what = SEND_MESSAGE_LOG;
//			handler.sendMessage(message);
//		}
//	}
}
